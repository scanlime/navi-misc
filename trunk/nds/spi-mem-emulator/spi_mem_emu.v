/*
 * spi_mem_emu.v - Module for an SPI memory emulator, compatible with
 *                 ST Microelectronics SPI EEPROMs.
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


module spi_mem_emu(mclk, reset,
                   spi_clk, spi_mosi, spi_miso, spi_cs,
                   mem_begin_wr, mem_begin_rd, mem_finish,
                   mem_addr, mem_data_wr, mem_data_rd);

   input mclk, reset;

   /*
    * SPI bus with tri-state MISO pin. CS is active-low.
    * We're emulating a slave device.
    */
   input spi_clk, spi_mosi, spi_cs;
   inout spi_miso;

   /*
    * Memory interface. We pulse mem_begin_rd or mem_begin_wr
    * to begin a memory read or write. The mem_addr is latched
    * on the same clock cycle, and is valid throughout the duration
    * of the memory operation.
    *
    * For writes, the data is also immediately available on mem_data_wr.
    *
    * For a read, mem_finish input must be pulsed for one clock cycle.
    * On the same cycle, mem_data_rd must contain valid data.
    */
   output mem_begin_wr, mem_begin_rd;
   input  mem_finish;
   output [15:0] mem_addr;
   output [7:0]  mem_data_wr;
   input [7:0]   mem_data_rd;


   /************************************************
    * SPI Input/Output
    */

   wire          spi_clk_sync, spi_mosi_sync, spi_cs_sync;
   wire          spi_clk_prev;
   wire          spi_clk_posedge = spi_clk_sync && !spi_clk_prev;
   wire          spi_clk_negedge = !spi_clk_sync && spi_clk_prev;
   reg           spi_miso_out;

   /* Input sync */
   d_flipflop_pair spi_dff_clk(mclk, reset, spi_clk, spi_clk_sync);
   d_flipflop_pair spi_dff_miso(mclk, reset, spi_mosi, spi_mosi_sync);
   d_flipflop_pair spi_dff_cs(mclk, reset, spi_cs, spi_cs_sync);

   /* For clock edge detection */
   d_flipflop spi_dff_clk_2(mclk, reset, spi_clk_sync, spi_clk_prev);

   /* Tri-state output buffer. Use non-sync'ed CS. */
   assign spi_miso = spi_cs ? 1'bZ : spi_miso_out;


   /************************************************
    * Shift register
    */

   reg [2:0]     bit_count;
   reg [7:0]     miso_reg;
   reg [7:0]     mosi_reg;
   reg           byte_strobe;

   reg           miso_load_strobe;
   reg [7:0]     miso_load_data;

   always @(posedge mclk or posedge reset)
     /*
      * Master reset or chip deselect: Reset everything.
      */
     if (reset) begin
        bit_count <= 0;
        mosi_reg <= 0;
        miso_reg <= 8'hFF;
        spi_miso_out <= 1;
        byte_strobe <= 0;
     end
     else if (spi_cs_sync) begin
        bit_count <= 0;
        mosi_reg <= 0;
        miso_reg <= 8'hFF;
        spi_miso_out <= 1;
        byte_strobe <= 0;
     end

     /*
      * Clock edges: Shift in and increment bit_count on positive
      * edges, shift out on negative edges.
      *
      * Our per-byte state machine begins immediately after the last
      * positive edge in the byte (bit_count == 7), and it must
      * provide a new result to mosi_reg before the next negative edge.
      */
     else if (spi_clk_posedge) begin
        bit_count <= bit_count + 1;
        mosi_reg <= {mosi_reg[6:0], spi_mosi_sync};
        byte_strobe <= bit_count == 7;
     end
     else if (spi_clk_negedge) begin
        miso_reg <= {miso_reg[6:0], 1'b1};
        spi_miso_out <= miso_reg[7];
        byte_strobe <= 0;
     end
     else if (miso_load_strobe) begin
        /* Must occur before the next SPI clock edge */
        miso_reg <= miso_load_data;
        byte_strobe <= 0;
     end
     else begin
        byte_strobe <= 0;
     end


   /************************************************
    * Byte state machine
    */

   /*
    * Instruction set
    */
   parameter I_WRSR  = 8'h01;  // Write status register
   parameter I_WRITE = 8'h02;  // Write memory
   parameter I_READ  = 8'h03;  // Read memory
   parameter I_WRDI  = 8'h04;  // Write disable
   parameter I_RDSR  = 8'h05;  // Read status register
   parameter I_WREN  = 8'h06;  // Write enable

   /*
    * States
    */
   parameter S_WAIT_FOR_CMD     = 4'h0;
   parameter S_CMD_WRITE_ADDR1  = 4'h2;
   parameter S_CMD_READ_ADDR1   = 4'h3;
   parameter S_CMD_RDSR         = 4'h5;
   parameter S_DONE             = 4'h7;
   parameter S_CMD_WRITE_ADDR2  = 4'h8;
   parameter S_CMD_WRITE_DATA   = 4'h9;
   parameter S_CMD_READ_ADDR2   = 4'hA;
   parameter S_CMD_READ_DATA    = 4'hB;

   reg [3:0]    state;
   reg          mem_begin_wr, mem_begin_rd;
   reg [15:0]   mem_addr;
   reg [7:0]    mem_data_wr;
   reg          write_enable;

   wire [7:0]   status_reg = write_enable ? 8'b0000_0010 : 8'b0000_0000;

   always @(posedge mclk or posedge reset)
     /*
      * Master reset or chip deselect: Reset everything.
      */
     if (reset) begin
        state <= S_WAIT_FOR_CMD;
        miso_load_strobe <= 1;
        miso_load_data <= 8'hFF;
        mem_begin_wr <= 0;
        mem_begin_rd <= 0;
        mem_addr <= 0;
        mem_data_wr <= 0;
        write_enable <= 0;
     end
     else if (spi_cs_sync) begin
        state <= S_WAIT_FOR_CMD;
        miso_load_strobe <= 1;
        miso_load_data <= 8'hFF;
        mem_begin_wr <= 0;
        mem_begin_rd <= 0;
        mem_data_wr <= 0;
     end

     else case (state)

            S_DONE: begin
               /*
                * Stay here until CS goes high.
                */
               state <= S_DONE;
               miso_load_strobe <= 1;
               miso_load_data <= 8'hFF;
               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            S_WAIT_FOR_CMD: begin
               /*
                * Next byte begins a new command
                */
               if (byte_strobe) begin
                  state <= (mosi_reg == I_WRSR  ? S_DONE :        // Ignore status write
                            mosi_reg == I_WRITE ? S_CMD_WRITE_ADDR1 :
                            mosi_reg == I_READ  ? S_CMD_READ_ADDR1 :
                            mosi_reg == I_WRDI  ? S_DONE :
                            mosi_reg == I_RDSR  ? S_CMD_RDSR :
                            mosi_reg == I_WREN  ? S_DONE :
                            S_DONE);                              // Unsupported command
               end
               else begin
                  state <= S_WAIT_FOR_CMD;
               end

               /*
                * Write enable/disable
                */
               if (byte_strobe && mosi_reg == I_WREN)
                 write_enable <= 1;
               else if (byte_strobe && mosi_reg == I_WRDI)
                 write_enable <= 0;

               /*
                * If we're starting a RDWR, we need to make the
                * status register available immediately. All other commands
                * start with at least another write byte.
                */
               if (byte_strobe && mosi_reg == I_RDSR) begin
                  miso_load_strobe <= 1;
                  miso_load_data <= status_reg;
               end
               else begin
                  miso_load_strobe <= 0;
                  miso_load_data <= 8'hXX;
               end

               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            S_CMD_RDSR: begin
               /*
                * If we're reading a second byte, send another copy of the status reg.
                */
               if (byte_strobe) begin
                  miso_load_strobe <= 1;
                  miso_load_data <= status_reg;
               end
               else begin
                  miso_load_strobe <= 0;
                  miso_load_data <= 8'hXX;
               end

               state <= S_CMD_RDSR;
               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            /*
             * Address states
             */

            S_CMD_WRITE_ADDR1: begin
               if (byte_strobe) begin
                  mem_addr[15:8] <= mosi_reg;
                  state <= S_CMD_WRITE_ADDR2;
               end

               miso_load_strobe <= 0;
               miso_load_data <= 8'hXX;
               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            S_CMD_WRITE_ADDR2: begin
               if (byte_strobe) begin
                  /*
                   * We increment our write address just before
                   * asserting mem_begin_wr. To compensate for this
                   * on the first written byte, subtract one from the
                   * address.
                   */
                  mem_addr <= { mem_addr[15:8], mosi_reg } - 16'h1;
                  state <= S_CMD_WRITE_DATA;
               end

               miso_load_strobe <= 0;
               miso_load_data <= 8'hXX;
               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            S_CMD_READ_ADDR1: begin
               if (byte_strobe) begin
                  mem_addr[15:8] <= mosi_reg;
                  state <= S_CMD_READ_ADDR2;
               end

               miso_load_strobe <= 0;
               miso_load_data <= 8'hXX;
               mem_begin_wr <= 0;
               mem_begin_rd <= 0;
               mem_data_wr <= 8'hXX;
            end

            S_CMD_READ_ADDR2: begin
               if (byte_strobe) begin
                  /*
                   * Begin first read
                   */
                  mem_addr[7:0] <= mosi_reg;
                  mem_begin_rd <= 1;
                  state <= S_CMD_READ_DATA;
               end
               else begin
                  mem_begin_rd <= 0;
               end

               miso_load_strobe <= 0;
               miso_load_data <= 8'hXX;
               mem_begin_wr <= 0;
               mem_data_wr <= 8'hXX;
            end

            /*
             * Data states
             */

            S_CMD_WRITE_DATA: begin
               if (byte_strobe) begin
                  /*
                   * Pre-increment address on write
                   */
                  mem_addr <= mem_addr + 1;
                  mem_begin_wr <= 1;
                  mem_data_wr <= mosi_reg;
               end
               else begin
                  mem_begin_wr <= 0;
               end

               miso_load_strobe <= 0;
               miso_load_data <= 8'hXX;
               mem_begin_rd <= 0;
            end

            S_CMD_READ_DATA: begin
               if (mem_finish) begin
                  miso_load_strobe <= 1;
                  miso_load_data <= mem_data_rd;
               end
               else begin
                  miso_load_strobe <= 0;
                  miso_load_data <= 8'hXX;
               end

               if (byte_strobe) begin
                  mem_addr <= mem_addr + 1;
                  mem_begin_rd <= 1;
               end
               else begin
                  mem_begin_rd <= 0;
               end

               mem_begin_wr <= 0;
               mem_data_wr <= 8'hXX;
            end

          endcase

endmodule
