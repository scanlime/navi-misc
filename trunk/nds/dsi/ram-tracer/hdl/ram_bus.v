/*
 * ram_bus.v - Utilities for dealing with the RAM bus
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
 * The ram_sampler module performs all of the initial signal conditioning we
 * need to do on the RAM bus. This synchronizes the raw inputs to our clock
 * domain, detects clock edges, and performs noise filtering.
 */

module ram_sampler(mclk, reset,
                   ram_a, ram_d, ram_oe, ram_we, ram_ce1, ram_ce2,
                   ram_ub, ram_lb, ram_adv, ram_clk,
                   filter_a, filter_d, filter_ublb, filter_read,
                   filter_write, filter_addr_latch, filter_strobe);

   input mclk, reset;

   input [22:0] ram_a;
   input [15:0] ram_d;
   input        ram_oe, ram_we, ram_ce1, ram_ce2;
   input        ram_ub, ram_lb, ram_adv, ram_clk;

   output [22:0] filter_a;
   output [15:0] filter_d;
   output [1:0]  filter_ublb;
   output        filter_read;
   output        filter_write;
   output        filter_addr_latch;
   output        filter_strobe;

   // Positive-logic control signals
   wire         ram_enable = !ram_ce1 && ram_ce2;
   wire         ram_read = ram_enable && !ram_oe;
   wire         ram_write = ram_enable && !ram_we;
   wire         ram_addr_latch = ram_enable && !ram_adv;
   wire [1:0]   ram_ublb = { !ram_ub, !ram_lb };

   // Clock domain synchronization

   wire [22:0]  ram_a_sync;
   wire [15:0]  ram_d_sync;
   wire [1:0]   ram_ublb_sync;
   wire         ram_read_sync;
   wire         ram_write_sync;
   wire         ram_addr_latch_sync;

   d_flipflop_pair_bus #(44) sync_dff(mclk, reset,
                                      { ram_a, ram_d, ram_ublb, ram_read,
                                        ram_write, ram_addr_latch },
                                      { ram_a_sync, ram_d_sync, ram_ublb_sync,
                                        ram_read_sync, ram_write_sync,
                                        ram_addr_latch_sync });

   // Clock shift register.
   // For edge detection, filtering, and clock domain sync.

   reg [9:0]    clk_shift;

   always @(posedge mclk or posedge reset)
     if (reset)
       clk_shift <= 0;
     else
       clk_shift <= {clk_shift[8:0], ram_clk};

   // Taps which detect positive edges N clocks ago relative to *_sync.

   wire         clk_posedge_0 = clk_shift[5:2] == 3'b011;     // 1 clock
   wire         clk_posedge_1 = clk_shift[7:2] == 5'b01111;   // 3 clocks
   wire         clk_posedge_2 = clk_shift[9:2] == 7'b0111111; // 5 clocks

   // Majority-detect: Latch clocks 1/3, generate output on 5

   reg [43:0]   filter_lat_0;
   reg [43:0]   filter_lat_1;
   wire [43:0]  filter_cur = { ram_a_sync, ram_d_sync, ram_ublb_sync,
                               ram_read_sync, ram_write_sync,
                               ram_addr_latch_sync };

   wire [43:0]  filter_detect;  // Un-latched output from majority-detector
   reg [43:0]   filter_out;     // Latched output
   reg          filter_strobe;  // Latched strobe

   assign {filter_a, filter_d, filter_ublb, filter_read,
           filter_write, filter_addr_latch} = filter_out;

   mdetect_3_arr #(44) filter_mdet(filter_lat_0, filter_lat_1,
                                   filter_cur, filter_detect);

   always @(posedge mclk or posedge reset)
     if (reset) begin
        filter_lat_0 <= 0;
        filter_lat_1 <= 0;
        filter_out <= 0;
        filter_strobe <= 0;
     end
     else if (clk_posedge_0) begin
        filter_lat_0 <= filter_cur;
        filter_strobe <= 0;
     end
     else if (clk_posedge_1) begin
        filter_lat_1 <= filter_cur;
        filter_strobe <= 0;
     end
     else if (clk_posedge_2) begin
        filter_out <= filter_detect;
        filter_strobe <= 1;
     end
     else begin
        filter_strobe <= 0;
     end

endmodule // ram_sampler
