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
 * Wrappers around platform-specific embedded SRAM.
 */
module sram_16bit_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we, d1_in;
	input [3:0] a1;
	input [3:0] a2;
	output d1_out, d2_out;

	RAM16X1D ram(.DPO(d2_out), .SPO(d1_out),
	             .A0(a1[0]), .A1(a1[1]), .A2(a1[2]), .A3(a1[3]),
	             .D(d1_in),
	             .DPRA0(a2[0]), .DPRA1(a2[1]), .DPRA2(a2[2]), .DPRA3(a2[3]),
	             .WCLK(clk), .WE(we));
endmodule

module sram_32bit_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we, d1_in;
	input [4:0] a1;
	input [4:0] a2;
	output d1_out, d2_out;

	sram_16bit_dualport h (clk, we && a1[4], d1_in, 
	                       a1[3:0], d1_out_high, a2[3:0], d2_out_high);
	sram_16bit_dualport l (clk, we && !a1[4], d1_in, 
	                       a1[3:0], d1_out_low, a2[3:0], d2_out_low);
	
	assign d1_out = a1[4] ? d1_out_high : d1_out_low;
	assign d2_out = a2[4] ? d2_out_high : d2_out_low;
endmodule

module sram_64bit_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we, d1_in;
	input [5:0] a1;
	input [5:0] a2;
	output d1_out, d2_out;

	sram_32bit_dualport h (clk, we && a1[5], d1_in, 
	                       a1[4:0], d1_out_high, a2[4:0], d2_out_high);
	sram_32bit_dualport l (clk, we && !a1[5], d1_in, 
	                       a1[4:0], d1_out_low, a2[4:0], d2_out_low);
	
	assign d1_out = a1[5] ? d1_out_high : d1_out_low;
	assign d2_out = a2[5] ? d2_out_high : d2_out_low;
endmodule

module sram_128bit_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we, d1_in;
	input [6:0] a1;
	input [6:0] a2;
	output d1_out, d2_out;

	sram_64bit_dualport h (clk, we && a1[6], d1_in, 
	                       a1[5:0], d1_out_high, a2[5:0], d2_out_high);
	sram_64bit_dualport l (clk, we && !a1[6], d1_in, 
	                       a1[5:0], d1_out_low, a2[5:0], d2_out_low);
	
	assign d1_out = a1[6] ? d1_out_high : d1_out_low;
	assign d2_out = a2[6] ? d2_out_high : d2_out_low;
endmodule

module sram_256bit_dualport (clk, we, d1_in, a1, d1_out, a2, d2_out);
	input clk, we, d1_in;
	input [7:0] a1;
	input [7:0] a2;
	output d1_out, d2_out;

	sram_128bit_dualport h (clk, we && a1[7], d1_in, 
	                        a1[6:0], d1_out_high, a2[6:0], d2_out_high);
	sram_128bit_dualport l (clk, we && !a1[7], d1_in, 
	                        a1[6:0], d1_out_low, a2[6:0], d2_out_low);
	
	assign d1_out = a1[7] ? d1_out_high : d1_out_low;
	assign d2_out = a2[7] ? d2_out_high : d2_out_low;
endmodule

/* The End */
