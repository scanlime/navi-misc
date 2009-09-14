/*
 * patch_store.v - Storage and retrieval module for memory patch data.
 *                 Patches are set up using config register writes, and
 *                 this module generates the signals necessary to trigger
 *                 patches and supply data for fake RAM reads.
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


module patch_store(mclk, reset,
                   config_addr, config_data, config_strobe,
                   burst_addr, burst_addr_strobe,
                   patch_trigger, patch_data, patch_data_next);

   input mclk, reset;

   input [15:0] config_addr;
   input [15:0] config_data;
   input        config_strobe;

   input [22:0] burst_addr;
   input        burst_addr_strobe;

   output        patch_trigger;
   output [15:0] patch_data;
   input         patch_data_next;

   reg [15:0]    patch_data;

   wire [23:0]   inject_trigger_addr = 24'hfee1e0;

   assign patch_trigger = burst_addr_strobe && (burst_addr == inject_trigger_addr[23:1]);

   always @(posedge mclk or posedge reset)
     if (reset)
       patch_data <= 0;
     else if (patch_trigger)
       patch_data <= 16'hF000;
     else if (patch_data_next)
       patch_data <= patch_data + 1;

endmodule
