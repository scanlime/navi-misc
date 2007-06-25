/************************************************************************
 *
 * Test harness for the PSX controller emulation core. Includes
 * a controller emulator as well as a built-in saxoscope compatible
 * logic analyzer.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module psxtest(FX2_CLK, FX2_FD, FX2_SLRD, FX2_SLWR, FX2_flags, 
	       FX2_PA_2, FX2_PA_3, FX2_PA_4, FX2_PA_5, FX2_PA_6, FX2_PA_7,
	       LED, BUTTON, SERIAL_RX,
	       PSX_ACK, PSX_CLK, PSX_SEL, PSX_CMD, PSX_DAT);

    /********************************************************************
     * 
     * FIFO Interface:
     * 
     * At the board level, we have a generic interface to the Cypress FX2's
     * general purpose I/O and GPIF ports. We use the default Saxo firmware
     * which gives us FIFO access to several USB 2.0 bulk endpoints.
     * 
     * This logic is identical to that in the example code for Saxo: rename
     * the signals, and convert everything to active-high logic.
     */
     
    input       FX2_CLK;
    inout [7:0] FX2_FD;
    input [2:0] FX2_flags;
    output 	FX2_SLRD, FX2_SLWR;
    output 	FX2_PA_2;
    output 	FX2_PA_3;
    output 	FX2_PA_4;
    output 	FX2_PA_5;
    output 	FX2_PA_6;
    input 	FX2_PA_7;

    /* FIFO control inputs */
    wire 	FIFO_clk 		      = FX2_CLK;
    wire 	FIFO2_empty 		      = ~FX2_flags[0];
    wire 	FIFO2_data_available 	      = ~FIFO2_empty;
    wire 	FIFO3_empty 		      = ~FX2_flags[1];
    wire 	FIFO3_data_available 	      = ~FIFO3_empty;
    wire 	FIFO4_full 		      = ~FX2_flags[2];
    wire 	FIFO4_ready_to_accept_data    = ~FIFO4_full;
    wire 	FIFO5_full 		      = ~FX2_PA_7;
    wire 	FIFO5_ready_to_accept_data    = ~FIFO5_full;

    /* FIFO control outputs */
    wire 	FIFO_RD, FIFO_WR, FIFO_PKTEND, FIFO_DATAIN_OE, FIFO_DATAOUT_OE;
    wire [1:0] 	FIFO_FIFOADR;
    assign 	FX2_SLRD = ~FIFO_RD;
    assign 	FX2_SLWR = ~FIFO_WR;
    assign 	FX2_PA_2 = ~FIFO_DATAIN_OE;
    assign 	FX2_PA_3 = 1'b1;
    assign 	FX2_PA_4 = FIFO_FIFOADR[0];
    assign 	FX2_PA_5 = FIFO_FIFOADR[1];
    assign 	FX2_PA_6 = ~FIFO_PKTEND;

    /* Bidirectional data bus */
    wire [7:0] 	FIFO_DATAIN    = FX2_FD;
    wire [7:0] 	FIFO_DATAOUT;
    assign 	FX2_FD = FIFO_DATAOUT_OE ? FIFO_DATAOUT : 8'hZZ;

    
    /********************************************************************
     * 
     * LED and Button
     */

    output [1:0] LED;
    input 	 BUTTON;

    reg [20:0] 	 led_counter;
    wire         error;
    wire         activity;
    
    wire         reset 	  = ~BUTTON;
    assign       LED[1]   = error;
    assign 	 LED[0]   = ~led_counter[20];

    always @(posedge FIFO_clk or posedge reset)
      if (reset)
	led_counter   <= 0;
      else if (activity)
	led_counter   <= 0;
      else if (~led_counter[20])
	led_counter   <= led_counter + 1;

    
    /********************************************************************
     * 
     * Probe input
     *
     * Synchronized for 2 clock cycles.
     */

    reg [7:0] 	 PROBE_s1;
    reg [7:0] 	 PROBE_sync;

    always @(posedge FIFO_clk or posedge reset)
      if (reset) begin
	  PROBE_s1     <= 0;
	  PROBE_sync   <= 0;
      end
      else begin
	  PROBE_s1     <= {rx_byte_count[1:0], SERIAL_RX, PSX_DAT, PSX_CMD, PSX_SEL, PSX_CLK, PSX_ACK};
	  PROBE_sync   <= PROBE_s1;
      end


    /********************************************************************
     * 
     * Data capture.
     *
     * Continuously send data from the probe to FIFO2.
     */

    assign FIFO_RD 	= 1'b0;
    assign FIFO_WR = 1'b1;
    assign FIFO_PKTEND = 1'b0;
    assign FIFO_DATAIN_OE = 1'b0;
    assign FIFO_DATAOUT_OE = 1'b1;
    assign FIFO_FIFOADR = 2'b10;    /* FIFO4 */
   
    assign FIFO_DATAOUT = PROBE_sync;

    assign error = FIFO4_full;

    /********************************************************************
     * 
     * Playstation controller emulator (and serial port glue)
     *
     */

    inout  PSX_ACK;
    input  PSX_CLK;
    input  PSX_SEL;
    input  PSX_CMD;
    inout  PSX_DAT;

    input  SERIAL_RX;
    
    reg [7:0] 	write_addr;
    reg [7:0] 	write_data;
    reg         write_en;
    
    psx_controller #(12) controller_1(FIFO_clk, reset,
				      PSX_ACK, PSX_CLK, PSX_SEL, PSX_CMD, PSX_DAT,
				      write_addr[4:0], write_data, write_en);

    wire [7:0] 	rx_data;
    wire 	rx_strobe;
    wire 	rx_idle;
    reg [4:0] 	rx_byte_count;
    reg         rx_error;
    reg [3:0] 	rx_port_number;
    reg [4:0] 	rx_expected_size;
    
    serial_rx rx_port(FIFO_clk, reset, SERIAL_RX, rx_data, rx_strobe, rx_idle);
    
    assign 	activity = write_en;

    always @(posedge FIFO_clk or posedge reset) begin
	if (reset) begin
	    rx_byte_count      <= 0;
	    rx_error 	       <= 0;
	    rx_port_number     <= 0;
   	    rx_expected_size   <= 2;

	    write_addr 	       <= 0;
	    write_data 	       <= 0;
	    write_en 	       <= 0;
	end
	else if (rx_idle) begin
	    /* If the line goes idle, reset and prepare to receive a minimal (2-byte) packet */

	    rx_byte_count      <= 0;
	    rx_error 	       <= 0;
	    rx_port_number     <= 0;
	    rx_expected_size   <= 2;
	end
	else if (rx_byte_count == rx_expected_size) begin
	    rx_byte_count      <= 0;
	end
	else if (rx_strobe && !rx_error) begin

	    if (rx_byte_count == 0) begin
		/*
		 * This is the start-of-packet and slot address byte.
		 * Ignore the packet if this is wrong.
		 */

		rx_error 	 <= (rx_data[7:4] != 4'h5);
		rx_port_number 	 <= rx_data[3:0];

	    end
	    else if (rx_byte_count == 1) begin
		/*
		 * This is the state flags byte. It tells us how large
		 * a packet to expect.
		 * 
		 * XXX: Clean up these state flags.
		 */

		rx_expected_size   <= rx_data[2] ? 20 : (rx_data[1] ? 8 : (rx_data[0] ? 4 : 2));

  	    end
	    else begin
		/*
		 * This is a data byte.
		 */

		write_addr   <= rx_byte_count - 2;
		write_data   <= rx_data;
		write_en     <= (rx_port_number == 0);
	    end

	    rx_byte_count    <= rx_byte_count + 1;
	end
	else begin
	    write_en 	     <= 0;
	end
    end
    
endmodule