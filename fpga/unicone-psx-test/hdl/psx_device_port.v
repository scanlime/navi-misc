/************************************************************************
 *
 * This is a full-duplex synchronous serial port for emulating
 * the device side of the Playstation's controller and memory card
 * slot protocol.
 * 
 * This module adapts the low-level I/O pins to a shared serial bus.
 * It does not contain any protocol-specific logic, but it does do
 * synchronization, timing, and edge detection. It also has a one-level
 * FIFO buffer for input and output data.
 *
 * We care about the following types of events on the bus:
 * 
 *   - A falling edge on SEL. This begins a new packet.
 *
 *   - A positive edge on CLK. This means that a new incoming
 *     data bit is available. This also begins an opportunity
 *     to generate an ACK pulse...
 *
 *   - ACK pulses. We are responsible for generating an ACK pulse
 *     after a positive CLK edge, assuming the host told us to send one.
 *
 *   - A negative edge on CLK. We output a buffered bit of data,
 *     and the buffer becomes ready for a new bit.
 *
 * We'll call our shared serial interface the Shared Serial Playstation
 * bus, or SSP. It represents the various conditions above using flags
 * that can be set or reset without constant attention from a controller
 * state machine.
 * 
 *   - SSP_begin goes HIGH when a new packet has started.
 *     This must be cleared by pulsing SSP_clear.
 * 
 *   - SSP_cmd_avail goes HIGH when a new command bit becomes
 *     available on SSP_cmd_data. It must be cleared by pulsing
 *     SSP_clear.
 * 
 *   - SSP_ack can be pulsed at any time, to cause an ACK pulse
 *     to be automatically generated after the next positive CLK edge.
 * 
 *   - SSP_reply_ready goes HIGH when we have room for a new reply
 *     bit. To indicate the presense of a new reply bit on SSP_reply_data,
 *     pulse SSP_reply_strobe. This has the side-effect of enabling the
 *     output buffer, which is automatically shut off when SEL goes back
 *     high. SSP_reply_ready is cleared with SSP_clear.
 * 
 * Note that reply bits always come before command bits.
 *
 * A separate module here is responsible for coordinating the operation
 * of several controllers. It communicates with individual device emulators
 * via a small shared memory block and an interrupt which indicates that new
 * data is available on a particular port.
 * 
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psx_device_port(clk, reset,
		       PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
		       SSP_begin, SSP_cmd_avail, SSP_cmd_data, SSP_reply_ready,
		       SSP_clear, SSP_ack, SSP_reply_data, SSP_reply_strobe);

    parameter CLOCK_MHZ = 25;

    // Typical ACK timing observed on a Dual Shock controller
    parameter ACK_DELAY_US = 8;
    parameter ACK_WIDTH_US = 3;

    parameter ACK_TIMER_BITS = 4;
    parameter ACK_PRESCALER_BITS = 5;

    input     clk, reset;

    input     PSX_clk, PSX_sel, PSX_cmd;
    inout     PSX_ack, PSX_dat;

    output    SSP_begin;
    output    SSP_cmd_avail;
    output    SSP_cmd_data;
    output    SSP_reply_ready;
    input     SSP_clear;
    input     SSP_ack;
    input     SSP_reply_data;
    input     SSP_reply_strobe;


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


    /********************************************************************
     * 
     * Core SSP interface
     *
     */

    /* Detect edges on CLK */
    wire   prev_clk;
    wire   clk_strobe_pos = sync_clk && ~prev_clk;
    wire   clk_strobe_neg = prev_clk && ~sync_clk;
    d_flipflop dff_clk_edge(clk, reset, sync_clk, prev_clk);

    /* Detect edges on SEL */
    wire   prev_sel;
    wire   sel_strobe_neg = prev_sel && ~sync_sel;
    d_flipflop dff_sel_edge(clk, reset, sync_sel, prev_sel);

    /* Generate SSP_begin */
    set_reset_flipflop sr_begin(clk, reset, sel_strobe_neg, SSP_clear, SSP_begin);

    /* Capture command bits on positive CLK edges */
    set_reset_flipflop sr_cmd(clk, reset, clk_strobe_pos, SSP_clear, SSP_cmd_avail);
    sync_latch sl_cmd(clk, reset, sync_cmd, clk_strobe_pos, SSP_cmd_data);

    /* Output reply bits on negative CLK edges */
    wire   reply_buffer;
    sync_latch sl_reply_out(clk, reset, reply_buffer, clk_strobe_neg, output_dat);
    sync_latch sl_reply_in(clk, reset, SSP_reply_data, SSP_reply_strobe, reply_buffer);
    set_reset_flipflop sr_reply(clk, reset, clk_strobe_neg, SSP_clear, SSP_reply_ready);

    /* Enable the output buffer on SSP_reply_strobe, disable at end of packet */
    set_reset_flipflop sr_reply_en(clk, reset, SSP_reply_strobe, sync_sel, output_dat_en);


    /********************************************************************
     * 
     * ACK timing generator.
     *
     * Generates an ACK pulse ACK_WIDTH_US microseconds wide, with a
     * ACK_DELAY_US microsecond delay after the last positive clock
     * edge in a byte as signalled by command_strobe.
     *
     * The ACK pulse is generated if SSP_ack is pulsed at any time
     * between clk_strobe_pos and the end of the ACK_DELAY window.
     *
     */

    wire [ACK_TIMER_BITS-1:0] ack_timer;
    wire ack_prescaler;
    wire ack_pulse_armed;
    wire packet_reset;
    assign packet_reset = reset || sync_sel;

    clock_divider #(ACK_PRESCALER_BITS) ack_clock_divider(clk, reset, ack_prescaler);
    
    counter_oneshot #(ACK_TIMER_BITS) ack_counter(.clk(clk), .reset(packet_reset),
						  .start(clk_strobe_pos),
						  .enable(ack_prescaler),
						  .out(ack_timer));
    
    d_flipflop ack_dff(clk, packet_reset,
		       ack_pulse_armed &&
		       ack_timer >= ((CLOCK_MHZ * ACK_DELAY_US) >> ACK_PRESCALER_BITS) &&
		       ack_timer < ((CLOCK_MHZ * (ACK_DELAY_US + ACK_WIDTH_US)) >> ACK_PRESCALER_BITS),
		       output_ack);

    set_reset_flipflop sr_ack(clk, packet_reset, SSP_ack, clk_strobe_pos, ack_pulse_armed);
endmodule


/*
 * Four instances of the psx_device_port, multiplexed onto one SSP. Multiplexing
 * our controllers while the data is still serial saves lots of FPGA area.
 */
module psx_quad_device_ports(clk, reset,
			     PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
			     SSP_port_number, SSP_ack_port_number,
			     SSP_begin, SSP_cmd_avail, SSP_cmd_data, SSP_reply_ready,
			     SSP_clear, SSP_ack, SSP_reply_data, SSP_reply_strobe);

    parameter CLOCK_MHZ = 25;

    input     clk, reset;

    inout [3:0] PSX_ack;
    input [3:0] PSX_clk;
    input [3:0] PSX_sel;
    input [3:0] PSX_cmd;
    inout [3:0] PSX_dat;

    input [1:0]  SSP_port_number;
    input [1:0]  SSP_ack_port_number;

    output 	 SSP_begin;
    output 	 SSP_cmd_avail;
    output 	 SSP_cmd_data;
    output 	 SSP_reply_ready;
    input 	 SSP_clear;
    input 	 SSP_ack;
    input 	 SSP_reply_data;
    input 	 SSP_reply_strobe;

    
    /********************************************************************
     * 
     * Controller Ports
     *
     */

    psx_device_port #(CLOCK_MHZ) port_0(clk, reset,
					PSX_ack[0], PSX_clk[0], PSX_sel[0], PSX_cmd[0], PSX_dat[0],
					SSP_begin_0,
					SSP_cmd_avail_0,
					SSP_cmd_data_0,
					SSP_reply_ready_0,
					(SSP_port_number == 2'h0) && SSP_clear,
					(SSP_ack_port_number == 2'h0) && SSP_ack,
					(SSP_port_number == 2'h0) && SSP_reply_data,
					(SSP_port_number == 2'h0) && SSP_reply_strobe);

    psx_device_port #(CLOCK_MHZ) port_1(clk, reset,
					PSX_ack[1], PSX_clk[1], PSX_sel[1], PSX_cmd[1], PSX_dat[1],
					SSP_begin_1,
					SSP_cmd_avail_1,
					SSP_cmd_data_1,
					SSP_reply_ready_1,
					(SSP_port_number == 2'h1) && SSP_clear,
					(SSP_ack_port_number == 2'h1) && SSP_ack,
					(SSP_port_number == 2'h1) && SSP_reply_data,
					(SSP_port_number == 2'h1) && SSP_reply_strobe);

    psx_device_port #(CLOCK_MHZ) port_2(clk, reset,
					PSX_ack[2], PSX_clk[2], PSX_sel[2], PSX_cmd[2], PSX_dat[2],
					SSP_begin_2,
					SSP_cmd_avail_2,
					SSP_cmd_data_2,
					SSP_reply_ready_2,
					(SSP_port_number == 2'h2) && SSP_clear,
					(SSP_ack_port_number == 2'h2) && SSP_ack,
					(SSP_port_number == 2'h2) && SSP_reply_data,
					(SSP_port_number == 2'h2) && SSP_reply_strobe);

    psx_device_port #(CLOCK_MHZ) port_3(clk, reset,
					PSX_ack[3], PSX_clk[3], PSX_sel[3], PSX_cmd[3], PSX_dat[3],
					SSP_begin_3,
					SSP_cmd_avail_3,
					SSP_cmd_data_3,
					SSP_reply_ready_3,
					(SSP_port_number == 2'h3) && SSP_clear,
					(SSP_ack_port_number == 2'h3) && SSP_ack,
					(SSP_port_number == 2'h3) && SSP_reply_data,
					(SSP_port_number == 2'h3) && SSP_reply_strobe);


    /********************************************************************
     * 
     * Output multiplexors
     *
     */

    wire 	 SSP_begin_0;
    wire 	 SSP_begin_1;
    wire 	 SSP_begin_2;
    wire 	 SSP_begin_3;

    wire 	 SSP_cmd_avail_0;
    wire 	 SSP_cmd_avail_1;
    wire 	 SSP_cmd_avail_2;
    wire 	 SSP_cmd_avail_3;

    wire 	 SSP_cmd_data_0;
    wire 	 SSP_cmd_data_1;
    wire 	 SSP_cmd_data_2;
    wire 	 SSP_cmd_data_3;

    wire 	 SSP_reply_ready_0;
    wire 	 SSP_reply_ready_1;
    wire 	 SSP_reply_ready_2;
    wire 	 SSP_reply_ready_3;
    
    assign SSP_begin = (SSP_port_number == 2'h0) ? SSP_begin_0 :
		       (SSP_port_number == 2'h1) ? SSP_begin_1 :
		       (SSP_port_number == 2'h2) ? SSP_begin_2 :
 	 	                                   SSP_begin_3 ;
    
    assign SSP_cmd_avail = (SSP_port_number == 2'h0) ? SSP_cmd_avail_0 :
			   (SSP_port_number == 2'h1) ? SSP_cmd_avail_1 :
			   (SSP_port_number == 2'h2) ? SSP_cmd_avail_2 :
 			                               SSP_cmd_avail_3 ;
  
    assign SSP_cmd_data = (SSP_port_number == 2'h0) ? SSP_cmd_data_0 :
			  (SSP_port_number == 2'h1) ? SSP_cmd_data_1 :
			  (SSP_port_number == 2'h2) ? SSP_cmd_data_2 :
 			                              SSP_cmd_data_3 ;
  
    assign SSP_reply_ready = (SSP_port_number == 2'h0) ? SSP_reply_ready_0 :
			     (SSP_port_number == 2'h1) ? SSP_reply_ready_1 :
			     (SSP_port_number == 2'h2) ? SSP_reply_ready_2 :
 			                                 SSP_reply_ready_3 ;
endmodule



/*
 * This module is responsible for coordinating the operation of four
 * PSX device ports, and providing a shared parallel interface to get
 * data into and out of the ports.
 * 
 * We'll call this shared memory bus the PPB (Parallel Playstation Bus).
 *
 * When a new data byte arrives, it is latched into our PPB outputs:
 *
 *    - PPB_cmd_strobe pulses. Each PPB_cmd_strobe pulse will
 *      be at least 4 clock cycles apart.
 *
 *    - PPB_cmd_port will have the current port number
 *
 *    - PPB_cmd_index will have the the 5-bit index
 *      of this byte within the current packet
 *
 *    - PPB_cmd_byte will hold the current command byte value.
 *      Note that PPB_cmd_byte is not latched on the same clock
 *      cycle as PPB_index, and it isn't guaranteed to
 *      be valid when PPB_cmd_strobe is not high.
 *
 * To set the *next* reply byte for the current port:
 *
 *    - Put a byte of data on PPB_reply_byte, corresponding
 *      with port PPB_reply_port.
 *
 *    - Pulse PPB_reply_strobe. This automatically enables
 *      the reply output drivers until the end of the packet.
 *      If you never pulse PPB_reply_strobe, the drivers stay
 *      high-Z.
 * 
 *    - PPB_reply_byte and PPB_reply_port must stay valid after
 *      PPB_reply_strobe, until the next PPB_cmd_strobe. *      
 * 
 * To acknowledge the command byte:
 * 
 *     - Pulse PPB_ack_strobe after PPB_cmd_strobe, within a
 *       few microseconds, to acknowledge port PPB_ack_port.
 *
 */
module psx_ppb_interface(clk, reset,
			 PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
			 PPB_cmd_port, PPB_cmd_index, PPB_cmd_byte, PPB_cmd_strobe,
			 PPB_reply_port, PPB_reply_byte, PPB_reply_strobe,
			 PPB_ack_port, PPB_ack_strobe);

    parameter CLOCK_MHZ = 25;
    
    input clk, reset;

    inout [3:0] PSX_ack;
    input [3:0] PSX_clk;
    input [3:0] PSX_sel;
    input [3:0] PSX_cmd;
    inout [3:0] PSX_dat;

    output [1:0] PPB_cmd_port;
    output [4:0] PPB_cmd_index;
    output [7:0] PPB_cmd_byte;
    output 	 PPB_cmd_strobe;

    input [1:0]  PPB_reply_port;
    input [7:0]  PPB_reply_byte;
    input 	 PPB_reply_strobe;
    
    input [1:0]  PPB_ack_port;
    input 	 PPB_ack_strobe;

    
    /********************************************************************
     * 
     * Low-level controller ports and Shared Serial Platstation bus.
     *
     */

    reg [1:0] 	 SSP_port_number;
    wire 	 SSP_begin;
    wire 	 SSP_cmd_avail;
    wire 	 SSP_cmd_data;
    wire 	 SSP_reply_ready;
    wire 	 SSP_clear;
    wire 	 SSP_reply_data;
    wire	 SSP_reply_strobe;

    psx_quad_device_ports #(CLOCK_MHZ) ports(clk, reset,
					     PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
					     SSP_port_number, PPB_ack_port,
					     SSP_begin, SSP_cmd_avail, SSP_cmd_data, SSP_reply_ready,
					     SSP_clear, PPB_ack_strobe, SSP_reply_data, SSP_reply_strobe);


    /********************************************************************
     * 
     * SRAM to store the internal per-port state of our controller emulation.
     *
     * The data bus is 9-bit, since we use an extra bit in REPLY_SHIFTREG to
     * indicate that the register has been set.
     */

    parameter 	 STATEMEM_BIT_COUNT = 2'b00;
    parameter    STATEMEM_CMD_SHIFTREG     = 2'b01;
    parameter    STATEMEM_REPLY_SHIFTREG   = 2'b10;

    wire [3:0] 	 statemem_addr;
    wire [8:0] 	 statemem_wdata;
    wire	 statemem_wr;
    wire [9:0] 	 statemem_rdata;

    lpm_ram_dp statemem(.wraddress(statemem_addr),
			.rdaddress(statemem_addr),
			.wrclock(clk),
			.rdclock(clk),
			.wren(statemem_wr),
			.rdclken(1'b1),
			.data(statemem_wdata),
			.q(statemem_rdata));

    defparam 	 statemem.lpm_rdaddress_control = "REGISTERED",
		 statemem.lpm_indata = "REGISTERED",
		 statemem.lpm_outdata = "UNREGISTERED",
		 statemem.lpm_hint = "USE_EAB=ON",
		 statemem.lpm_wraddress_control = "REGISTERED",
		 statemem.lpm_width = 9,
		 statemem.lpm_widthad = 4;

    
    /********************************************************************
     * 
     * Main state machine. This polls each controller port in sequence,
     * taking action when any of the following events occur:
     * 
     *   1. A port is beginning a new packet (SSP_begin).
     *       - Send an SSP_clear
     *       - Reset the port's bit counter
     * 
     *   2. A new command bit is available
     *       - Load the port's command shift register
     *       - Store the shift register, with the new bit shifted in
     *       - Load the bit counter
     *       - Store the incremented bit counter
     * 
     *   3. A new reply bit can be stored
     *       - Load the port's reply shift register
     *       - Store the reply shift register, shifted by one
     * 
     * Additionally, we have to service interrupt requests asking us to
     * store a new reply byte. These requests require us to store the upper
     * 7 bits of the reply byte in RAM for later, and immediately send
     * the LSB to the device port.
     */

    parameter 	 S_VISIT_PORT = 3'h0;
    parameter    S_WR_ZERO_BITCOUNT = 3'h1;
    parameter    S_WR_ZERO_REPLY_BYTE = 3'h2;
    parameter    S_RD_INC_BITCOUNT = 3'h3;
    parameter 	 S_WR_INC_BITCOUNT = 3'h4;
    parameter 	 S_WR_SHIFT_IN = 3'h5;
    parameter 	 S_WR_SHIFT_OUT = 3'h6;
    parameter    S_WR_REPLY_BYTE = 3'h7;

    reg [2:0] 	 state;
    reg [7:0] 	 PPB_cmd_byte;
    
    assign 	 SSP_clear = (state == S_VISIT_PORT && ~PPB_reply_strobe);
    assign 	 SSP_reply_data = (state == S_WR_SHIFT_OUT ? statemem_rdata[0] :
				   state == S_WR_REPLY_BYTE ? PPB_reply_byte[0] : 1'bX);
    assign 	 SSP_reply_strobe = (state == S_WR_SHIFT_OUT ? statemem_rdata[8] :
				     state == S_WR_REPLY_BYTE ? 1'b1 : 1'b0);

    wire [8:0]	 shifted_data = {statemem_rdata[8], SSP_cmd_data, statemem_rdata[7:1]};
    wire [1:0] 	 next_port_number = SSP_port_number + 2'h1;

    wire 	 reply_byte_pending;
    set_reset_flipflop sr_reply(clk, reset, PPB_reply_strobe, SSP_clear, reply_byte_pending);

    assign 	 PPB_cmd_port = SSP_port_number;
    assign 	 PPB_cmd_index = statemem_rdata[7:3];
    assign 	 PPB_cmd_strobe = (state == S_WR_INC_BITCOUNT && statemem_rdata[2:0] == 3'b111);
    
    assign 	 statemem_addr = {SSP_port_number,
				  (state == S_VISIT_PORT ? (SSP_cmd_avail ? STATEMEM_CMD_SHIFTREG :
							    STATEMEM_REPLY_SHIFTREG) :
				   state == S_WR_ZERO_BITCOUNT ? STATEMEM_BIT_COUNT :
				   state == S_RD_INC_BITCOUNT ? STATEMEM_BIT_COUNT :
				   state == S_WR_INC_BITCOUNT ? STATEMEM_BIT_COUNT :
				   state == S_WR_ZERO_REPLY_BYTE ? STATEMEM_REPLY_SHIFTREG :
				   state == S_WR_REPLY_BYTE ? STATEMEM_REPLY_SHIFTREG :
				   state == S_WR_SHIFT_IN ? STATEMEM_CMD_SHIFTREG :
				   state == S_WR_SHIFT_OUT ? STATEMEM_REPLY_SHIFTREG : 2'hX)};
    
    assign 	 statemem_wdata = (state == S_WR_ZERO_BITCOUNT ? 9'h00 :
				   state == S_WR_ZERO_REPLY_BYTE ? 9'h00 :
				   state == S_WR_INC_BITCOUNT ? {1'b0, statemem_rdata[7:0] + 8'h01} :
				   state == S_WR_SHIFT_IN ? shifted_data :
				   state == S_WR_SHIFT_OUT ? shifted_data :
				   state == S_WR_REPLY_BYTE ? {2'b10, PPB_reply_byte[7:1]} : 9'hXX);

    assign 	 statemem_wr = (state == S_WR_ZERO_BITCOUNT ||
				state == S_WR_ZERO_REPLY_BYTE ||
				state == S_WR_INC_BITCOUNT ||
				state == S_WR_SHIFT_IN ||
				state == S_WR_SHIFT_OUT ||
				state == S_WR_REPLY_BYTE);

    always @(posedge clk or posedge reset)
      if (reset) begin
	  SSP_port_number   <= 0;
	  PPB_cmd_byte 	    <= 0;
	  state 	    <= S_VISIT_PORT;
      end
      else case (state)

	     S_VISIT_PORT: begin
		 /* We just switched to a new port. Decide what to do... */

		 if (PPB_reply_strobe) begin
		   /*
		    * Take a break to store a reply byte, which may be for a different port.
		    * PPB_reply_byte must be higher priority than SSP_cmd_avail.
		    */
		     state 	       <= S_WR_REPLY_BYTE;
		     SSP_port_number   <= PPB_reply_port;
		 end
		 
		 else if (SSP_begin)
		   state 	       <= S_WR_ZERO_BITCOUNT;
		 
		 else if (SSP_cmd_avail)
		   state   <= S_WR_SHIFT_IN;
		 
		 else if (SSP_reply_ready)
		   state   <= S_WR_SHIFT_OUT;
		 
		 else
		   SSP_port_number   <= next_port_number;
	     end

	     S_WR_REPLY_BYTE: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end
	     
	     S_WR_ZERO_BITCOUNT: begin
		 state 		     <= S_WR_ZERO_REPLY_BYTE;
	     end

	     S_WR_ZERO_REPLY_BYTE: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	     S_WR_SHIFT_IN: begin
		 state 		     <= S_RD_INC_BITCOUNT;
		 PPB_cmd_byte 	     <= shifted_data[7:0];
	     end

	     S_WR_SHIFT_OUT: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	     S_RD_INC_BITCOUNT: begin
		 state 		     <= S_WR_INC_BITCOUNT;
	     end

	     S_WR_INC_BITCOUNT: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	   endcase
    
endmodule