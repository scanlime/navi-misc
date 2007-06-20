/************************************************************************
 *
 * Emulates the Playstation 2 Dual Shock controller.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psx_controller(clk, reset,
		      PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat);

    parameter CLOCK_MHZ = 25;

    input     clk, reset;
    inout     PSX_ack, PSX_dat;
    input     PSX_clk, PSX_sel, PSX_cmd;

    /********************************************************************
     * 
     * Low-level controller port.
     *
     */

    wire       PPB_packet_reset;
    wire [7:0] PPB_command;
    wire       PPB_command_strobe;
    reg [7:0]  PPB_reply;
    wire       PPB_reply_ready;
    wire       PPB_ack_strobe;

    psx_device_port #(CLOCK_MHZ) port(clk, reset,
				      PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
				      PPB_packet_reset, PPB_command, PPB_command_strobe,
				      PPB_reply, PPB_reply_ready, PPB_ack_strobe);
 
				      
    /********************************************************************
     * 
     * Main state machine
     *
     */

    parameter  S_IDLE 				   = 0;
    parameter  S_HEADER_ADDRESS  = 1;
    parameter  S_HEADER_COMMAND  = 2;
    parameter  S_CMD_POLL        = 3;

    parameter  MODE_DIGITAL = 4'h4;
    parameter  MODE_ANALOG  = 4'h7;
    parameter  MODE_ESCAPE  = 4'hF;

    parameter  PADDING_BYTE = 8'h5A;
    
    /* Packet state */
    reg [3:0] 	 packet_state;
    
    /* Controller state */
    reg [3:0] 	 current_mode;
    reg [3:0] 	 current_reply_len;

    assign PPB_ack_strobe = (packet_state != S_IDLE);

 
    always @(posedge clk or posedge reset)
      if (reset) begin
	  /*
	   * Reset all controller state.
	   */
	  packet_state 	      <= S_IDLE;
	  current_mode 	      <= MODE_DIGITAL;
	  current_reply_len   <= 1;
      end
      else if (PPB_packet_reset) begin
	  /*
	   * Begin a new packet. Wait for the address byte.
	   * The response to the address byte is always 0xFF.
	   */
	  PPB_reply 	 <= 8'hFF;
	  packet_state 	 <= S_HEADER_ADDRESS;
      end
      else if (PPB_command_strobe) begin
	  case (packet_state)

	      S_HEADER_ADDRESS: begin
		  /*
		   * Receive the address byte. This should always be 01,
		   * to indicate that a controller on port 1 is being addressed.
		   *
		   * The next byte specifies our command, and the next reply is
		   * a combination of the current mode and current reply length.
		   */
		  PPB_reply 	 <= {current_mode, current_reply_len};
		  packet_state 	 <= (PPB_command == 8'h01) ? S_HEADER_COMMAND : S_IDLE;
	      end

	      S_HEADER_COMMAND: begin
		  /*
		   * Receive and validate the command byte. If this is a valid
		   * command in the current mode, proceed to that command's state.
		   * The next (third) reply byte is padding.
		   */
		  PPB_reply   <= PADDING_BYTE;

		  case (PPB_command)
		      8'h42:    packet_state   <= S_CMD_POLL;
		      default:  packet_state   <= S_IDLE;
		  endcase // case (PPB_command)
	      end

	      S_IDLE: begin
		  packet_state 	<= S_IDLE;
		  PPB_reply     <= PADDING_BYTE;
	      end

	      S_CMD_POLL: begin
		  packet_state  <= S_CMD_POLL;
		  PPB_reply     <= 8'h55;
	      end
	      
	  endcase // case (packet_state)
      end
    
endmodule