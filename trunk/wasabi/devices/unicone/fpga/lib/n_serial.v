/*
 * n_serial.v - Verilog implementation of a receiver/transmitter
 *              for the low-level serial protocol used by the Nintendo
 *              Gamecube and N64 controller ports.
 *
 *              The protocol is synchronous serial, with an encoding
 *              similar to manchester, but with asymmetric high and low
 *              periods. It is described by:
 *
 *              http://www.int03.co.uk/crema/hardware/gamecube/gc-control.htm
 *
 *              More resources on the protocol, especially with regard to
 *              the N64, are available in the 'cube64' project.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


/*
 * Combines separate transmit and receive signals into the open-collector
 * I/O used by N64 and Gamecube consoles. Synchronizes inputs for two
 * clock cycles, and outputs for one.
 */
module n_serial_io_buffer (clk, reset,
                           pad, tx, rx);
	input clk;
	input reset;
	inout pad;
	input tx;
	output rx;
	
	reg [1:2] rx_sync;
	reg tx_sync;
	
	/* Imply an open-collector output */
	assign pad = tx_sync ? 1'bz : 1'b0;

	/* Our receiver gets the sync'ed input */
	assign rx = rx_sync[2];
	
	always @(posedge clk or posedge reset)
		if (reset) begin
			rx_sync <= 2'b11;
			tx_sync <= 1'b1;
		end
		else begin
			/* Sync inputs for two cycles */
			rx_sync[1] <= pad;
			rx_sync[2] <= rx_sync[1];
			
			/* Sync output once */
			tx_sync <= tx;
		end
endmodule


/*
 * Generates the serial timebase used for the transmit and receive modules.
 * The resulting 'tick' signal is high for one clock cycle every 1us.
 * This should be adjusted depending on the oscillator in use.
 *
 * Our hardware currently uses a 20MHz clock, therefore this implementation
 * is effectively a 1/20 clock divider.
 *
 * Two synchronous resets are provided- sync_begin resets to the beginning of
 * one tick cycle, sync_middle resets close to the center of a cycle. The 
 * cycle immediately after sync_reset is asserted, a tick will be generated.
 */
module n_serial_timebase (clk, reset, tick, sync_begin, sync_middle);
	input clk, reset, sync_begin, sync_middle;
	output tick;
	reg tick;
	reg [5:0] counter;

	always @(posedge clk or posedge reset)
		if (reset) begin
			tick <= 0;
			counter <= 0;
		end
		else if (sync_begin) begin
			tick <= 1;
			counter <= 0;
		end
		else if (sync_middle) begin
			tick <= 0;
			counter <= 10;
		end
		else if (counter == 19) begin
			tick <= 1;
			counter <= 0;
		end
		else begin
			tick <= 0;
			counter <= counter + 1;
		end
endmodule


/*
 * A receiver for N64/Gamecube formatted serial data. 
 *
 * A typical transmission:
 *   1. Activity is detected on the input
 *   2. rx_start is asserted for one clock cycle
 *   3. Data bytes are received. Each one is returned
 *      via rx_data, with a 1-cycle pulse on strobe.
 *   4. If inappropriate codes are received or the
 *      packet ends without a stop bit, rx_error is
 *      asserted for one clock cycle.
 *   5. If the packet is ended successfully, rx_stop
 *      is asserted.
 */
module n_serial_rx (clk, reset, serial_in,
                    rx_start, rx_stop, rx_error,
                    rx_data, strobe);
	input clk, reset;
	input serial_in;
	output rx_start, rx_stop, rx_error;
	output [7:0] rx_data;
	output strobe;

	reg rx_start, rx_stop, rx_error, strobe;
	reg [7:0] rx_data;
	reg [2:0] bit_count;
	reg [7:0] shifter;
	reg [3:0] qbits;
	reg [2:0] state;
	parameter
		S_IDLE = 0,
		S_WAIT_FOR_Q1 = 1,
		S_WAIT_FOR_Q2 = 2,
		S_WAIT_FOR_Q3 = 3,
		S_WAIT_FOR_Q4 = 4,
		S_DECODE_QBITS = 5;

	/* 1us serial timebase. Keep it in reset halfway through a tick
	 * as long as we're idle.
	 */
	wire tick;
	wire tick_reset = state == S_IDLE;
	n_serial_timebase rx_ticker(clk, reset, tick, 1'b0, tick_reset);

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_IDLE;
			qbits <= 4'b1111;
			bit_count <= 0;
			shifter <= 0;

			rx_start <= 0;
			rx_stop <= 0;
			rx_error <= 0;
			strobe <= 0;
			rx_data <= 0;

		end
		else case (state)

			S_IDLE: begin
				// We're waiting for incoming data, our timebase is in reset.
				// Leave when we see our serial input go low.
				if (!serial_in) begin
					state <= S_WAIT_FOR_Q1;
					rx_start <= 1;
				end
				else begin
					rx_start <= 0;
				end
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
				rx_data <= 0;
				bit_count <= 0;
				shifter <= 0;
				qbits <= 4'b1111;
			end
			
			S_WAIT_FOR_Q1: begin
				// At the next clock tick, we sample the Q1 quarter-bit
				if (tick) begin
					state <= S_WAIT_FOR_Q2;
					qbits[3] <= serial_in;
				end
				rx_start <= 0;
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
			end

			S_WAIT_FOR_Q2: begin
				// At the next clock tick, we sample the Q2 quarter-bit
				if (tick) begin
					state <= S_WAIT_FOR_Q3;
					qbits[2] <= serial_in;
				end
				rx_start <= 0;
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
			end

			S_WAIT_FOR_Q3: begin
				// At the next clock tick, we sample the Q3 quarter-bit
				if (tick) begin
					state <= S_WAIT_FOR_Q4;
					qbits[1] <= serial_in;
				end
				rx_start <= 0;
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
			end

			S_WAIT_FOR_Q4: begin
				// At the next clock tick, we sample the Q4 quarter-bit
				if (tick) begin
					state <= S_DECODE_QBITS;
					qbits[0] <= serial_in;
				end
				rx_start <= 0;
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
			end
			
			S_DECODE_QBITS: begin
				// We've received a full bit, decode the quarter-bits to
				// determine what it was.
				if (qbits == 4'b0111) begin
					// Successfully received a '1' bit

					if (bit_count == 7) begin
						// Last bit in this byte
						bit_count <= 0;
						rx_data <= {shifter[6:0], 1'b1};
						strobe <= 1;
					end
					else begin
						bit_count <= bit_count + 1;
						shifter <= {shifter[6:0], 1'b1};
						strobe <= 0;
					end
					state <= S_WAIT_FOR_Q1;

				end
				else if (qbits == 4'b0001) begin
					// Successfully received a '0' bit

					if (bit_count == 7) begin
						// Last bit in this byte
						bit_count <= 0;
						rx_data <= {shifter[6:0], 1'b0};
						strobe <= 1;
					end
					else begin
						bit_count <= bit_count + 1;
						shifter <= {shifter[6:0], 1'b0};
						strobe <= 0;
					end
					state <= S_WAIT_FOR_Q1;

				end
				else if (qbits == 4'b1111) begin
					// The line is idle again. If we just got a stop bit,
					// this is the end of a transmission
					
					if (bit_count == 1 && shifter[0] == 1'b1) begin
						// Yep, we have a stop bit. Signal a successful stop
						rx_stop <= 1;
						rx_error <= 0;
					end
					else begin
						// No stop bit, this is an error
						rx_error <= 1;
						rx_stop <= 0;
					end
					state <= S_IDLE;
					strobe <= 0;

				end

				else begin
					// Anything else is an error

					rx_error <= 1;
					rx_stop <= 0;
					state <= S_IDLE;
					strobe <= 0;

				end
			end
		endcase
endmodule


/*
 * A transmitter for N64/Gamecube formatted serial data. Input is via
 * a pair of one-byte buffers. The transmit register can be loaded by the
 * module's user simultaneously while data in the internal shift register
 * is being transmitted.
 *
 * Writes should proceed as follows:
 *   1. Wait for 'busy' to be deasserted
 *   2. Either:
 *      - Set tx_stopbit to 1
 *      - ...or set tx_stopbit to 0 and place valid data in 'tx_data'
 *   3. Transition 'strobe' from low to high
 *
 * Note that busy=0 does not mean we aren't transmitting, just that we
 * are ready for more data to be received.
 *
 * Our timing is derived from the n_serial_timebase module.
 */
module n_serial_tx (clk, reset,
                    busy, tx_stopbit, tx_data, strobe,
                    serial_out);
	input clk, reset;
	output busy;
	input tx_stopbit;
	input [7:0] tx_data;
	input strobe;
	output serial_out;
	reg serial_out;
	
	/* 1us serial timebase, without the sync resets hooked up */
	wire tick;
	n_serial_timebase tx_ticker(clk, reset, tick, 0, 0);
	
	/* Buffer the incoming data stream, consisting
	 * of both the tx_data and the stop bit flag.
	 */
	wire buffer_full, buffer_clear;
	wire buffered_stopbit;
	wire [7:0] buffered_data;
	assign busy = buffer_full;
	shallow_buffer #(9) buffer(
		clk, reset, buffer_full,
		{tx_stopbit, tx_data}, strobe,
		{buffered_stopbit, buffered_data}, buffer_clear);

	/* Split the output of the buffer into stop bits and normal bits */
	wire normal_byte_ready = buffer_full && !buffered_stopbit;
	wire stop_bit_ready = buffer_full && buffered_stopbit;
	wire normal_byte_ack;
	reg stop_bit_ack;
	assign buffer_clear = normal_byte_ack || stop_bit_ack;

	/* Serialize the buffered data stream. Note that this
	 * will not proceed if it encounters a stop bit.
	 * If ser_ready is high and new data is coming in, ser_strobe
	 * will go high for each new bit output on ser_data.
	 */
	wire ser_data, ser_strobe, ser_is_empty;
	reg ser_ready;
	serializer ser(clk, reset, buffered_data, normal_byte_ready,
		       normal_byte_ack, ser_data, ser_ready, ser_strobe, ser_is_empty);
	
	/* Our actual serial transmission is divided
	 * Into four quarters, plus wait states. The four
	 * quarters correspond to each 1us section of the
	 * protocol's 4us-long bits.
	 */
	reg [2:0] state;
	parameter
		S_WAIT_FOR_BIT = 0,
		S_WAIT = 1,
		S_WAIT_FOR_TICK = 2,
		S_Q1 = 3,
		S_Q2 = 4,
		S_Q3 = 5;

	reg current_bit;

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_WAIT_FOR_BIT;
			ser_ready <= 0;
			serial_out <= 1;
			current_bit <= 0;
			stop_bit_ack <= 0;

		end
		else case (state)

			S_WAIT_FOR_BIT: begin
				// This is a wait state we enter any time current_bit is invalid
				// We fetch a new bit either from the serializer or from any
				// of our special cases where we need to stuff bits in.

				if (stop_bit_ready && ser_is_empty) begin
					// Insert a stop bit
					current_bit <= 1;
					ser_ready <= 0;
					state <= S_WAIT_FOR_TICK;
					stop_bit_ack <= 1;
					ser_ready <= 0;
				end
				else if (ser_strobe) begin
					// Insert a normal bit
					current_bit <= ser_data;
					state <= S_WAIT_FOR_TICK;
					ser_ready <= 0;
				end
				else begin
					// Ask for another bit. Stall for one clock cycle
					// while the serializer gets it for us.
					ser_ready <= 1;
					current_bit <= 0;
					state <= S_WAIT;
				end
			end

			S_WAIT: begin
				// Do nothing for one cycle, immediately go back to S_WAIT_FOR_BIT
				state <= S_WAIT_FOR_BIT;
				ser_ready <= 0;
			end
			
			S_WAIT_FOR_TICK: begin
				// Between S_WAIT_FOR_BIT and S_Q1, sync to a clock tick
				if (tick) begin
					state <= S_Q1;
					serial_out <= 0;
				end
				stop_bit_ack <= 0;
			end

			S_Q1: begin
				// First quarter of the output bit, always 1.
				// The next state is always S_Q2, reflecting the current bit
				if (tick) begin
					state <= S_Q2;
					serial_out <= current_bit;
				end
				stop_bit_ack <= 0;
			end				
				
			S_Q2: begin
				// Second quarter of the output bit, reflects the current bit.
				// Next state is S_Q3, but serial_out doesn't change.
				if (tick) begin
					state <= S_Q3;
					serial_out <= current_bit;
				end
				stop_bit_ack <= 0;
			end	
			
			S_Q3: begin
				// Third quarter of the output bit, reflects the current bit.
				// The fourth quarter is always 1. We set that up here, but
				// there is no explicit state for it- it extends from after this
				// state until before S_Q1 recurs.
				if (tick) begin
					state <= S_WAIT_FOR_BIT;
					serial_out <= 1;
				end
				stop_bit_ack <= 0;
			end				

		endcase
endmodule

/* The End */
