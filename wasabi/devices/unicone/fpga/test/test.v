module test (clk, foo);

input clk;
output foo;

reg [10:0] div;

always @(posedge clk)
	div <= div+1;

assign foo = div[10];

endmodule
