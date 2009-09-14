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

/*
 * This implementation of patch_store uses block RAM to store the actual
 * patch data, and it uses a Xilinx CAM (Content Addressable Memory) core
 * to quickly look up patches for any given burst_addr.
 *
 * We support up to 64 patches loaded at a time. Each patch consists of:
 *
 *    - An address and bitmask for the CAM. This defines which
 *      memory bursts will be affected by the patch.
 *
 *    - An address offset. This is added to the address to calculate a
 *      location in the patch buffer to start reading at. These offsets
 *      are stored in a separate RAM block.
 *
 *    - Any number of 16-bit data words, retrieved starting at the
 *      calculated address in an 16 kB patch data buffer.
 *
 * Patches are configured over USB, using the config bus. The 16-bit
 * data memory is mapped directly onto the config bus starting at
 * address 0x8000. Addresses are mapped at 0x7800, in little-endian
 * byte order.
 *
 * Writing to the CAM is more complex since the write cycle requires
 * 46 bits of data in addition to an address, and write operations
 * take 16 clock cycles to complete. To handle the large amount of
 * data necessary for each write, we use an extra level of indirection:
 * address and data are set up using four 16-bit registers, then we
 * begin a write when a CAM address is written to a fifth config
 * register. We don't have to worry about the duration of the write,
 * since it takes at least 25 clock cycles to receive all of that
 * data over USB.
 *
 * Register map:
 *
 *    7000       CAM address value, low
 *    7001       CAM address value, high
 *    7002       CAM address mask, low
 *    7003       CAM address mask, high
 *    7004       CAM patch index / write trigger
 *    7800-787f  Patch buffer offsets
 *    8000-9fff  Patch content
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


   /************************************************
    * Patch content memory
    */

   //synthesis attribute ram_style of offset_mem is block
   reg [15:0]    content_mem[8191:0];

   always @(posedge mclk)
     if (config_strobe && config_addr[15])
       content_mem[config_addr[12:0]] <= config_data;


   /************************************************
    * Patch buffer offset memory
    */

   //synthesis attribute ram_style of offset_mem_low is block
   reg [15:0]    offset_mem_low[63:0];

   //synthesis attribute ram_style of offset_mem_high is block
   reg [15:0]    offset_mem_high[63:0];

   always @(posedge mclk)
     if (config_strobe && config_addr[15:8] == 8'h78) begin
       if (config_addr[0])
         offset_mem_high[config_addr[6:1]] <= config_data;
       else
         offset_mem_low[config_addr[6:1]] <= config_data;
     end


   /************************************************
    * Content Addressable Memory
    */

   wire [31:0]   cfg_cam_addr;
   wire [31:0]   cfg_cam_mask;

   wire          cam_write_strobe = config_strobe && (config_addr == 16'h7004);
   wire [5:0]    cam_write_index = config_data[5:0];

   usb_config #(16'h7000) cfg1(mclk, reset, config_addr, config_data,
                               config_strobe, cfg_cam_addr[15:0]);
   usb_config #(16'h7001) cfg2(mclk, reset, config_addr, config_data,
                               config_strobe, cfg_cam_addr[31:16]);

   usb_config #(16'h7002) cfg3(mclk, reset, config_addr, config_data,
                               config_strobe, cfg_cam_mask[15:0]);
   usb_config #(16'h7003) cfg4(mclk, reset, config_addr, config_data,
                               config_strobe, cfg_cam_mask[31:16]);

   camdp_23_64 cam(.clk(mclk),
                   .cmp_data_mask(23'h000000),
                   .cmp_din(burst_addr),
                   .data_mask(cfg_cam_mask[22:0]),
                   .din(cfg_cam_addr[22:0]),
                   .en(1),
                   .we(cam_write_strobe),
                   .wr_addr(cam_write_index),
                   .busy(),
                   .match(cam_match),
                   .match_addr(cam_match_addr));


   /************************************************
    * Patch control
    */

   // Delay burst_addr and burst_addr_strobe by one cycle, to match the CAM's latency

   reg           burst_strobe_1;
   reg [22:0]    burst_addr_1;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        burst_strobe_1 <= 0;
        burst_addr_1 <= 0;
     end
     else begin
        burst_strobe_1 <= burst_addr_strobe;
        burst_addr_1 <= burst_addr;
     end

   // Trigger when the CAM matches, and latch the patch index.

   assign patch_trigger = burst_strobe_1 && cam_match;

   // Patch data comes straight from content RAM

   reg [12:0] content_addr;
   assign patch_data = content_mem[content_addr];

   // Memory addresses for the current patch index, valid on patch_trigger.

   wire [31:0] current_patch_offset = { offset_mem_high[cam_match_addr],
                                        offset_mem_low[cam_match_addr] };
   wire [22:0] current_patch_ptr = current_patch_offset[22:0] + burst_addr_1;

   // Content address control

   always @(posedge mclk or posedge reset)
     if (reset)
       content_addr <= 0;
     else if (patch_trigger)
       content_addr <= content_addr[12:0];
     else if (patch_data_next)
       content_addr <= content_addr + 1;

endmodule
