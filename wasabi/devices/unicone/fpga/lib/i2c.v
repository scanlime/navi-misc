/*
 * i2c.v - Verilog implementation of a simple I2C slave device
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
 * On-chip I2C peripherals use a three-wire unidirectional bus.
 * This converts it into a two-wire bus, for use externally. It
 * synchronizes inputs for two clock cycles and outputs for one.
 *
 * Note that "int_sda_out" and "int_sda_in" are named relative
 * to this chip, so "int_sda_in" is actually an output in this module.
 */
module i2c_io_buffer (clk, reset,
                      ext_scl, ext_sda,
                      int_scl, int_sda_in, int_sda_out);
	input clk;
	input reset;
	input ext_scl;
	inout ext_sda;
	output int_scl;
	output int_sda_in;
	input int_sda_out;

	reg [1:2] sda_in_sync;
	reg [1:2] scl_in_sync;
	reg sda_out_sync;
	
	/* An open-collector driver for ext_sda using our sync'ed sda_out */
	assign ext_sda = sda_out_sync ? 1'bz : 1'b0;
	
	/* Our internal I2C bus is driven by the sync'ed inputs */
	assign int_scl = scl_in_sync[2];
	assign int_sda_in = sda_in_sync[2];
	
	always @(posedge clk or posedge reset)
		if (reset) begin
			sda_in_sync <= 2'b11;
			sda_out_sync <= 1'b1;
			scl_in_sync <= 2'b11;
		end
		else begin
			/* Sync inputs for two cycles */
			sda_in_sync[1] <= ext_sda;
			sda_in_sync[2] <= sda_in_sync[1];
			scl_in_sync[1] <= ext_scl;
			scl_in_sync[2] <= scl_in_sync[1];
			
			/* Sync output once */
			sda_out_sync <= int_sda_out;
		end
endmodule


/*
 * A simple I2C slave that lets the host write to a single N-byte register.
 * reg_out will always hold the current contents of the register.
 * reg_out_strobe goes high for one clock cycle after reg_out has been updated
 * by the I2C master.
 */
module i2c_slave_reg (clk, reset,
                      scl, sda_in, sda_out,
                      reg_out, reg_out_strobe);

	parameter I2C_ADDRESS = 0;
	parameter BYTES_WIDE = 1;
	parameter DEFAULT_VALUE = 0;

	parameter BITS_WIDE = BYTES_WIDE * 8;
	parameter COUNT_WIDTH = 3;

	input clk, reset;
	input scl, sda_in;
	output sda_out;
	output [BITS_WIDE-1:0] reg_out;
	output reg_out_strobe;

	reg [BITS_WIDE-1:0] reg_out;
	reg [BITS_WIDE-1:0] out_buffer;
	reg [COUNT_WIDTH-1:0] byte_count;
	reg reg_out_strobe;

	wire start, stop, wr;
	reg wr_ack;
	wire [7:0] write_data;
	i2c_slave_serializer i2cs(clk, reset,
	                          scl, sda_in, sda_out,
	                          start, stop,
	                          write_data, wr, wr_ack);

	reg [1:0] state;
	parameter
		S_IDLE = 0,
		S_WAIT_FOR_ADDRESS = 1,
		S_WAIT_FOR_BYTE = 2,
		S_WAIT_FOR_STOP = 3;

	always @(posedge clk or posedge reset)
		if (reset) begin
		
			reg_out <= DEFAULT_VALUE;
			out_buffer <= 0;
			byte_count <= 0;
			state <= S_IDLE;
			wr_ack <= 0;
			reg_out_strobe <= 0;
		
		end
		else case (state)

			S_IDLE: begin
				// Wait for a start condition
				if (start)
					state <= S_WAIT_FOR_ADDRESS;
				byte_count <= 0;
				reg_out_strobe <= 0;
			end

			S_WAIT_FOR_ADDRESS: begin
				// Wait for our address byte
				if (start)
					state <= S_WAIT_FOR_ADDRESS;
				else if (wr) begin
					// Is this our address?
					if (write_data[7:1] == I2C_ADDRESS) begin
						state <= S_WAIT_FOR_BYTE;
						wr_ack <= 1;
					end
					else begin
						state <= S_IDLE;
						wr_ack <= 0;
					end	
				end
				reg_out_strobe <= 0;
			end

			S_WAIT_FOR_BYTE: begin
				// Wait for another data byte
				if (start) begin
					state <= S_WAIT_FOR_ADDRESS;
					byte_count <= 0;
				end
				else if (wr) begin
					if (BITS_WIDE > 8)
						out_buffer <= {out_buffer[BITS_WIDE-9:0], write_data};
					else
						out_buffer <= write_data;
					
					wr_ack <= 1;
					if (byte_count == BYTES_WIDE-1) begin
						state <= S_WAIT_FOR_STOP;
						byte_count <= 0;
					end
					else begin
						byte_count <= byte_count + 1;
					end
				end
				reg_out_strobe <= 0;
			end

			S_WAIT_FOR_STOP: begin
				// We have everything. If we get a stop condition, latch it all
				if (start) begin
					state <= S_WAIT_FOR_ADDRESS;
					reg_out_strobe <= 0;
				end
				else if (stop) begin
					reg_out <= out_buffer;
					reg_out_strobe <= 1;
					state <= S_IDLE;
				end
				else begin
					reg_out_strobe <= 0;
				end
				byte_count <= 0;
			end
		endcase
endmodule

/*
 * Convert I2C to a parallel protocol consisting of strobed 8-bit bytes,
 * and separate signals indicating start and stop conditions.
 *
 * Incoming transactions:
 *    1. start is asserted for one clock cycle
 *    2. Bytes arrive, according to the sequence below
 *    3. stop is asserted for one clock cycle
 *
 * Incoming bytes:
 *    1. For one clock cycle, simultaneously:
 *         - wr is asserted
 *         - write_data holds the received byte
 *    2. wr_ack must become valid before SCL next goes low
 */
module i2c_slave_serializer (clk, reset,
                             scl, sda_in, sda_out,
                             start, stop,
                             write_data, wr, wr_ack);
	input clk, reset;
	input scl, sda_in;
	output sda_out;
	reg sda_out;

	output [7:0] write_data;
	reg [7:0] write_data;
	output wr;
	reg wr;
	input wr_ack;
	output start;
	reg start;
	output stop;
	reg stop;

	reg [3:0] bit_count;

	// States, one-hot encoding
	reg [2:0] state;
	parameter
		S_WAIT_FOR_START = 0,
		S_WAIT_FOR_SCL_LOW = 1,
		S_WAIT_FOR_SCL_HIGH = 2;

	// SDA edge detection
	reg prev_sda;
	always @(posedge clk or posedge reset)
		if (reset)
			prev_sda <= 1'b1;
		else
			prev_sda <= sda_in;

	always @(posedge clk or posedge reset)
		if (reset) begin
		
			sda_out <= 1;
			write_data <= 0;
			wr <= 0;
			start <= 0;
			stop <= 0;
			bit_count <= 0;
			state <= S_WAIT_FOR_START;
		
		end
		else case (state)

			S_WAIT_FOR_START: begin
				// Ignore SCL. If SDA falls, we're in a start condition.
				sda_out <= 1;
				write_data <= 0;
				wr <= 0;
				stop <= 0;
				bit_count <= 0;

				if ((!sda_in) && prev_sda) begin
					// A start condition. Note it and wait for SCL to go low
					state <= S_WAIT_FOR_SCL_LOW;
					start <= 1;
				end
				else begin
					// No start condition yet...
					start <= 0;
				end
			end

			S_WAIT_FOR_SCL_LOW: begin
				// SCL is high. If we see it fall, time for another bit.
				// If SDA goes high, this is a stop condition and we go back
				// to S_WAIT_FOR_START after latching the latest data.
				wr <= 0;
				start <= 0;

				if (!scl) begin
					// Another data bit
					state <= S_WAIT_FOR_SCL_HIGH;
					stop <= 0;

					// If the next bit is the ACK, go ahead
					// and pull SDA low now, to ensure our ACK
					// bit is valid once SCL goes back high.
					if (bit_count == 8)
						sda_out <= !wr_ack;
					else
						sda_out <= 1;
				end
				else if (sda_in && (!prev_sda)) begin
					// Stop condition
					stop <= 1;
					state <= S_WAIT_FOR_START;
				end
			end

			S_WAIT_FOR_SCL_HIGH: begin
				// SCL is low. When it goes high, another data bit will have been
				// clocked in. If it's a normal bit, store it- if it's an ACK,
				// pull SDA low.
				if (scl) begin
					if (bit_count == 8) begin
						// ACK bit. Leave sda_out in its previous state,
						// and reset our bit count.
						bit_count <= 0;
						wr <= 0;
					end
					else begin
						// If we just got the last bit, assert wr
						if (bit_count == 7)
							wr <= 1;
						else
							wr <= 0;
					
						// Normal bit
						bit_count <= bit_count + 1;
						sda_out <= 1;

						// Sample a bit from SDA into our big shift register
						write_data <= { write_data[6:0], sda_in };
					end
					state <= S_WAIT_FOR_SCL_LOW;
				end
				else begin
					wr <= 0;
				end
			end

		endcase
endmodule

/* The End */
