module test (clk, reset,
             scl, sda, led);

	input clk, reset;
	output led;
	input scl;
	inout sda;

	// Shared I2C frontend
	wire [1:0] i2c_interface_tx;
	wire [20:0] i2c_interface_rx;
	i2c_frontend i2c_fe(
		clk, reset, 
		scl, sda,
		i2c_interface_tx, i2c_interface_rx);

	// Add an I2C-addressable LED brightness control
	pwm_i2c #(7'h21) led_pwm(
		clk, reset, 
		i2c_interface_tx, i2c_interface_rx,
		led);
endmodule
