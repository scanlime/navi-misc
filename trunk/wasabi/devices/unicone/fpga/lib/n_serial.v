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
 * Generates timing signals shared by all serial transmitters.
 * The timing bus is of the form: { sync, quarter }. sync is
 * high for the one cycle preceeding a new bit, and quarter is a
 * 2-bit bus indicating which quarter-bit is currently active.
 */
module n_serial_tx_timebase (clk, reset, tx_timing);
	// Rate 'clk' oscillates at, in megahertz
	parameter CLOCK_MHZ = 25;

	// Nominal bit width, in microseconds
	parameter BIT_WIDTH_US = 4;

	// Nominal bit width in clock cycles
	parameter CLOCKS_PER_BIT = CLOCK_MHZ * BIT_WIDTH_US;

	input clk, reset;
	output [2:0] tx_timing;

	// Our only persistent state is a simple counter that loops every bit.
	// This must be large enough to hold CLOCKS_PER_BIT-1.
	reg [6:0] timer;
	always @(posedge clk or posedge reset)
		if (reset)
			timer <= 0;
		else if (timer == CLOCKS_PER_BIT-1)
			timer <= 0;
		else
			timer <= timer + 1;
	
	// The actual timing signals are generated combinationally,
	// for reduced FPGA area. If you need to optimize for speed,
	// these outputs should be registered.
	wire [1:0] quarter = timer < CLOCKS_PER_BIT/4   ? 0 :
	                     timer < CLOCKS_PER_BIT/2   ? 1 :
	                     timer < CLOCKS_PER_BIT*3/4 ? 2 : 3;
	wire sync = (timer == CLOCKS_PER_BIT-1);
	assign tx_timing = { sync, quarter };
endmodule


/*
 * A bit generator for transmitting N64/Gamecube formatted serial data.
 * Input is via a one-bit buffer loadable by our data source, output is
 * locked to the provided tx_timing bus. Start conditions are implicit,
 * and stop bits should be written as normal '1' bits.
 *
 * Writes should proceed as follows:
 *   1. Wait for 'tx_busy' to be deasserted
 *   2. Place the next bit in 'tx_data'
 *   3. Pulse 'strobe' high for one clock cycle
 *
 * Note that busy=0 does not mean we aren't transmitting, just that we
 * are ready for more data to be received.
 */
module n_serial_bit_generator (clk, reset, tx_timing,
                               busy, tx_data, strobe,
                               serial_out);
	input clk, reset;
	input [2:0] tx_timing;
	output busy;
	input tx_data, strobe;
	output serial_out;

	/* Unpack the tx_timing bus. 'sync' pulses the cycle before a new bit
	 * begins, and 'quarter' contains the current quarter-bit number.
	 */
	wire sync = tx_timing[2];
	wire [1:0] quarter = tx_timing[1:0];

	/* Buffer for incoming data. It fills when our data source
	 * pulses 'strobe'. At every sync pulse, the synchronized bit
	 * and fullness status move into sync_data and sync_full.
	 */
	reg buffer_data, buffer_full;
	reg sync_data, sync_full;
	assign busy = buffer_full;
	always @(posedge clk or posedge reset)
		if (reset) begin
			buffer_data <= 0;
			buffer_full <= 0;
			sync_data <= 0;
			sync_full <= 0;
		end
		else begin
			if (strobe && sync) begin
				buffer_data <= tx_data;
				buffer_full <= 1;
				sync_data <= buffer_data;
				sync_full <= buffer_full;
			end
			else if (strobe) begin
				buffer_data <= tx_data;
				buffer_full <= 1;
			end
			else if (sync) begin
				buffer_full <= 0;
				sync_data <= buffer_data;
				sync_full <= buffer_full;
			end
		end
	
	/* Generate an output waveform corresponding
	 * to our sync'ed bit. Zeroes generate 0001, ones generate 0111.
	 */
	wire sync_waveform = quarter == 0 ? 0 :
	                     quarter == 1 ? sync_data :
	                     quarter == 2 ? sync_data : 1;
	
	/* If our sync_data is valid, output the sync_waveform-
	 * otherwise leave the bus in its idle state (1).
	 */
	assign serial_out = sync_full ? sync_waveform : 1;
endmodule


/*
 * A variation on n_serial_bit_generator that includes a builtin
 * timebase. This is mostly intended for convenience when simulating.
 */
module n_serial_bit_generator_with_timebase (clk, reset,
                                             busy, tx_data, strobe,
                                             serial_out);
	input clk, reset;
	output busy;
	input tx_data, strobe;
	output serial_out;
	
	wire [2:0] tx_timing;
	n_serial_tx_timebase timebase(clk, reset, tx_timing);

        n_serial_bit_generator bitgen(clk, reset, tx_timing, busy, tx_data, strobe, serial_out);
endmodule

/* The End */
