 /*
  * sram.v - SRAM control. The Digilent Spartan 3 board has 2x 256x16 SRAM chips.
  *          So we have a total of 18 address bits and 32 data bits. This module
  *          is a memory controller which coordinates byte-wide reads and writes
  *          from two clients.
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


module sram_ctrl(clk, reset,
                 sram_a, sram_oe, sram_we,
                 sram1_io, sram1_ce, sram1_ub, sram1_lb,
                 sram2_io, sram2_ce, sram2_ub, sram2_lb,
                 c1_begin_wr, c1_begin_rd, c1_finish,
                 c1_addr, c1_data_wr, c1_data_rd,
                 c2_begin_wr, c2_begin_rd, c2_finish,
                 c2_addr, c2_data_wr, c2_data_rd);

   input clk, reset;

   output [17:0] sram_a;
   output        sram_oe, sram_we;
   output        sram1_ce, sram1_ub, sram1_lb;
   output        sram2_ce, sram2_ub, sram2_lb;
   inout [15:0]  sram1_io;
   inout [15:0]  sram2_io;

   input         c1_begin_wr, c1_begin_rd;
   output        c1_finish;
   input [19:0]  c1_addr;
   input [7:0]   c1_data_wr;
   output [7:0]  c1_data_rd;

   input         c2_begin_wr, c2_begin_rd;
   output        c2_finish;
   input [19:0]  c2_addr;
   input [7:0]   c2_data_wr;
   output [7:0]  c2_data_rd;


   /************************************************
    * SRAM Control Signals
    */

   assign sram1_ub = 0;
   assign sram1_lb = 0;
   assign sram2_ub = 0;
   assign sram2_lb = 0;

   /*
    * Share one output register for both SRAM chips.
    */
   reg [15:0]    sram_out;
   reg           sram_out_drv;
   assign sram1_io = sram_out_drv ? sram_out : 16'hZZZZ;
   assign sram2_io = sram_out_drv ? sram_out : 16'hZZZZ;

   /*
    * Control signal latches.
    *
    * For OE and WE, we un-invert them for our sanity.
    * The state machine only has to deal with positive logic.
    *
    * These inverters should also add a tiny bit of
    * propagation delay relative to sram_a and sram_out.
    */
   reg           oe;
   reg           we;
   assign sram_oe = !oe;
   assign sram_we = !we;

   /*
    * Internal byte address latch.
    * Set (active low) chip enables based on address bit 1.
    */
   reg [19:0]    addr;
   wire [1:0]    byte_index = addr[1:0];
   assign        sram_a = addr[19:2];
   assign        sram1_ce = byte_index[1];
   assign        sram2_ce = !byte_index[1];


   /************************************************
    * Request latches
    */

   /*
    * When a request comes in, remember it until we finish.
    */

   wire          c1_pending_rd, c1_pending_wr;
   wire          c2_pending_rd, c2_pending_wr;
   wire          c1_finish_rd, c1_finish_wr;
   wire          c2_finish_rd, c2_finish_wr;

   set_reset_flipflop c1_rd_ff(clk, reset, c1_begin_rd, c1_finish_rd, c1_pending_rd);
   set_reset_flipflop c1_wr_ff(clk, reset, c1_begin_wr, c1_finish_wr, c1_pending_wr);
   set_reset_flipflop c2_rd_ff(clk, reset, c2_begin_rd, c2_finish_rd, c2_pending_rd);
   set_reset_flipflop c2_wr_ff(clk, reset, c2_begin_wr, c2_finish_wr, c2_pending_wr);

   assign c1_finish = c1_finish_rd;
   assign c2_finish = c2_finish_rd;

   /*
    * Responses to reads: Select the proper byte
    */
   wire [15:0]   word_rd = byte_index[1] ? sram2_io : sram1_io;
   wire [7:0]    byte_rd = byte_index[0] ? word_rd[15:8] : word_rd[7:0];
   assign c1_data_rd = byte_rd;
   assign c2_data_rd = byte_rd;


   /************************************************
    * State Machine
    */

   /*
    * Reads happen in one cycle.
    *
    * Writes take multiple steps:
    *   - Read back the 16-bit word
    *   - Output the modified 16-bit word
    *   - Strobe WE
    */

   parameter S_IDLE   = 0;
   parameter S_C1_RD  = 1;
   parameter S_C1_WR  = 2;
   parameter S_C1_WR2 = 3;
   parameter S_C1_WR3 = 4;
   parameter S_C2_RD  = 5;
   parameter S_C2_WR  = 6;
   parameter S_C2_WR2 = 7;
   parameter S_C2_WR3 = 8;

   reg [3:0]     state;

   assign c1_finish_rd = (state == S_C1_RD);
   assign c1_finish_wr = (state == S_C1_WR3);
   assign c2_finish_rd = (state == S_C2_RD);
   assign c2_finish_wr = (state == S_C2_WR3);

   always @(posedge clk or posedge reset)
     if (reset) begin
        state <= S_IDLE;
        sram_out <= 0;
        sram_out_drv <= 0;
        addr <= 0;
        oe <= 0;
        we <= 0;
     end

     else case (state)

            S_IDLE: begin
               /*
                * Prep address for read or write
                */
               if (c1_pending_rd || c1_pending_wr)
                 addr <= c1_addr;
               else if (c2_pending_rd || c2_pending_wr)
                 addr <= c2_addr;
               else
                 addr <= 20'hXXXXX;

               /*
                * Both read and write cycles start with a read.
                */
               oe <= (c1_pending_wr || c1_pending_rd ||
                      c2_pending_wr || c2_pending_rd);

               state <= (c1_pending_rd ? S_C1_RD :
                         c1_pending_wr ? S_C1_WR :
                         c2_pending_rd ? S_C2_RD :
                         c2_pending_wr ? S_C2_WR :
                         S_IDLE);

               sram_out <= 16'hXXXX;
               sram_out_drv <= 0;
               we <= 0;
            end // case: S_IDLE

            /*
             * Read cycles.
             *
             * End the read. Data will be delivered to
             * clients via the 'finish' signals above.
             */

            S_C1_RD: begin
               oe <= 0;
               state <= S_IDLE;
               sram_out <= 16'hXXXX;
               sram_out_drv <= 0;
               we <= 0;
            end

            S_C2_RD: begin
               oe <= 0;
               state <= S_IDLE;
               sram_out <= 16'hXXXX;
               sram_out_drv <= 0;
               we <= 0;
            end

            /*
             * First write cycle:
             *
             * The read is done. Substitute in our new data, and
             * drive it onto the bus.
             */

            S_C1_WR: begin
               oe <= 0;
               state <= S_C1_WR2;
               sram_out_drv <= 1;
               we <= 0;
               if (byte_index[0])
                 sram_out <= { c1_data_wr, word_rd[7:0] };
               else
                 sram_out <= { word_rd[15:8], c1_data_wr };
            end

            S_C2_WR: begin
               oe <= 0;
               state <= S_C2_WR2;
               sram_out_drv <= 1;
               we <= 0;
               if (byte_index[0])
                 sram_out <= { c2_data_wr, word_rd[7:0] };
               else
                 sram_out <= { word_rd[15:8], c2_data_wr };
            end

            /*
             * Second write cycle:
             *
             * Strobe the WE line high.
             */

            S_C1_WR2: begin
               oe <= 0;
               state <= S_C1_WR3;
               sram_out_drv <= 1;
               we <= 1;
            end

            S_C2_WR2: begin
               oe <= 0;
               state <= S_C2_WR3;
               sram_out_drv <= 1;
               we <= 1;
            end

            /*
             * Third write cycle:
             *
             * Keep the data stable, but bring WE low.
             */

            S_C1_WR3: begin
               oe <= 0;
               state <= S_IDLE;
               sram_out_drv <= 1;
               we <= 0;
            end

            S_C2_WR3: begin
               oe <= 0;
               state <= S_IDLE;
               sram_out_drv <= 1;
               we <= 0;
            end

          endcase

endmodule
