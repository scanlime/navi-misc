module test (clk, reset,
             led,
             scl, sda,
             serial_out);

	input clk, reset;
	output led;
	input scl;
	inout sda;
	inout serial_out;

	/* Buffer between external 2-wire I2C and on-chip 3-wire I2C */
	wire int_scl, int_sda_in;
	wire [1:0] int_sda_out;
	i2c_io_buffer i2cbuf(clk, reset,
	                     scl, sda,
	                     int_scl, int_sda_in, int_sda_out == 2'b11);

	/* Add an I2C-addressable LED brightness control */
	pwm_i2c #(7'h21) led_controller(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[0],
		led);

	/************** Nintendo serial transmitter, sending 1-byte packets supplied via I2C */

	wire tx, rx;
	n_serial_io_buffer iobuffer(clk, reset, serial_out, tx, rx);

	wire busy;
	reg tx_stopbit;
	wire [7:0] tx_data;
	reg tx_strobe;
	n_serial_tx #(5) transmitter(clk, reset, busy, tx_stopbit, tx_data, tx_strobe, tx);

	wire reg_strobe;
	i2c_slave_reg #(7'h30, 1) n_i2c_reg(
		clk, reset,
		int_scl, int_sda_in, int_sda_out[1],
		tx_data, reg_strobe);

	reg [1:0] state;
	parameter
		S_WAIT_FOR_I2C = 0,
		S_WAIT_BEFORE_STOP = 1,
		S_STOP = 2;

	always @(posedge clk or posedge reset)
		if (reset) begin

			state <= S_WAIT_FOR_I2C;
			tx_stopbit <= 0;
			tx_strobe <= 0;

		end
		else case (state)

			S_WAIT_FOR_I2C: begin
				if (reg_strobe) begin
					state <= S_WAIT_BEFORE_STOP;
					tx_stopbit <= 0;
					tx_strobe <= 1;
				end
				else begin
					tx_stopbit <= 0;
					tx_strobe <= 0;
				end
			end

			S_WAIT_BEFORE_STOP: begin
				state <= S_STOP;
				tx_stopbit <= 0;
				tx_strobe <= 0;
			end

			S_STOP: begin
				state <= S_WAIT_FOR_I2C;
				tx_stopbit <= 1;
				tx_strobe <= 1;
			end
		endcase

endmodule
