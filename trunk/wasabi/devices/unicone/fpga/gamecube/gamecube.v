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

	// Shared I2C frontend
	wire [1:0] i2c_interface_tx_1;
	wire [1:0] i2c_interface_tx_2;
	wire [20:0] i2c_interface_rx;
	i2c_frontend i2c_fe(
		clk, reset, 
		scl, sda,
		{ i2c_interface_tx_1[1] && i2c_interface_tx_2[1], 
		  i2c_interface_tx_1[0] || i2c_interface_tx_2[0] },
		i2c_interface_rx);

	/* Add an I2C-addressable LED brightness control */
	pwm_i2c #(7'h21) led_core(
		clk, reset,
		i2c_interface_tx_1, i2c_interface_rx,
		led);

	/* Four I2C-addressable Gamecube controller emulators */
	gc_i2c #(7'h40) gc_core(
		clk, reset,
		i2c_interface_tx_2, i2c_interface_rx,
		gc_ports);
endmodule


/*
 * An I2C-addressable quad gamecube controller emulator. This appears
 * as a 32-byte I2C memory device containing the 8-byte status
 * of each controller.
 */
module gc_i2c (clk, reset,
               i2c_interface_tx, i2c_interface_rx,
               gc_ports);
        parameter I2C_ADDRESS = 0;

	input clk, reset;
	output [1:0] i2c_interface_tx;
	input [20:0] i2c_interface_rx;
	inout [3:0] gc_ports;

	/* Our I2C core, a dual-port SRAM */
	reg [7:0] ram_addr;
	wire ram_data;
	i2c_slave_256bit_sram #(I2C_ADDRESS) i2c_ram(
		clk, reset, i2c_interface_tx, i2c_interface_rx,
		ram_addr, ram_data);

	/* Multiplex access to the SRAM.
	 * The ram_mux sequences access to the RAM, generating address/data pairs
	 * synchronized with each other. The individual controller ports have a say
	 * in which addresses the mux visits, and they can observe the address/data pair
	 * to see if the current address is useful to them.
	 */
	wire [7:0] port1_addr;
	wire [7:0] port2_addr;
	wire [7:0] port3_addr;
	wire [7:0] port4_addr;
	wire [7:0] ram_addr_out;
	wire ram_data_out;
	sram_mux4 ram_mux(clk, reset, ram_addr, ram_data,
	                  ram_data_out, ram_addr_out,
	                  port1_addr, port2_addr, port3_addr, port4_addr);

	/* A shared tranmsitter timebase for all controller ports */
	wire [2:0] tx_timing;
	n_serial_tx_timebase tx_timebase(clk, reset, tx_timing);

	/* The controller simulator cores, one for each port. They
	 * access the above RAM as it's available to them, servicing
	 * requests coming in from the GC. Each controller gets status
	 * bits from the GC during each polling cycle, including the
	 * state of the rumble motor.
	 */
	wire [3:0] rumble;
	gc_controller #(0) port1(clk, reset, tx_timing, gc_ports[0], rumble[0],
	                         port1_addr, ram_addr_out, ram_data_out);
	gc_controller #(1) port2(clk, reset, tx_timing, gc_ports[1], rumble[1],
	                         port2_addr, ram_addr_out, ram_data_out);
	gc_controller #(2) port3(clk, reset, tx_timing, gc_ports[2], rumble[2],
	                         port3_addr, ram_addr_out, ram_data_out);
	gc_controller #(3) port4(clk, reset, tx_timing, gc_ports[3], rumble[3],
	                         port4_addr, ram_addr_out, ram_data_out);
endmodule


/*
 * 4-way SRAM multiplexer. We have four clients giving us addresses to visit.
 * In turn, we produce synchronized pairs of address and data from the SRAM.
 */
module sram_mux4 (clk, reset, ram_addr, ram_data,
                  ram_data_out, ram_addr_out,
                  client1_addr, client2_addr, client3_addr, client4_addr);
	parameter ADDR_WIDTH = 8;

	input clk, reset;
	output [ADDR_WIDTH-1:0] ram_addr;
	input ram_data;
	output ram_data_out;
	output [ADDR_WIDTH-1:0] ram_addr_out;
	input [ADDR_WIDTH-1:0] client1_addr;
	input [ADDR_WIDTH-1:0] client2_addr;
	input [ADDR_WIDTH-1:0] client3_addr;
	input [ADDR_WIDTH-1:0] client4_addr;
	
	reg [ADDR_WIDTH-1:0] ram_addr;
	reg [ADDR_WIDTH-1:0] ram_addr_out;
	reg ram_data_out;
	reg [1:0] current_client;
	
	reg [1:0] state;
	parameter
		S_LOAD_ADDR = 0,
		S_WAIT = 1,
		S_UPDATE_RESULT = 2;

	always @(posedge clk or posedge reset)
		if (reset) begin
			ram_addr_out <= 0;
			ram_data_out <= 0;
			ram_addr <= 0;
			current_client <= 0;
			state <= S_LOAD_ADDR;
		end
		else case (state)

			S_LOAD_ADDR: begin
				// Put the current client's request on the SRAM's inputs
				if (current_client == 0)
					ram_addr <= client1_addr;
				else if (current_client == 1)
					ram_addr <= client2_addr;
				else if (current_client == 2)
					ram_addr <= client3_addr;
				else
					ram_addr <= client4_addr;
				state <= S_WAIT;
			end
			
			S_WAIT: state <= S_UPDATE_RESULT;
			
			S_UPDATE_RESULT: begin
				// Save this address/data pair
				ram_data_out <= ram_data;
				ram_addr_out <= ram_addr;
				
				// Next client
				current_client <= current_client + 1;
				state <= S_LOAD_ADDR;
			end

		endcase
endmodule


/*
 * The controller emulator state machine. This receives requests
 * from the console, and transmits back responses to emulate
 * a standard Nintendo Gamecube controller.
 *
 * This emulator responds to the following request words from the gamecube:
 *
 *    0x00      - Request controller ID.
 *                Returns 09 00 00, the same ID returned by standard gamecube
 *                controllers.
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
 *                we return ideal calibrations:
 *
 *                  00 80 80 80 80 80 00 00 02 02
 *
 * There are no other 1-byte commands that official controllers respond do.
 * I haven't checked for other three-byte commands yet.
 *
 * This retrieves the controller state as it is needed using an interface designed
 * for time-multiplexed access to SRAM. The required address, in bits, is placed on
 * state_req_addr. The actual address and data available from the SRAM are given in
 * state_in_addr and state_in_data.
 *
 */
module gc_controller (clk, reset, tx_timing, gc_port, rumble,
                      state_req_addr, state_in_addr, state_in_data);
	parameter SRAM_PAGE = 0;

	input clk, reset;
	input [2:0] tx_timing;
	inout gc_port;
	
	output rumble;
	output [7:0] state_req_addr;
	input [7:0] state_in_addr;
	input state_in_data;

	// The bit being transmitted or received
	reg [6:0] bit_count;
	
	// Split our GC port into separate transmit and receive wires
	wire tx, rx;
	n_serial_io_buffer iobuffer(clk, reset, gc_port, tx, rx);

	// Hook a bit generator up to the transmit output,
	// and a bit detector to our receive input. These give us
	// a simple serial interface to the raw codes used on the wire.
	wire tx_busy;
	reg tx_strobe, tx_data;
	wire rx_start, rx_stop, rx_error, rx_data, rx_strobe;
	n_serial_bit_generator bit_gen(clk, reset, tx_timing, tx_busy,
	                               tx_data, tx_strobe, tx);
	n_serial_bit_detector bit_det(clk, reset, rx, rx_start, rx_stop,
	                              rx_error, rx_data, rx_strobe);
	
	// For each type of request we can receive, keep track
	// of whether it's still possible that we'll be receiving
	// it. If we hit a bit that doesn't match, clear the associated bit.
	// Each command, when active, is responsible for supplying three signals
	// to the transmitter:
	//
	//  - resp_*_data
	//    The next bit to transmit (includes stop bit)
	//  - resp_*_ack
	//    One, if and only if txs_data is valid
	//  - resp_*_done
	//    A function of bit_count indicating whether we're done transmitting
	//
	
	// ID request: 00
	reg req_id_matched;
	wire req_id_complete = req_id_matched && rx_stop && (bit_count == 8);
	always @(posedge clk or posedge reset)
		if (reset)
			req_id_matched <= 1;
		else if (rx_start)
			req_id_matched <= 1;
		else if (!req_id_matched)
			req_id_matched <= 0;
		else if (rx_strobe)
			req_id_matched <= !rx_data;    // All zeroes

	// ID response: 09 00 00 stop
	wire resp_id_data = (bit_count == 4) || (bit_count == 7) || (bit_count == 24);
	wire resp_id_ack = 1;
	wire resp_id_done = bit_count > 24;

	// Get origins: 41
	reg req_origins_matched;
	wire req_origins_complete = req_origins_matched && rx_stop && (bit_count == 8);
	always @(posedge clk or posedge reset)
		if (reset)
			req_origins_matched <= 1;
		else if (rx_start)
			req_origins_matched <= 1;
		else if (!req_origins_matched)
			req_origins_matched <= 0;
		else if (rx_strobe)
			case (bit_count)
				0:  req_origins_matched <= !rx_data;  // Expect 0
				1:  req_origins_matched <=  rx_data;  // Expect 1
				2:  req_origins_matched <= !rx_data;  // Expect 0
				3:  req_origins_matched <= !rx_data;  // Expect 0		
				4:  req_origins_matched <= !rx_data;  // Expect 0		
				5:  req_origins_matched <= !rx_data;  // Expect 0		
				6:  req_origins_matched <= !rx_data;  // Expect 0		
				7:  req_origins_matched <=  rx_data;  // Expect 1
				default: req_origins_matched <= 1;    // Don't care		
			endcase

	// Get origins response: 00 80 80 80 80 80 00 00 02 02 stop
	wire resp_origins_data = (bit_count == 8)  || (bit_count == 16) ||
	                         (bit_count == 24) || (bit_count == 32) ||
	                         (bit_count == 40) || (bit_count == 70) ||
	                         (bit_count == 78) || (bit_count == 80);
	wire resp_origins_ack = 1;
	wire resp_origins_done = bit_count > 80;		

	// Get status: 40 03 XX
	// The last byte has flags sent from GC to controller:
	//   Bit 0:  Rumble flag
	reg req_status_matched;
	reg rumble;
	wire req_status_complete = req_status_matched && rx_stop && (bit_count == 24);
	always @(posedge clk or posedge reset)
		if (reset) begin
			req_status_matched <= 1;
			rumble <= 0;
		end
		else if (rx_start)
			req_status_matched <= 1;
		else if (!req_status_matched)
			req_status_matched <= 0;
		else if (rx_strobe)
			case (bit_count)
				0:  req_status_matched <= !rx_data;  // Expect 0
				1:  req_status_matched <=  rx_data;  // Expect 1
				2:  req_status_matched <= !rx_data;  // Expect 0
				3:  req_status_matched <= !rx_data;  // Expect 0
				4:  req_status_matched <= !rx_data;  // Expect 0
				5:  req_status_matched <= !rx_data;  // Expect 0
				6:  req_status_matched <= !rx_data;  // Expect 0
				7:  req_status_matched <= !rx_data;  // Expect 0
				8:  req_status_matched <= !rx_data;  // Expect 0
				9:  req_status_matched <= !rx_data;  // Expect 0
				10: req_status_matched <= !rx_data;  // Expect 0
				11: req_status_matched <= !rx_data;  // Expect 0
				12: req_status_matched <= !rx_data;  // Expect 0
				13: req_status_matched <= !rx_data;  // Expect 0
				14: req_status_matched <=  rx_data;  // Expect 1
				15: req_status_matched <=  rx_data;  // Expect 1
				23: begin
				    req_status_matched <= 1;         // Don't care
				    rumble <= rx_data;               // Save rumble bit
				    end
				default: req_status_matched <= 1;    // Don't care		
			endcase

	// Status response: 64 bits, read as we need them from external RAM, plus stop bit
	wire resp_status_data = (bit_count >= 64) ? 1 : state_in_data;
	wire resp_status_ack = (bit_count >= 64) ? 1 : (state_req_addr == state_in_addr);
	wire resp_status_done = bit_count > 64;
	wire [1:0] sram_page_bits = SRAM_PAGE;
	assign state_req_addr = {sram_page_bits, bit_count[5:0]};

	// Multiplex the various receive and transmit modes for each request.
	// This in effect creates a four-state machine determining which command
	// we're transmitting, with the above receive detectors determining the next state.
	// This all provides a single interface between all requests and a generic
	// receive/transmit state machine.
	parameter
		REQ_NONE    = 3'b000,
		REQ_STATUS  = 3'b100,
		REQ_ID      = 3'b010,
		REQ_ORIGINS = 3'b001;
	wire [2:0] rx_request = {req_status_complete, req_id_complete, req_origins_complete};
	reg [2:0] tx_request;
	wire resp_current_data = (tx_request == REQ_ID)      ? resp_id_data :
	                         (tx_request == REQ_STATUS)  ? resp_status_data :
	                         (tx_request == REQ_ORIGINS) ? resp_origins_data : 0;
	wire resp_current_ack  = (tx_request == REQ_ID)      ? resp_id_ack :
	                         (tx_request == REQ_STATUS)  ? resp_status_ack :
	                         (tx_request == REQ_ORIGINS) ? resp_origins_ack : 0;
	wire resp_current_done = (tx_request == REQ_ID)      ? resp_id_done :
	                         (tx_request == REQ_STATUS)  ? resp_status_done :
	                         (tx_request == REQ_ORIGINS) ? resp_origins_done : 1;

	// Keep track of the current bit count.
	// If tx_request == REQ_NONE, we're receiving- the bit count is controlled by
	// rx_start. Otherwise, the bit count is controlled by a small state machine
	// that pumps data into our transmitter.
	reg wait_for_tx;
	always @(posedge clk or posedge reset)
		if (reset) begin
			bit_count <= 0;
			tx_request <= REQ_NONE;
			tx_strobe <= 0;
			tx_data <= 0;
			wait_for_tx <= 0;
		end
		else begin

			if (tx_request == REQ_NONE) begin
				// Our transmitter is idle, we're receiving

				// Have we just finished receiving something?
				if (rx_request != REQ_NONE) begin
					// Good. Start transmitting
					tx_request <= rx_request;
					bit_count <= 0;
					tx_strobe <= 0;
				end
				else begin
					// Keep receiving, tracking the number
					// of bits captured so far without
					// allowing rollovers.

					tx_strobe <= 0;
					if (rx_start)
						bit_count <= 0;
					else if (bit_count == 127)
						bit_count <= 127;
					else if (rx_strobe)
						bit_count <= bit_count + 1;
				end
				
				wait_for_tx <= 0;
			end

			else if (wait_for_tx) begin
				// Kill one cycle immediately after we transmit something,
				// to let the transmitter buffer update itself.
				wait_for_tx <= 0;
				tx_strobe <= 0;
				bit_count <= bit_count + 1;
			end

			else begin
				// We're in the middle of transmitting something.
				// It doesn't matter what, since above we've multiplexed
				// all flow control signals into resp_current_ack and
				// resp_current_done.
				
				if (resp_current_done) begin
					// We just finished transmitting the whole thing, including stop bit
					tx_request <= REQ_NONE;
					bit_count <= 0;
					tx_strobe <= 0;
					wait_for_tx <= 0;
				end
				else if (resp_current_ack && !tx_busy) begin
					// We happen to have data available and room in the transmitter.
					// Strobe out one bit.
					tx_strobe <= 1;
					tx_data <= resp_current_data;
					wait_for_tx <= 1;
				end
			end
		end
endmodule


/*
 * A variation on gc_controller that includes a builtin
 * timebase. This is mostly intended for convenience when simulating.
 */
module gc_controller_with_timebase (clk, reset, gc_port, rumble,
                                    state_addr, state_data, state_ack);
	input clk, reset;
	inout gc_port;
	output rumble;
	output [5:0] state_addr;
	input state_data, state_ack;
	
	wire [2:0] tx_timing;
	n_serial_tx_timebase timebase(clk, reset, tx_timing);

        gc_controller gcc(clk, reset, tx_timing, gc_port, rumble, state_addr,
                          state_data, state_ack);
endmodule

/* The End */

