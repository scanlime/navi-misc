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
 * When a new command byte arrives, the PPB raises the port's corresponding
 * PPB_irq_flags bit. The flag is automatically lowered when a PPB_read
 * occurs for that port.
 * 
 * At any time, the PPB can be used to read the command and byte index
 * for a port, or to write the next reply byte for a port. Reads
 * and writes for a particular port should only be performed between
 * bytes. (After PPB_irq for that port, but before the PSX begins
 * requesting the next byte a few microseconds later).
 * 
 * The PPB host can make three types of requests from the PPB interface:
 * 
 *    - Pulse PPB_ack to request an acknowledgement pulse be sent
 *      on the port PPB_port.
 * 
 *    - Bring PPB_read high to read both the last command byte and the
 *      byte index on the port PPB_port. Between 1 and 3 clock
 *      cycles later, PPB_done will pulse. On the same cycle,
 *      PPB_command and PPB_index will be valid.
 * 
 *      PPB_read must stay high until PPB_done pulses, and it must be
 *      low on the next clock cycle unless another read cycle is desired.
 * 
 *    - Bring PPB_write high to write the next reply byte on port PPB_port.
 *      Between 1 and 3 clock cycles later, PPB_done will pulse to
 *      indicate that the write has completed. The value at PPB_reply
 *      must be valid for this entire time span.
 * 
 *      PPB_write must stay high until PPB_done pulses, and it must
 *      be low on the next clock cycle unless another read cycle is desired.
 *
 */
module psx_ppb_interface(clk, reset,
			 PSX_ack, PSX_clk, PSX_sel, PSX_cmd, PSX_dat,
			 PPB_irq_flags,
			 PPB_port, PPB_ack, PPB_read, PPB_write, PPB_reply,
			 PPB_done, PPB_command, PPB_index);

    parameter CLOCK_MHZ = 25;
    
    input clk, reset;

    inout [3:0] PSX_ack;
    input [3:0] PSX_clk;
    input [3:0] PSX_sel;
    input [3:0] PSX_cmd;
    inout [3:0] PSX_dat;

    output [3:0] PPB_irq_flags;

    input [1:0]  PPB_port;
    input 	 PPB_ack;
    input 	 PPB_read;
    input 	 PPB_write;
    input [7:0]  PPB_reply;

    output 	 PPB_done;
    output [7:0] PPB_command;
    output [4:0] PPB_index;

    
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
					     SSP_port_number, PPB_port,
					     SSP_begin, SSP_cmd_avail, SSP_cmd_data, SSP_reply_ready,
					     SSP_clear, PPB_ack, SSP_reply_data, SSP_reply_strobe);


    /********************************************************************
     * 
     * SRAM to store the internal per-port state of our controller emulation.
     * 
     * We have 4 controllers, and 25 bits of state to store for each:
     *   - 8-bit command bit count (3-bits, plus 5-bit byte counter)
     *   - 8-bit command shift register
     *   - 8-bit reply shift register
     *   - Reply enable bit
     * 
     * To fit this memory in one Altera EAB, we use a 16-bit bus and combine
     * all reply state and all command state into one address each.
     */

    wire [2:0] 	 mem_addr;
    wire [15:0]  mem_wdata;
    wire	 mem_wr;
    wire [15:0]  mem_rdata;

    lpm_ram_dp mem(.wraddress(mem_addr),
		   .rdaddress(mem_addr),
		   .wrclock(clk),
		   .rdclock(clk),
		   .wren(mem_wr),
		   .rdclken(1'b1),
		   .data(mem_wdata),
		   .q(mem_rdata));

    defparam 	 mem.lpm_rdaddress_control  = "REGISTERED",
		 mem.lpm_indata 	    = "REGISTERED",
		 mem.lpm_outdata 	    = "UNREGISTERED",
		 mem.lpm_hint 		    = "USE_EAB=ON",
		 mem.lpm_wraddress_control  = "REGISTERED",
		 mem.lpm_width 		    = 16,
		 mem.lpm_widthad 	    = 3;

    
    /********************************************************************
     * 
     * IRQ flags. Raise a port's flag when a new command byte is available,
     * lower it when that command byte is read.
     *
     */

    wire 	 raise_irq = (state == S_WR_SHIFT_COMMAND && mem_rdata[10:8] == 3'b111);
    wire 	 lower_irq = PPB_read;

    set_reset_flipflop sr_irq0(clk, reset,
			       raise_irq && SSP_port_number == 2'h0,
			       lower_irq && PPB_port 	    == 2'h0,
			       PPB_irq_flags[0]);
    
    set_reset_flipflop sr_irq1(clk, reset,
			       raise_irq && SSP_port_number == 2'h1,
			       lower_irq && PPB_port 	    == 2'h1,
			       PPB_irq_flags[1]);

    set_reset_flipflop sr_irq2(clk, reset,
			       raise_irq && SSP_port_number == 2'h2,
			       lower_irq && PPB_port 	    == 2'h2,
			       PPB_irq_flags[2]);

    set_reset_flipflop sr_irq3(clk, reset,
			       raise_irq && SSP_port_number == 2'h3,
			       lower_irq && PPB_port 	    == 2'h3,
			       PPB_irq_flags[3]);

    
    /********************************************************************
     * 
     * Main state machine. This polls each controller port in sequence,
     * taking action when any of the following events occur:
     * 
     *   1. A port is beginning a new packet (SSP_begin).
     *       - Send an SSP_clear
     *       - Store 0 to the bit counter
     * 
     *   2. A new command bit is available
     *       - Load the port's command shift register and the bit counter
     *       - Store the shift register, with the new bit shifted in,
     *         and an incremented bit counter.
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

    parameter 	 S_VISIT_PORT 			   = 3'h0;
    parameter    S_WR_ZERO_BITCOUNT = 3'h1;
    parameter    S_WR_ZERO_REPLY_BYTE = 3'h2;
    parameter 	 S_WR_SHIFT_COMMAND = 3'h3;
    parameter 	 S_WR_SHIFT_REPLY = 3'h4;
    parameter    S_WR_LOAD_REPLY = 3'h5;
    parameter    S_LOAD_COMMAND = 3'h6;

    reg [2:0] 	 state;

    wire 	 reply_byte_pending;
    set_reset_flipflop sr_reply(clk, reset, PPB_write, SSP_clear, reply_byte_pending);

    wire [2:0]   next_visitport_state  = (PPB_write ? S_WR_LOAD_REPLY :
					  PPB_read ? S_LOAD_COMMAND :
					  SSP_begin ? S_WR_ZERO_BITCOUNT :
					  SSP_cmd_avail ? S_WR_SHIFT_COMMAND :
					  SSP_reply_ready ? S_WR_SHIFT_REPLY :
					  S_VISIT_PORT);

    wire [1:0] 	 next_port_number      = SSP_port_number + 2'h1;

    assign 	 SSP_clear             = (state == S_VISIT_PORT &&
					  next_visitport_state != S_WR_LOAD_REPLY &&
					  next_visitport_state != S_LOAD_COMMAND);
    assign 	 SSP_reply_data        = (state == S_WR_SHIFT_REPLY ? mem_rdata[0] :
				          state == S_WR_LOAD_REPLY ? PPB_reply[0] :
					  1'bX);
    assign 	 SSP_reply_strobe      = (state == S_WR_SHIFT_REPLY ? mem_rdata[8] :
				          state == S_WR_LOAD_REPLY ? 1'b1 :
					  1'b0);
    
    assign 	 PPB_index             = mem_rdata[15:11];
    assign 	 PPB_command           = mem_rdata[7:0];
    assign       PPB_done              = (state == S_WR_LOAD_REPLY || state == S_LOAD_COMMAND);
    
    assign 	 mem_addr              = {(state                      == S_VISIT_PORT &&
					   next_visitport_state       == S_LOAD_COMMAND) ? PPB_port : SSP_port_number,

					  /* During S_VISIT_PORT, we pre-load the correct read address for the next command. */
					  (state 		      == S_VISIT_PORT ?
					   (next_visitport_state    == S_WR_ZERO_BITCOUNT ? 1'b1 :
					    next_visitport_state    == S_WR_ZERO_REPLY_BYTE ? 1'b0 :
					    next_visitport_state    == S_WR_SHIFT_COMMAND ? 1'b1 :
					    next_visitport_state    == S_WR_SHIFT_REPLY ? 1'b0 :
					    next_visitport_state    == S_WR_LOAD_REPLY ? 1'b0 :
					    next_visitport_state    == S_LOAD_COMMAND ? 1'b1 :
					    1'bX) :

					   state 		      == S_WR_ZERO_BITCOUNT ? 1'b1 :
					   state 		      == S_WR_ZERO_REPLY_BYTE ? 1'b0 :
					   state 		      == S_WR_SHIFT_COMMAND ? 1'b1 :
					   state 		      == S_WR_SHIFT_REPLY ? 1'b0 :
					   state 		      == S_WR_LOAD_REPLY ? 1'b0 :
					   state 		      == S_LOAD_COMMAND ? 1'b1 :
					   1'bX)};

    /*
     * Note that the initial byte count (PPB_index) is -1, to account for the fact
     * that the byte counter is incremented before S_LOAD_COMMAND.
     */
    assign 	 mem_wdata              = (state == S_WR_ZERO_BITCOUNT ? 16'b1111100000000000 :
					   state == S_WR_ZERO_REPLY_BYTE ? 16'h0000 :
					   state == S_WR_SHIFT_COMMAND ? {mem_rdata[15:8] + 8'h01, SSP_cmd_data, mem_rdata[7:1]} :
					   state == S_WR_SHIFT_REPLY ? {7'bXXXXXXX, mem_rdata[8], 1'bX, mem_rdata[7:1]} :
					   state == S_WR_LOAD_REPLY ? {9'bXXXXXXX10, PPB_reply[7:1]} :
					   16'hXXXX);
    
    assign 	 mem_wr                 = (state == S_WR_ZERO_BITCOUNT ||
					   state == S_WR_ZERO_REPLY_BYTE ||
					   state == S_WR_SHIFT_COMMAND||
					   state == S_WR_SHIFT_REPLY ||
					   state == S_WR_LOAD_REPLY);

    always @(posedge clk or posedge reset)
      if (reset) begin
	  SSP_port_number   <= 0;
	  state 	    <= S_VISIT_PORT;
      end
      else case (state)

	     S_VISIT_PORT: begin
		 state 	 <= next_visitport_state;

		 /*
		  * Note that PPB_port is a little strange:
		  * We need to warp SSP_port_number to PPB_port when we
		  * are about to change state to S_WR_LOAD_REPLY, so that
		  * the reply LSB goes to the right SSP port. For
		  * S_WR_LOAD_COMMAND on the other hand, we need to set
		  * mem_addr correctly on *this* clock cycle so that
		  * the command byte is available during S_LOAD_COMMAND.
		  */

		 if (next_visitport_state == S_WR_LOAD_REPLY)
		   SSP_port_number   <= PPB_port;
		 
		 else if (next_visitport_state == S_VISIT_PORT)
		   SSP_port_number   <= next_port_number;
	     end

	     S_WR_LOAD_REPLY: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	     S_LOAD_COMMAND: begin
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

	     S_WR_SHIFT_COMMAND: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	     S_WR_SHIFT_REPLY: begin
		 SSP_port_number     <= next_port_number;
		 state 		     <= S_VISIT_PORT;
	     end

	   endcase
    
endmodule