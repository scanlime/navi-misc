/*
 * usb_comm.v - USB communications for the RAM Tracer.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Assemble a 32-bit packet from its common component pieces.
 * This only fills in the parts that every packet has- there is still
 * a 23-bit portion which has a type-specific meaning.
 *
 * The general packet structure is:
 *
 *   Byte 0             Byte 1             Byte 2             Byte 3
 *
 *   7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
 *  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+
 *   1 t t p p p p p    0 p p p p p p p    0 p p p p p p p    0 p p p p c c c
 *  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+
 *         2 2 2 1 1      1 1 1 1 1 1 1      1
 *     1 0 2 1 0 9 8      7 6 5 4 3 2 1      0 9 8 7 6 5 4      3 2 1 0 2 1 0
 *
 *  MSB:  Flag bit, for packet alignment.
 *  t:    Type. 0=addr, 1=read data, 2=write data, 3=timestamp
 *  p:    23-bit payload (type-specific)
 *  c:    3-bit checksum.
 *
 * The checksum is computed by padding the payload to 24-bits with an extra zero, adding
 * each 3-bit portion of that payload, then adding a (zero-padded) copy of the type code.
 *
 * Payloads
 * --------
 *
 * Address Packets (0):
 *   Payload consists of only a 23-bit address.
 *
 * Read data word (1) / Write data word (2):
 *
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    t t t t t u l d d d d d d d d d d d d d d d d
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                  1 1 1 1 1 1
 *    4 3 2 1 0     5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *
 *    d: 16-bit data word
 *    u: upper byte valid
 *    l: lower byte valid
 *    t: 5-bit count of how many RAM clock cycles were skipped
 *       since the last packet that contained a timestamp.
 *
 * Timestamp (3):
 *   Payload consists of a 23-bit count of how many RAM cycles
 *   were skipped since the last packet that contained a timestamp.
 */

module usb_packet_assemble(packet_out, type_code, payload);

   output [31:0] packet_out;
   input [1:0]   type_code;
   input [22:0]  payload;

   wire [2:0] check = ( { 1'b0, type_code } +
                        { 1'b0, payload[22:21] } +
                        payload[20:18] +
                        payload[17:15] +
                        payload[14:12] +
                        payload[11:9] +
                        payload[8:6] +
                        payload[5:3] +
                        payload[2:0] );

   assign packet_out = { 1'b1, type_code, payload[22:18],
                         1'b0, payload[17:11],
                         1'b0, payload[10:4],
                         1'b0, payload[3:0], check };
endmodule


/*
 * This module is in charge of USB communications through an
 * FT2232H in synchronous FIFO mode. It provides a fast on-chip
 * 32-bit-wide FIFO buffer that feeds the off-chip 8-bit-wide FIFO.
 *
 * We handle flow control between the two FIFOs, but if the
 * on-chip FIFO fills up, we signal an overflow error back to
 * the host.
 */

module usb_comm(mclk, reset,
                usb_d, usb_rxf_n, usb_txe_n, usb_rd_n, usb_wr_n,
                packet_data, packet_strobe);

   /*
    * Size of the local FIFO buffer.
    * Make this as big as you can afford!
    */
   parameter LOG_BUF_SIZE = 8;
   parameter BUF_SIZE = 2**LOG_BUF_SIZE;
   parameter BUF_MSB = LOG_BUF_SIZE - 1;

   input mclk, reset;

   inout [7:0] usb_d;
   input       usb_rxf_n, usb_txe_n;
   output      usb_rd_n, usb_wr_n;

   input [31:0]  packet_data;
   input         packet_strobe;


   /************************************************
    * USB Setup
    */

   wire          rx_full = !usb_rxf_n;
   wire          tx_empty = !usb_txe_n;

   reg           read_request;
   reg           write_request;
   reg [7:0]     write_data;

   assign usb_rd_n = !read_request;
   assign usb_wr_n = !write_request;
   assign usb_d = write_request ? write_data : 8'hZZ;


   /************************************************
    * Packet FIFO buffer
    */

   // XXX: This gets synchronized as distributed RAM. Block RAM would be more efficient.
   reg [31:0]       fifo_mem[BUF_SIZE-1:0];

   reg [BUF_MSB:0]  fifo_write_ptr;
   reg [BUF_MSB:0]  fifo_read_ptr;

   wire [BUF_MSB:0] next_write_ptr = fifo_write_ptr + 1;

   assign overflow = (next_write_ptr == fifo_read_ptr) && packet_strobe;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        fifo_write_ptr <= 0;
     end
     else if (packet_strobe && !overflow) begin
        fifo_write_ptr <= next_write_ptr;
        fifo_mem[fifo_write_ptr] <= packet_data;
     end


   /************************************************
    * USB FIFO State Machine
    */

   reg [31:0]        usb_reg;
   reg [2:0]         usb_bytecnt;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        usb_reg <= 0;
        usb_bytecnt <= 0;
        write_data <= 0;
        write_request <= 0;
        read_request <= 0;
     end
     else if (write_request) begin
        /*
         * We just wrote a byte. Wait one cycle, to give the FIFO
         * a chance to report whether it's full or not.
         */
        write_request <= 0;
        read_request <= 0;
     end
     else if (rx_full) begin
        /*
         * Wait for room in the USB FIFO
         */
        write_request <= 0;
        read_request <= 0;
     end
     else if (usb_bytecnt == 0) begin
        // Idle. Do we have another packet to send?
        if (fifo_read_ptr != fifo_write_ptr) begin
           // Start a write. Load our shift register, clear the packet buffer.
           usb_reg <= fifo_mem[fifo_read_ptr];
           usb_bytecnt <= 4;
           fifo_read_ptr <= fifo_read_ptr + 1;
        end
        else begin
           // Sit idle
           usb_bytecnt <= 0;
        end

        write_request <= 0;
        read_request <= 0;
     end
     else begin
        // Next byte
        usb_reg <= { usb_reg[23:0], 8'hXX };
        usb_bytecnt <= usb_bytecnt - 1;
        write_data <= usb_reg[31:24];
        write_request <= 1;
        read_request <= 0;
     end

endmodule // usb_comm
