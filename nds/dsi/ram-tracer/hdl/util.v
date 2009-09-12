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

   reg    d_out;

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


/*
 * A d_flipflop_pair for busses.
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


/*
 * A set/reset flipflop which is set on sync_set and reset by sync_reset.
 */
module set_reset_flipflop(clk, reset, sync_set, sync_reset, out);
   input clk, reset, sync_set, sync_reset;
   output out;
   reg    out;

   always @(posedge clk or posedge reset)
     if (reset)
       out   <= 0;
     else if (sync_set)
       out   <= 1;
     else if (sync_reset)
       out   <= 0;
endmodule
