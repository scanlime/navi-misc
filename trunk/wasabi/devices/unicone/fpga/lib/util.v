/*
 * util.v - Miscellaneous utility hardware shared by other Unicone modules
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
 * A low-to-high edge detector. 'edge_detected' is high on cycle N
 * if 'in' is high and 'in' was low on cycle N-1.
 */
module rising_edge_detector (clk, reset, in, edge_detected);
	input clk, reset;
	input in;
	output edge_detected;

	reg in_prev;
	assign edge_detected = in && !in_prev;
	
	always @(posedge clk or posedge reset)
		if (reset)
			/* Defaulting to '1' prevents spurious edge detection */
			in_prev <= 1;
		else
			in_prev <= in;
endmodule	


/*
 * A high-to-low edge detector. 'edge_detected' is high on cycle N
 * if 'in' is low and 'in' was high on cycle N-1.
 */
module falling_edge_detector (clk, reset, in, edge_detected);
	input clk, reset;
	input in;
	output edge_detected;

	reg in_prev;
	assign edge_detected = in_prev && !in;
	
	always @(posedge clk or posedge reset)
		if (reset)
			/* Defaulting to '0' prevents spurious edge detection */
			in_prev <= 0;
		else
			in_prev <= in;
endmodule


/*
 * Synchronous edge triggered set/reset latch. A rising edge on 'set'
 * brings the output high, a rising edge on 'clear' brings the output low.
 * If both 'set' and 'clear' have rising edges on the same clock cycle,
 * 'set' takes precedence. Default state is 0.
 */
module rising_edge_latch (clk, reset, set, clear, out);
	input clk, reset, set, clear;
	output out;
	reg out;

	wire set_edge, clear_edge;
	rising_edge_detector set_edgedet(clk, reset, set, set_edge);
	rising_edge_detector clear_edgedet(clk, reset, clear, clear_edge);

	always @(posedge clk or posedge reset)
		if (reset)
			out <= 0;
		else if (set_edge)
			out <= 1;
		else if (clear_edge)
			out <= 0;
endmodule


/*
 * Wrappers around platform-specific embedded SRAM.
 */
module sram_32byte_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we;
	input [7:0] d1_in;
	input [4:0] a1;
	input [4:0] a2;
	output [7:0] d1_out;
	output [7:0] d2_out;

	wire low_we = a1[4] ? 0 : we;
	wire high_we = a1[4] ? we : 0;
	wire [7:0] high_d1_out;
	wire [7:0] high_d2_out;
	wire [7:0] low_d1_out;
	wire [7:0] low_d2_out;
	assign d1_out = a1[4] ? high_d1_out : low_d1_out;
	assign d2_out = a2[4] ? high_d2_out : low_d2_out;

	sram_16byte_dualport low_half(clk, low_we, d1_in, a1[3:0],
	                              low_d1_out, a2[3:0], low_d2_out);
	sram_16byte_dualport high_half(clk, high_we, d1_in, a1[3:0],
	                               high_d1_out, a2[3:0], high_d2_out);

endmodule
 
module sram_16byte_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we;
	input [7:0] d1_in;
	input [3:0] a1;
	input [3:0] a2;
	output [7:0] d1_out;
	output [7:0] d2_out;

	RAM16X1D bit0(.DPO(d2_out[0]), .SPO(d1_out[0]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[0]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit1(.DPO(d2_out[1]), .SPO(d1_out[1]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[1]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit2(.DPO(d2_out[2]), .SPO(d1_out[2]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[2]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit3(.DPO(d2_out[3]), .SPO(d1_out[3]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[3]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit4(.DPO(d2_out[4]), .SPO(d1_out[4]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[4]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit5(.DPO(d2_out[5]), .SPO(d1_out[5]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[5]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit6(.DPO(d2_out[6]), .SPO(d1_out[6]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[6]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
	RAM16X1D bit7(.DPO(d2_out[7]), .SPO(d1_out[7]), .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	              .D(d1_in[7]), .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	              .WCLK(clk), .WE(we));
endmodule

/*
 * Allow four devices with suitable handshaking to share
 * one SRAM chip with an arbitrary-width address bus.
 * addr_out connects to the SRAM device, the other pins all
 * connect to the four cooperating devices.
 *
 * Each device, to read a byte, does the following:
 *  1. Place the requested address on dev#_addr.
 *  2. Transition dev#_request from low to high
 *  3. On the same cycle that dev#_ack is asserted, your address
 *     will be made available to the SRAM.
 *
 * Device 1 is the highest priority and 4 the lowest, in the
 * event of a conflict.
 */
module sram_arbiter4 (clk, reset, addr_out,
                      dev1_addr, dev1_request, dev1_ack,
                      dev2_addr, dev2_request, dev2_ack,
                      dev3_addr, dev3_request, dev3_ack,
                      dev4_addr, dev4_request, dev4_ack);
	parameter ADDR_WIDTH = 5;

	input clk, reset;
	output [ADDR_WIDTH-1:0] addr_out;

	input [ADDR_WIDTH-1:0] dev1_addr;
	input dev1_request;
	output dev1_ack;
	input [ADDR_WIDTH-1:0] dev2_addr;
	input dev2_request;
	output dev2_ack;
	input [ADDR_WIDTH-1:0] dev3_addr;
	input dev3_request;
	output dev3_ack;
	input [ADDR_WIDTH-1:0] dev4_addr;
	input dev4_request;
	output dev4_ack;
	
	reg addr_out;
	reg dev1_ack;
	reg dev2_ack;
	reg dev3_ack;
	reg dev4_ack;
	
	/* Generate the 'pending' signals for each device. They go high
	 * when a request edge is detected, and low on ack.
	 */
	wire dev1_pending;
	wire dev2_pending;
	wire dev3_pending;
	wire dev4_pending;
	rising_edge_latch dev1_pending_latch(clk, reset, dev1_request, dev1_ack, dev1_pending);
	rising_edge_latch dev2_pending_latch(clk, reset, dev2_request, dev2_ack, dev2_pending);
	rising_edge_latch dev3_pending_latch(clk, reset, dev3_request, dev3_ack, dev3_pending);
	rising_edge_latch dev4_pending_latch(clk, reset, dev4_request, dev4_ack, dev4_pending);
		
	always @(posedge clk or posedge reset)
		if (reset) begin	
			addr_out <= 0;
			dev1_ack <= 0;
			dev2_ack <= 0;
			dev3_ack <= 0;
			dev4_ack <= 0;	
		end
		else if (dev1_pending) begin
			addr_out <= dev1_addr;
			dev1_ack <= 1;
			dev2_ack <= 0;
			dev3_ack <= 0;
			dev4_ack <= 0;	
		end
		else if (dev2_pending) begin
			addr_out <= dev2_addr;
			dev1_ack <= 0;
			dev2_ack <= 1;
			dev3_ack <= 0;
			dev4_ack <= 0;	
		end
		else if (dev3_pending) begin
			addr_out <= dev3_addr;
			dev1_ack <= 0;
			dev2_ack <= 0;
			dev3_ack <= 1;
			dev4_ack <= 0;	
		end
		else if (dev4_pending) begin
			addr_out <= dev4_addr;
			dev1_ack <= 0;
			dev2_ack <= 0;
			dev3_ack <= 0;
			dev4_ack <= 1;	
		end
		else begin
			dev1_ack <= 0;
			dev2_ack <= 0;
			dev3_ack <= 0;
			dev4_ack <= 0;	
		end		
endmodule


/*
 * An MSB-first parallel to serial shift register with flow
 * control. This can be used with shallow_buffer or other
 * modules with similar interfaces.
 *
 * On the serial side:
 *  1. Wait states are inserted until ser_ready goes high
 *  2. If a bit is available, ser_strobe is high
 *     and at the same clock cycle ser_data contains the
 *     new bit. At all other times ser_available is low.
 *
 * If ser_ready is held high continuously, one bit is output
 * per clock cycle as long as they are available. Single
 * bits can be extracted with 1-cycle pulses of ser_ready.
 * Note that ser_strobe is level triggered, not edge triggered.
 *
 * On the parallel side:
 *  1. Wait states are inserted until par_ready goes high
 *  2. A byte is read from par_data
 *  3. par_strobe goes high for one cycle
 */
module serializer (clk, reset,
                   par_data, par_ready, par_strobe,
                   ser_data, ser_ready, ser_strobe,
                   is_empty);
	parameter WIDTH = 8;
	parameter COUNT_WIDTH = 3;
	
	input clk, reset;
	input [WIDTH-1:0] par_data;
	input par_ready;
	output par_strobe;
	output ser_data;
	input ser_ready;
	output ser_strobe;
	output is_empty;
	
	reg par_strobe;
	reg ser_data;
	reg ser_strobe;
	reg is_empty;
	reg [COUNT_WIDTH-1:0] bit_count;
	reg [WIDTH-1:0] shifter;
	
	reg state;
	parameter
		S_WAIT_FOR_PAR = 0,
		S_SHIFT_BIT = 1;

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_WAIT_FOR_PAR;
			shifter <= 0;
			bit_count <= 0;
			par_strobe <= 0;
			ser_strobe <= 0;
			ser_data <= 0;
			is_empty <= 1;
		
		end
		else case (state)

			S_WAIT_FOR_PAR: begin
				// Wait for parallel data. This state is only used
				// when we're idle- if parallel data is available,
				// this state is not inserted between bytes.
				if (par_ready) begin
					// Bring in the new word of parallel data
					shifter <= par_data;
					bit_count <= 0;
					par_strobe <= 1;
					ser_strobe <= 0;
					state <= S_SHIFT_BIT;
					is_empty <= 0;
				end
				else begin
					par_strobe <= 0;
					ser_strobe <= 0;
					is_empty <= 1;
				end				
			end
						
			S_SHIFT_BIT: begin
				// Normal operation, when we're not in a parallel wait state
				
				if (ser_ready) begin
					// Clock out the next serial bit
				
					ser_data <= shifter[WIDTH-1];
					ser_strobe <= 1;
				
					if (bit_count == WIDTH-1) begin
						// Last bit- if we have parallel data ready, pull
						// it in without missing a beat. If not, insert a
						// wait state.
						if (par_ready) begin
							shifter <= par_data;
							bit_count <= 0;
							par_strobe <= 1;
						end
						else begin
							state <= S_WAIT_FOR_PAR;
							par_strobe <= 0;
						end
					end
					else begin
						// Just another bit
						bit_count <= bit_count + 1;
						shifter <= {shifter[WIDTH-2:0], 1'b0};
						par_strobe <= 0;
					end

				end
				else begin
					par_strobe <= 0;
					ser_strobe <= 0;
				end	
			end

		endcase	
endmodule


/*
 * A one-level FIFO buffer
 *
 * From the input side:
 *  1. Wait for full to be deasserted
 *  2. Place new data on in_data
 *  3. Transition in_strobe from low to high
 *
 * From the output side:
 *  1. Wait for full to be asserted
 *  2. Read out_data
 *  3. Transition out_strobe from low to high
 */
module shallow_buffer (clk, reset, full,
                       in_data, in_strobe,
                       out_data, out_strobe);
	parameter WIDTH = 8;

	input clk, reset;
	output full;
	input [WIDTH-1:0] in_data;
	output [WIDTH-1:0] out_data;
	input in_strobe, out_strobe;

	reg full;
	reg [WIDTH-1:0] out_data;

	/* Detect edges in the two strobe signals */
	wire in_edge, out_edge;
	rising_edge_detector in_edgedet(clk, reset, in_strobe, in_edge);
	rising_edge_detector out_edgedet(clk, reset, out_strobe, out_edge);
	
	always @(posedge clk or posedge reset)
		if (reset) begin
			full <= 0;
			out_data <= 0;
		end
		else begin
			if (in_edge) begin
				/* We have new data coming in. Ignore out_edge-
				 * if we're being used correctly it shouldn't
				 * be high, but if it is for some reason this still
				 * provides the most sensible behaviour.
				 */
				 full <= 1;
				 out_data <= in_data;
			end
			else if (out_edge) begin
				/* Data going out. No need to change
				 * the out_data register, but we do need
				 * to clear the 'full' flag.
				 */
				full <= 0;
			end
		end
endmodule

/* The End */
