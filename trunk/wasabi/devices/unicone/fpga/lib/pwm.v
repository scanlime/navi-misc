/*
 * pwm.v - Implementation of an I2C-controlled pulse width
 *         modulation peripheral.
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

// A simple 16-bit pulse width modulation module
module pwm16 (clk, reset,
	      duty_cycle, out);

	input clk, reset;
	input [15:0] duty_cycle;
	output out;
	reg out;
	reg [16:0] pwmreg;

	always @(posedge clk)
		if (reset) begin
			pwmreg <= 0;
			out <= 0;
		end
		else begin
			if (pwmreg < 17'h10000)
				pwmreg <= pwmreg + 1;
			else
				pwmreg <= 0;
			out <= ( {1'b0, duty_cycle} >= pwmreg );
		end			
endmodule

/* The End */
