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
	wire [1:0] int_sda_out;
	i2c_io_buffer i2cbuf(
		clk, reset, scl, sda,
		int_scl, int_sda_in, int_sda_out == 2'b11);
	
	/* Add an I2C-addressable LED brightness control */
	pwm16_i2c #(7'h21) led_core(
		clk, reset, 
		int_scl, int_sda_in, int_sda_out[0],
		led);
		
	/* Four I2C-addressable Gamecube controller emulators */
	gc_i2c #(7'h40) gc_core(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[1],
		gc_ports);
endmodule


/*
 * An I2C-addressable quad gamecube controller emulator. This appears
 * as a 32-byte I2C memory device containing the 8-byte status
 * of each controller.
 */
module gc_i2c (clk, reset,
               scl, sda_in, sda_out,
               gc_ports);
        parameter I2C_ADDRESS = 0;
               
	input clk, reset;
	input scl, sda_in;
	output sda_out;
	inout [3:0] gc_ports;

	/* Our I2C core, a dual-port SRAM */
	wire [4:0] ram_addr;
	wire [4:0] ram_data;
	i2c_slave_32byte_sram i2c_ram(clk, reset, scl, sda_in, sda_out,
	                              ram_addr, ram_data);

	/* Share this RAM among our four controller emulators.
	 * This also maps each port into a different quarter of our
	 * RAM's address space.
	 */
	wire [2:0] port1_addr;
	wire [2:0] port2_addr;
	wire [2:0] port3_addr;
	wire [2:0] port4_addr;
	wire port1_ack, port1_request;
	wire port2_ack, port2_request;
	wire port3_ack, port3_request;
	wire port4_ack, port4_request;
	sram_arbiter4 ram_share(clk, reset, ram_addr,
		{ 2'b00, port1_addr }, port1_request, port1_ack,
		{ 2'b01, port2_addr }, port2_request, port2_ack,
		{ 2'b10, port3_addr }, port3_request, port3_ack,
		{ 2'b11, port4_addr }, port4_request, port4_ack);

	/* The I/O buffer gives us clean synchronized unidirectional
	 * tx and rx signals for controlling and monitoring the GC port
	 */
	wire [3:0] tx;
	wire [3:0] rx;
	n_serial_io_buffer iobuffer1(clk, reset, gc_ports[0], tx[0], rx[0]);
	n_serial_io_buffer iobuffer2(clk, reset, gc_ports[1], tx[1], rx[1]);
	n_serial_io_buffer iobuffer3(clk, reset, gc_ports[2], tx[2], rx[2]);
	n_serial_io_buffer iobuffer4(clk, reset, gc_ports[3], tx[3], rx[3]);

	/* The controller simulator core, one for each port. It makes 
	 * requests for each byte of controller state it needs, as it's
	 * inefficient to pass the whole 64-bit monstrosity around.
	 * Each core also gives us a rumble bit.
	 */
	wire [3:0] rumble;
	gc_controller port1(clk, reset, tx[0], rx[0], rumble[0],
	                    port1_addr, port1_request, ram_data, port1_ack);
	gc_controller port2(clk, reset, tx[1], rx[1], rumble[1],
	                    port2_addr, port2_request, ram_data, port2_ack);
	gc_controller port3(clk, reset, tx[2], rx[2], rumble[2],
	                    port3_addr, port3_request, ram_data, port3_ack);
	gc_controller port4(clk, reset, tx[3], rx[3], rumble[3],
	                    port4_addr, port4_request, ram_data, port4_ack);
endmodule


/*
 * The controller emulator state machine. This receives requests
 * from the console, and transmits back responses to emulate
 * a standard Nintendo Gamecube controller. 
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
 * This retrieves the controller state as it is needed using a memory-friendly
 * interface. The required address is placed on 'state_addr' and a rising edge
 * is applied to state_request. Once the data corresponding with that address
 * is valid at state_data, state_ack must be brought high. This is compatible
 * with sram_arbiter in util.v, so several similar devices can share one memory.
 *
 * Latency between address and data is not critical, since our transmitter core
 * includes double-buffering.
 *
 */
module gc_controller (clk, reset, tx, rx, rumble,
                      state_addr, state_request, state_data, state_ack);
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
	output rumble;
	output [2:0] state_addr;
	output state_request;
	input [7:0] state_data;
	input state_ack;

	reg rumble;
	reg state_request;
	reg [7:0] rx_poll_flags;

	// Share one byte count register for all types of responses
	reg [4:0] byte_count;
	wire [3:0] state_addr = byte_count[3:0];

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

	// state_ack is edge triggered
	wire state_ack_edge;
	rising_edge_detector state_ack_edgedet(clk, reset, state_ack, state_ack_edge);

	reg [3:0] state;
	parameter
		/* Receiving commands */
		S_IDLE = 0,
		S_RX_CMD_0 = 1,
		S_RX_POLL_CMD_1 = 2,
		S_RX_POLL_CMD_2 = 3,
		
		/* Acting on commands, setting up responses */
		S_FINISH_ID_COMMAND = 4,
		S_FINISH_POLL_COMMAND = 5,
		S_FINISH_ORIGINS_COMMAND = 6,

		/* Transmitting controller state */
		S_TX_STATE_WAIT_FOR_RAM = 7,
		S_TX_STATE_WAIT_FOR_TRANSMITTER = 8,
		S_TX_STATE_WAIT = 9,

		/* Transmitting controller ID */
		S_TX_ID_BYTE = 10,
		S_TX_ID_WAIT = 11,

		/* Transmitting 'get origins' response */
		S_TX_ORIGINS_BYTE = 12,
		S_TX_ORIGINS_WAIT = 13,
		
		/* Common to all transmissions */
		S_TX_STOPBIT = 14;

	always @(posedge clk or posedge reset)
		if (reset) begin
		
			tx_stopbit <= 0;
			tx_strobe <= 0;
			tx_data <= 0;

			rumble <= 0;
			rx_poll_flags <= 0;
			
			byte_count <= 0;
			state_request <= 0;

			state <= S_IDLE;

		end
		else case (state)

			/************************* Receiving commands ****/

			S_IDLE: begin
				// We got nothing. Wait for the beginning of a received request
				if (rx_start)
					state <= S_RX_CMD_0;
				tx_stopbit <= 0;
				tx_strobe <= 0;
				tx_data <= 0;
				byte_count <= 0;
				state_request <= 0;
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

			
			/************************* Acting on commands, setting up responses ****/
			
			S_FINISH_ID_COMMAND: begin
				// We've recieved an ID command's start pulse and content- wait for a stop,
				// to conclude the ID command, then start sending the response.
				if (rx_start || rx_error || rx_strobe)
					state <= S_IDLE;
				else if (rx_stop) begin
					byte_count <= 0;
					state <= S_TX_ID_BYTE;
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
					byte_count <= 0;
					state <= S_TX_STATE_WAIT_FOR_RAM;					
				end
			end
	
			S_FINISH_ORIGINS_COMMAND: begin
				// We've recieved a 'get origins' command. Wait for the stop, then assemble
				// and start sending a big response.
				if (rx_start || rx_error || rx_strobe)
					state <= S_IDLE;
				else if (rx_stop) begin
					byte_count <= 0;
					state <= S_TX_ORIGINS_BYTE;
				end
			end		


			/************************* Transmitting controller state ****/
				
			S_TX_STATE_WAIT_FOR_RAM: begin
				// We're ready to send the controller state, but we need
				// to wait for a data byte from our RAM interface
				if (state_ack_edge) begin
					// Yay, we got a byte. Pass it on.
					tx_data <= state_data;
					state_request <= 0;
					tx_stopbit <= 0;
					tx_strobe <= 1;
					state <= S_TX_STATE_WAIT;
				end
				else begin
					// Keep asking for one
					state_request <= 1;
				end
			end
			
			S_TX_STATE_WAIT: begin
				// Do nothing for one cycle while we let the transmitter update its flags
				state <= S_TX_STATE_WAIT_FOR_TRANSMITTER;
				tx_strobe <= 0;
			end

			S_TX_STATE_WAIT_FOR_TRANSMITTER: begin
				// We recently sent a controller state byte. Wait for
				// the transmitter to become available again, then
				// head to either the next state address or finish.
				if (!tx_busy) begin
					if (state_addr == 7) begin
						state <= S_TX_STOPBIT;
					end
					else begin
						state <= S_TX_STATE_WAIT_FOR_RAM;
						byte_count <= byte_count + 1;
					end
				end
			end


			/************************* Transmitting controller ID ****/

			S_TX_ID_BYTE: begin
				// Send another byte of the controller ID if we can
				if (!tx_busy) begin
					case (byte_count)
						0:   tx_data <= CONTROLLER_ID[23:16];
						1:   tx_data <= CONTROLLER_ID[15:8];
						2:   tx_data <= CONTROLLER_ID[7:0];
					endcase
					tx_strobe <= 1;
					tx_stopbit <= 0;
					state <= S_TX_ID_WAIT;
					byte_count <= byte_count + 1;
				end
			end
			
			S_TX_ID_WAIT: begin
				// Give the transmitter one cycle to update status
				tx_strobe <= 0;
				if (byte_count == 3)
					state <= S_TX_STOPBIT;
				else
					state <= S_TX_ID_BYTE;
			end


			/************************* Transmitting 'get origins' response ****/

			S_TX_ORIGINS_BYTE: begin
				// Send another byte of the controller origins response if we can
				if (!tx_busy) begin
					case (byte_count)

						// First two bytes are supposed to be the current button status.
						// To avoid fetching it from RAM when this really isn't that
						// important, we just pretend everything's inactive (0x0080)
						0:   tx_data <= 8'h00;
						1:   tx_data <= 8'h80;

						// Report ideal origins for all axes
						2:   tx_data <= 8'h80;
						3:   tx_data <= 8'h80;
						4:   tx_data <= 8'h80;
						5:   tx_data <= 8'h80;
						6:   tx_data <= 8'h00;
						7:   tx_data <= 8'h00;
					
						// The last two bytes are, for now, magic numbers
						8:   tx_data <= 8'h02;
						9:   tx_data <= 8'h02;
					
					endcase
					tx_strobe <= 1;
					tx_stopbit <= 0;
					state <= S_TX_ORIGINS_WAIT;
					byte_count <= byte_count + 1;
				end
			end
			
			S_TX_ORIGINS_WAIT: begin
				// Give the transmitter one cycle to update status
				tx_strobe <= 0;
				if (byte_count == 10)
					state <= S_TX_STOPBIT;
				else
					state <= S_TX_ORIGINS_BYTE;
			end


			/************************* Common to all transmissions ****/

			S_TX_STOPBIT: begin
				// Transmit a stop bit next chance we get, then head back to idle
				if (!tx_busy) begin
					tx_stopbit <= 1;
					tx_strobe <= 1;
					state <= S_IDLE;
				end
			end

		endcase
endmodule

/* The End */

