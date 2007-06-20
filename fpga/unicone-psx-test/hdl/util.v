/************************************************************************
 *
 * Random utility modules.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module d_flipflop(clk, reset, d_in, d_out);
    input clk, reset, d_in;
    output d_out;

    reg d_out;

    always @(posedge clk or posedge reset)
      if (reset) begin
	  d_out   <= 0;
      end
      else begin
	  d_out   <= d_in;
      end
endmodule

module d_flipflop_pair(clk, reset, d_in, d_out);
    input  clk, reset, d_in;
    output d_out;
    wire   intermediate;

    d_flipflop dff1(clk, reset, d_in, intermediate);
    d_flipflop dff2(clk, reset, intermediate, d_out);
endmodule

