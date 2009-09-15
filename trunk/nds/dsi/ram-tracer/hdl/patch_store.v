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
 * address 0x8000. Addresses are mapped at 0x7800. The addresses only
 * need to be 13 bits wide, so they're stored in 16-bit registers.
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
 *    7800-783f  Patch buffer offsets (16-bit)
 *    8000-9fff  Patch content
 *
 * Notes on latency:
 *
 *   - We have up to one full RAM clock after patch_data_next to
 *     provide new data on patch_data, and we have about 2 RAM clocks
 *     from burst_addr_strobe to when we must trigger the patch
 *     (but triggering earlier may help signal integrity).
 *
 *   - At the slowest sysclock speeds, we have about 12 master clock
 *     cycles per RAM clock cycle. So we actually have a fair amount
 *     of time to crunch on each word.
 *
 *   - To avoid timing problems, this implementation is currently pretty
 *     heavily pipelined. It should still be plenty fast enough for 2 MHz
 *     sysclock speeds (~4MHz RAM clock), but we might need to optimize it
 *     if runnign at higher frequencies.
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

   reg           content_wr_enable;
   reg [15:0]    content_wr_data;
   reg [12:0]    content_wr_addr;

   reg [15:0]    content_rd_data;
   reg [12:0]    content_rd_addr;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        content_wr_enable <= 0;
        content_wr_data <= 0;
        content_wr_addr <= 0;
        content_rd_data <= 0;
     end
     else begin
        content_wr_enable <= config_strobe && config_addr[15];
        content_wr_data <= config_data;
        content_wr_addr <= config_addr[12:0];
        content_rd_data <= content_mem[content_rd_addr];

        if (content_wr_enable)
          content_mem[content_wr_addr] <= content_wr_data;
     end


   /************************************************
    * Patch buffer offset memory
    */

   //synthesis attribute ram_style of offset_mem is block
   reg [12:0]    offset_mem[63:0];

   reg           offset_wr_enable;
   reg [12:0]    offset_wr_data;
   reg [5:0]     offset_wr_addr;

   reg           offset_rd_strobe_in;
   reg           offset_rd_strobe_out;
   reg [12:0]    offset_rd_data;
   reg [5:0]     offset_rd_addr;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        offset_wr_enable <= 0;
        offset_wr_data <= 0;
        offset_wr_addr <= 0;
        offset_rd_strobe_out <= 0;
        offset_rd_data <= 0;
     end
     else begin
        offset_wr_enable <= config_strobe && config_addr[15:8] == 8'h78;
        offset_wr_data <= config_data[12:0];
        offset_wr_addr <= config_addr[5:0];

        offset_rd_strobe_out <= offset_rd_strobe_in;
        offset_rd_data <= offset_mem[offset_rd_addr];

        if (offset_wr_enable)
          offset_mem[offset_wr_addr] <= offset_wr_data;
     end


   /************************************************
    * Content Addressable Memory
    */

   reg           cam_wr_enable;
   reg [22:0]    cam_wr_addr;
   reg [22:0]    cam_wr_mask;
   reg [5:0]     cam_wr_index;

   reg           cam_rd_enable;
   reg [22:0]    cam_rd_addr;
   reg [5:0]     cam_rd_index;
   reg           cam_rd_match;

   reg           cam_rd_enable_out;
   wire [5:0]    cam_rd_index_out;
   wire          cam_rd_match_out;

   camdp_23_64 cam(.clk(mclk),
                   .cmp_data_mask(23'h000000),
                   .cmp_din(cam_rd_addr),
                   .data_mask(cam_wr_mask),
                   .din(cam_wr_addr),
                   .en(1),
                   .we(cam_wr_enable),
                   .wr_addr(cam_wr_index),
                   .busy(),
                   .match(cam_rd_match_out),
                   .match_addr(cam_rd_index_out));

   wire          cfg_cam_addr_low  = config_strobe && (config_addr == 16'h7000);
   wire          cfg_cam_addr_high = config_strobe && (config_addr == 16'h7001);
   wire          cfg_cam_mask_low  = config_strobe && (config_addr == 16'h7002);
   wire          cfg_cam_mask_high = config_strobe && (config_addr == 16'h7003);
   wire          cfg_cam_index     = config_strobe && (config_addr == 16'h7004);

   always @(posedge mclk or posedge reset)
     if (reset) begin
        cam_wr_enable <= 0;
        cam_wr_addr <= 0;
        cam_wr_mask <= 0;
        cam_wr_index <= 0;

        cam_rd_enable <= 0;
        cam_rd_addr <= 0;
        cam_rd_index <= 0;
        cam_rd_match <= 0;

        cam_rd_enable_out <= 0;
     end
     else begin
        if (cfg_cam_addr_low)   cam_wr_addr[15:0] <= config_data;
        if (cfg_cam_addr_high)  cam_wr_addr[22:16] <= config_data[6:0];
        if (cfg_cam_mask_low)   cam_wr_mask[15:0] <= config_data;
        if (cfg_cam_mask_high)  cam_wr_mask[22:16] <= config_data[6:0];
        if (cfg_cam_index)      cam_wr_index <= config_data[5:0];
        cam_wr_enable <= cfg_cam_index;

        cam_rd_enable <= burst_addr_strobe;
        cam_rd_addr <= burst_addr;
        cam_rd_index <= cam_rd_index_out;
        cam_rd_match <= cam_rd_match_out && cam_rd_enable_out;

        cam_rd_enable_out <= cam_rd_enable;
     end


   /************************************************
    * Patch control
    */

   assign patch_trigger = cam_rd_match;   // Trigger as soon as the CAM matches
   assign patch_data = content_rd_data;   // Patch data comes straight from content RAM

   reg [12:0] burst_offset;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        burst_offset <= 0;
        content_rd_addr <= 0;
        offset_rd_addr <= 0;
        offset_rd_strobe_in <= 0;
     end
     else begin

        if (burst_addr_strobe)
          burst_offset <= burst_addr[12:0];

        // Read offset after the CAM matches
        offset_rd_addr <= cam_rd_index;
        offset_rd_strobe_in <= cam_rd_match;

        // After offset read, latch new content addr
        if (offset_rd_strobe_out)
          content_rd_addr <= offset_rd_data + burst_offset;
        else if (patch_data_next)
          content_rd_addr <= content_rd_addr + 1;

     end

endmodule
