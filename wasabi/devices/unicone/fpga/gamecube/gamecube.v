
module test (clk, reset,
             led,
             scl, sda);

	input clk, reset;
	output led;
	input scl;
	inout sda;
	
	/* Buffer between external 2-wire I2C and on-chip 3-wire I2C */
	wire int_scl, int_sda_in, int_sda_out;
	i2c_io_buffer i2cbuf(clk, reset,
	                     scl, sda,
	                     int_scl, int_sda_in, int_sda_out);
	
	wire [15:0] brightness;
	pwm16 led_pwm(clk, reset, brightness, led);
	i2c_slave_reg16 #(7'h21) led_i2c(clk, reset,
	                                 int_scl, int_sda_in, int_sda_out,
	                                 brightness);
endmodule
