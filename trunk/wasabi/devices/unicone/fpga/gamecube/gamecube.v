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
	pwm_i2c #(7'h21) led_core(
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
	wire [7:0] ram_data;
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
	n_serial_io_buffer iobuffer2(clk, reset, gc_ports[1], tx[1], rx[1]);
	n_serial_io_buffer iobuffer3(clk, reset, gc_ports[2], tx[2], rx[2]);
	n_serial_io_buffer iobuffer4(clk, reset, gc_ports[3], tx[3], rx[3]);

	/* A shared tranmsitter timebase for all controller ports */
	wire [2:0] tx_timing;
	n_serial_tx_timebase tx_timebase(clk, reset, tx_timing);

	/* The controller simulator core, one for each port. It makes
	 * requests for each byte of controller state it needs, as it's
	 * inefficient to pass the whole 64-bit monstrosity around.
	 * Each core also gives us a rumble bit.
	 */
	wire [3:0] rumble;
	gc_controller port1(clk, reset, tx_timing,
	                    tx[0], rx[0], rumble[0],
	                    port1_addr, port1_request, ram_data, port1_ack);
	gc_controller port2(clk, reset, tx_timing,
	                    tx[1], rx[1], rumble[1],
	                    port2_addr, port2_request, ram_data, port2_ack);
	gc_controller port3(clk, reset, tx_timing,
	                    tx[2], rx[2], rumble[2],
	                    port3_addr, port3_request, ram_data, port3_ack);
	gc_controller port4(clk, reset, tx_timing,
	                    tx[3], rx[3], rumble[3],
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
 *                we return ideal calibrations, given in CONTROLLER_CALIBRATION.
 *
 * There are no other 1-byte commands that official controllers respond do.
 * I haven't checked for other three-byte commands yet.
 *
 * This retrieves the controller state as it is needed using a memory-friendly
 * one-bit-wide interface. The required address, in bits, is placed on state_addr
 * and state_request is brought high. Once state_ack goes high, the corresponding
 * bit should be available at state_data. The data source has a little less than
 * one bit time to make this happen- on the order of several hundred clock cycles.
 *
 */
module gc_controller (clk, reset, tx_timing, gc_port, rumble,
                      state_addr, state_request, state_data, state_ack,
                      debug);
	parameter CONTROLLER_ID = 24'h090000;
	parameter CONTROLLER_CALIBRATION = 80'h00808080808000000202;

	input clk, reset;
	input [2:0] tx_timing;
	inout gc_port;
	
	output rumble;
	output [5:0] state_addr;
	output state_request;
	input state_data;
	input state_ack;
	
	output [2:0] debug;

	// Split our GC port into separate transmit and receive wires
	wire tx, rx;
	n_serial_io_buffer iobuffer(clk, reset, gc_port, tx, rx);

	// Hook a bit generator up to the transmit output,
	// and a bit detector to our receive input. These give us
	// a simple serial interface to the raw codes used on the wire.
	wire tx_busy, tx_data, tx_strobe;
	wire rx_start, rx_stop, rx_error, rx_data, rx_strobe;
	n_serial_bit_generator bit_gen(clk, reset, tx_timing, tx_busy,
	                               tx_data, tx_strobe, tx);
	n_serial_bit_detector bit_det(clk, reset, rx, rx_start, rx_stop,
	                              rx_error, rx_data, rx_strobe);

	// Keep track of the number of bits received so far in this transfer,
	// without allowing rollovers.
	reg [6:0] bit_count;
	always @(posedge clk or posedge reset)
		if (reset)
			bit_count <= 0;
		else if (rx_start)
			bit_count <= 0;
		else if (bit_count == 127)
			bit_count <= 127;
		else if (rx_strobe)
			bit_count <= bit_count + 1;
	
	// For each type of request we can receive, keep track
	// of whether it's still possible that we'll be receiving
	// it. If we hit a bit that doesn't match, clear the associated bit.
	
	// ID request: 0x00
	reg req_id;
	wire req_id_complete = req_id && rx_stop && (bit_count == 8);
	always @(posedge clk or posedge reset)
		if (reset)
			req_id <= 1;
		else if (rx_start)
			req_id <= 1;
		else if (!req_id)
			req_id <= 0;
		else if (rx_strobe)
			req_id <= !rx_data;	// All bits zero

	// Get origins: 0x41
	reg req_origins;
	wire req_origins_complete = req_origins && rx_stop && (bit_count == 8);
	always @(posedge clk or posedge reset)
		if (reset)
			req_origins <= 1;
		else if (rx_start)
			req_origins <= 1;
		else if (!req_origins)
			req_origins <= 0;
		else if (rx_strobe)
			case (bit_count)
				0:  req_origins <= !rx_data;  // Expect 0
				1:  req_origins <=  rx_data;  // Expect 1
				2:  req_origins <= !rx_data;  // Expect 0
				3:  req_origins <= !rx_data;  // Expect 0		
				4:  req_origins <= !rx_data;  // Expect 0		
				5:  req_origins <= !rx_data;  // Expect 0		
				6:  req_origins <= !rx_data;  // Expect 0		
				7:  req_origins <=  rx_data;  // Expect 1
				default: req_origins <= 1;    // Don't care		
			endcase

	// Get status: 0x4003XX
	// The last byte has flags sent from GC to controller:
	//   Bit 0:  Rumble flag
	reg req_status;
	reg rumble;
	wire req_status_complete = req_status && rx_stop && (bit_count == 24);
	always @(posedge clk or posedge reset)
		if (reset) begin
			req_status <= 1;
			rumble <= 0;
		end
		else if (rx_start)
			req_status <= 1;
		else if (!req_status)
			req_status <= 0;
		else if (rx_strobe)
			case (bit_count)
				0:  req_status <= !rx_data;  // Expect 0
				1:  req_status <=  rx_data;  // Expect 1
				2:  req_status <= !rx_data;  // Expect 0
				3:  req_status <= !rx_data;  // Expect 0
				4:  req_status <= !rx_data;  // Expect 0
				5:  req_status <= !rx_data;  // Expect 0
				6:  req_status <= !rx_data;  // Expect 0
				7:  req_status <= !rx_data;  // Expect 0
				8:  req_status <= !rx_data;  // Expect 0
				9:  req_status <= !rx_data;  // Expect 0
				10: req_status <= !rx_data;  // Expect 0
				11: req_status <= !rx_data;  // Expect 0
				12: req_status <= !rx_data;  // Expect 0
				13: req_status <= !rx_data;  // Expect 0
				14: req_status <=  rx_data;  // Expect 1
				15: req_status <=  rx_data;  // Expect 1
				23: begin
				    req_status <= 1;         // Don't care
				    rumble <= rx_data;       // Save rumble bit
				    end
				default: req_status <= 1;    // Don't care		
			endcase

	assign debug = {req_id_complete, req_status_complete, req_origins_complete};

endmodule


/*
 * A variation on gc_controller that includes a builtin
 * timebase. This is mostly intended for convenience when simulating.
 */
module gc_controller_with_timebase (clk, reset, gc_port, rumble,
                                    state_addr, state_request, state_data, state_ack,
                                    debug);
	input clk, reset;
	inout gc_port;
	output rumble;
	output [5:0] state_addr;
	output state_request;
	input state_data, state_ack;
	output [2:0] debug;
	
	wire [2:0] tx_timing;
	n_serial_tx_timebase timebase(clk, reset, tx_timing);

        gc_controller gcc(clk, reset, tx_timing, gc_port, rumble, state_addr,
                          state_request, state_data, state_ack, debug);
endmodule

/* The End */

