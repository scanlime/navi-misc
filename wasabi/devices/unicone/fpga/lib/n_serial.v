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
			rx_sync[2] <= sda_in_sync[1];
			
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
 */
module n_serial_timebase (clk, reset, tick);
	input clk, reset;
	output tick;
	reg tick;
	reg [5:0] counter;

	always @(posedge clk or posedge reset)
		if (reset) begin
			tick <= 0;
			counter <= 0;
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
	reg busy;
	input tx_stopbit;
	input [7:0] tx_data;
	input strobe;
	output serial_out;
	reg serial_out;
	
	/* 1us serial timebase */
	wire tick;
	n_serial_timebase ticker(clk, reset, tick);
	
	/* Accept incoming data from our user.
	 * This stores one byte in 'buffer' when
	 * the strobe transitions from low to high
	 * and sets 'busy'. The transmitter itself
	 * will then clear 'busy' after it has read
	 * the buffer's contents.
	 */
	reg [7:0] buffer;
	reg prev_strobe;
	always @(posedge clk or posedge reset)
		if (reset) begin
			busy <= 0;
			buffer <= 0;
			prev_strobe <= 1;
		end
		else begin
			prev_strobe <= strobe;
			if (
                    

/* The End */
