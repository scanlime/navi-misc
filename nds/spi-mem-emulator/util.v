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


/*
 * Synchronous latch, with enable.
 */
module sync_latch(clk, reset, in, enable, out);
   parameter BITS = 1;

   input clk, reset, enable;
   input [BITS-1:0] in;
   output [BITS-1:0] out;

   reg [BITS-1:0]    out;

   always @(posedge clk or posedge reset)
     if (reset)
       out   <= 0;
     else if (enable)
       out   <= in;
endmodule


/*
 * A simple counter with synchronous enable. This should
 * be efficient to synthesize in one block per bit.
 */
module counter(clk, reset, enable, out);
   parameter BITS = 1;

   input     clk, reset, enable;

   output [BITS-1:0] out;
   reg [BITS-1:0]    out;

   parameter         ONE = {{BITS-1{1'b0}}, 1'b1};

   always @(posedge clk or posedge reset)
     if (reset)
       out   <= 0;
     else if (enable)
       out   <= out + ONE;
endmodule


/*
 * Counter with synchronous enable and overflow detect.
 * The 'overflow' output goes high on the same clock cycle
 * that the counter wraps to zero.
 */
module counter_with_overflow(clk, reset, enable, out, overflow);
   parameter BITS = 1;

   input     clk, reset, enable;

   output [BITS-1:0] out;
   reg [BITS-1:0]    out;

   output            overflow;
   reg               overflow;

   parameter         ONE = {{BITS-1{1'b0}}, 1'b1};
   wire [BITS-1:0]   next_out    = out + ONE;

   always @(posedge clk or posedge reset)
     if (reset) begin
         out        <= 0;
         overflow   <= 0;
     end
     else if (enable) begin
         out        <= next_out;
         overflow   <= (next_out == 0);
     end
     else begin
         overflow   <= 0;
     end
endmodule


/*
 * Counter which begins from zero when 'start' pulses,
 * and automatically stops after overflow.
 */
module counter_oneshot(clk, reset, start, enable, out);
   parameter BITS = 1;

   input     clk, reset, start, enable;

   output [BITS-1:0] out;

   parameter         ONE = {{BITS-1{1'b0}}, 1'b1};
   wire [BITS-1:0]   next_out    = out + ONE;
   reg               running;

   always @(posedge clk or posedge reset)
     if (reset)
       running   <= 1'b0;
     else if (start)
       running   <= 1'b1;
     else if (next_out == 0)
       running   <= 1'b0;

   counter #(BITS) ctr(clk, reset || start, enable && running, out);
endmodule


/*
 * Right-to-left shift register, with enable and load.
 */
module left_shift_reg(clk, reset, enable, in_bit,
                      out, load_value, load_enable);
   parameter BITS = 8;

   input   clk, reset, enable, in_bit, load_enable;
   output [BITS-1:0] out;
   input [BITS-1:0]  load_value;

   reg [BITS-1:0]    out;

   always @(posedge clk or posedge reset)
     if (reset)
       out <= 0;
     else if (load_enable)
       out <= load_value;
     else if (enable)
       out <= {out[BITS-2:0], in_bit};
endmodule


/*
 * Output goes high for one clock cycle during the
 * first '1' input after one or more '0's.
 */
module positive_edge_detector(clk, reset, in, out);
   input clk, reset, in;
   output out;
   wire   prev;

   d_flipflop dff1(clk, reset, in, prev);
   assign out = in && ~prev;
endmodule


/*
 * Button debouncer.
 *
 * When the input changes, the output doesn't change
 * unless the input remains stable for at least the
 * duration of a counter overflow.
 */
module debounce(clk, reset, in, out);
   parameter BITS = 16;

   input  clk, reset, in;
   output out;
   wire   in_sync;

   reg            out;
   reg [BITS-1:0] counter;

   d_flipflop_pair dff(clk, reset, in, in_sync);

   always @(posedge clk or posedge reset)
     if (reset) begin
        out <= 0;
        counter <= 0;
     end
     else if (out == in_sync)
       counter <= 0;
     else if (&counter)
       out <= in_sync;
     else
       counter <= counter + 1;
endmodule


/*
 * Pulse stretcher.
 *
 * When the input goes high, the output goes high
 * for as long as the input is high, or as long as
 * it takes our timer to roll over- whichever is
 * longer.
 */
module pulse_stretcher(clk, reset, in, out);
   parameter BITS = 20;

   input  clk, reset, in;
   output out;
   reg    out;

   reg [BITS-1:0] counter;

   always @(posedge clk or posedge reset)
     if (reset) begin
        out <= 0;
        counter <= 0;
     end
     else if (counter == 0) begin
        out <= in;
        counter <= in ? 1 : 0;
     end
     else if (&counter) begin
        if (in) begin
           out <= 1;
        end
        else begin
           out <= 0;
           counter <= 0;
        end
     end
     else begin
        out <= 1;
        counter <= counter + 1;
     end
endmodule

