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
