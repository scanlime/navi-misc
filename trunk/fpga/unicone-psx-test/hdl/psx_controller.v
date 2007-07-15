/************************************************************************
 *
 * Emulates the Playstation 2 Dual Shock controller, using controller
 * data from any of several "data sources". Each data source is bound
 * to at most one controller port, and all data sources for a particular
 * controller are mixed together on read.
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
     * Parallel Playstation Bus interface with four controllers.
     *
     */

    wire [3:0] PPB_irq_flags;

    reg [1:0]  PPB_port;
    wire       PPB_ack;
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
     * SRAM for controller and data-source state.
     * 
     * Memory layout:
     *
     *   0x00: Controller port 0
     *     0x00:  Misc state
     *       [0]:  packet_valid
     *     0x01:  Mode and reply length
     *   0x20: Controller port 1
     *   0x40: Controller port 2
     *   0x60: Controller port 3
     *
     *   0x80: Data source 0
     *     0x80-0x91: Controller data
     *     0x9F:  Controller port number for data source 0
     *   0xA0: Data source 1
     *   0xC0: Data source 2
     *   0xE0: Data source 3
     *
     */

    wire [7:0] mem_addr;
    wire [7:0] mem_wdata;
    wire       mem_wr;
    wire [7:0] mem_rdata;

    lpm_ram_dp mem(.wraddress(mem_addr),
		   .rdaddress(mem_addr),
		   .wrclock(clk),
		   .rdclock(clk),
		   .wren(mem_wr),
		   .rdclken(1'b1),
		   .data(mem_wdata),
		   .q(mem_rdata));

    defparam   mem.lpm_rdaddress_control    = "REGISTERED",
	       mem.lpm_indata 		    = "REGISTERED",
	       mem.lpm_outdata 		    = "UNREGISTERED",
	       mem.lpm_hint 		    = "USE_EAB=ON",
	       mem.lpm_wraddress_control    = "REGISTERED",
	       mem.lpm_width 		    = 8,
	       mem.lpm_widthad 		    = 8;


    /********************************************************************
     * 
     * Main state machine
     *
     */

    parameter  S_IDLE = 3'h0;
    parameter  S_READ_COMMAND = 3'h1;
    parameter  S_PROCESS_COMMAND_STATE = 3'h2;
    parameter  S_WRITE_REPLY = 3'h3;

    reg [2:0]  state;
    reg [7:0]  command_byte;
    reg [4:0]  byte_index;

    /* 'packet_valid' signal, usable during S_PROCESS_COMMAND_STATE */
    wire packet_valid  = (byte_index == 5'h00 ? command_byte == 8'h01 :
			  mem_rdata[0]);
    
    assign PPB_ack  = (state == S_PROCESS_COMMAND_STATE && packet_valid);
    
    assign mem_addr = (state == S_READ_COMMAND ? {1'b0, PPB_port, 5'h00} :
		       state == S_PROCESS_COMMAND_STATE ? {1'b0, PPB_port, 5'h00} :
		       8'hXX);

    assign mem_wdata = (state == S_PROCESS_COMMAND_STATE ? {7'b0000000, packet_valid} :
			8'hXX);

    assign mem_wr = (state == S_PROCESS_COMMAND_STATE);
    
    
    always @(posedge clk or posedge reset)
      if (reset) begin
	  state 	   <= S_IDLE;

	  PPB_port 	   <= 0;
	  PPB_read 	   <= 0;
	  PPB_write 	   <= 0;
	  PPB_reply 	   <= 0;

	  command_byte 	   <= 0;
	  byte_index 	   <= 0;
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

		 PPB_write 	<= 0;
	     end

	     S_READ_COMMAND: begin
		 if (PPB_done) begin
		     /*
		      * We have a new command byte, hurray.
		      * Start loading the state for this port.
		      */
		     PPB_read 	    <= 0;
		     command_byte   <= PPB_command;
		     byte_index     <= PPB_index;
		     state 	    <= S_PROCESS_COMMAND_STATE;
		 end

		 PPB_write 	    <= 0;
	     end

	     S_PROCESS_COMMAND_STATE: begin
		 /*
		  * We just loaded the current state from RAM,
		  * and we have a new command byte from the current
		  * port. Figure out what action to take next,
		  * and write back any modified state.
		  */

		 PPB_read 	    <= 0;

		 if (packet_valid) begin
		     PPB_reply 	 <= 8'h42;
		     PPB_write 	 <= packet_valid;
		     state 	 <= S_WRITE_REPLY;
		 end
		 else begin
		     PPB_reply 	 <= 8'hXX;
		     PPB_write 	 <= 1'b0;
		     state 	 <= S_IDLE;
		 end
	     end

	     S_WRITE_REPLY: begin
		 if (PPB_done) begin
		     PPB_write 	 <= 0;
		     state 	 <= S_IDLE;
		 end
	     end
	     
	   endcase
endmodule

