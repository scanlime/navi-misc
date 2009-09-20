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
 * Convenience module for implementing configuration registers:
 * Attaches to the configuration bus, monitors a single address,
 * and provides a latched output for that address.
 */

module usb_config(mclk, reset, config_addr, config_data, config_strobe, reg_out);
   parameter ADDRESS = 0;

   input mclk, reset;
   input [15:0] config_addr;
   input [15:0] config_data;
   input        config_strobe;
   output [15:0] reg_out;
   reg [15:0]    reg_out;

   always @(posedge mclk or posedge reset)
     if (reset)
       reg_out <= 0;
     else if (config_strobe && config_addr == ADDRESS)
       reg_out <= config_data;
endmodule


/*
 * This module is in charge of USB communications through an
 * FT2232H in synchronous FIFO mode. Communications to and from
 * the PC are handled somewhat differently:
 *
 *  - Communication to the PC is designed for high-bandwidth
 *    delivery of 32-bit packets. This module has an on-die FIFO
 *    which feeds the FT2232H's fifo.
 *
 *    Normally we're sending 32-bit packets to the PC, in the format
 *    described above. If an overflow occurs, we send 0xFFFFFFFF
 *    until reset.
 *
 *  - Communication from the PC is used for lower-bandwidth
 *    configuration info. This channel emulates a write-only
 *    register space with 16-bit data and 16-bit address. Writes
 *    are encoded into a 5-byte packet:
 *
 *   Byte 0             Byte 1             Byte 2             Byte 3             Byte 4
 *
 *   7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0    7 6 5 4 3 2 1 0
 *  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+
 *   1 r r r a a d d    0 a a a a a a a    0 a a a a a a a    0 d d d d d d d    0 d d d d d d d
 *  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+  +-+-+-+-+-+-+-+-+
 *           1 1 1 1      1 1 1 1                               1 1 1 1
 *           5 4 5 4      3 2 1 0 9 8 7      6 5 4 3 2 1 0      3 2 1 0 9 8 7      6 5 4 3 2 1 0
 *
 *   r = reserved, must be zero
 *   a = 16-bit register address
 *   d = data
 */

module usb_comm(mclk, reset,
                usb_d, usb_rxf_n, usb_txe_n, usb_rd_n, usb_wr_n, usb_oe_n,
                packet_data, packet_strobe,
                config_addr, config_data, config_strobe);
   /*
    * Size of the local FIFO buffer.
    * Make this as big as you can afford!
    */
   parameter LOG_BUF_SIZE = 11;
   parameter BUF_SIZE = 2**LOG_BUF_SIZE;
   parameter BUF_MSB = LOG_BUF_SIZE - 1;

   input mclk, reset;

   inout [7:0] usb_d;
   input       usb_rxf_n, usb_txe_n;
   output      usb_rd_n, usb_wr_n, usb_oe_n;

   input [31:0]  packet_data;
   input         packet_strobe;

   output [15:0] config_addr;
   output [15:0] config_data;
   output        config_strobe;


   /************************************************
    * Control signals
    *
    * Make positive-logic signals with nice names.
    */

   wire          write_ready = !usb_txe_n;
   wire          read_ready = !usb_rxf_n;

   wire          read_request;
   wire          read_oe;
   wire          write_request;
   wire [7:0]    write_data;

   assign usb_rd_n = !read_request;
   assign usb_wr_n = !write_request;
   assign usb_oe_n = !read_oe;
   assign usb_d = write_request ? write_data : 8'hZZ;


   /************************************************
    * Packet FIFO buffer
    */

   //synthesis attribute ram_style of fifo_mem is block
   reg [31:0] fifo_mem[BUF_SIZE-1:0];

   // When fifo_rd_en is asserted, fifo_dout is valid on the following clock cycle.
   wire        fifo_rd_en;
   reg [31:0]  fifo_dout;
   wire        fifo_empty;
   wire        fifo_full;
   wire        fifo_overflow;

   reg [BUF_MSB:0] fifo_read_ptr;
   reg [BUF_MSB:0] fifo_write_ptr;
   wire [BUF_MSB:0] next_write_ptr = fifo_write_ptr + 1;

   assign fifo_empty = fifo_read_ptr == fifo_write_ptr;
   assign fifo_full = next_write_ptr == fifo_read_ptr;
   assign fifo_overflow = fifo_full && packet_strobe;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        fifo_write_ptr <= 0;
        fifo_read_ptr <= 0;
        fifo_dout <= 0;
     end
     else begin
        if (packet_strobe && !fifo_full) begin
           fifo_write_ptr <= next_write_ptr;
           fifo_mem[fifo_write_ptr] <= packet_data;
        end
        if (fifo_rd_en && !fifo_empty) begin
           fifo_read_ptr <= fifo_read_ptr + 1;
           fifo_dout <= fifo_mem[fifo_read_ptr];
        end
     end

   // Latch any overflow errors. If an overflow occurs, we continue
   // reporting an error condition until reset.

   //synthesis attribute INIT of error_latch is "R"
   reg         error_latch;

   always @(posedge mclk or posedge reset)
     if (reset)
       error_latch <= 0;
     else if (fifo_overflow)
       error_latch <= 1;


   /************************************************
    * USB FIFO State Machine
    *
    * This state machine coordinates reads and writes on the external
    * USB FIFO. Reads (commands) take first priority, since they are
    * lower-bandwidth. Writes (packets) are very high bandwidth and
    * they would starve reads if they took priority.
    *
    * To saturate the USB 2.0 bus, we must take no more than 2 clock
    * cycles to output each byte. We will spend these cycles alternately
    * outputting bytes and checking whether the FT2232H's FIFO is full.
    *
    *    Cycle 0:
    *        - Sample fifo_empty, read_ready, write_ready. Decide
    *          whether to read or write.
    *        - If we're writing and we need another 32-bit word,
    *          fifo_rd_en is asserted for this cycle.
    *        - If we're reading, read_request is asserted.
    *
    *    Cycle 1, writes:
    *        - fifo_dout is valid now. Strobe this data to the FT2232H's FIFO.
    *        - Advance the write buffer
    *
    *    Cycle 1, reads:
    *        - usb_d is valid now. Shift it into the read buffer
    *        - If we have a complete packet, strobes a command
    */

   parameter S_C0 = 0;
   parameter S_C1_WRITE = 1;
   parameter S_C1_READ = 2;

   reg [23:0]        packet_reg;
   reg [1:0]         packet_cnt;
   reg [27:0]        cmd_reg;     // Lower 7 bits from first 4 bytes
   reg [2:0]         cmd_cnt;
   reg [1:0]         state;

   reg [15:0]        config_addr;
   reg [15:0]        config_data;
   reg               config_strobe;

   wire              write_next_packet = packet_cnt == 0;
   wire              write_avail = error_latch || !(write_next_packet && fifo_empty);

   wire              c0 = state == S_C0;
   wire              c0_read = c0 && read_ready;
   wire              c0_write = c0 && !read_ready && write_avail && write_ready;
   wire              c1_write = state == S_C1_WRITE;
   wire              c1_read = state == S_C1_READ;

   wire              read_sync_bit = usb_d[7];

   assign read_request = c0_read;
   assign read_oe = c0_read || c1_read;
   assign write_request = c1_write;

   assign write_data = error_latch ? 8'hFF :
                       write_next_packet ? fifo_dout[31:24] : packet_reg[23:16];
   assign fifo_rd_en = c0_write && write_next_packet;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        packet_reg <= 0;
        packet_cnt <= 0;
        cmd_reg <= 0;
        cmd_cnt <= 0;
        state <= 0;

        config_addr <= 0;
        config_data <= 0;
        config_strobe <= 0;
     end
     else
       case (state)

         S_C0: begin
            if (c0_read)
              state <= S_C1_READ;
            else if (c0_write)
              state <= S_C1_WRITE;
            config_strobe <= 0;
         end

         S_C1_WRITE: begin
            if (write_next_packet)          // We just read the next 32-bit word
              packet_reg <= fifo_dout[23:0];
            else
              packet_reg <= { packet_reg[16:0], 8'hXX };
            packet_cnt <= packet_cnt - 1;   // Rolls over at zero
            state <= S_C0;
            config_strobe <= 0;
         end

         S_C1_READ: begin

            if (read_sync_bit) begin
               // First byte of command (reset)
               cmd_cnt <= 1;
            end
            else if (cmd_cnt == 0) begin
               // Not in a command yet
            end
            else if (&cmd_cnt) begin
               // Overrun past the end of the command
            end
            else begin
               cmd_cnt <= cmd_cnt + 1;

               if (cmd_cnt == 4) begin
                  // Last byte of 5-byte command

                  config_addr <= { cmd_reg[24:23], cmd_reg[20:7] };
                  config_data <= { cmd_reg[22:21], cmd_reg[6:0], usb_d[6:0] };

                  // cmd_reg[27:25] is reserved, must be zero.
                  config_strobe <= cmd_reg[27:25] == 0;
               end
            end

            cmd_reg <= { cmd_reg[20:0], usb_d[6:0] };  // Save the low 7 bits
            state <= S_C0;
         end
       endcase

endmodule // usb_comm
