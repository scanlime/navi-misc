module test (clk, foo);

input clk;
output foo;

reg [24:0] div;

always @(posedge clk)
	div <= div+1;

assign foo = div[24];

endmodule
