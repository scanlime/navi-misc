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
    defparam 	input_state.DATA_BITS = 8;
    defparam 	input_state.ADDR_BITS = 5;
    sync_dualport_sram input_state(clk, write_en, write_addr, write_data,
				   5'h00, input_state_data);    

    
    /********************************************************************
     * 
     * Parallel Playstation Bus interface with four controllers.
     *
     */

    wire [3:0] PPB_irq_flags;

    reg [1:0]  PPB_port;
    reg        PPB_ack;
    reg        PPB_read;
    reg        PPB_write;
    reg [7:0]  PPB_reply;

    wire       PPB_done;
    wire [7:0] PPB_command;
    wire [4:0] PPB_index;

    psx_ppb_interface #(CLOCK_MHZ) ppb1(clk, reset,
					PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
					PPB_irq_flags,
					PPB_port, PPB_ack, PPB_read, PPB_write, PPB_reply,
					PPB_done, PPB_command, PPB_index);

    
    /********************************************************************
     * 
     * Main state machine
     *
     */

    parameter  S_IDLE = 3'h0;
    parameter  S_READ_COMMAND = 3'h1;
    parameter  S_WRITE_REPLY = 3'h2;

    reg [2:0]  state;

    always @(posedge clk or posedge reset)
      if (reset) begin
	  state       <= S_IDLE;
	  PPB_port    <= 0;
	  PPB_ack     <= 0;
	  PPB_read    <= 0;
	  PPB_write   <= 0;
	  PPB_reply   <= 0;
      end
      else case (state)

	     S_IDLE: begin
		 /*
		  * Visit the next port that has a command byte ready
		  */
		 if (PPB_irq_flags != 0) begin
		     state 	<= S_READ_COMMAND;
		     PPB_read 	<= 1;
		     PPB_port 	<= (PPB_irq_flags[0] ? 2'h0 :
				    PPB_irq_flags[1] ? 2'h1 :
				    PPB_irq_flags[2] ? 2'h2 :
				    PPB_irq_flags[3] ? 2'h3 :
				    2'hX);
		 end
		 else begin
		     PPB_read 	<= 0;
		 end

		 PPB_ack 	<= 0;
		 PPB_write 	<= 0;
	     end

	     S_READ_COMMAND: begin
		 if (PPB_done) begin
		     /*
		      * We now have a new command byte for this port.
		      * Invert it, and start sending it as a reply.
		      */
		     PPB_read 	 <= 0;
		     PPB_ack 	 <= 1;
		     PPB_write 	 <= 1;
		     PPB_reply 	 <= PPB_index;
		     state 	 <= S_WRITE_REPLY;
		 end
	     end

	     S_WRITE_REPLY: begin
		 PPB_ack 	 <= 0;
		 if (PPB_done) begin
		     PPB_write 	 <= 0;
		     state 	 <= S_IDLE;
		 end
	     end
	     
	   endcase
endmodule

