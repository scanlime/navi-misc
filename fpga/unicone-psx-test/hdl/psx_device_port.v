/************************************************************************
 *
 * Emulates the device side of the low-level protocol shared by
 * Playstation/PS2 controllers and memory cards.
 *
 * This module acts as an adapter between the PSX serial bus, and
 * an internal parallel bus we'll call the PPB (Playstation Parallel Bus).
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psx_device_port(clk, reset,
		       PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
		       PPB_packet_reset, PPB_ack_strobe,
		       PPB_command, PPB_command_strobe,
		       PPB_reply, PPB_reply_en);

    parameter CLOCK_MHZ = 25;

    // Typical ACK timing observed on a Dual Shock controller
    parameter ACK_DELAY_US = 8;
    parameter ACK_WIDTH_US = 3;

    parameter ACK_TIMER_BITS = 4;
    parameter ACK_PRESCALER_BITS = 5;

    input     clk, reset;

    input     PSX_clk, PSX_sel, PSX_cmd;
    inout     PSX_ack, PSX_dat;

    output       PPB_packet_reset;       // HIGH when all per-packet state should be reset
    input        PPB_ack_strobe;         // Bring HIGH for at least one clock cycle after PPB_command_strobe
                                         //   to acknowledge the command byte. Can occur any time less than
                                         //   ACK_DELAY_US microseconds after PPB_command_strobe.

    output [7:0] PPB_command;            // Received command byte
    output 	 PPB_command_strobe;     // HIGH for one clock after PPB_command has changed

    input [7:0]  PPB_reply;              // Outgoing reply byte, sampled at first negative clock edge
    input 	 PPB_reply_en;           // Reply enable bit, sampled concurrently with PPB_reply


    /********************************************************************
     * 
     * PSX Inputs and outputs
     *
     */

    // All inputs are asynchronous. Synch them to our clock for two cycles.

    wire  sync_clk;
    d_flipflop_pair dffp_clk(clk, reset, PSX_clk, sync_clk);

    wire  sync_sel;
    d_flipflop_pair dffp_sel(clk, reset, PSX_sel, sync_sel);

    wire  sync_cmd;
    d_flipflop_pair dffp_cmd(clk, reset, PSX_cmd, sync_cmd);
    
    // Outputs are registered, and tri-stated when not active.

    wire   output_dat;
    wire   output_dat_en;
    assign PSX_dat = output_dat_en ? output_dat : 1'bz;
    
    wire   output_ack;
    assign PSX_ack = output_ack ? 1'b0 : 1'bz;

    // Treat SEL as a reset: when the host is not addressing this device,
    // the device's protocol layer should be in a reset state.

    assign PPB_packet_reset = reset || sync_sel;

    // Detect edges on CLK

    wire   prev_clk;
    wire   clk_strobe_pos = sync_clk && ~prev_clk;
    wire   clk_strobe_neg = prev_clk && ~sync_clk;
    d_flipflop dff_clk_edge(clk, reset, sync_clk, prev_clk);

    
    /********************************************************************
     * 
     * Serial input shift register.
     *
     * This collects input bits. Each completed byte is latched into
     * cmd_buffer, and PPB_command_strobe is asserted for one cycle.
     *
     */

    wire [2:0] bit_counter;
    
    right_shift_reg input_shiftreg(clk, PPB_packet_reset, clk_strobe_pos,
				   sync_cmd, PPB_command);

    counter_with_overflow #(3) input_bit_counter(clk, PPB_packet_reset,
						 clk_strobe_pos,
						 bit_counter,
						 PPB_command_strobe);


    /********************************************************************
     * 
     * Serial output shift register.
     *
     * Just before sending the first bit, the PPB_reply and PPB_reply_en are latched.
     *
     */

    wire [7:0] output_reg;
    assign     output_dat = output_reg[0];

    wire       reply_latch_enable    = clk_strobe_neg && (bit_counter == 0);
    
    right_shift_reg_loadable output_shiftreg(clk, PPB_packet_reset, clk_strobe_neg,
					     1'b0, output_reg, PPB_reply, reply_latch_enable);

    bit_latch output_enable_latch(clk, PPB_packet_reset,
				  PPB_reply_en, reply_latch_enable, output_dat_en);

    
    /********************************************************************
     * 
     * ACK timing generator.
     *
     * Generates an ACK pulse ACK_WIDTH_US microseconds wide, with a
     * ACK_DELAY_US microsecond delay after the last positive clock
     * edge in a byte as signalled by PPB_command_strobe.
     *
     * The ACK pulse is generated if ack_strobe is pulsed at any time
     * between PPB_command_strobe and the end of the ACK_DELAY window.
     *
     */

    wire [ACK_TIMER_BITS-1:0] ack_timer;
    wire ack_prescaler;
    reg  ack_pulse_armed;

    clock_divider #(ACK_PRESCALER_BITS) ack_clock_divider(clk, reset, ack_prescaler);
    
    counter_oneshot #(ACK_TIMER_BITS) ack_counter(.clk(clk), .reset(PPB_packet_reset),
						  .start(PPB_command_strobe),
						  .enable(ack_prescaler),
						  .out(ack_timer));
    
    d_flipflop ack_dff(clk, PPB_packet_reset,
		       ack_pulse_armed &&
		       ack_timer >= ((CLOCK_MHZ * ACK_DELAY_US) >> ACK_PRESCALER_BITS) &&
		       ack_timer < ((CLOCK_MHZ * (ACK_DELAY_US + ACK_WIDTH_US)) >> ACK_PRESCALER_BITS),
		       output_ack);

    always @(posedge clk or posedge PPB_packet_reset)
      if (PPB_packet_reset) begin
	  ack_pulse_armed   <= 0;
      end
      else if (PPB_command_strobe) begin
	  ack_pulse_armed   <= 0;
      end
      else if (PPB_ack_strobe) begin
	  ack_pulse_armed   <= 1;
      end
endmodule