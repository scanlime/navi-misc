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
 * An MSB-first serial to parallel shift register with flow
 * control. This can be used with shallow_buffer or other
 * modules with similar interfaces.
 *
 * On the serial side:
 *  1. The user, if possible, waits until ser_ready is high
 *  2. A serial bit is placed on ser_data, and a rising
 *     edge is applied to ser_strobe.
 *
 * On the parallel side:
 *  1. If par_ready is not asserted, the busy state is
 *     propagated as necessary.
 *  2. A completed word is placed on par_data
 *  3. par_strobe goes high for one clock cycle
 */
 module deserializer (clk, reset,
                      ser_data, ser_ready, ser_strobe,
                      par_data, par_ready, par_strobe);
	parameter WIDTH = 8;
	parameter COUNT_WIDTH = 4;
	
	input clk, reset;
	input ser_data, ser_strobe;
	output ser_ready;
	output [WIDTH-1:0] par_data;
	input par_ready;
	output par_strobe;

	reg [WIDTH-1:0] par_data;
	reg [WIDTH-1:0] shifter;
	reg par_strobe;
	reg [COUNT_WIDTH-1:0] bit_count;

	wire ser_strobe_edge;
	rising_edge_detector ser_strobe_edgedet(clk, reset, ser_strobe, ser_strobe_edge);
			
	/* Generate flow control with combinational logic,
	 * don't lag it by one clock cycle. If our shift register
	 * is full, make them wait for our parallel interface to
	 * become ready- the next serial bit will cause output
	 * on the parallel interface.
	 */
	assign ser_ready = (bit_count == WIDTH) ? par_ready : 1;
	
	always @(posedge clk or posedge reset)
		if (reset) begin
			par_data <= 0;
			par_strobe <= 1;
			bit_count <= 1;	
			shifter <= 0;
		end
		else begin
		
			/* Shift in new bits, and output completed bytes */
			if (ser_strobe_edge) begin
				/* We just got another bit */
				if (bit_count == WIDTH) begin
					/* The last bit just came in, output and reset the byte */
					par_strobe <= 1;
					bit_count <= 0;
					par_data <= {shifter[WIDTH-2:0], ser_data};
					shifter <= 0;
				end
				else begin
					par_strobe <= 0;
					bit_count <= bit_count + 1;
					shifter <= {shifter[WIDTH-2:0], ser_data};
				end
			end
			else begin
				par_strobe <= 0;
			end		
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
