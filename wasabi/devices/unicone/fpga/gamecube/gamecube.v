/*
 * gamecube.v - Top-level module for the Unicone's gamecube emulation mode
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

module gamecube (clk, reset,
                 led, scl, sda,
                 gc_ports);

	input clk, reset;
	output led;
	input scl;
	inout sda;
	inout [3:0] gc_ports;
	
	/* Buffer between external 2-wire I2C and on-chip 3-wire I2C */
	wire int_scl, int_sda_in;
	wire [4:0] int_sda_out;
	i2c_io_buffer i2cbuf(
		clk, reset, scl, sda,
		int_scl, int_sda_in, int_sda_out == 5'b11111);
	
	/* Add an I2C-addressable LED brightness control */
	pwm16_i2c #(7'h21) led_controller(
		clk, reset, 
		int_scl, int_sda_in, int_sda_out[0],
		led);
		
	/* Four I2C-addressable Gamecube controller emulators, with different addresses */
	gc_i2c #(7'h40) controller_1(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[1],
		gc_ports[0]);
	gc_i2c #(7'h41) controller_2(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[2],
		gc_ports[1]);
	gc_i2c #(7'h42) controller_3(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[3],
		gc_ports[2]);
	gc_i2c #(7'h43) controller_4(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[4],
		gc_ports[3]);
endmodule


/*
 * An I2C-addressable Gamecube controller emulator. This combines
 * the controller emulator core with I/O buffers and an I2C slave.
 */
module gc_i2c (clk, reset,
               scl, sda_in, sda_out,
               gc_port);
	input clk, reset;
	input scl, sda_in;
	output sda_out;
	input gc_port;
	
	/* The I/O buffer gives us clean synchronized unidirectional
	 * tx and rx signals for controlling and monitoring the GC port
	 */
	wire tx, rx;
	n_serial_io_buffer iobuffer(clk, reset, gc_port, tx, rx);

	assign tx = 1'b1;
endmodule


/*
 * The controller emulator state machine. This receives requests
 * from the console, and transmits back responses to emulate
 * a standard Nintendo Gamecube controller. It emulates the controller
 * identification request, and controller status. Status is returned
 * as an 8-byte packet of all analog and button status.
*/
module gc_controller (clk, reset, tx, rx, controller_state, rumble);
	/*
	 * Controller IDs are expained in the gamecube-linux
	 * project's YAGCD:
	 *
	 * http://www.gc-linux.org/docs/yagcd/chap9.html
	 */
	parameter CONTROLLER_ID = 24'h090000;

	input clk, reset;
	input rx;
	output tx;
	input [63:0] controller_state;
	output rumble;

	reg rumble;
	reg [7:0] rx_poll_flags;
	reg [63:0] latched_controller_state;
	reg [3:0] tx_byte_count;
	reg [23:0] latched_controller_id;

	wire rx_start, rx_stop, rx_error, rx_strobe;
	wire [7:0] rx_data;
	n_serial_rx rx_core(clk, reset, rx,
                            rx_start, rx_stop, rx_error,
                            rx_data, rx_strobe);

	wire tx_busy;
	reg tx_stopbit, tx_strobe;
	reg [7:0] tx_data;
	n_serial_tx tx_core(clk, reset, tx_busy, tx_stopbit,
	                    tx_data, tx_strobe, tx);

	reg [3:0] state;
	parameter
		S_IDLE = 0,
		S_RX_CMD_0 = 1,
		S_RX_POLL_CMD_1 = 2,
		S_RX_POLL_CMD_2 = 3,
		S_FINISH_ID_COMMAND = 4,
		S_FINISH_POLL_COMMAND = 5,
		S_TX_STOPBIT = 6,
		S_TX_ID = 7,
		S_TX_ID_WAIT = 8,
		S_TX_STATE = 9,
		S_TX_STATE_WAIT = 10;

	always @(posedge clk or posedge reset)
		if (reset) begin

			tx_stopbit <= 0;
			tx_strobe <= 0;
			tx_data <= 0;
			rumble <= 0;
			rx_poll_flags <= 0;
			tx_byte_count <= 0;
			latched_controller_state <= 0;
			latched_controller_id <= 0;
			state <= S_IDLE;

		end
		else case (state)

			S_IDLE: begin
				// We got nothing. Wait for the beginning of a received request
				if (rx_start)
					state <= S_RX_CMD_0;
				tx_stopbit <= 0;
				tx_strobe <= 0;
				tx_data <= 0;
				tx_byte_count <= 0;
			end

			S_RX_CMD_0: begin
				// A request has begun, but we haven't received any of its bytes
				// yet. When we get one, if it's an identification request (0x00)
				// we can act on it immediately. If it's the beginning of a polling
				// command (0x40030) wait for the rest.
				if (rx_stop || rx_start || rx_error)
					state <= S_IDLE;
				else if (rx_strobe) begin
					
					if (rx_data == 8'h00)
						/* An identification request command, send the 1st ID byte */
						state <= S_FINISH_ID_COMMAND;
					else if (rx_data == 8'h40)
						/* The beginning of a polling command */
						state <= S_RX_POLL_CMD_1;
					else
						/* Something we don't care about */
						state <= S_IDLE;
						
				end
			end
			
			S_RX_POLL_CMD_1: begin
				// We have the first byte of a polling command, look for the second
				if (rx_stop || rx_start || rx_error)
					state <= S_IDLE;
				else if (rx_strobe) begin
					if (rx_data == 8'h03)
						state <= S_RX_POLL_CMD_2;
					else
						state <= S_IDLE;
				end
			end
			
			S_RX_POLL_CMD_2: begin
				// We have the first two bytes of a polling command, accept the last one.
				// We don't validate this byte, since it seems to be a status bitfield
				// rather than a command ID- bit 1 has some unknown use, and bit 0 controls
				// the rumble motor. Store the important parts of this command bit and go
				// on to the next state.
				if (rx_stop || rx_start || rx_error)
					state <= S_IDLE;
				else if (rx_strobe) begin

					rx_poll_flags <= rx_data;
					state <= S_FINISH_POLL_COMMAND;				

				end
			end
			
			S_FINISH_ID_COMMAND: begin
				// We've recieved an ID command's start pulse and content- wait for a stop,
				// to conclude the ID command, then start sending the response.
				if (rx_start || rx_error || rx_strobe)
					state <= S_IDLE;
				else if (rx_stop) begin
					// Prepare to send a response
					latched_controller_id <= CONTROLLER_ID;
					tx_byte_count <= 0;
					state <= S_TX_ID;
				end
			end
			
			S_FINISH_POLL_COMMAND: begin
				// We've recieved a polling command's start pulse and content- wait for a stop,
				// to conclude the command, then start sending the response and latch our flags.
				if (rx_start || rx_error || rx_strobe)
					state <= S_IDLE;
				else if (rx_stop) begin
					// Save the rumble flag, now that we know the command
					// reception was successful.
					rumble <= rx_poll_flags[0];

					// Prepare to send a response
					latched_controller_state <= controller_state;
					tx_byte_count <= 0;
					state <= S_TX_STATE;					
				end
			end
			
			S_TX_STOPBIT: begin
				// Transmit a stop bit next chance we get, then head back to idle
				if (!tx_busy) begin
					tx_stopbit <= 1;
					tx_strobe <= 1;
					state <= S_IDLE;
				end
			end
			
			S_TX_ID: begin
				// Send back the controller ID
				if (!tx_busy) begin
					tx_data <= latched_controller_id[23:16];
					latched_controller_id <= { latched_controller_id[15:0], 8'h00 };
					tx_stopbit <= 0;
					tx_strobe <= 1;
					tx_byte_count <= tx_byte_count + 1;
					
					// Wait one cycle for the transmitter
					// to update its queue status
					state <= S_TX_ID_WAIT;
				end
			end
			
			S_TX_ID_WAIT: begin
				// Waiting for the transmitter to update itself.
				// Head back to another ID bit, or the stop bit.
				tx_strobe <= 0;
				if (tx_byte_count == 3)
					state <= S_TX_STOPBIT;
				else
					state <= S_TX_ID;
			end
			
			S_TX_STATE: begin
				// Send back the controller state
				if (!tx_busy) begin
					tx_data <= latched_controller_state[63:56];
					latched_controller_state <= { latched_controller_state[55:0], 8'h00 };
					tx_stopbit <= 0;
					tx_strobe <= 1;
					tx_byte_count <= tx_byte_count + 1;
					
					// Wait one cycle for the transmitter
					// to update its queue status
					state <= S_TX_ID_WAIT;
				end
			end

			S_TX_STATE_WAIT: begin
				// Waiting for the transmitter to update itself.
				// Head back to another ID bit, or the stop bit.
				tx_strobe <= 0;
				if (tx_byte_count == 8)
					state <= S_TX_STOPBIT;
				else
					state <= S_TX_STATE;
			end

		endcase
endmodule

/* The End */

