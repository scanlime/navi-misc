/*
 * main.v - Top-level module for the RAM Tracer.
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


module main(mclk, clk50,
            usb_d, usb_rxf_n, usb_txe_n, usb_rd_n, usb_wr_n, usb_oe_n,
            ram_a, ram_d, ram_oe, ram_we, ram_ce1_in,
            ram_ce1_out, ram_ce2, ram_ub, ram_lb, ram_adv, ram_clk,
            dsi_sysclk, dsi_reset);

   input mclk, clk50;
   wire  reset = 1'b0;   // Placeholder- no need for reset yet

   inout [7:0] usb_d;
   input       usb_rxf_n, usb_txe_n;
   output      usb_rd_n, usb_wr_n, usb_oe_n;

   input [22:0] ram_a;
   inout [15:0] ram_d;
   input        ram_oe, ram_we, ram_ce1_in, ram_ce2;
   input        ram_ub, ram_lb, ram_adv, ram_clk;

   output       ram_ce1_out;
   output       dsi_sysclk, dsi_reset;


   /************************************************
    * USB FIFO
    */

   wire [31:0]   packet_data;
   reg           packet_strobe;
   reg [1:0]     packet_type;
   reg [22:0]    packet_payload;

   wire [15:0]   config_addr;
   wire [15:0]   config_data;
   wire          config_strobe;

   usb_comm usbc(mclk, reset,
                 usb_d, usb_rxf_n, usb_txe_n, usb_rd_n, usb_wr_n, usb_oe_n,
                 packet_data, packet_strobe,
                 config_addr, config_data, config_strobe);

   usb_packet_assemble usbpa(packet_data, packet_type, packet_payload);


   /************************************************
    * Oscillator Simulator
    */

   osc_sim dsosc(mclk, reset, config_addr, config_data, config_strobe,
                 clk50, dsi_sysclk);


   /************************************************
    * RAM Bus
    */

   wire [22:0] filter_a;
   wire [15:0] filter_d;
   wire [1:0]  filter_ublb;
   wire        filter_read;
   wire        filter_write;
   wire        filter_addr_latch;
   wire        filter_strobe;
   wire [15:0] nfilter_d;

   ram_sampler ramsam(mclk, reset,
                      ram_a, ram_d, ram_oe, ram_we, ram_ce1_in, ram_ce2,
                      ram_ub, ram_lb, ram_adv, ram_clk,
                      filter_a, filter_d, filter_ublb, filter_read,
                      filter_write, filter_addr_latch, filter_strobe,
                      nfilter_d);

   /*
    * Which cycle are we on in this read/write burst?
    *
    * We need this to check for RAM latency later on, plus
    * we use it to report read burst length back to the host.
    */

   reg [7:0]   burst_cycle;

   always @(posedge mclk or posedge reset)
     if (reset)
       burst_cycle <= 0;
     else if (filter_strobe) begin
        if (filter_addr_latch)
          burst_cycle <= 0;
        else if ((filter_write || filter_read) && !(&burst_cycle))
          burst_cycle <= burst_cycle + 1;
     end


   /************************************************
    * Configuration
    */

   wire [15:0] trace_flags;
   wire        trace_reads = trace_flags[0];
   wire        trace_writes = trace_flags[1];
   wire        trace_any = trace_reads || trace_writes;

   usb_config #(16'h0001) trace_cfg(mclk, reset, config_addr, config_data,
                                    config_strobe, trace_flags);

   wire [15:0] power_flags;

   assign      dsi_reset = power_flags[0] ? 1'b0 : 1'bZ;

   usb_config #(16'h0002) power_cfg(mclk, reset, config_addr, config_data,
                                    config_strobe, power_flags);


   /************************************************
    * Tracing state machine
    */

   parameter READ_LATENCY = 4;
   parameter WRITE_LATENCY = 3;

   reg [22:0] timestamp_counter;

   // Split the timestamp into a piece that we can represent in 5 bits, and the remainder
   wire [4:0] timestamp5 = timestamp_counter > 5'b11111
                           ? 5'b11111 : timestamp_counter[4:0];
   wire [22:0] timestamp5_remainder = timestamp_counter - { 18'b0, timestamp5 };

   always @(posedge mclk or posedge reset)
     if (reset) begin
        timestamp_counter <= 0;
        packet_type <= 0;
        packet_payload <= 0;
        packet_strobe <= 0;
     end
     else if (trace_any && filter_strobe && filter_addr_latch) begin
        /*
         * Send an address packet
         */

        timestamp_counter <= timestamp_counter + 1;
        packet_type <= 2'b00;
        packet_payload <= filter_a;
        packet_strobe <= 1;
     end
     else if (trace_writes && filter_strobe && filter_write &&
              burst_cycle >= (WRITE_LATENCY - 1)) begin
        /*
         * Send a write word packet
         */

        timestamp_counter <= timestamp5_remainder;
        packet_type <= 2'b10;
        packet_payload <= { timestamp5, filter_ublb, filter_d };
        packet_strobe <= 1;
     end
     else if (trace_reads && filter_strobe && filter_read &&
              burst_cycle >= (READ_LATENCY - 1)) begin
        /*
         * Send a read word packet.
         *
         * Note that data is latched on the negative clock edge,
         * but we send the packet on the positive edge and all control
         * signals are sampled at the next positive edge.
         */

        timestamp_counter <= timestamp5_remainder;
        packet_type <= 2'b01;
        packet_payload <= { timestamp5, filter_ublb, nfilter_d };
        packet_strobe <= 1;
     end
     else if (trace_any && filter_strobe &&
              ( (burst_cycle == 1 && timestamp5_remainder) ||
                timestamp_counter[22] )) begin
        /*
         * Our RAM is otherwise idle, and we have a timestamp remainder-
         * send a timestamp packet to sync up the host with our clock.
         *
         * We only do this during the latency clocks in a read or write,
         * not while the RAM is completely idle. If we did it while totally
         * idle, we'd send a packet every 32 clocks, which would just waste
         * space. But we still want every packet to have an accurate timestamp.
         * By sending it during this latency cycle, we end up with an address
         * packet that may have an outdated timestamp, but the end of the burst
         * (which is what matters) will always have an up-to-date timestamp.
         *
         * We'll also send a timestamp packet if the timestamp counter is
         * in danger of overflowing soon.
         */

        timestamp_counter <= 0;
        packet_type <= 2'b11;
        packet_payload <= timestamp_counter;
        packet_strobe <= 1;
     end
     else begin
        /*
         * Idle. Just count cycles.
         */

        if (filter_strobe)
          timestamp_counter <= timestamp_counter + 1;

        packet_type <= 2'bXX;
        packet_payload <= 23'hXXXXXX;
        packet_strobe <= 0;
     end


   /************************************************
    * Injection state machine
    */

   reg ram_ce1_out;  // 0=normal, 1=override RAM

   wire        patch_trigger;
   wire        patch_data_next;
   wire [15:0] patch_data;

   reg  [15:0] ram_d_latch;

   // Drive ram_d any time we're overriding RAM.
   assign      ram_d = ram_ce1_out ? ram_d_latch : 16'hZZZZ;

   /*
    * Patch storage module. We give it addresses, it tells us whether
    * to inject and it provides the data for each word.
    */
   patch_store pstore(mclk, reset,
                      config_addr, config_data, config_strobe,
                      filter_a, filter_strobe && filter_addr_latch,
                      patch_trigger, patch_data, patch_data_next);

   /*
    * Triggering latch. Patch is active when the patch store
    * says it is, goes inactive when the current burst ends.
    */

   reg  patch_active;

   always @(posedge mclk or posedge reset)
     if (reset)
       patch_active <= 0;
     else if (filter_strobe && !filter_read)
       patch_active <= 0;
     else if (patch_trigger)
       patch_active <= 1;

   /*
    * Read burst injection, using the bus state maintained above for tracing.
    *
    * In a patched read cycle. The first actual data word is sent
    * out in time for the cycle at READ_LATENCY. But we disable the
    * real RAM and start driving the bus immediately.
    *
    * In order to make the RAM data available for cycle N, we must write
    * it here when burst_cycle is N-1. Also, the READ_LATENCY is 1-based,
    * like in the data sheet. So we actually need to write the first
    * data word when burst_cycle is READ_LATENCY - 2.
    */

   wire patched_read = patch_active && filter_strobe && filter_read;
   assign patch_data_next = patched_read && (burst_cycle >= (READ_LATENCY - 2));

   always @(posedge mclk or posedge reset)
     if (reset) begin
        ram_ce1_out <= 0;
        ram_d_latch <= 0;
     end
     else if (filter_strobe && !filter_read) begin
        /*
         * End of a read- cancel the burst and give the RAM bus back.
         *
         * XXX: There is a somewhat annoying amount of latency between
         *      when the CPU deasserts OE and when we actually give the
         *      bus back- hopefully this doesn't cause too many problems
         *      with contention if we haven't given the bus back by the
         *      time the CPU starts using it.
         */
        ram_ce1_out <= 0;
        ram_d_latch <= 0;
     end
     else if (patched_read) begin
        ram_ce1_out <= 1;
        ram_d_latch <= patch_data;
     end

endmodule


/*
 * Configurable Oscillator Simulator --
 *
 *   This is a configurable frequency generator. We use a Xilinx DCM to
 *   multiply the incoming clock frequency (to achieve better clock resolution),
 *   then we use an accumulator to divide the clock by a 16-bit fixed-point value.
 *
 *   This oscillator can synthesize frequencies up to 24.9996 MHz, with a
 *   resolution of 381.5 kHz.
 *
 *   Note: mclk is the master system clock, to which config_* is synchronous.
 *         clk50 is the auxiliary clock that we use to drive the oscillator.
 */

module osc_sim(mclk, reset, config_addr, config_data, config_strobe, clk50, osc_out);

   input mclk, reset, clk50;
   output osc_out;

   input [15:0] config_addr;
   input [15:0] config_data;
   input        config_strobe;

   reg [18:0]  accum;
   reg         accum_out_buf;
   reg         osc_out;

   reg [15:0]  cfg_rate_buf1;
   reg [15:0]  cfg_rate_buf2;
   wire [15:0] cfg_rate;

   usb_config #(16'h0000) cfg(mclk, reset, config_addr, config_data,
                              config_strobe, cfg_rate);

   wire clk4x;

   DCM #(.CLKDV_DIVIDE(2.0),
         .CLKFX_DIVIDE(1),
         .CLKFX_MULTIPLY(4),
         .CLKIN_DIVIDE_BY_2("FALSE"),
         .CLKOUT_PHASE_SHIFT("NONE"),
         .CLK_FEEDBACK("NONE")
         ) dcm4x (.CLKFX(clk4x),
                  .CLKIN(clk50),
                  .RST(reset));

   /*
    * Performance is critical here, as this adder will be running at 200 MHz!
    * We double-register the input (cfg_rate) and output (osc_out) in order
    * to help isolate this accumulator from the placement of other logic elements.
    *
    * Also, it's a good idea to double-register the input since we're
    * crossing clock domains.
    */

   always @(posedge clk4x or posedge reset)
     if (reset) begin
        accum <= 0;
        accum_out_buf <= 0;
        osc_out <= 0;
        cfg_rate_buf1 <= 0;
        cfg_rate_buf2 <= 0;
     end
     else begin
        accum <= accum + { 3'b000, cfg_rate_buf2 };
        accum_out_buf <= accum[18];
        osc_out <= accum_out_buf;
        cfg_rate_buf1 <= cfg_rate;
        cfg_rate_buf2 <= cfg_rate_buf1;
     end

endmodule
