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
 *      which are handled internally using the active-high 'ack' signal, but
 *      it does count the 8 bits received as part of the address byte.
 *      Data is transmitted analogously by placing valid data on tx_content when
 *      content_strobe pulses.
 *   5. Once a stop condition is received, pkt_addressed goes low.
 *
 * All I2C signals are packed into a bus:
 *   i2c_interface_tx = {tx_content, ack}
 *   i2c_interface_rx = {rx_stop, rx_content, content_strobe, bit_count[8:0]
 *                       pkt_address[6:0], pkt_read_wr, pkt_addressed}
 *
 */
module i2c_frontend (clk, reset, scl, sda, i2c_interface_tx, i2c_interface_rx);
	input clk, reset, scl;
	inout sda;
	input [1:0] i2c_interface_tx;
	output [20:0] i2c_interface_rx;
	
	// Unpack the received bus
	wire tx_content = i2c_interface_tx[1];
	wire ack = i2c_interface_tx[0];
	
	// Pack the transmitted bus
	wire rx_stop, rx_content, content_strobe;
	reg [8:0] bit_count;
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
	reg ack_flag;
	always @(posedge clk or posedge reset)
		if (reset) begin
			bit_count <= 0;
			ack_flag <= 0;
		end
		else if (rx_start) begin
			bit_count <= 0;
			ack_flag <= 0;
		end
		else if (strobe) begin
			if (ack_flag) begin
				ack_flag <= 0;
				bit_count <= bit_count + 1;
			end
			else if (bit_count[2:0] == 7) begin
				ack_flag <= 1;
			end
			else begin
				ack_flag <= 0;
				bit_count <= bit_count + 1;
			end
		end
	
	// Send out ACK bits as appropriate
	assign tx_data = ack_flag ? (!ack) : tx_content;

	// Pass on content once we've decoded the address and we aren't ACK'ing
	assign content_strobe = pkt_addressed && strobe && (!ack_flag);
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
					// Got a bit. Is this the last one in our address byte?
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
 * An I2C slave that lets the host write to a single arbitrary-width register. There is
 * no address pointer, as with I2C memories- all data addressed to this device
 * will be stored in the output register.
 *
 * reg_out will always be valid with the current contents of the register.
 * All new bits are latched into reg_out simultaneously after the I2C stop condition.
 * Writes of the wrong length will be ignored.
 */
module i2c_slave_reg (clk, reset, i2c_interface_tx, i2c_interface_rx, reg_out);

	parameter I2C_ADDRESS = 0;
	parameter WIDTH = 8;
	parameter DEFAULT_VALUE = 0;

	input clk, reset;
	output [1:0] i2c_interface_tx;
	input [20:0] i2c_interface_rx;
	output [WIDTH-1:0] reg_out;

	// Expand on-chip busses
	wire tx_content, ack;
	assign i2c_interface_tx = {tx_content, ack};
	wire rx_stop = i2c_interface_rx[20];
	wire rx_content = i2c_interface_rx[19];
	wire content_strobe = i2c_interface_rx[18];
	wire [8:0] bit_count = i2c_interface_rx[17:9];
	wire [6:0] pkt_address = i2c_interface_rx[8:2];
	wire pkt_read_wr = i2c_interface_rx[1];
	wire pkt_addressed = i2c_interface_rx[0];
	
	// Match our device address
	wire dev_addressed = (pkt_address == I2C_ADDRESS) && pkt_addressed;
	
	// We have no content to transmit. Acknowledge when we're addressed for
	// writing, since we have no read interface yet.
	assign tx_content = 1;
	assign ack = dev_addressed && (!pkt_read_wr);
	
	// Shift register and output register. We shift in new data when this
	// device has been addressed and we're getting content bits. These shifted
	// in bits get copied to our output register only if we get a stop condtition
	// and we've received at least enough bits.

	reg [WIDTH-1:0] shifter;
	reg [WIDTH-1:0] reg_out;

	always @(posedge clk or posedge reset)
		if (reset) begin
			shifter <= 0;
			reg_out <= DEFAULT_VALUE;
		end
		else if (content_strobe && dev_addressed && bit_count < WIDTH + 8) begin
			shifter <= { shifter[WIDTH-2:0], rx_content };
		end
		else if (rx_stop && bit_count >= WIDTH + 8) begin
			reg_out <= shifter;
		end
endmodule	

/* The End */
