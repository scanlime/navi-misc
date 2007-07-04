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


/*
 * A fully synchronous dual-port SRAM, where one port is read-only
 * and the other is write-only.
 */
module sync_dualport_sram(clk, write_en, write_addr, write_data, read_addr, read_data);
    parameter DATA_BITS;
    parameter ADDR_BITS;
    parameter MEMORY_DEPTH = 2 ** ADDR_BITS;

    input     clk, write_en;
    input [ADDR_BITS-1:0] write_addr;
    input [DATA_BITS-1:0] write_data;
    input [ADDR_BITS-1:0] read_addr;
    output [DATA_BITS-1:0] read_data;
    
    reg [DATA_BITS-1:0] memory[MEMORY_DEPTH-1:0];
    reg [DATA_BITS-1:0] read_data;

    always @(posedge clk) begin
	if (write_en)
	  memory[write_addr]   <= write_data;

	read_data 	       <= memory[read_addr];
    end
endmodule


/*
 * A simple counter with synchronous enable. This should
 * be efficient to synthesize in one block per bit.
 */
module counter(clk, reset, enable, out);
    parameter BITS;

    input     clk, reset, enable;

    output [BITS-1:0] out;
    reg [BITS-1:0]    out;
        
    always @(posedge clk or posedge reset)
      if (reset)
	out   <= 0;
      else if (enable)
	out   <= out + 1;
endmodule


/*
 * Output goes high for one clock cycle, one cycle after the first
 * '1' input after one or more '0's.
 */
module positive_edge_detector(clk, reset, in, out);
    input clk, reset, in;
    output out;
    wire   prev;

    d_flipflop dff1(clk, reset, in, prev);
    d_flipflop dff2(clk, reset, in && ~prev, out);
endmodule


/*
 * A clock divider that generates a one-cycle pulse every 1<<BITS clock cycles.
 *
 * It is intended that this clock divider is included multiple times, by
 * each sub-module that needs a divider. Most of the submodules will end up
 * generating exactly the same circuitry, which will get optimized out.
 */
module clock_divider(clk, reset, out);
    parameter BITS;

    input     clk, reset;
    output    out;

    wire [BITS-1:0] counter;

    counter #(BITS) counter1(clk, reset, 1'b1, counter);
    positive_edge_detector posedge1(clk, reset, counter[BITS-1], out);
endmodule


/*
 * Watchdog timer with synchronous reset and steady-high 'expired' output.
 * Between (1 << BITS) - (1 << SHARED_BITS) and (1 << BITS) cycles after
 * sync_reset is asserted, 'expired' will be continuously asserted.
 *
 * This uses a clock_divider to save FPGA space by making most of the timer's
 * register space shareable by default.
 */
module watchdog(clk, reset, sync_reset, expired);
    parameter BITS;
    parameter EXPIRED_ON_RESET = 1;

    parameter LOCAL_BITS = 5;
    parameter SHARED_BITS = BITS - LOCAL_BITS;

    input     clk, reset, sync_reset;
    output    expired;
    
    reg       reset_reg;
    reg       expired;
    
    wire [LOCAL_BITS-1:0] counter;
    wire 		  tick;

    always @(posedge clk or posedge reset)
      if (reset) 
        reset_reg   <= 1;
      else if (sync_reset)
	reset_reg   <= 1;
      else
	reset_reg   <= 0;

    clock_divider #(SHARED_BITS) wdt_clkdiv(clk, reset, tick);
    counter #(LOCAL_BITS) counter1(clk, reset_reg, tick && ~expired, counter);

    /* Detect expiration */
    always @(posedge clk or posedge reset)
      if (reset)
	expired   <= EXPIRED_ON_RESET;
      else if (sync_reset)
	expired   <= 0;
      else
	expired   <= counter[LOCAL_BITS-1];
endmodule