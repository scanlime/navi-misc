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
 * The resulting 'tick' signal is high for one clock cycle every 1/4 bit.
 * This should be adjusted depending on the oscillator in use.
 *
 * It's a little unclear whether the protocol was designed to use fixed bit
 * widths, or to measure the relative times between high and low periods.
 * We assume fixed 4us-wide bits. Note that the GC controllers and the N64
 * seem to use 4us bits, while the GC console uses 5us bits. Our receiver
 * resync's its timebase frequently, so this shouldn't be a problem. I suspect
 * the Nintendo hardware does something similar.
 *
 * Two synchronous resets are provided- sync_begin resets to the beginning of
 * one tick cycle, sync_middle resets close to the center of a cycle. The
 * cycle immediately after sync_reset is asserted, a tick will be generated.
 */
module n_serial_timebase (clk, reset, tick, sync_begin, sync_middle);
	parameter BIT_WIDTH_US = 4;

	parameter CLOCK_MHZ = 25;
	parameter CLOCKS_PER_TICK = CLOCK_MHZ * BIT_WIDTH_US / 4;

	input clk, reset, sync_begin, sync_middle;
	output tick;
	reg tick;
	reg [7:0] counter;

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
			counter <= CLOCKS_PER_TICK/2-1;
		end
		else if (counter == CLOCKS_PER_TICK-1) begin
			tick <= 1;
			counter <= 0;
		end
		else begin
			tick <= 0;
			counter <= counter + 1;
		end
endmodule


/* A shared phase generator for the transmitters. Given a 'tick' signal
 * from the n_serial_timebase, this generates a 'phase' output that
 * corresponds to the four quarter-bits.
 */
module n_serial_tx_phase (clk, reset, tick, phase);
	input clk, reset, tick;
	output [1:0] phase;
	reg [1:0] phase;
	
	always @(posedge clk or posedge reset)
		if (reset)
			phase <= 0;
		else if (tick)
			phase <= phase+1;
endmodule


/*
 * A bit detector for receiving N64/Gamecube formatted
 * serial data. This detects the following types of conditions:
 *
 * 1. Start conditions (a bit beginning on an idle line)
 *     - rx_start goes high for one cycle
 *
 * 2. Stop bits (a 1 bit with no high-to-low transition)
 *     - rx_stop goes high for one cycle
 *
 * 3. Error conditions (stop bits or normal bits with out-of-range timings)
 *     - rx_error goes high for one cycle
 *
 * 4. Normal data bits
 *     - rx_data holds the received bit while strobe goes high for one cycle
 */
module n_serial_bit_detector (clk, reset, serial_in,
	                      rx_start, rx_stop, rx_error,
                              rx_data, strobe);
	// Rate 'clk' oscillates at, in megahertz
	parameter CLOCK_MHZ = 25;

	// Nominal bit width, in microseconds
	parameter BIT_WIDTH_US = 5;

	// Nominal bit width in clock cycles
	parameter CLOCKS_PER_BIT = CLOCK_MHZ * BIT_WIDTH_US;

	// Boundary, in the low-time of the input, between a zero and a one bit
	parameter ZERO_ONE_BOUNDARY = CLOCKS_PER_BIT / 2;
	
	// Minimum and maximum periods for one bit
	parameter MAX_BIT_WIDTH = CLOCKS_PER_BIT + CLOCKS_PER_BIT/3;
	parameter MIN_BIT_WIDTH = CLOCKS_PER_BIT - CLOCKS_PER_BIT/3;

	// Large enough to hold MAX_BIT_WIDTH
	reg [7:0] timer;

	input clk, reset;
	input serial_in;
	output rx_start, rx_stop, rx_error, rx_data, strobe;
	reg rx_start, rx_stop, rx_error, rx_data, strobe;

	reg [1:0] state;
	parameter
		S_IDLE = 0,
		S_LOW_PERIOD = 1,
		S_HIGH_PERIOD = 2;

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_IDLE;
			timer <= 0;

			rx_start <= 0;
			rx_stop <= 0;
			rx_error <= 0;
			strobe <= 0;
			rx_data <= 0;

		end
		else case (state)

			S_IDLE: begin
				// We're waiting for incoming data, our timebase is in reset.
				// Leave when we see our serial input go low. We only hit this
				// state between transactions, since consecutive bits jump right from
				// S_HIGH_PERIOD to S_LOW_PERIOD.
				if (serial_in) begin
					timer <= 0;
					rx_start <= 0;
				end
				else begin
					state <= S_LOW_PERIOD;
					timer <= 1;
					rx_start <= 1;
				end
				rx_stop <= 0;
				rx_error <= 0;
				strobe <= 0;
			end
			
			S_LOW_PERIOD: begin
				// We're timing the low period of this bit
				if (timer > MAX_BIT_WIDTH) begin
					// Overflow, this couldn't have been a bit. Signal an error and reset.
					state <= S_IDLE;
					timer <= 0;
					rx_error <= 1;
				end
				else if (serial_in) begin
					// The low period just ended, evaluate it
					rx_data <= (timer < ZERO_ONE_BOUNDARY);
					state <= S_HIGH_PERIOD;
					timer <= timer + 1;
					rx_error <= 0;
				end
				else begin
					// Keep timing
					timer <= timer + 1;
					rx_error <= 0;
				end
				rx_start <= 0;
				rx_stop <= 0;
				strobe <= 0;
			end

			S_HIGH_PERIOD: begin
				// Time the high period of this bit
				if (timer > MAX_BIT_WIDTH) begin
					// Overflow. It's been high too long to be a normal bit.
					// If the low period was measured as a one, this was a stop
					// bit. Otherwise it was just an error condition.
					state <= S_IDLE;
					timer <= 0;
					strobe <= 0;
					if (rx_data) begin
						rx_error <= 0;
						rx_stop <= 1;
					end
					else begin
						rx_error <= 1;
						rx_stop <= 0;
					end
				end
				else if (serial_in) begin
					// Keep timing
					timer <= timer + 1;
					rx_error <= 0;
					strobe <= 0;
					rx_stop <= 0;
				end
				else begin
					// The high period just ended, evaluate it
					if (timer > MAX_BIT_WIDTH || timer < MIN_BIT_WIDTH) begin
						// Out of range, this wasn't a valid bit
						state <= S_IDLE;
						timer <= 0;
						rx_error <= 1;
						strobe <= 0;
						rx_stop <= 0;
					end
					else begin
						// Looks like that was a successful bit. Strobe it
						// and get on with timing the next bit's low period.
						state <= S_LOW_PERIOD;
						timer <= 1;
						rx_error <= 0;
						strobe <= 1;
						rx_stop <= 0;
					end
				end
				rx_start <= 0;
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
 * An external n_serial_timebase must supply the 'tick' signal, and
 * an n_serial_tx_phase module must supply 'phase'. One timebase can
 * be shared among all transmitters.
 */
module n_serial_tx (clk, reset, tick, phase,
                    busy, tx_stopbit, tx_data, strobe,
                    serial_out);
	input clk, reset, tick;
	output busy;
	input tx_stopbit;
	input [7:0] tx_data;
	input [1:0] phase;
	input strobe;
	output serial_out;

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

	reg [1:0] state;
	parameter
		S_WAIT_FOR_BIT = 0,
		S_WAIT = 1,
		S_SYNC_STOP_BIT = 2,
		S_SYNC_NORMAL_BIT = 3;

	/* Our state machine generates 'synced_bit', synchronized to the beginning of phase 0.
	 * From there we use combinational logic to generate the actual transmitted waveform:
	 * 0111 for ones, and 0001 for zeroes.
         */
	reg synced_bit;
	wire bus_idle = (state == S_WAIT_FOR_BIT || state == S_WAIT) && ser_is_empty && !buffer_full;
        assign serial_out = bus_idle ? 1 : 
                            phase == 0 ? 0 :
                            phase == 3 ? 1 :
                            synced_bit;

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_WAIT_FOR_BIT;
			ser_ready <= 0;
			synced_bit <= 1;
			stop_bit_ack <= 0;

		end
		else case (state)

			S_WAIT_FOR_BIT: begin
				// This is a wait state we enter any time a new bit isn't
				// available and waiting to be synchronized. We look for a new
				// bit either from the serializer or in the form of a stop
				// bit straight from our buffer.

				if (stop_bit_ready && ser_is_empty) begin
					// Insert a stop bit
					state <= S_SYNC_STOP_BIT;					
					stop_bit_ack <= 1;
					ser_ready <= 0;
				end
				else if (ser_strobe) begin
					// Insert a normal bit
					state <= S_SYNC_NORMAL_BIT;
					stop_bit_ack <= 0;
					ser_ready <= 0;
				end
				else begin
					// Ask for another bit. Stall for one clock cycle
					// while the serializer gets it for us.
					state <= S_WAIT;
					stop_bit_ack <= 0;
					ser_ready <= 1;					
				end
			end

			S_WAIT: begin
				// Do nothing for one cycle, immediately go back to S_WAIT_FOR_BIT
				state <= S_WAIT_FOR_BIT;
				stop_bit_ack <= 0;
				ser_ready <= 0;					
			end

			S_SYNC_STOP_BIT: begin
				// Output a stop bit next time we're about to flip to phase 0
				if (tick && phase==3) begin
					state <= S_WAIT_FOR_BIT;
					synced_bit <= 1;
				end
				stop_bit_ack <= 0;
				ser_ready <= 0;					
			end

			S_SYNC_NORMAL_BIT: begin
				// Output a bit from our serializer next time we're about to flip to phase 0
				if (tick && phase==3) begin
					state <= S_WAIT_FOR_BIT;
					synced_bit <= ser_data;
				end
				stop_bit_ack <= 0;
				ser_ready <= 0;					
			end

		endcase
endmodule

/* The End */
