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
 * Internally, we represent I2C as a three-wire unidirectional bus.
 * This converts it into a two-wire bus, for use externally. It
 * synchronizes inputs for two clock cycles.
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

	/* An open-collector driver for ext_sda */
	assign ext_sda = int_sda_out ? 1'bz : 1'b0;

	/* Our internal I2C bus is driven by the sync'ed inputs */
	assign int_scl = scl_in_sync[2];
	assign int_sda_in = sda_in_sync[2];

	always @(posedge clk or posedge reset)
		if (reset) begin
			sda_in_sync <= 2'b11;
			scl_in_sync <= 2'b11;
		end
		else begin
			sda_in_sync[1] <= ext_sda;
			sda_in_sync[2] <= sda_in_sync[1];
			scl_in_sync[1] <= ext_scl;
			scl_in_sync[2] <= scl_in_sync[1];
		end
endmodule


/*
 * A code detector/generator for I2C slaves. This converts the incoming
 * I2C signal into 1-cycle pulses indicating start condition, stop condition,
 * or received bit. Output data is latched on the falling edge of SCL.
 */
module i2c_slave_code_detect (clk, reset, scl, sda,
                              rx_start, rx_stop, rx_data, tx_data, strobe);
	input clk, reset;
	input scl;
	inout sda;
	output rx_start, rx_stop, rx_data, strobe;
	input tx_data;

	wire int_scl, int_sda_in;
	reg int_sda_out;
	i2c_io_buffer buffer(clk, reset, scl, sda, int_scl, int_sda_in, int_sda_out);


	/* Store the previous value of SDA and SCL, for edge detection */
	reg prev_sda, prev_scl;
	always @(posedge clk or posedge reset)
		if (reset) begin
			prev_sda <= 1;
			prev_scl <= 1;
		end
		else begin
			prev_sda <= int_sda_in;
			prev_scl <= int_scl;
		end
	
	/* Latch the SDA output on falling SCL edge */
	always @(posedge clk or posedge reset)
		if (reset)
			int_sda_out <= 1;
		else if (prev_scl && ! int_scl)
			int_sda_out <= tx_data;

	/* Detect bus conditions */
	reg rx_start, rx_stop, strobe, rx_data;
	always @(posedge clk or posedge reset)
		if (reset) begin
			rx_stop <= 0;
			rx_start <= 0;
			rx_data <= 0;
			strobe <= 0;
		end
		else begin
			// Start conditions: SCL is stable, SDA falls
			rx_start <= (int_scl && prev_scl && prev_sda && !int_sda_in);

			// Stop concitions: SCL is stable, SDA rises
			rx_stop <= (int_scl && prev_scl && int_sda_in && !prev_sda);
			
			// A data bit is valid: SCL rises
			if (int_scl && !prev_scl) begin
				rx_data <= int_sda_in;
				strobe <= 1;
			end
			else begin
				strobe <= 0;
			end
		end
endmodule


/*
 * A high level packet detector for I2C slaves. This receives device
 * addresses and exposes a higher level interface for processing I2C packets:
 *
 *   1. A start condition and address byte are received.
 *   2. The pkt_address and pkt_read_wr outputs become valid,
 *      pkt_addressed goes high, bit_count resets to zero.
 *   3. Our peer pulls 'ack' high. Once the proper time comes, we send an ACK bit
 *   4. If we receive further data, individual bits are clocked out on 'rx_content'
 *      using 1-cycle pulses on 'content_strobe'. pkt_address and pkt_read_wr remain valid.
 *      bit_count is incremented after each strobe. This does not count ACK bits,
 *      which are handled internally using the active-high 'ack' signal.
 *      Data is transmitted analogously by placing valid data on tx_content when
 *      content_strobe pulses.
 *   5. Once a stop condition is received, pkt_addressed goes low.
 *
 * All I2C signals are packed into a bus:
 *   i2c_interface_tx = {tx_content, ack}
 *   i2c_interface_rx = {rx_stop, rx_content, content_strobe, bit_count[7:0]
 *                       pkt_address[6:0], pkt_read_wr, pkt_addressed}
 *
 */
module i2c_frontend (clk, reset, scl, sda, i2c_interface_tx, i2c_interface_rx);
	input clk, reset, scl;
	inout sda;
	input [1:0] i2c_interface_tx;
	output [19:0] i2c_interface_rx;
	
	// Unpack the received bus
	wire tx_content = i2c_interface_tx[1];
	wire ack = i2c_interface_tx[0];
	
	// Pack the transmitted bus
	wire rx_stop, rx_content, content_strobe;
	reg [7:0] bit_count;
	wire [6:0] pkt_address;
	wire pkt_read_wr;
	reg pkt_addressed;
	assign i2c_interface_rx = {rx_stop, rx_content, content_strobe,
	                           bit_count, pkt_address, pkt_read_wr, pkt_addressed};

	// I2C bit detector
	wire rx_start, rx_data, tx_data, strobe;
	i2c_slave_code_detect code_det(clk, reset, scl, sda,
	                               rx_start, rx_stop, rx_data, tx_data, strobe);

	// Keep an 8-bit shift register for the I2C address byte
	reg [7:0] addr_shift;
	assign pkt_address = addr_shift[7:1];
	assign pkt_read_wr = addr_shift[0];

	// Our bit counter is reset by the bit_count_reset net, and advanced by strobe.
	// Every 9th bit, the ACK, pulls 'ack_flag' high for one bit-time rather than
	// incrementing bit_count.
	wire bit_count_reset;
	reg ack_flag;
	always @(posedge clk or posedge reset)
		if (reset) begin
			bit_count <= 0;
			ack_flag <= 0;
		end
		else if (bit_count_reset) begin
			bit_count <= 0;
			ack_flag <= 0;
		end
		else if (strobe) begin
			if (ack_flag) begin
				ack_flag <= 0;
				bit_count <= bit_count + 1;
			end
			else if (bit_count[2:0] == 3'b111) begin
				ack_flag <= 1;
			end
			else begin
				ack_flag <= 0;
				bit_count <= bit_count + 1;
			end
		end
		
	// Reset the bit count either on rx_start, or when we've just
	// finished receiving the I2C address.
	assign bit_count_reset = rx_start || (bit_count == 8 && pkt_addressed);
	
	// Send out ACK bits as appropriate
	assign tx_data = ack_flag ? (!ack) : tx_content;

	// Pass on content once we've decoded the address and we aren't ACK'ing
	assign content_strobe = strobe && pkt_addressed && !ack_flag;
	assign rx_content = rx_data;

	// Enter and leave addressed mode as appropriate
	always @(posedge clk or posedge reset)
		if (reset) begin
			addr_shift <= 0;
			pkt_addressed <= 0;
		end
		else begin
			if (!pkt_addressed) begin
				// We're waiting on receiving the address byte
				if (strobe) begin
					// Got a bit. Is this the last one?
					if (bit_count == 7)
						pkt_addressed <= 1;
					addr_shift <= {addr_shift[6:0], rx_data};
				end
			end
			else begin
				// We've been addressed, just receive until we stop
				if (rx_start || rx_stop)
					pkt_addressed <= 0;
			end	
		end	
endmodule

/*
 * An I2C-accessable dual port 32-byte SRAM. The I2C host can write to the SRAM,
 * other entities can only read from it. The reader will not be interrupted
 * by I2C access to the SRAM.
 *
 * The I2C interface works like most EEPROM memories: the first byte in a
 * write sets an address pointer, all subsequent bytes write to that address
 * then increment it.
 *
 * Currently the host can only write- this interface could be logically extended
 * to also allow readback.
 *
 * Other entities can asynchronously read the contents of our SRAM using the
 * supplied address input and data output.
 */
module i2c_slave_32byte_sram (clk, reset,
                              scl, sda_in, sda_out,
                              user_addr, user_data);

	parameter I2C_ADDRESS = 0;

	input clk, reset;
	input scl, sda_in;
	output sda_out;
	input [4:0] user_addr;
	output [7:0] user_data;

	wire start, stop, wr;
	reg wr_ack;
	wire [7:0] write_data;
	i2c_slave_serializer i2cs(clk, reset,
	                          scl, sda_in, sda_out,
	                          start, stop,
	                          write_data, wr, wr_ack);

	reg [4:0] current_addr;
	reg ram_write;
	wire [7:0] current_ram_contents;
	sram_32byte_dualport ram(clk, ram_write, write_data,
	                         current_addr, current_ram_contents,
	                         user_addr, user_data);

	reg [2:0] state;
	parameter
		S_IDLE = 0,
		S_WAIT_FOR_I2C_ADDRESS = 1,
		S_WAIT_FOR_MEM_ADDRESS = 2,
		S_WAIT_FOR_BYTE = 3,
		S_NEXT_ADDRESS = 4;

	always @(posedge clk or posedge reset)
		if (reset) begin

			current_addr <= 0;
			ram_write <= 0;
			state <= S_IDLE;
			wr_ack <= 0;

		end
		else case (state)

			S_IDLE: begin
				// Wait for a start condition
				if (start)
					state <= S_WAIT_FOR_I2C_ADDRESS;
			end

			S_WAIT_FOR_I2C_ADDRESS: begin
				// Wait for our i2c address byte
				if (start)
					state <= S_WAIT_FOR_I2C_ADDRESS;
				else if (stop)
					state <= S_IDLE;
				else if (wr) begin
					// Is this our address?
					if (write_data[7:1] == I2C_ADDRESS) begin
						state <= S_WAIT_FOR_MEM_ADDRESS;
						wr_ack <= 1;
					end
					else begin
						state <= S_IDLE;
						wr_ack <= 0;
					end
				end
			end

			S_WAIT_FOR_MEM_ADDRESS: begin
				// Wait for our current memory address
				if (start)
					state <= S_WAIT_FOR_I2C_ADDRESS;
				else if (stop)
					state <= S_IDLE;
				else if (wr) begin
					current_addr <= write_data[4:0];
					wr_ack <= 1;
					state <= S_WAIT_FOR_BYTE;
				end
			end


			S_WAIT_FOR_BYTE: begin
				// Wait for a byte to write.
				if (start)
					state <= S_WAIT_FOR_I2C_ADDRESS;
				else if (stop)
					state <= S_IDLE;
				else if (wr) begin
					ram_write <= 1;
					wr_ack <= 1;
					state <= S_NEXT_ADDRESS;
				end
			end

			S_NEXT_ADDRESS: begin
				// Increment our write address safely after the byte has been
				// written, then go back to looking for another byte
				if (start)
					state <= S_WAIT_FOR_I2C_ADDRESS;
				else if (stop)
					state <= S_IDLE;
				else
					state <= S_WAIT_FOR_BYTE;
				ram_write <= 0;
				current_addr <= current_addr + 1;
			end

		endcase
endmodule


/*
 * An I2C slave that lets the host write to a single N-byte register. There is
 * no address pointer, as with I2C memories- all data addressed to this device
 * will be stored in the output register.
 *
 * reg_out will always be valid with the current contents of the register. New
 * data is latched into reg_out simultaneously after the I2C stop condition.
 */
module i2c_slave_reg (clk, reset,
                      scl, sda_in, sda_out,
                      reg_out);

	parameter I2C_ADDRESS = 0;
	parameter BYTES_WIDE = 1;
	parameter COUNT_WIDTH = 2;
	parameter DEFAULT_VALUE = 0;

	parameter BITS_WIDE = BYTES_WIDE * 8;

	input clk, reset;
	input scl, sda_in;
	output sda_out;
	output [BITS_WIDE-1:0] reg_out;

	reg [BITS_WIDE-1:0] reg_out;
	reg [BITS_WIDE-1:0] out_buffer;
	reg [COUNT_WIDTH-1:0] byte_count;

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

		end
		else case (state)

			S_IDLE: begin
				// Wait for a start condition
				if (start)
					state <= S_WAIT_FOR_ADDRESS;
				byte_count <= 0;
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
			end

			S_WAIT_FOR_STOP: begin
				// We have everything. If we get a stop condition, latch it all
				if (start) begin
					state <= S_WAIT_FOR_ADDRESS;
				end
				else if (stop) begin
					reg_out <= out_buffer;
					state <= S_IDLE;
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
