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
		       PPB_reply, PPB_reply_ready);

    parameter CLOCK_MHZ = 25;

    // Typical ACK timing observed on a Dual Shock controller
    parameter ACK_DELAY_US = 8;
    parameter ACK_WIDTH_US = 3;

    input     clk, reset;

    input     PSX_clk, PSX_sel, PSX_cmd;
    inout     PSX_ack, PSX_dat;

    output       PPB_packet_reset;       // HIGH when all per-packet state should be reset
    input        PPB_ack_strobe;         // Bring HIGH for at least one clock cycle after PPB_command_strobe
                                         //   to acknowledge the command byte. Can occur any time less than
                                         //   ACK_DELAY_US microseconds after PPB_command_strobe.

    output [7:0] PPB_command;            // Received command byte
    output 	 PPB_command_strobe;     // HIGH for one clock after PPB_command has changed

    input [7:0]  PPB_reply;              // Outgoing reply byte
    output 	 PPB_reply_ready;        // HIGH for one clock cycle after PPB_reply has been read


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
    
    // Outputs are registered, and tri-stated when this device isn't selected.

    reg    output_dat;
    assign PSX_dat = sync_sel ? 1'bz : output_dat;
    
    reg    output_ack;
    assign PSX_ack = sync_sel ? 1'bz : output_ack;

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

    reg [7:0] cmd_shiftreg;
    reg [2:0] cmd_state;
    reg [7:0] PPB_command;
    reg       PPB_command_strobe;
        
    always @(posedge clk or posedge PPB_packet_reset)
      if (PPB_packet_reset) begin
	  PPB_command 	      <= 0;
	  PPB_command_strobe  <= 0;
	  cmd_shiftreg 	      <= 0;
	  cmd_state 	      <= 0;	  
      end
      else if (clk_strobe_pos && cmd_state == 3'b111) begin
	  PPB_command 	      <= {sync_cmd, cmd_shiftreg[7:1]};
	  PPB_command_strobe  <= 1;
	  cmd_shiftreg 	      <= 0;
	  cmd_state 	      <= 0;
      end
      else if (clk_strobe_pos) begin
	  PPB_command_strobe  <= 0;
	  cmd_shiftreg 	      <= {sync_cmd, cmd_shiftreg[7:1]};
	  cmd_state 	      <= cmd_state + 1;
      end
      else begin
	  PPB_command_strobe  <= 0;
      end


    /********************************************************************
     * 
     * Serial output shift register.
     *
     * Just before sending the first bit, the dat_buffer is latched
     * and dat_buffer_ready is asserted for one cycle.
     *
     */

    reg [6:0] dat_shiftreg;
    reg [2:0] dat_state;
    reg       PPB_reply_ready;
    
    always @(posedge clk or posedge PPB_packet_reset)
      if (PPB_packet_reset) begin
	  output_dat 	     <= 0;
	  dat_shiftreg 	     <= 0;
	  dat_state 	     <= 0;
	  PPB_reply_ready    <= 0;
      end
      else if (clk_strobe_neg && dat_state == 0) begin
	  output_dat 	     <= PPB_reply[0];
	  dat_shiftreg 	     <= PPB_reply[7:1];
	  dat_state 	     <= dat_state + 1;
	  PPB_reply_ready    <= 1;
      end
      else if (clk_strobe_neg) begin
	  output_dat 	     <= dat_shiftreg[0];
	  dat_shiftreg 	     <= {1'b0, dat_shiftreg[6:1]};
	  dat_state 	     <= dat_state + 1;
	  PPB_reply_ready    <= 0;
      end
      else begin
	  PPB_reply_ready    <= 0;
      end

    
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

    reg ack_timer_running;
    reg ack_pulse_armed;
    reg [8:0] ack_timer;

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

    always @(posedge clk or posedge PPB_packet_reset)
      if (PPB_packet_reset) begin
	  ack_timer_running   <= 0;
	  ack_timer 	      <= 0;
	  output_ack 	      <= 1;
      end
      else if (PPB_command_strobe) begin
	  ack_timer_running   <= 1;
	  ack_timer 	      <= 0;
	  output_ack 	      <= 1;
      end
      else if (ack_timer_running && ack_timer == CLOCK_MHZ * ACK_DELAY_US) begin
	  ack_timer_running   <= 1;
	  ack_timer 	      <= ack_timer + 1;	  
	  output_ack 	      <= ~ack_pulse_armed;
      end
      else if (ack_timer_running && ack_timer == CLOCK_MHZ * (ACK_DELAY_US + ACK_WIDTH_US)) begin
	  ack_timer_running   <= 0;
	  ack_timer 	      <= 0;
	  output_ack 	      <= 1;
      end
      else if (ack_timer_running) begin
	  ack_timer 	      <= ack_timer + 1;
      end
endmodule