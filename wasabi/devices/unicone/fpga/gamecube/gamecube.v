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
	inout [0:0] gc_ports;

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
/*
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
*/

	assign int_sda_out[2] = 1'b1;
	assign int_sda_out[3] = 1'b1;
	assign int_sda_out[4] = 1'b1;
endmodule


/*
 * An I2C-addressable Gamecube controller emulator. This combines
 * the controller emulator core with I/O buffers and an I2C slave.
 */
module gc_i2c (clk, reset,
               scl, sda_in, sda_out,
               gc_port);
        parameter I2C_ADDRESS = 0;
               
	input clk, reset;
	input scl, sda_in;
	output sda_out;
	inout gc_port;

	/* The I/O buffer gives us clean synchronized unidirectional
	 * tx and rx signals for controlling and monitoring the GC port
	 */
	wire tx, rx;
	n_serial_io_buffer iobuffer(clk, reset, gc_port, tx, rx);

	/* The controller simulator core. It expects a 64-bit controller
	 * status word to send out on request, and gives us a rumble bit
	 * that updates whenever the GC polls.
	 */
	wire [63:0] controller_state;
	wire rumble;
	gc_controller controller(clk, reset, tx, rx, controller_state, rumble);

	// Big 64-bit-wide I2C-accessable register to hold the current controller state
	wire i2c_reg_strobe;
	i2c_slave_reg #(I2C_ADDRESS, 8) i2creg64(
		clk, reset,
		scl, sda_in, sda_out,
		controller_state, i2c_reg_strobe);
endmodule


/*
 * The controller emulator state machine. This receives requests
 * from the console, and transmits back responses to emulate
 * a standard Nintendo Gamecube controller. It emulates the controller
 * identification request, and controller status. Status is returned
 * as an 8-byte packet of all analog and button status.
 *
 * This emulator responds to the following request words from the gamecube:
 *
 *    0x00      - Request controller ID.
 *                Returns CONTROLLER_ID, which defaults to the same ID returned
 *                by standard gamecube controllers.
 *
 *    0x4003XX  - Poll controller status.
 *                The last byte of the command seems to be a collection of flags
 *                sent from console to controller. Most are unknown, but the LSB
 *                is the rumble motor flag. Returns the 8-byte controller state.
 *
 *    0x41      - Get origins.
 *                This command appears to get joystick calibration information
 *                from the controller. This returns a 10-byte packet. An official
 *                controller I was testing returned this one:
 *
 *                  00 80 84 7B 84 82 1D 1D 02 02
 *
 *                The first two bytes indicate button status, just as in the normal
 *                controller poll command. The next four bytes hold analog axes,
 *                just as in the normal status packet, but instead of current axis
 *                positions these bytes hold neutral positions calibrated at powerup.
 *                The last bytes appear to always be 0x02 0x02. They might indicate
 *                a hardcoded joystick tolerance or dead zone.
 *
 *                Since we're assuming our host has already calibrated each axis,
 *                we return ideal calibrations: the joystick and c-stick axes
 *                have origins at 0x80, and both triggers have origins at 0x00.
 *
 * There are no other 1-byte commands that official controllers respond do.
 * I haven't checked for other three-byte commands yet.
 *
 */
module gc_controller (clk, reset, tx, rx, controller_state, rumble);
	/*
	 * Controller IDs are expained in the gamecube-linux
	 * project's YAGCD:
	 *
	 * http://www.gc-linux.org/docs/yagcd/chap9.html
	 *
	 * This is the value that standard controllers are observed to return:
	 */
	parameter CONTROLLER_ID = 24'h090000;

	input clk, reset;
	input rx;
	output tx;
	input [63:0] controller_state;
	output rumble;

	reg rumble;
	reg [7:0] rx_poll_flags;

	/* Wide byte-at-a-time shift register. Must be wide enough to hold our
	 * 10-byte 'get origins' response. Smaller responses are left-justified.
	 */
	parameter SHIFTER_WIDTH = 80;
	reg [SHIFTER_WIDTH-1:0] shifter;
	reg [3:0] tx_bytes_remaining;

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
		S_FINISH_ORIGINS_COMMAND = 6,
		S_TX_STOPBIT = 7,
		S_TX_BYTE = 8,
		S_TX_WAIT = 9;

	always @(posedge clk or posedge reset)
		if (reset) begin

			tx_stopbit <= 0;
			tx_strobe <= 0;
			tx_data <= 0;
			rumble <= 0;
			rx_poll_flags <= 0;
			tx_bytes_remaining <= 0;
			shifter <= 0;
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
				tx_bytes_remaining <= 0;
			end

			S_RX_CMD_0: begin
				// A request has begun, but we haven't received any of its bytes yet
				if (rx_stop || rx_start || rx_error)
					state <= S_IDLE;
				else if (rx_strobe) begin
					
					if (rx_data == 8'h00) begin
						/* An identification request command */

						state <= S_FINISH_ID_COMMAND;

					end
					else if (rx_data == 8'h40) begin
						/* The beginning of a polling command */

						state <= S_RX_POLL_CMD_1;

					end
					else if (rx_data == 8'h41) begin
						/* A 'get origins' command */

						state <= S_FINISH_ORIGINS_COMMAND;

					end
					else begin
						/* An unknown command, ignore it */

						state <= S_IDLE;

					end						
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
					// Load our controller ID into the response buffer and start sending
					shifter[SHIFTER_WIDTH-1:SHIFTER_WIDTH-24] <= CONTROLLER_ID;
					tx_bytes_remaining <= 3;
					state <= S_TX_BYTE;
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
					shifter[SHIFTER_WIDTH-1:SHIFTER_WIDTH-64] <= controller_state;
					tx_bytes_remaining <= 8;
					state <= S_TX_BYTE;					
				end
			end
	
			S_FINISH_ORIGINS_COMMAND: begin
				// We've recieved a 'get origins' command. Wait for the stop, then assemble
				// and start sending a big response.
				if (rx_start || rx_error || rx_strobe)
					state <= S_IDLE;
				else if (rx_stop) begin
				
					// The first two bytes hold the current button status
					shifter[SHIFTER_WIDTH-1:SHIFTER_WIDTH-16] <= controller_state[63:48];
					
					// All axes have ideal origins
					shifter[SHIFTER_WIDTH-17:SHIFTER_WIDTH-64] <= 48'h808080800000;
					
					// The last two bytes, for now, are magic numbers
					shifter[SHIFTER_WIDTH-65:SHIFTER_WIDTH-80] <= 16'h0202;

					tx_bytes_remaining <= 10;
					state <= S_TX_BYTE;					
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
			
			S_TX_BYTE: begin
				// Send the next byte from our shifter
				if (!tx_busy) begin
					tx_data <= shifter[SHIFTER_WIDTH-1:SHIFTER_WIDTH-8];
					shifter <= { shifter[SHIFTER_WIDTH-9:0], 8'h00 };

					tx_stopbit <= 0;
					tx_strobe <= 1;
					tx_bytes_remaining <= tx_bytes_remaining - 1;
					
					// Wait one cycle for the transmitter
					// to update its queue status
					state <= S_TX_WAIT;
				end
			end
			
			S_TX_WAIT: begin
				// Waiting for the transmitter to update itself.
				// Head back to another ID bit, or the stop bit.
				tx_strobe <= 0;
				if (tx_bytes_remaining == 0)
					state <= S_TX_STOPBIT;
				else
					state <= S_TX_BYTE;
			end

		endcase
endmodule

/* The End */

