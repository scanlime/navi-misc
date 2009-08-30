/*
 * main.v - Top-level design for the DS RAM Tracer.
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
            usb_d, usb_slwr, usb_pktend, usb_sloe, usb_fifoadr, usb_flagb,
            ram_a, ram_d, ram_oe, ram_we, ram_ce1, ram_ce2,
            ram_ub, ram_lb, ram_adv);

   input mclk, reset;

   output [7:0] ledseg_c;
   output [3:0] ledseg_a;
   output [7:0] led;

   output       sram_oe, sram_we;

   output [7:0]  usb_d;
   output [1:0]  usb_fifoadr;
   output        usb_slwr, usb_pktend, usb_sloe;
   input         usb_flagb;

   input [22:0] ram_a;
   input [15:0] ram_d;
   input        ram_oe, ram_we, ram_ce1, ram_ce2;
   input        ram_ub, ram_lb, ram_adv;

   // Disable SRAM, we're using the same pins for USB
   assign sram_oe = 1;
   assign sram_we = 1;

   // USB setup
   assign usb_fifoadr = 2'b10;  // EP6
   assign usb_sloe = 1;
   assign usb_pktend = reset;
   wire   usb_full = !usb_flagb;

   reg [10:0]   ctr;
   assign usb_d = ctr[10:3];
   assign usb_slwr = !ctr[2];

   always @(posedge mclk or posedge reset)
     if (reset)
       ctr <= 0;
     else
       ctr <= ctr + 1;


   /************************************************
    * RAM Bus
    */

   // Positive-logic read/write enables
   wire         ram_enable = !ram_ce1 && !ram_ce2;
   wire         ram_read = ram_enable && ram_oe;
   wire         ram_write = ram_enable && ram_we;

   // Synchronize to our clock domain with two flip-flops

   wire [22:0]  ram_a_sync;
   wire [15:0]  ram_d_sync;
   wire         ram_read_sync;
   wire         ram_write_sync;

   d_flipflop_pair dffp1(mclk, reset, ram_read, ram_read_sync);
   d_flipflop_pair dffp2(mclk, reset, ram_write, ram_write_sync);
   d_flipflop_pair_bus #(23) dffp3(mclk, reset, ram_a, ram_a_sync);
   d_flipflop_pair_bus #(16) dffp4(mclk, reset, ram_d, ram_d_sync);

   /************************************************
    * Debug
    */

   wire         pulse_read, pulse_write, pulse_overflow;

   pulse_stretcher s1(mclk, reset, ram_read_sync, pulse_read);
   pulse_stretcher s2(mclk, reset, ram_write_sync, pulse_write);
   pulse_stretcher s3(mclk, reset, usb_full, pulse_overflow);

   /* 8 individual status LEDs */
   assign led = { pulse_read, pulse_write, 5'b0, pulse_overflow };

   /* LED display: Current memory address, high 16 bits. */
   led_hex display(mclk, reset, ram_a_sync[22:7], ledseg_c, ledseg_a);

endmodule // main

