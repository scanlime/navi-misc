/************************************************************************
 *
 * Random utility modules.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


/*
 * A pair of flip-flops for each line on a bus.
 */
module d_flipflop_pair_bus(clk, reset, d_in, d_out);
   parameter WIDTH = 1;

   input  clk, reset;
   input  [WIDTH-1:0] d_in;
   output [WIDTH-1:0] d_out;
   reg [WIDTH-1:0]    r;
   reg [WIDTH-1:0]    d_out;

   always @(posedge clk or posedge reset)
     if (reset) begin
        r <= 0;
        d_out <= 0;
     end
     else begin
        r <= d_in;
        d_out <= r;
     end
endmodule


/*
 * Majority detect: Outputs a 1 if two of the three inputs are 1.
 */

module mdetect_3(a, b, c, out);
   input a, b, c;
   output out;

   assign out = (a && b) || (a && c) || (b && c);
endmodule


/*
 * An array of majority detect modules.
 */
module mdetect_3_arr(a, b, c, out);
   parameter COUNT = 8;

   input [COUNT-1:0] a;
   input [COUNT-1:0] b;
   input [COUNT-1:0] c;
   output [COUNT-1:0] out;

   genvar i;

   generate for (i = 0; i < COUNT; i = i+1)
     begin: inst
        mdetect_3 md3_i(a[i], b[i], c[i], out[i]);
     end
   endgenerate
endmodule
