/************************************************************************
 *
 * Emulates the Playstation 2 Dual Shock controller.
 * 
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psx_quad_controller(clk, reset,
			   PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
			   write_addr, write_data, write_en);

    parameter CLOCK_MHZ = 25;

    input     clk, reset;

    inout [3:0] PSX_ack;
    input [3:0] PSX_clk;
    input [3:0] PSX_sel;
    input [3:0] PSX_cmd;
    inout [3:0] PSX_dat;

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
				   5'h00, input_state_data);    

    
    /********************************************************************
     * 
     * Parallel Playstation Bus interface with four controllers.
     *
     */

    wire [1:0] PPB_cmd_port;
    wire [4:0] PPB_cmd_index;
    wire [7:0] PPB_cmd_byte;
    wire       PPB_cmd_strobe;

    wire [1:0] PPB_reply_port;
    reg [7:0]  PPB_reply_byte;
    reg        PPB_reply_strobe;
    
    wire [1:0] PPB_ack_port;
    reg        PPB_ack_strobe;

    psx_ppb_interface #(CLOCK_MHZ) ppb1(clk, reset,
					PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
					PPB_cmd_port, PPB_cmd_index, PPB_cmd_byte, PPB_cmd_strobe,
					PPB_reply_port, PPB_reply_byte, PPB_reply_strobe,
					PPB_ack_port, PPB_ack_strobe);


    /********************************************************************
     * 
     * Emulation state machine
     *
     */

    assign     PPB_reply_port = current_port;
    assign     PPB_ack_port = current_port;
    
    reg [1:0]  current_port;

    always @(posedge clk or posedge reset)
      if (reset) begin
	  current_port 	     <= 0;
	  PPB_reply_byte     <= 0;
	  PPB_reply_strobe   <= 0;
	  PPB_ack_strobe     <= 0;
      end
      else if (PPB_cmd_strobe) begin
	  current_port 	     <= PPB_cmd_port;
	  PPB_reply_byte     <= PPB_cmd_byte + 1;
	  PPB_reply_strobe   <= 1;
	  PPB_ack_strobe     <= 1;
      end
      else begin
	  PPB_reply_strobe   <= 0;
	  PPB_ack_strobe     <= 0;
      end

endmodule

