 /*
  * main.v - Top-level design for a SPI EEPROM emulator,
  *          targeted at the Digilent Spartan 3 Starter Board.
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


module main(mclk, switch, button,
            ledseg_c, ledseg_a, led,
            serial_txd, serial_rxd,
            spi_clk, spi_mosi, spi_miso, spi_cs,
            sram_a, sram_oe, sram_we,
            sram1_io, sram1_ce, sram1_ub, sram1_lb,
            sram2_io, sram2_ce, sram2_ub, sram2_lb);

   input mclk;
   input [7:0] switch;
   input [3:0] button;

   output [7:0] ledseg_c;
   output [3:0] ledseg_a;
   output [7:0] led;

   output       serial_txd;
   input        serial_rxd;

   input        spi_clk, spi_mosi, spi_cs;
   inout        spi_miso;

   output [17:0] sram_a;
   output        sram_oe, sram_we;
   output        sram1_ce, sram1_ub, sram1_lb;
   output        sram2_ce, sram2_ub, sram2_lb;
   inout [15:0]  sram1_io;
   inout [15:0]  sram2_io;


   /************************************************
    * Clock and Reset
    */

   wire         reset = button[0];

   reg [14:0]   clkdiv;
   always @(posedge mclk or posedge reset)
     if (reset)
       clkdiv <= 0;
     else
       clkdiv <= clkdiv + 1;


   /************************************************
    * SPI
    */

   wire         spi_mem_begin_wr, spi_mem_begin_rd, spi_mem_finish;
   wire [15:0]  spi_mem_addr;
   wire [7:0]   spi_mem_data_wr;
   wire [7:0]   spi_mem_data_rd;

   spi_mem_emu spi(mclk, reset,
                   spi_clk, spi_mosi, spi_miso, spi_cs,
                   spi_mem_begin_wr, spi_mem_begin_rd, spi_mem_finish,
                   spi_mem_addr, spi_mem_data_wr, spi_mem_data_rd);


   /************************************************
    * RS-232 Serial Port
    */

   wire         sio_ce, sio_ce_x4;
   wire         sio_rts_o;
   wire         sio_cts_i = 0;
   wire         sio_txfull, sio_rxempty;
   wire [7:0]   sio_rxd;
   reg [7:0]    sio_txd;
   reg          sio_rxe, sio_txe;

   /*
    * 115200 baud with a 50 MHz master clock:
    *    Baud x4 clock is 460.8 kHz.
    *    50 MHz / 460.8 kHz = 108.507 ~ 4*27
    */
   sasc_brg sio_brg(mclk, !reset, 8'd2, 8'd27, sio_ce, sio_ce_x4);

   sasc_top sio_uart(mclk, !reset, serial_rxd, serial_txd,
                     sio_cts_i, sio_rts_o, sio_ce, sio_ce_x4,
                     sio_txd, sio_rxd, sio_rxe, sio_txe,
                     sio_txfull, sio_rxempty);

   parameter S_IDLE = 0;
   parameter S_RX_BYTE = 1;
   reg       state;

   /*
    * XXX TODO:
    *   - Protocol for reading/writing memory
    *   - Move to separate module
    */

   always @(posedge mclk or posedge reset)
     if (reset) begin
        state <= S_IDLE;
        sio_txd <= 0;
        sio_rxe <= 0;
        sio_txe <= 0;
     end
     else case (state)

            S_IDLE: begin
               if (!sio_rxempty) begin
                  state <= S_RX_BYTE;
                  sio_txd <= 0;
                  sio_rxe <= 1;
                  sio_txe <= 0;
               end
               else begin
                  state <= S_IDLE;
                  sio_txd <= 0;
                  sio_rxe <= 0;
                  sio_txe <= 0;
               end
            end

            S_RX_BYTE: begin
               state <= S_IDLE;
               sio_txd <= sio_rxd + 1;
               sio_rxe <= 0;
               sio_txe <= 1;
            end

          endcase


   /************************************************
    * SRAM
    */

   wire         ser_mem_begin_wr = 0;
   wire         ser_mem_begin_rd = 0;
   wire         ser_mem_finish;
   wire [19:0]  ser_mem_addr = 0;
   wire [7:0]   ser_mem_data_wr = 0;
   wire [7:0]   ser_mem_data_rd;

   sram_ctrl sram(mclk, reset,
                  sram_a, sram_oe, sram_we,
                  sram1_io, sram1_ce, sram1_ub, sram1_lb,
                  sram2_io, sram2_ce, sram2_ub, sram2_lb,

                  spi_mem_begin_wr, spi_mem_begin_rd, spi_mem_finish,
                  { 4'b0000, spi_mem_addr }, spi_mem_data_wr, spi_mem_data_rd,

                  ser_mem_begin_wr, ser_mem_begin_rd, ser_mem_finish,
                  ser_mem_addr, ser_mem_data_wr, ser_mem_data_rd);


   /************************************************
    * Debug
    */

   wire         pulse_clk, pulse_wr, pulse_rd;
   pulse_stretcher s1(mclk, reset, !spi_cs, pulse_cs);
   pulse_stretcher s2(mclk, reset, !spi_clk, pulse_clk);
   pulse_stretcher s3(mclk, reset, spi_mem_begin_rd, pulse_rd);
   pulse_stretcher s4(mclk, reset, spi_mem_begin_wr, pulse_wr);

   assign led = { pulse_clk, pulse_cs,
                  sio_rxe, sio_txe, sio_txfull, sio_rxempty,
                  pulse_wr, pulse_rd };
   led_hex display(clkdiv[14], reset, spi_mem_addr, ledseg_c, ledseg_a);

endmodule
