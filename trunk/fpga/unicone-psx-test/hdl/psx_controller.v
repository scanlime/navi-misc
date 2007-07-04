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
				   reply_byte_index - 5'h03, input_state_data);    

    
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
     * Packet state tracking
     *
     */

    wire [4:0] command_byte_index;                           /* Index of byte currently in PPB_command */
    wire [4:0] reply_byte_index    = command_byte_index;     /* Index of byte to put in PPB_reply */
    
    counter #(5) byte_counter(clk, PPB_packet_reset, PPB_command_strobe, command_byte_index);

    reg        packet_valid;
    reg [7:0]  current_command;
    reg        const_offset;        
    
    parameter  CMD_INIT_PRESSURE = 8'h40;
    parameter  CMD_GET_AVAILABLE_POLL_RESULTS = 8'h41;
    parameter  CMD_POLL = 8'h42;
    parameter  CMD_ESCAPE = 8'h43;
    parameter  CMD_SET_MAJOR_MODE = 8'h44;
    parameter  CMD_READ_EXT_STATUS_1 = 8'h45;
    parameter  CMD_READ_CONST_1 = 8'h46;
    parameter  CMD_READ_CONST_2 = 8'h47;
    parameter  CMD_READ_CONST_3 = 8'h4c;
    parameter  CMD_SET_POLL_CMD_FORMAT = 8'h4d;
    parameter  CMD_SET_POLL_RESULT_FORMAT = 8'h4f;
   
    always @(posedge clk or posedge PPB_packet_reset)
      if (PPB_packet_reset) begin
	  packet_valid 	    <= 1'b1;
	  current_command   <= 0;
	  const_offset 	    <= 0;
      end
      else if (PPB_command_strobe && packet_valid) begin

	  /* Byte 0:
	   *
	   *   Receive the address byte. This should always be 01,
	   *   to indicate that a controller on port 1 is being addressed.
	   */
	  if (command_byte_index == 5'h00) begin
	      packet_valid   <= (PPB_command == 8'h01);
	  end

	  /* Byte 1:
	   *
	   *   Validate the command byte, and store it. Note that most of
	   *   our commands are valid only in escape mode, so this depends on
	   *   the current controller state.
	   */
	  else if (command_byte_index == 5'h01) begin
	      current_command 	<= PPB_command;
	      case (PPB_command)
		  CMD_INIT_PRESSURE:               packet_valid   <= escape_mode;
		  CMD_GET_AVAILABLE_POLL_RESULTS:  packet_valid   <= escape_mode;
		  CMD_POLL:                        packet_valid   <= 1'b1;
		  CMD_ESCAPE:                      packet_valid   <= 1'b1;
		  CMD_SET_MAJOR_MODE:              packet_valid   <= escape_mode;
		  CMD_READ_EXT_STATUS_1:           packet_valid   <= escape_mode;
		  CMD_READ_CONST_1:                packet_valid   <= escape_mode;
		  CMD_READ_CONST_2:                packet_valid   <= escape_mode;
		  CMD_READ_CONST_3:                packet_valid   <= escape_mode;
		  CMD_SET_POLL_CMD_FORMAT:         packet_valid   <= escape_mode;
		  CMD_SET_POLL_RESULT_FORMAT:      packet_valid   <= escape_mode;
		  default:                         packet_valid   <= 1'b0;
	      endcase // case (PPB_command)
	  end

	  /* Byte 2:
	   *   Padding.
	   */

	  /* Byte 3:
	   *   First command-specific byte. Pick out the const_offset,
	   *   which we'll need in order to generate replies to the read_const
	   *   commands.
	   */
	  else if (command_byte_index == 5'h03) begin
	      const_offset   <= PPB_command[0];
	  end
      end

    
    /********************************************************************
     * 
     * Reply generation
     *
     */

    parameter  PADDING_BYTE   = 8'h5A;
    
    parameter  MODE_DIGITAL = 4'h4;
    parameter  MODE_ANALOG  = 4'h7;
    parameter  MODE_ESCAPE  = 4'hF;

    parameter  REPLY_LEN_ESCAPE = 4'h3;
    parameter  REPLY_LEN_DIGITAL = 4'h1;
    parameter  REPLY_LEN_ANALOG = 4'h3;    

    /*
     * Enable the reply output after we've been addressed successfully.
     */
    assign PPB_reply_en = (packet_valid && !PPB_packet_reset &&
                           command_byte_index != 0);

    /*
     * ACK starting after the first valid packet byte. Normally we don't 
     * need to ACK the last byte (we could check this with command_byte_index <
     * current_reply_bytes) but we don't do this because some escape commands
     * appear to require that final ACK anyway.
     */
    assign PPB_ack_strobe = PPB_reply_en;
    
    /*
     * Look up a reply byte according to the current command and index.
     */
    always @(current_command or const_offset or reply_byte_index or input_state_data or
	     current_reply_words or current_mode or analog_mode)

      casez ({current_command[3:0], const_offset, reply_byte_index})

	  /* Reply byte 0: Output drivers are not active, we don't care. */
	  {4'h?, 1'b?, 5'h00}: PPB_reply 				   = 8'hXX;

	  /* Reply byte 1: Current controller mode and reply length. */
	  {4'h?, 1'b?, 5'h01}: PPB_reply 				   = {current_mode, current_reply_words};

	  /* Reply byte 2: Header padding */
	  {4'h?, 1'b?, 5'h02}: PPB_reply 				   = PADDING_BYTE;
	  
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h03}: PPB_reply 		   = 8'h00;   // (Unknown)
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h04}: PPB_reply 		   = 8'h00;   // (Unknown)
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h05}: PPB_reply 		   = 8'h02;   // (Unknown)
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h06}: PPB_reply 		   = 8'h00;   // (Unknown)
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h07}: PPB_reply 		   = 8'h00;   // (Unknown)
	  {CMD_INIT_PRESSURE[3:0], 1'b?, 5'h08}: PPB_reply 		   = PADDING_BYTE;

	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h03}: PPB_reply    = 8'hFF;   // Bitmask of available polling results
	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h04}: PPB_reply    = 8'hFF;   //  ...
	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h05}: PPB_reply    = 8'h03;   //  ...
	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h06}: PPB_reply    = 8'h00;   //  ...
	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h07}: PPB_reply    = 8'h00;   //  ...
	  {CMD_GET_AVAILABLE_POLL_RESULTS[3:0], 1'b?, 5'h08}: PPB_reply    = PADDING_BYTE;

	  /*
	   * Support up to 18 bytes of replies, enough for a Dual Shock
	   * controller with all the bells and whistles. This looks a lot
	   * uglier than using "5'h??" for the byte count, but Altera's
	   * synthesizer produces *much* better output for this case statement
	   * when it contains no overlaps.
	   * 
	   * Also, note the '?' for the lower bit in CMD_POLL. This is a shortcut
	   * to allow either CMD_POLL or CMD_ESCAPE.
	   */
	  {CMD_POLL[3:1], 2'b??, 5'h03}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h04}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h05}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h06}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h07}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h08}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h09}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0A}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0B}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0C}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0D}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0E}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h0F}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h10}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h11}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h12}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h13}: PPB_reply 			   = input_state_data;
	  {CMD_POLL[3:1], 2'b??, 5'h14}: PPB_reply 			   = input_state_data;

	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h05}: PPB_reply 		   = 8'h00;
	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h06}: PPB_reply 		   = 8'h00;
	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h07}: PPB_reply 		   = 8'h00;
	  {CMD_SET_MAJOR_MODE[3:0], 1'b?, 5'h08}: PPB_reply 		   = 8'h00;

	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h03}: PPB_reply 		   = 8'h03;   // (Unknown)
	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h04}: PPB_reply 		   = 8'h02;   // (Unknown)
	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h05}: PPB_reply 		   = {7'h00, analog_mode};
	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h06}: PPB_reply 		   = 8'h02;   // (Unknown)
	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h07}: PPB_reply 		   = 8'h01;   // (Unknown)
	  {CMD_READ_EXT_STATUS_1[3:0], 1'b?, 5'h08}: PPB_reply 		   = 8'h00;   // (Unknown)

	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h05}: PPB_reply 		   = 8'h01;
	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h06}: PPB_reply 		   = 8'h02;
	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h07}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_1[3:0], 1'b0, 5'h08}: PPB_reply 		   = 8'h0a;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h05}: PPB_reply 		   = 8'h01;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h06}: PPB_reply 		   = 8'h01;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h07}: PPB_reply 		   = 8'h01;
	  {CMD_READ_CONST_1[3:0], 1'b1, 5'h08}: PPB_reply 		   = 8'h14;

	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h05}: PPB_reply 		   = 8'h02;
	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h06}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h07}: PPB_reply 		   = 8'h01;
	  {CMD_READ_CONST_2[3:0], 1'b0, 5'h08}: PPB_reply 		   = 8'h00;

	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h05}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h06}: PPB_reply 		   = 8'h04;
	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h07}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b0, 5'h08}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h03}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h04}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h05}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h06}: PPB_reply 		   = 8'h07;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h07}: PPB_reply 		   = 8'h00;
	  {CMD_READ_CONST_3[3:0], 1'b1, 5'h08}: PPB_reply 		   = 8'h10;

	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h03}: PPB_reply 	   = 8'hFF;   // XXX
	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h04}: PPB_reply 	   = 8'hFF;   // XXX
	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h05}: PPB_reply 	   = 8'hFF;   // XXX
	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h06}: PPB_reply 	   = 8'hFF;   // XXX
	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h07}: PPB_reply 	   = 8'hFF;   // XXX
	  {CMD_SET_POLL_CMD_FORMAT[3:0], 1'b?, 5'h08}: PPB_reply 	   = 8'hFF;   // XXX

	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h03}: PPB_reply 	   = 8'h00;
	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h04}: PPB_reply 	   = 8'h00;
	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h05}: PPB_reply 	   = 8'h00;
	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h06}: PPB_reply 	   = 8'h00;
	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h07}: PPB_reply 	   = 8'h00;
	  {CMD_SET_POLL_RESULT_FORMAT[3:0], 1'b?, 5'h08}: PPB_reply 	   = PADDING_BYTE;

	  default: PPB_reply 						   = 8'hXX;
      endcase

     
    /********************************************************************
     * 
     * Controller state
     *
     */

    reg escape_mode;
    reg analog_mode;

    reg [3:0] poll_reply_words;
    
    wire [3:0] current_reply_words    = escape_mode ? REPLY_LEN_ESCAPE : poll_reply_words;
    wire [4:0] current_reply_bytes    = 5'h03 + {current_reply_words, 1'b0};
    wire [3:0] current_mode 	      = escape_mode ? MODE_ESCAPE : (analog_mode ? MODE_ANALOG : MODE_DIGITAL);

    always @(posedge clk or posedge controller_reset)
	if (controller_reset) begin
	    escape_mode        <= 0;
	    analog_mode        <= 0;
	    poll_reply_words   <= REPLY_LEN_DIGITAL;
	end
	else if (PPB_command_strobe && packet_valid)
	  case ({current_command[3:0], command_byte_index})

	      {CMD_SET_MAJOR_MODE[3:0], 5'h03}: begin
		  /*
		   * Setting major mode: The first byte of the command's payload
		   * is 1 to enter analog mode, 0 to enter digital mode. In either
		   * case, other controller state is reset.
		   */
		  analog_mode 	     <= PPB_command[0];
		  poll_reply_words   <= PPB_command[0] ? REPLY_LEN_ANALOG : REPLY_LEN_DIGITAL;
	      end

	      {CMD_ESCAPE[3:0], 5'h03}: begin
		  /*
		   * Entering or leaving escape mode
		   */
		  escape_mode 	     <= PPB_command[0];
	      end

	      {CMD_POLL[3:0], 5'h03}: begin
		  /*
		   * Poll command always kicks us out of escape mode.
		   */
		  escape_mode 	     <= 1'b0;
	      end

	  endcase

    
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