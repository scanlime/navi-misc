module test (clk, led, scl, sda);
	input clk;
	output led;
	input scl;
	inout sda;
	wire [15:0] brightness;

	pwm16 led_pwm(clk, brightness, led);
	i2c_slave led_i2c(clk, scl, sda, brightness);
endmodule
