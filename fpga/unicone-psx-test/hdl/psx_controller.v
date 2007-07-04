/************************************************************************
 *
 * Emulates the Playstation 2 Dual Shock controller.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psx_controller(clk, reset,
		      PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
		      write_addr, write_data, write_en);

    parameter CLOCK_MHZ = 12;

    /*
     * This should be set such that (1 << WDT_BITS) cycles
     * is in the neighbourhood of 2 seconds.
     */
    parameter WDT_BITS = 24;

    input     clk, reset;
    inout     PSX_ack, PSX_dat;
    input     PSX_clk, PSX_sel, PSX_cmd;

    input [4:0] write_addr;
    input [7:0] write_data;
    input 	write_en;

    /********************************************************************
     * 
     * Controller state RAM
     *
     */

    wire [7:0] 	input_state_data;
    defparam 	 input_state.DATA_BITS = 8;
    defparam 	 input_state.ADDR_BITS = 5;
    sync_dualport_sram input_state(clk, write_en, write_addr, write_data,
				   byte_count, input_state_data);    


    /********************************************************************
     * 
     * Low-level controller port.
     *
     */

    wire       PPB_packet_reset;
    wire [7:0] PPB_command;
    wire       PPB_command_strobe;
    reg [7:0]  PPB_reply;
    wire       PPB_reply_en;
    wire       PPB_reply_ready;
    wire       PPB_ack_strobe;

    psx_device_port #(CLOCK_MHZ) port(clk, reset,
				      PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
				      PPB_packet_reset, PPB_ack_strobe,
				      PPB_command, PPB_command_strobe,
				      PPB_reply, PPB_reply_en, PPB_reply_ready);
 
    /********************************************************************
     * 
     * Main state machine
     *
     */

    /*
     * Packet states. Their values are arbitrary, since the synthesis
     * tool should just re-assign these anyway.
     */
    parameter  S_IDLE                           = 0;
    parameter  S_HEADER_ADDRESS                 = 1;
    parameter  S_HEADER_COMMAND                 = 2;
    parameter  S_CMD_INIT_PRESSURE              = 3;
    parameter  S_CMD_GET_AVAILABLE_POLL_RESULTS = 4;
    parameter  S_CMD_POLL                       = 5;
    parameter  S_CMD_ESCAPE                     = 6;
    parameter  S_CMD_SET_MAJOR_MODE             = 7;
    parameter  S_CMD_READ_EXT_STATUS_1          = 8;
    parameter  S_CMD_READ_CONST_1               = 9;
    parameter  S_CMD_READ_CONST_2               = 10;
    parameter  S_CMD_READ_CONST_3               = 11;
    parameter  S_CMD_READ_CONST_OFFSET          = 12;
    parameter  S_CMD_READ_CONST_DATA            = 13;
    parameter  S_CMD_SET_POLL_CMD_FORMAT        = 14;
    parameter  S_CMD_SET_POLL_RESULT_FORMAT     = 15;
    
    parameter  MODE_DIGITAL = 4'h4;
    parameter  MODE_ANALOG  = 4'h7;
    parameter  MODE_ESCAPE  = 4'hF;

    parameter  REPLY_LEN_ESCAPE = 4'h3;
    parameter  REPLY_LEN_DIGITAL = 4'h1;
    parameter  REPLY_LEN_ANALOG = 4'h3;
    
    parameter  PADDING_BYTE = 8'h5A;
    
    /* Packet state */
    reg [4:0] 	 packet_state;
    reg [4:0] 	 byte_count;
    
    /* Controller state */
    reg          escape_mode;
    reg          analog_mode;
    reg [3:0] 	 current_reply_len;

    assign PPB_ack_strobe = (packet_state != S_IDLE);
    assign PPB_reply_en = (packet_state != S_IDLE && packet_state != S_HEADER_ADDRESS);
 
    always @(posedge clk or posedge controller_reset)
      if (controller_reset) begin
	  /*
	   * Reset all controller state.
	   */
	  escape_mode 	      <= 1'b0;
	  analog_mode 	      <= 1'b0;
	  current_reply_len   <= 4'h1;

	  PPB_reply 	      <= 8'hFF;
	  byte_count 	      <= 5'h0;
	  packet_state 	      <= S_IDLE;
      end
      else if (PPB_packet_reset) begin
	  /*
	   * Begin a new packet. Wait for the address byte.
	   * It's important that we don't send a response (DAT is Hi-Z)
	   * until we're addressed.
	   */
	  PPB_reply 	 <= 8'hFF;
	  byte_count 	 <= 5'h0;
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
		  PPB_reply <= {escape_mode ? MODE_ESCAPE : (analog_mode ? MODE_ANALOG : MODE_DIGITAL),
				     escape_mode ? REPLY_LEN_ESCAPE : current_reply_len };
		  byte_count 	 <= 5'h0;
		  packet_state 	 <= (PPB_command == 8'h01) ? S_HEADER_COMMAND : S_IDLE;
	      end

	      S_HEADER_COMMAND: begin
		  /*
		   * Receive and validate the command byte. If this is a valid
		   * command in the current mode, proceed to that command's state.
		   * The next (third) reply byte is padding.
		   */
		  PPB_reply 	 <= PADDING_BYTE;
		  byte_count 	 <= 5'h00;

		  case (PPB_command)
		      8'h40:    packet_state   <= escape_mode ? S_CMD_INIT_PRESSURE : S_IDLE;
		      8'h41:    packet_state   <= escape_mode ? S_CMD_GET_AVAILABLE_POLL_RESULTS : S_IDLE;
		      8'h42:    packet_state   <= S_CMD_POLL;
		      8'h43:    packet_state   <= S_CMD_ESCAPE;
		      8'h44:    packet_state   <= escape_mode ? S_CMD_SET_MAJOR_MODE : S_IDLE;
		      8'h45:    packet_state   <= escape_mode ? S_CMD_READ_EXT_STATUS_1 : S_IDLE;
		      8'h46:    packet_state   <= escape_mode ? S_CMD_READ_CONST_1 : S_IDLE;
		      8'h47:    packet_state   <= escape_mode ? S_CMD_READ_CONST_2 : S_IDLE;
		      8'h4c:    packet_state   <= escape_mode ? S_CMD_READ_CONST_3 : S_IDLE;
		      8'h4d:    packet_state   <= escape_mode ? S_CMD_SET_POLL_CMD_FORMAT : S_IDLE;
		      8'h4f:    packet_state   <= escape_mode ? S_CMD_SET_POLL_RESULT_FORMAT : S_IDLE;
		      default:  packet_state   <= S_IDLE;
		  endcase // case (PPB_command)
	      end

	      S_IDLE: begin
		  byte_count 		       <= 5'h00;
		  PPB_reply 		       <= 8'hFF;
	      end

	      S_CMD_INIT_PRESSURE: begin
		  byte_count 		       <= byte_count + 5'h1;
		  case (byte_count)
		      5'h00:    PPB_reply   <= 8'h00;    // (Unknown)
		      5'h01:    PPB_reply   <= 8'h00;    // (Unknown)
		      5'h02:    PPB_reply   <= 8'h02;    // (Unknown)
		      5'h03:    PPB_reply   <= 8'h00;    // (Unknown)
		      5'h04:    PPB_reply   <= 8'h00;    // (Unknown)
		      default:  PPB_reply   <= PADDING_BYTE;
		  endcase // case (byte_count)
	      end 

	      S_CMD_GET_AVAILABLE_POLL_RESULTS: begin
		  byte_count 		       <= byte_count + 1;
		  case (byte_count)
		      5'h00:    PPB_reply   <= 8'hFF;    // Bitmask of available polling results
		      5'h01:    PPB_reply   <= 8'hFF;    //   ...
		      5'h02:    PPB_reply   <= 8'h03;    //   ...
		      5'h03:    PPB_reply   <= 8'h00;    //   ...
		      5'h04:    PPB_reply   <= 8'h00;    //   ...
		      default:  PPB_reply   <= PADDING_BYTE;
		  endcase // case (byte_count)
	      end 
	      
	      S_CMD_POLL: begin
		  byte_count 		    <= byte_count + 5'h1;
		  PPB_reply 		    <= input_state_data;
	      end

     	      S_CMD_ESCAPE: begin
		  byte_count 		    <= byte_count + 5'h1;
		  PPB_reply 		    <= input_state_data;

		  if (byte_count == 1) begin
		      /*
		       * This is the first command data byte.
		       * (byte 0 is the header padding command byte or
		       * the first response byte). It tells us whether
		       * to enter or exit escape mode.
		       */
		      escape_mode   <= PPB_command[0];
		  end
	      end

	      S_CMD_SET_MAJOR_MODE: begin
		  byte_count 	    <= byte_count + 5'h1;
		  PPB_reply 	    <= 8'h00;
		  
		  if (byte_count == 1) begin
		      /*
		       * First command data byte. This is 1 to enter
		       * analog mode, 0 for digital mode. This also
		       * resets all mode-related state, such as the
		       * command/response mapping and the reply length.
		       */
		      analog_mode 	  <= PPB_command[0];
		      current_reply_len   <= PPB_command[0] ? REPLY_LEN_ANALOG : REPLY_LEN_DIGITAL;
		  end
	      end 

	      S_CMD_READ_EXT_STATUS_1: begin
		  byte_count 		  <= byte_count + 5'h1;
		  case (byte_count)
		      5'h00:    PPB_reply   <= 8'h03;    // (Unknown)
		      5'h01:    PPB_reply   <= 8'h02;    // (Unknown)
		      5'h02:    PPB_reply   <= {7'b0000000, analog_mode};
		      5'h03:    PPB_reply   <= 8'h02;    // (Unknown)
		      5'h04:    PPB_reply   <= 8'h01;    // (Unknown)
		      5'h05:    PPB_reply   <= 8'h00;    // (Unknown)
		      default:  PPB_reply   <= PADDING_BYTE;
		  endcase // case (byte_count)
	      end 

	      S_CMD_READ_CONST_1: begin
		  /* Begin reading const 1, store the address in byte_count */
		  packet_state 		    <= S_CMD_READ_CONST_OFFSET;
		  byte_count 		    <= 5'h00;	  
		  PPB_reply 		    <= 8'h00;
	      end 

	      S_CMD_READ_CONST_2: begin
		  /* Begin reading const 2, store the address in byte_count */
		  packet_state 		    <= S_CMD_READ_CONST_OFFSET;
		  byte_count 		    <= 5'h08;
		  PPB_reply 		    <= 8'h00;
	      end 

	      S_CMD_READ_CONST_3: begin
		  /* Begin reading const 2, store the address in byte_count */
		  packet_state 		    <= S_CMD_READ_CONST_OFFSET;
		  byte_count 		    <= 5'h0C;
		  PPB_reply 		    <= 8'h00;
	      end 

	      S_CMD_READ_CONST_OFFSET: begin
		  /*
		   * Read the offset byte for any of the const commands,
		   * and output the first data byte. We don't store the
		   * first byte in our ROM, since it's assumed to always
		   * be zero.
		   */
		  packet_state 		    <= S_CMD_READ_CONST_DATA;		  
		  byte_count 		    <= PPB_command[0] ? (byte_count + 5'h04) : byte_count;
		  PPB_reply 		    <= 8'h00;
	      end 
	      
	      S_CMD_READ_CONST_DATA: begin
		  /*
		   * ROM lookup table for all constant reply commands.
		   * We only store the last 4 bytes of each response.
		   */
		  byte_count 		    <= byte_count + 5'h1;
		  case (byte_count)
		      /* CONST_1, offset 0 */
		      5'h00:    PPB_reply   <= 8'h01;
		      5'h01:    PPB_reply   <= 8'h02;
		      5'h02:    PPB_reply   <= 8'h00;
		      5'h03:    PPB_reply   <= 8'h0a;
		      /* CONST_1, offset 1 */
		      5'h04:    PPB_reply   <= 8'h01;
		      5'h05:    PPB_reply   <= 8'h01;
		      5'h06:    PPB_reply   <= 8'h01;
		      5'h07:    PPB_reply   <= 8'h14;
		      /* CONST_2, offset 0 */
		      5'h08:    PPB_reply   <= 8'h02;
		      5'h09:    PPB_reply   <= 8'h00;
		      5'h0A:    PPB_reply   <= 8'h01;
		      5'h0B:    PPB_reply   <= 8'h00;
		      /* CONST_3, offset 0 */
		      5'h0C:    PPB_reply   <= 8'h00;
		      5'h0D:    PPB_reply   <= 8'h04;
		      5'h0E:    PPB_reply   <= 8'h00;
		      5'h0F:    PPB_reply   <= 8'h00;
		      /* CONST_3, offset 1 */
		      5'h10:    PPB_reply   <= 8'h00;
		      5'h11:    PPB_reply   <= 8'h07;
		      5'h12:    PPB_reply   <= 8'h00;
		      5'h13:    PPB_reply   <= 8'h10;

		      default:  PPB_reply   <= 8'h00;
		  endcase // case (byte_count)
	      end 

	      S_CMD_SET_POLL_CMD_FORMAT: begin
		  byte_count 		    <= byte_count + 5'h1;

		  /* XXX: Store the new command format */
		  
		  /*
		   * XXX: We're supposed to reply with the previous poll cmd format, but
		   *      I doubt any games actually care. Reply with FF (disabled) for
		   *      all channels, since this is the value a real controller gives
		   *      after reset.
		   */
		  PPB_reply 		    <= 8'hFF;
	      end

	      S_CMD_SET_POLL_RESULT_FORMAT: begin
		  byte_count 		    <= byte_count + 5'h1;

		  /* XXX: Store the new result format */

		  PPB_reply 		    <= (byte_count > 5'h04) ? PADDING_BYTE : 8'h00;
	      end
	    
	  endcase // case (packet_state)
      end // if (PPB_command_strobe)

    
    /********************************************************************
     * 
     * Watchdog timer
     * 
     * This resets the controller after a couple seconds of bus activity.
     * It is necessary because one game (or the BIOS) may leave the
     * controller in a mode that not all other games understand. The bus
     * is idle for several seconds while a game is booting, so official
     * Dual Shock controllers detect this gap and reset themselves. We
     * do the same.
     * 
     * Note that controller_reset is HIGH on reset, so we don't need
     * a separate reset signal for the controller FSM.
     *
     */

    wire controller_reset;    
    watchdog #(WDT_BITS) wdt(clk, reset, !PPB_packet_reset, controller_reset);
    
endmodule