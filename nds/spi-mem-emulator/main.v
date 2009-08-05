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


module main(mclk, reset,
            ledseg_c, ledseg_a, led,
            serial_txd, serial_rxd,
            spi_clk, spi_mosi, spi_miso, spi_cs,
            sram_a, sram_oe, sram_we,
            sram1_io, sram1_ce, sram1_ub, sram1_lb,
            sram2_io, sram2_ce, sram2_ub, sram2_lb);

   input mclk, reset;

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
    * Clock
    */

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
    * High-speed RS-232 Serial Port
    */

   wire         ser_mem_begin_wr;
   wire         ser_mem_begin_rd;
   wire         ser_mem_finish;
   wire [19:0]  ser_mem_addr;
   wire [7:0]   ser_mem_data_wr;
   wire [7:0]   ser_mem_data_rd;

   sio_protocol sio(mclk, reset, serial_txd, serial_rxd,
                    ser_mem_begin_wr, ser_mem_begin_rd, ser_mem_finish,
                    ser_mem_addr, ser_mem_data_wr, ser_mem_data_rd);


   /************************************************
    * SRAM
    */

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

   assign led = { pulse_clk, pulse_cs, 4'b0, pulse_wr, pulse_rd };
   led_hex display(clkdiv[14], reset, ser_mem_addr[15:0], ledseg_c, ledseg_a);

endmodule


/*
 * Serial protocol for reading/writing memory.
 *
 * The protocol is built on top of the packet framing implemented
 * by serial_escape/serial_unescape. Every packet consists of:
 *
 *   1. A flag byte
 *   2. A 4-bit command (in the high nybble)
 *   3. A 20-bit address (big endian)
 *   4. Any number of data bytes.
 *
 * Addresses auto-increment after every data byte.
 *
 * All commands send a reply which begins with a copy of
 * the packet header from the command. Data in the reply
 * varies depending on the command type.
 *
 * Currently there is no buffering of commands
 * or responses. Any new command you send will
 * interrupt a command that the device is already
 * processing. This means you need to wait for one
 * command to fully complete before sending the next.
 *
 * Commands:
 *
 *  0: Ping.
 *     Does nothing except send a response header.
 *
 *  1: Write memory.
 *     All data bytes are written to SRAM at the
 *     corresponding addresses. No reply data.
 *     Keeps writing until the next received flag byte.
 *
 *  2: Read 16 bytes.
 *
 *  3: Read 1024 bytes.
 *
 *  4: Read 64 kB
 */

module sio_protocol(mclk, reset, txd, rxd,
                    mem_begin_wr, mem_begin_rd, mem_finish,
                    mem_addr, mem_data_wr, mem_data_rd);

   input mclk, reset;

   output txd;
   input  rxd;

   output        mem_begin_wr, mem_begin_rd;
   input         mem_finish;
   output [19:0] mem_addr;
   output [7:0]  mem_data_wr;
   input [7:0]   mem_data_rd;

   wire          tx_ready;
   reg [7:0]     tx_data;
   reg           tx_data_strobe;
   reg           tx_flag;

   wire [7:0]    rx_data;
   wire          rx_data_strobe;
   wire          rx_flag;

   serial_escaped_uart uart(mclk, reset, txd, rxd,
                            tx_ready, tx_data, tx_data_strobe, tx_flag,
                            rx_data, rx_data_strobe, rx_flag);

   /*
    * Command packet receive state machine
    */

   parameter RX_S_ERROR   = 0;
   parameter RX_S_COMMAND = 1;
   parameter RX_S_ADDR_2  = 3;
   parameter RX_S_ADDR_3  = 4;
   parameter RX_S_DATA    = 5;

   reg [2:0]     rx_state;
   reg [3:0]     cmd_id;       // Current/last command
   reg [19:0]    cmd_address;  // Address being received / address of RX data

   parameter CMD_PING      = 0;
   parameter CMD_WRITE     = 1;
   parameter CMD_READ_16   = 2;
   parameter CMD_READ_1K   = 3;
   parameter CMD_READ_64K  = 4;

   wire          cmd_is_write   = (cmd_id == CMD_WRITE);
   wire          cmd_is_read    = (cmd_id == CMD_READ_16 ||
                                   cmd_id == CMD_READ_1K ||
                                   cmd_id == CMD_READ_64K);
   wire [16:0]   cmd_reply_len  = (cmd_id == CMD_READ_16 ? 17'h10 :
                                   cmd_id == CMD_READ_1K ? 17'h400 :
                                   cmd_id == CMD_READ_64K ? 17'h10000 :
                                   17'h0);

   wire [7:0]    cmd_reply_data = mem_data_rd;

   /* Note: cmd_address isn't ready until the clock cycle after cmd_header_strobe */
   wire          cmd_header_strobe = (rx_state == RX_S_ADDR_3 && rx_data_strobe);
   wire          cmd_data_strobe = (rx_state == RX_S_DATA && rx_data_strobe);

   always @(posedge mclk or posedge reset)
     if (reset) begin
        rx_state <= RX_S_ERROR;
        cmd_id <= 0;
        cmd_address <= 0;
     end
     else if (rx_flag) begin
        /* Flag bytes can occur at any time */
        rx_state <= RX_S_COMMAND;
        cmd_id <= 4'hX;
        cmd_address <= 20'hXXXXX;
     end
     else if (rx_data_strobe)
       case (rx_state)

         RX_S_ERROR: begin
            rx_state <= RX_S_ERROR;
            cmd_id <= 4'hX;
            cmd_address <= 20'hXXXXX;
         end

         RX_S_COMMAND: begin
            rx_state <= RX_S_ADDR_2;
            cmd_id <= rx_data[7:4];
            cmd_address[19:16] <= rx_data[3:0];
         end

         RX_S_ADDR_2: begin
            rx_state <= RX_S_ADDR_3;
            cmd_address[15:8] <= rx_data;
         end

         RX_S_ADDR_3: begin
            rx_state <= RX_S_DATA;
            cmd_address[7:0] <= rx_data;
         end

         RX_S_DATA: begin
            rx_state <= RX_S_DATA;
            cmd_address <= cmd_address + 1;
         end

       endcase

   /*
    * Packet transmit state machine
    */

   parameter TX_S_IDLE    = 0;
   parameter TX_S_FLAG    = 1;
   parameter TX_S_COMMAND = 2;
   parameter TX_S_ADDR_2  = 3;
   parameter TX_S_ADDR_3  = 4;
   parameter TX_S_DATA    = 5;

   reg [2:0]  tx_state;
   reg [16:0] tx_reply_len;
   reg [19:0] tx_address;     // Address of TX data

   always @(posedge mclk or posedge reset)
     if (reset) begin
        tx_state <= TX_S_IDLE;
        tx_data <= 0;
        tx_data_strobe <= 0;
        tx_flag <= 0;
     end
     else if (cmd_header_strobe) begin
        /* Wait until transmitter is ready for flag */
        tx_state <= TX_S_FLAG;
        tx_data <= 8'hXX;
        tx_data_strobe <= 0;
        tx_flag <= 0;
        tx_reply_len <= cmd_reply_len;
        tx_address <= cmd_address;
     end
     else if (tx_ready && !tx_data_strobe && !tx_flag)
        case (tx_state)

          TX_S_IDLE: begin
             tx_state <= TX_S_IDLE;
             tx_data <= 8'hXX;
             tx_data_strobe <= 0;
             tx_flag <= 0;
          end

          TX_S_FLAG: begin
             tx_state <= TX_S_COMMAND;
             tx_data <= 8'hXX;
             tx_data_strobe <= 0;
             tx_flag <= 1;
          end

          TX_S_COMMAND: begin
             tx_state <= TX_S_ADDR_2;
             tx_data <= { cmd_id, cmd_address[19:16] };
             tx_data_strobe <= 1;
             tx_flag <= 0;
          end

          TX_S_ADDR_2: begin
             tx_state <= TX_S_ADDR_3;
             tx_data <= cmd_address[15:8];
             tx_data_strobe <= 1;
             tx_flag <= 0;
          end

          TX_S_ADDR_3: begin
             tx_state <= TX_S_DATA;
             tx_data <= cmd_address[7:0];
             tx_data_strobe <= 1;
             tx_flag <= 0;
          end

          TX_S_DATA: begin
             if (tx_reply_len) begin
                tx_state <= TX_S_DATA;
                tx_data <= cmd_reply_data;
                tx_data_strobe <= 1;
                tx_flag <= 0;
                tx_reply_len <= tx_reply_len - 1;
                tx_address <= tx_address + 1;
             end
             else begin
                tx_state <= TX_S_DATA;
                tx_data <= 8'hXX;
                tx_data_strobe <= 0;
                tx_flag <= 0;
                tx_reply_len <= 0;
             end
          end

        endcase
     else begin
        tx_data <= 8'hXX;
        tx_data_strobe <= 0;
        tx_flag <= 0;
     end

   /*
    * Memory control state machine
    */

   assign mem_addr = tx_address;
   assign mem_begin_rd = cmd_is_read && (cmd_header_strobe || cmd_data_strobe);
   
   parameter 

   parameter TX_S_IDLE    = 0;
   parameter TX_S_FLAG    = 1;
   parameter TX_S_COMMAND = 2;
   parameter TX_S_ADDR_2  = 3;
   parameter TX_S_ADDR_3  = 4;
   parameter TX_S_DATA    = 5;
   
   reg           mem_begin_wr;
   reg [7:0]     mem_data_wr;

endmodule
