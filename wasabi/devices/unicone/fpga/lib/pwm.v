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


/* An I2C-attached PWM module. The I2C register is always
 * two bytes, but the actual precision of the PWM generator
 * may be adjusted. If the precision is less than 16 bits, 
 * some of the low-order bits will be ignored.
 */
module pwm_i2c (clk, reset,
                  scl, sda_in, sda_out,
                  out);
        parameter I2C_ADDRESS = 0;
        parameter PWM_BITS = 16;
               
	input clk, reset;
	input scl, sda_in;
	output sda_out, out;
	wire [15:0] duty_cycle;

	pwm #(PWM_BITS) p(clk, reset, duty_cycle[15:16-PWM_BITS], out);
	i2c_slave_reg #(I2C_ADDRESS, 2) i2creg16(
		clk, reset,
		scl, sda_in, sda_out,
		duty_cycle);
endmodule

/* A simple pulse width modulation module, of arbitrary precision */
module pwm (clk, reset, duty_cycle, out);
	parameter BITS = 16;

	input clk, reset;
	input [BITS-1:0] duty_cycle;
	output out;
	reg out;
	reg [BITS:0] pwmreg;

	always @(posedge clk or posedge reset)
		if (reset) begin
			pwmreg <= 0;
			out <= 0;
		end
		else begin
			if (pwmreg < { 1'b1, {BITS{1'b0}} })
				pwmreg <= pwmreg + 1;
			else
				pwmreg <= 0;
			out <= ( {1'b0, duty_cycle} >= pwmreg );
		end
endmodule

/* The End */
