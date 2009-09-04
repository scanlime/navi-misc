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


module main(mclk, reset,
            ledseg_c, ledseg_a, led,
            sram_oe, sram_we,
            usb_d, usb_slwr, usb_slrd, usb_pktend,
            usb_sloe, usb_fifoadr, usb_flagb,
            ram_a, ram_d, ram_oe, ram_we, ram_ce1, ram_ce2,
            ram_ub, ram_lb, ram_adv, ram_clk,
            switch, ds_osc_out);

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

   input [0:0]  switch;
   output       ds_osc_out;

   // Disable SRAM, we're using the same pins for USB
   assign sram_oe = 1;
   assign sram_we = 1;


   /************************************************
    * USB FIFO
    */

   wire [31:0]   packet_data;
   reg           packet_strobe;
   wire          err_overflow;

   usb_comm usbc(mclk, reset,
                 usb_d, usb_slwr, usb_slrd, usb_pktend,
                 usb_sloe, usb_fifoadr, usb_flagb,
                 packet_data, packet_strobe,
                 err_overflow);

   reg [1:0]     packet_type;
   reg [22:0]    packet_payload;

   usb_packet_assemble usbpa(packet_data, packet_type, packet_payload);


   /************************************************
    * Oscillator Simulator
    *
    * Switch 0 is a 'turbo' switch.
    */

   osc_sim dsosc(mclk, reset, switch[0], ds_osc_out);


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

   ram_sampler ramsam(mclk, reset,
                      ram_a, ram_d, ram_oe, ram_we, ram_ce1, ram_ce2,
                      ram_ub, ram_lb, ram_adv, ram_clk,
                      filter_a, filter_d, filter_ublb, filter_read,
                      filter_write, filter_addr_latch, filter_strobe);

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
    * Tracing state machine
    */

   parameter S_NORMAL = 0;
   parameter S_READ = 1;

   parameter READ_LATENCY = 4;
   parameter WRITE_LATENCY = 3;

   reg        state;
   reg [22:0] timestamp_counter;
   reg [7:0]  read_checksum;
   reg [1:0]  read_ublb;

   wire [7:0] read_count = burst_cycle - (READ_LATENCY - 1);

   // Split the timestamp into a piece that we can represent in 5 bits, and the remainder
   wire [4:0] timestamp5 = timestamp_counter > 5'b11111
                           ? 5'b11111 : timestamp_counter[4:0];
   wire [22:0] timestamp5_remainder = timestamp_counter - { 18'b0, timestamp5 };

   always @(posedge mclk or posedge reset)
     if (reset) begin
        state <= S_NORMAL;
        timestamp_counter <= 0;
        read_checksum <= 0;
        read_ublb <= 0;
        packet_type <= 0;
        packet_payload <= 0;
        packet_strobe <= 0;
     end
     else if (state == S_READ) begin
        /*
         * We're in the middle of a read burst.
         *
         * As long as the read is still going, count cycles
         * and keep a packet checksum. During the first idle
         * cycle after the read, we'll send a packet.
         */

        if (filter_read) begin
           if (filter_strobe) begin
              timestamp_counter <= timestamp_counter + 1;
              read_checksum <= read_checksum + filter_d[15:8] + filter_d[7:0];
              read_ublb <= read_ublb & filter_ublb;
           end

           state <= S_READ;
           packet_type <= 2'bXX;
           packet_payload <= 8'hXX;
           packet_strobe <= 0;
        end
        else begin
           state <= S_NORMAL;
           timestamp_counter <= timestamp5_remainder;
           read_checksum <= 8'hXX;
           read_ublb <= 2'bXX;
           packet_type <= 2'b01;
           packet_payload <= { timestamp5, read_ublb, read_checksum, read_count };
           packet_strobe <= 1;
        end
     end
     else if (filter_strobe && filter_addr_latch) begin
        /*
         * Send an address packet
         */

        state <= S_NORMAL;
        timestamp_counter <= timestamp_counter + 1;
        read_checksum <= 8'hXX;
        read_ublb <= 2'bXX;
        packet_type <= 2'b00;
        packet_payload <= filter_a;
        packet_strobe <= 1;
     end
     else if (filter_strobe && filter_write && burst_cycle >= (WRITE_LATENCY - 1)) begin
        /*
         * Send a write word packet
         */

        state <= S_NORMAL;
        timestamp_counter <= timestamp5_remainder;
        read_checksum <= 8'hXX;
        read_ublb <= 2'bXX;
        packet_type <= 2'b10;
        packet_payload <= { timestamp5, filter_ublb, filter_d };
        packet_strobe <= 1;
     end
     else if (filter_strobe && filter_read && burst_cycle == (READ_LATENCY - 1)) begin
        /*
         * Begin a read burst
         */

        state <= S_READ;
        timestamp_counter <= timestamp_counter + 1;
        read_checksum <= filter_d[15:8] + filter_d[7:0];
        read_ublb <= filter_ublb;
        packet_type <= 2'bXX;
        packet_payload <= 23'hXXXXXX;
        packet_strobe <= 0;
     end
     else if (filter_strobe && burst_cycle == 1 && timestamp5_remainder) begin
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
         */

        state <= S_NORMAL;
        timestamp_counter <= 0;
        read_checksum <= 8'hXX;
        read_ublb <= 2'bXX;
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

        state <= S_NORMAL;
        read_checksum <= 8'hXX;
        read_ublb <= 2'bXX;
        packet_type <= 2'bXX;
        packet_payload <= 23'hXXXXXX;
        packet_strobe <= 0;
     end


   /************************************************
    * Debug
    */

   /* 8 individual status LEDs */
   pulse_stretcher_arr led_ps(mclk, reset,
                              {filter_read, filter_write,
                               5'b0, err_overflow },
                              led);

   /* LED display: Current memory address, high 16 bits. */
   led_hex display(mclk, reset, filter_a[22:7], ledseg_c, ledseg_a);

endmodule // main


/*
 * DS Oscillator Simulator --
 *
 *   mclk is 48 MHz.
 *
 *   Generates a 1 MHz clock (near the minimum for PLL startup) when turbo=0.
 *   Generates an approximate 16.756 MHz clock (the normal frequency) when turbo=1.
 */

module osc_sim(mclk, reset, turbo, osc_out);

   parameter MCLK_KHZ_DIV2 = 16'd24000;
   parameter SLOW_KHZ = 16'd1000;
   parameter FAST_KHZ = 16'd16756;

   input mclk, reset, turbo;
   output osc_out;
   reg    osc_out;

   reg [15:0] accum;

   wire [15:0] accum_next_fast = accum + FAST_KHZ;
   wire [15:0] accum_next_slow = accum + SLOW_KHZ;
   wire [15:0] accum_next = turbo ? accum_next_fast : accum_next_slow;

   always @(posedge mclk or posedge reset)
     if (reset) begin
        accum <= 0;
        osc_out <= 0;
     end
     else if (accum_next > MCLK_KHZ_DIV2) begin
        accum <= accum_next - MCLK_KHZ_DIV2;
        osc_out <= !osc_out;
     end
     else begin
        accum <= accum_next;
     end
endmodule
