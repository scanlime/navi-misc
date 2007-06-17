/************************************************************************
 *
 * Simple logic analyzer using an fpga4fun Saxo board.
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module saxoscope(FX2_CLK, FX2_FD, FX2_SLRD, FX2_SLWR, FX2_flags, 
		 FX2_PA_2, FX2_PA_3, FX2_PA_4, FX2_PA_5, FX2_PA_6, FX2_PA_7,
		 LED, BUTTON, PROBE);

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
     * 
     * One LED just blinks to indicate activity and clock speed, the
     * other indicates error. The button is our reset.
     */

    output [1:0] LED;
    input 	 BUTTON;

    reg [22:0] 	 led_counter;
    wire         error;
    wire         reset 	  = ~BUTTON;
    assign       LED[1]   = error;
    assign 	 LED[0]   = led_counter[22];

    always @(posedge FIFO_clk or posedge reset)
      if (reset)
	led_counter   <= 0;
      else
	led_counter   <= led_counter + 1;

    
    /********************************************************************
     * 
     * Probe input
     *
     * Synchronized for 2 clock cycles.
     */

    input [7:0]	 PROBE;
    reg [7:0] 	 PROBE_s1;
    reg [7:0] 	 PROBE_sync;

    always @(posedge FIFO_clk or posedge reset)
      if (reset) begin
	  PROBE_s1     <= 0;
	  PROBE_sync   <= 0;
      end
      else begin
	  PROBE_s1     <= PROBE;
	  PROBE_sync   <= PROBE_s1;
      end


    /********************************************************************
     * 
     * Data capture.
     *
     * Continuously send data from the probe to FIFO2.
     */

    assign FIFO_RD = 1'b0;
    assign FIFO_WR = 1'b1;
    assign FIFO_PKTEND = 1'b0;
    assign FIFO_DATAIN_OE = 1'b0;
    assign FIFO_DATAOUT_OE = 1'b1;
    assign FIFO_FIFOADR = 2'b10;    /* FIFO4 */
    assign FIFO_DATAOUT = PROBE_sync;

    assign error = FIFO4_full;
    
endmodule