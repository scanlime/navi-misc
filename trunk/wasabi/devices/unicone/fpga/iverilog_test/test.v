module test (clk, reset,
             led);

	input clk, reset;
	output led;
        reg led;

        always @(posedge clk or posedge reset)
                if (reset)
                        led <= 0;
                else
                        led <= !led;
endmodule
