 /*
  * serial.v - High-speed serial support. Includes a baud generator, UART,
  *            and a simple PPP-derived packet framing protocol.
  *
  *            This module is designed for a 50 MHz clock, and a 3 Mbaud
  *            serial port. This is the highest data rate supported by
  *            the popular FT232 USB-to-serial chip.
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
 * Baud rate generator
 *
 * This generates a pulse at 4x our baud rate, or 12 MHz.
 * Since 12 MHz doesn't divide evenly into our master clock
 * frequency, this needs to use an accumulator to multiply
 * our clock by a ratio of 6/25.
 *
 * We also generate an additional pulse at 1x the baud rate.
 */

module serial_brg(mclk, reset, baud_x4, baud_x1);

   input mclk, reset;
   output baud_x4, baud_x1;

   reg [4:0] accumulator;
   reg [1:0] div4;
   reg       baud_x4;
   reg       baud_x1;

   /* Ratio: add 6, subtract 25 */
   wire [4:0] accum_p = accumulator + 5'd6;
   wire [5:0] accum_m = {1'b0, accum_p} - 6'd25;

   wire       underflow = accum_m[5];
   wire [4:0] accum_next = underflow ? accum_p : accum_m[4:0];

   always @(posedge mclk or posedge reset)
      if (reset) begin
         accumulator <= 0;
         baud_x4 <= 0;
      end
      else begin
         accumulator <= accum_next;
         baud_x4 <= !underflow;
      end

   /*
    * Divide by 4
    */
   always @(posedge mclk or posedge reset)
      if (reset) begin
         div4 <= 0;
         baud_x1 <= 0;
      end
      else if (baud_x4) begin
         baud_x1 <= &div4;
         div4 <= div4 + 1;
      end
      else begin
         baud_x1 <= 0;
      end

endmodule

module serial_brg_testbench;
   /*
    * Test bench: Run the BRG, measure its average pulse frequency.
    *
    * iverilog serial.v util.v -s serial_brg_testbench -o brg.vvp && vvp brg.vvp
    *
    * Should converge to X4 at 12 MHz, X1 at 3 MHz.
    */

   reg clock;
   reg reset;
   wire baud_x1, baud_x4;
   integer ticks, x1_count, x4_count;
   parameter CLK_MHZ = 50.0;

   serial_brg brg(clock, reset, baud_x4, baud_x1);

   initial begin
      clock = 0;
      reset = 1;
      ticks = 0;
      x1_count = 0;
      x4_count = 0;

      #1 reset = 0;
      #100000 $finish;
   end

   always begin
      #5 clock = !clock;
      if (clock) begin
         ticks <= ticks + 1;

         if (baud_x1)
           x1_count <= x1_count + 1;
         if (baud_x4)
           x4_count <= x4_count + 1;

         if (ticks % 100 == 0)
           $display ("%t -- baud_x4 %f MHz -- baud_x1 %f MHz",
                     $time,
                     CLK_MHZ * x4_count / ticks,
                     CLK_MHZ * x1_count / ticks);
      end
   end
endmodule


/*
 * Byte transmitter, RS-232 8-N-1
 *
 * Transmits on 'serial'. When 'ready' goes high, we can accept another byte.
 * It should be supplied on 'data' with a pulse on 'data_strobe'.
 */

module serial_uart_tx(mclk, reset, baud_x1,
                      serial, ready, data, data_strobe);

   input mclk, reset, baud_x1, data_strobe;
   input [7:0] data;
   output serial, ready;

   /*
    * Left-to-right shift register.
    * Loaded with data, start bit, and stop bit.
    *
    * The stop bit doubles as a flag to tell us whether data has been
    * loaded; we initialize the whole shift register to zero on reset,
    * and when the register goes zero again, it's ready for more data.
    */
   reg [9:0]   shiftreg;
   assign      ready = (shiftreg == 0);

   /*
    * Serial output register. This is like an extension of the
    * shift register, but we never load it separately. This gives
    * us one bit period of latency to prepare the next byte.
    *
    * This register is inverted, so we can give it a reset value
    * of zero and still keep the 'serial' output high when idle.
    */
   reg         serial_r;
   assign      serial = !serial_r;

   /*
    * State machine
    */

   always @(posedge mclk or posedge reset)
     if (reset) begin
        shiftreg <= 0;
        serial_r <= 0;
     end
     else if (data_strobe) begin
        shiftreg <= {1'b1, data, 1'b0};
     end
     else if (baud_x1) begin
        shiftreg <= {1'b0, shiftreg[9:1]};
        if (ready)
          /* Idle state */
          serial_r <= 0;
        else
          serial_r <= !shiftreg[0];
     end
endmodule

module serial_uart_tx_testbench;
   /*
    * Test bench: Transmit a byte.
    *
    * iverilog serial.v util.v -s serial_uart_tx_testbench -o brg.vvp && vvp brg.vvp
    */

   reg clock;
   reg reset;

   wire baud_x1, baud_x4;
   wire serial, ready;
   integer bit_tick;
   reg  strobe;
   reg [7:0] data;

   serial_brg brg(clock, reset, baud_x4, baud_x1);
   serial_uart_tx uart(clock, reset, baud_x1, serial, ready, data, strobe);

   initial begin
      clock = 0;
      reset = 1;
      data = 0;
      strobe = 0;
      bit_tick = 0;

      $monitor("t = %t  (bit %d) -- ser %b -- ready %b",
               $time, bit_tick, serial, ready);

      #1 reset = 0;

      #1000 data = 8'h55;
      strobe = 1;
      #5 strobe = 0;

      #10000 $finish;
   end

   always begin
      #5 clock = !clock;
      if (clock && baud_x1)
        bit_tick <= bit_tick + 1;
   end
endmodule


/*
 * Byte receiver, RS-232 8-N-1
 *
 * Receives on 'serial'. When a properly framed byte is
 * received, 'data_strobe' pulses while the byte is on 'data'.
 *
 * Error bytes are ignored.
 */

module serial_uart_rx(mclk, reset, baud_x4,
                      serial, data, data_strobe);

   input        mclk, reset, baud_x4, serial;
   output [7:0] data;
   output       data_strobe;

   /*
    * Synchronize the serial input to this clock domain
    */
   wire         serial_sync;
   d_flipflop_pair input_dff(mclk, reset, serial, serial_sync);

   /*
    * State machine: Four clocks per bit, 10 total bits.
    */
   reg [8:0]    shiftreg;
   reg [5:0]    state;
   reg          data_strobe;
   wire [3:0]   bit_count = state[5:2];
   wire [1:0]   bit_phase = state[1:0];

   wire         sampling_phase = (bit_phase == 1);
   wire         start_bit = (bit_count == 0 && sampling_phase);
   wire         stop_bit = (bit_count == 9 && sampling_phase);

   wire         waiting_for_start = (state == 0 && serial_sync == 1);

   wire         error = ( (start_bit && serial_sync == 1) ||
                          (stop_bit && serial_sync == 0) );

   assign       data = shiftreg[7:0];

   always @(posedge mclk or posedge reset)
     if (reset) begin
        state <= 0;
        data_strobe <= 0;
     end
     else if (baud_x4) begin

        if (waiting_for_start || error || stop_bit)
          state <= 0;
        else
          state <= state + 1;

        if (bit_phase == 1)
          shiftreg <= { serial_sync, shiftreg[8:1] };

        data_strobe <= stop_bit && !error;

     end
     else begin
        data_strobe <= 0;
     end

endmodule

module serial_uart_rx_testbench;
   /*
    * Test bench: Loopback test, for exercising the receiver.
    *
    * iverilog serial.v util.v -s serial_uart_rx_testbench -o brg.vvp && vvp brg.vvp
    */

   integer     bit_tick;
   reg         clock;
   reg         reset;
   reg         tx_strobe;
   reg [7:0]   tx_data;
   wire        baud_x1, baud_x4;
   wire        serial, tx_ready;
   wire [7:0]  rx_data;
   wire        rx_strobe;

   serial_brg brg(clock, reset, baud_x4, baud_x1);
   serial_uart_tx tx(clock, reset, baud_x1, serial, tx_ready, tx_data, tx_strobe);
   serial_uart_rx rx(clock, reset, baud_x4, serial, rx_data, rx_strobe);

   initial begin
      clock = 0;
      reset = 1;
      tx_data = 0;
      tx_strobe = 0;
      bit_tick = 0;

      $monitor("t = %t  (bit %d) -- ser %b -- tx_ready %b, rx_strobe %b, rx_data %02x",
               $time, bit_tick, serial, tx_ready, rx_strobe, rx_data);

      #1 reset = 0;

      #1000 tx_data = 8'h42;
      tx_strobe = 1;
      #5 tx_strobe = 0;

      #2500 $finish;
   end

   always begin
      #5 clock = !clock;
      if (clock && baud_x1)
        bit_tick <= bit_tick + 1;
   end
endmodule