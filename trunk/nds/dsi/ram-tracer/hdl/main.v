/*
 * main.v - DS RAM Write Tracer.
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


module main(mclk, reset,
            ledseg_c, ledseg_a, led,
            sram_oe, sram_we,
            usb_d, usb_slwr, usb_slrd, usb_pktend,
            usb_sloe, usb_fifoadr, usb_flagb,
            ram_a, ram_d, ram_oe, ram_we, ram_ce1, ram_ce2,
            ram_ub, ram_lb, ram_adv, ram_clk,
            ram_buf_debug, ram_clk_debug);

   input mclk, reset;

   output [7:0] ledseg_c;
   output [3:0] ledseg_a;
   output [7:0] led;

   output       sram_oe, sram_we;

   output [7:0]  usb_d;
   output [1:0]  usb_fifoadr;
   output        usb_slwr, usb_slrd;
   output        usb_pktend, usb_sloe;
   input         usb_flagb;

   input [22:0] ram_a;
   input [15:0] ram_d;
   input        ram_oe, ram_we, ram_ce1, ram_ce2;
   input        ram_ub, ram_lb, ram_adv, ram_clk;
   output       ram_clk_debug, ram_buf_debug;

   // Disable SRAM, we're using the same pins for USB
   assign sram_oe = 1;
   assign sram_we = 1;

   // USB setup
   assign usb_fifoadr = 2'b10;  // EP6
   assign usb_sloe = 1;
   assign usb_slrd = 1;
   assign usb_pktend = !reset;
   wire   usb_full = !usb_flagb;

   // USB write registers, positive logic
   reg [7:0]  usb_d;
   reg        usb_wr_strobe;
   assign usb_slwr = !usb_wr_strobe;


   /************************************************
    * RAM Bus
    */

   // Positive-logic read/write enables
   // XXX: Ignoring CE for now
   wire         ram_read = !ram_oe;
   wire         ram_write = !ram_we;
   wire         ram_addr_latch = !ram_adv;

   // Synchronize to our clock domain with two flip-flops

   wire [22:0]  ram_a_sync;
   wire [15:0]  ram_d_sync;
   wire         ram_read_sync;
   wire         ram_write_sync;
   wire         ram_addr_latch_sync;

   d_flipflop_pair dffp1(mclk, reset, ram_read, ram_read_sync);
   d_flipflop_pair dffp2(mclk, reset, ram_write, ram_write_sync);
   d_flipflop_pair_bus #(23) dffp3(mclk, reset, ram_a, ram_a_sync);
   d_flipflop_pair_bus #(16) dffp4(mclk, reset, ram_d, ram_d_sync);
   d_flipflop_pair dffp5(mclk, reset, ram_addr_latch, ram_addr_latch_sync);

   // Shift register for synchronization, edge detection,
   //  and 1-cycle debouncer/delay on CLK

   reg [4:0]    ram_clk_shiftreg;
   wire         ram_clk_posedge = ram_clk_shiftreg[4:2] == 3'b011;
   wire         ram_clk_sync = ram_clk_shiftreg[1];

   always @(posedge mclk or posedge reset)
     if (reset)
       ram_clk_shiftreg <= 0;
     else
       ram_clk_shiftreg <= { ram_clk_shiftreg[3:0], ram_clk };

   // Packet buffer.
   //
   // This is a 3-byte packet that holds either an address or
   // a data byte, differentiated by the high bit of the first byte.

   reg [23:0]  pktbuf;
   wire        pktbuf_full;
   reg         pktbuf_reset;

   wire        pktbuf_set = ram_clk_posedge && (ram_write_sync || ram_addr_latch_sync);
   wire        pktbuf_overflow = pktbuf_set && pktbuf_full;

   set_reset_flipflop pktff(mclk, reset, pktbuf_set, pktbuf_reset, pktbuf_full);

   always @(posedge mclk or posedge reset)
     if (reset)
       pktbuf <= 0;
     else if (pktbuf_set)
       pktbuf <= ram_write_sync ? { 8'hAA, ram_d_sync } : { 1'b0, ram_a_sync };


   /************************************************
    * USB FIFO State Machine
    *
    * Above, we collect 3-byte data/address packets.
    * Now we stream them out over the synchronous
    * byte-wide USB FIFO.
    */

   reg [23:0]  usb_reg;
   reg [1:0]   usb_bytecnt;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        usb_reg <= 0;
        usb_bytecnt <= 0;
        usb_d <= 0;
        usb_wr_strobe <= 0;
        pktbuf_reset <= 0;
     end
     else begin
        if (usb_bytecnt == 0) begin
           // Idle. Do we have another packet to send?
           if (pktbuf_full) begin
              // Start a write
              usb_reg <= pktbuf;
              usb_bytecnt <= 3;
              usb_wr_strobe <= 0;
              pktbuf_reset <= 1;
           end
           else begin
              // Sit idle
              usb_bytecnt <= 0;
              usb_wr_strobe <= 0;
              pktbuf_reset <= 0;
           end
        end
        else begin
           // Next byte
           usb_reg <= { usb_reg[15:0], 8'hXX };
           usb_bytecnt <= usb_bytecnt - 1;
           usb_d <= usb_reg[23:16];
           usb_wr_strobe <= 1;
           pktbuf_reset <= 0;
        end
     end

   /************************************************
    * Debug
    */

   assign ram_clk_debug = ram_clk;

   reg ram_buf_debug;
   always @(posedge mclk)
     if (pktbuf_set)
       ram_buf_debug <= !ram_buf_debug;

   wire         pulse_read, pulse_write, pulse_overflow, pulse_pktbuf_overflow;

   pulse_stretcher s1(mclk, reset, ram_read_sync, pulse_read);
   pulse_stretcher s2(mclk, reset, ram_write_sync, pulse_write);
   pulse_stretcher s3(mclk, reset, usb_full, pulse_fifo_overflow);
   pulse_stretcher s4(mclk, reset, pktbuf_overflow, pulse_pktbuf_overflow);

   /* 8 individual status LEDs */
   assign led = { pulse_read, pulse_write, pktbuf_full, 3'b0,
                  pulse_fifo_overflow, pulse_pktbuf_overflow };

   /* LED display: Current memory address, high 16 bits. */
   led_hex display(mclk, reset, ram_a_sync[22:7], ledseg_c, ledseg_a);

endmodule // main

