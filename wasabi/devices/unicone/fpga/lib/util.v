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


/* The End */
