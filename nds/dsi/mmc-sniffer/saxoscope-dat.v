/************************************************************************
 *
 * Simple logic analyzer using an fpga4fun Saxo board.
 *
 * XXX: This version was modified for DSi NAND flash debugging.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module saxoscope(FX2_CLK, FX2_FD, FX2_SLRD, FX2_SLWR, FX2_flags, 
		 FX2_PA_2, FX2_PA_3, FX2_PA_4, FX2_PA_5, FX2_PA_6, FX2_PA_7,
		 LED, BUTTON, MMC_CLK, MMC_DAT, MMC_CMD,
		 DEBUG_mmc_clk10, DEBUG_fx_clk10);

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
     * One LED just blinks to indicate activity, the
     * other indicates error. The button is our reset.
     */

    output [1:0] LED;
    input 	 BUTTON;

    wire         error;
    wire         reset 	  = ~BUTTON;
    assign       LED[1]   = error;
    assign 	     LED[0]   = activity_reg & blink_out;

	/*
     * Activity state machine.
     */
     
    reg [23:0]  activity_ctr;
	reg 		activity_reg;

    always @(posedge FIFO_clk or posedge reset)
      if (reset) begin
		activity_ctr <= 0;
		activity_reg <= 0;
	  end
	  else if (MMC_dat_s4 != MMC_dat_s3 || MMC_cmd_s4 != MMC_cmd_s3) begin
		activity_ctr <= 0;
		activity_reg <= 1;
	  end
	  else if (activity_ctr == 26'b11111111_11111111_11111111) begin
		activity_reg <= 0;
	  end
	  else begin
		activity_ctr <= activity_ctr + 1;
	  end
		
	/*
     * Clock divider for blinking
     */

    reg [22:0] 	blink_reg;
	wire 		blink_out = blink_reg[22];
     
    always @(posedge FIFO_clk or posedge reset)
      if (reset)
		blink_reg <= 0;
      else
		blink_reg <= blink_reg + 1;

    
    /********************************************************************
     * 
     * MMC port input
     *
     */

	input        MMC_CLK;
	input        MMC_CMD;
    input [3:0]  MMC_DAT;
    
	/*
	 * Synchronize for two clock cycles,
	 * then latch for two additional clock cycles so we can detect edges.
	 */
	
	reg			 MMC_clk_s1;
	reg			 MMC_cmd_s1;
	reg [3:0]    MMC_dat_s1;

	reg			 MMC_clk_s2;
	reg			 MMC_cmd_s2;
	reg [3:0]    MMC_dat_s2;

	reg			 MMC_clk_s3;
	reg			 MMC_cmd_s3;
	reg [3:0]    MMC_dat_s3;

	reg			 MMC_clk_s4;
	reg			 MMC_cmd_s4;
	reg [3:0]    MMC_dat_s4;
	
    always @(posedge FIFO_clk or posedge reset)
      if (reset) begin
		MMC_clk_s1 <= 0;
		MMC_cmd_s1 <= 0;
		MMC_dat_s1 <= 0;
		
		MMC_clk_s2 <= 0;
		MMC_cmd_s2 <= 0;
		MMC_dat_s2 <= 0;

		MMC_clk_s3 <= 0;
		MMC_cmd_s3 <= 0;
		MMC_dat_s3 <= 0;

		MMC_clk_s4 <= 0;
		MMC_cmd_s4 <= 0;
		MMC_dat_s4 <= 0;
	  end
      else begin
		MMC_clk_s1 <= MMC_CLK;
		MMC_cmd_s1 <= MMC_CMD;
		MMC_dat_s1 <= MMC_DAT;
		
		MMC_clk_s2 <= MMC_clk_s1;
		MMC_cmd_s2 <= MMC_cmd_s1;
		MMC_dat_s2 <= MMC_dat_s1;

		MMC_clk_s3 <= MMC_clk_s2;
		MMC_cmd_s3 <= MMC_cmd_s2;
		MMC_dat_s3 <= MMC_dat_s2;

		MMC_clk_s4 <= MMC_clk_s3;
		MMC_cmd_s4 <= MMC_cmd_s3;
		MMC_dat_s4 <= MMC_dat_s3;
      end

	/* Detect positive edges on CLK, latch CMD/DAT. */
	
	reg			MMC_cmd_l;
	reg [3:0]	MMC_dat_l;
	reg			MMC_strobe_l;

    always @(posedge FIFO_clk or posedge reset)
      if (reset) begin
		MMC_cmd_l <= 0;
		MMC_dat_l <= 0;
		MMC_strobe_l <= 0;
	  end
      else begin
		if (MMC_clk_s4 && !MMC_clk_s3) begin
			MMC_cmd_l <= MMC_cmd_s4;
			MMC_dat_l <= MMC_dat_s4;
			MMC_strobe_l <= 1;
		end
		else begin
			MMC_strobe_l <= 0;
		end
	  end

    /********************************************************************
     * 
     * Data capture.
     *
     * Send MMC data to FIFO4 any time a byte is ready.
     */

    assign FIFO_RD = 1'b0;
    assign FIFO_WR = MMC_strobe_l;
    assign FIFO_PKTEND = 1'b0;
    assign FIFO_DATAIN_OE = 1'b0;
    assign FIFO_DATAOUT_OE = 1'b1;
    assign FIFO_FIFOADR = 2'b10;    /* FIFO4 */
    assign FIFO_DATAOUT = { err_warning_bit, 2'b00, MMC_cmd_l, MMC_dat_l };
    
    assign error = FIFO4_full;

	/*
     * If the FIFO fills up, warn the host that we lost some data.
     * We need to delay this warning bit until after the FIFO has room.
     */

	reg		err_warning_bit;

    always @(posedge FIFO_clk or posedge reset)
      if (reset)
		err_warning_bit <= 0;
	  else
		err_warning_bit <= error;
	  

    /********************************************************************
     *
     * Debug
     */
     
    reg [3:0] debug_mmc_clkdiv;
	output    DEBUG_mmc_clk10;
	reg		  DEBUG_mmc_clk10;

    always @(posedge MMC_CLK) begin
		if (debug_mmc_clkdiv == 10)
			debug_mmc_clkdiv <= 0;
		else
			debug_mmc_clkdiv <= debug_mmc_clkdiv + 1;
		DEBUG_mmc_clk10 <= debug_mmc_clkdiv >= 5;
	end
	
	reg [3:0] debug_fx_clkdiv;
	output    DEBUG_fx_clk10;
	reg		  DEBUG_fx_clk10;

    always @(posedge FIFO_clk) begin
		if (debug_fx_clkdiv == 10)
			debug_fx_clkdiv <= 0;
		else
			debug_fx_clkdiv <= debug_fx_clkdiv + 1;
		DEBUG_fx_clk10 <= debug_fx_clkdiv >= 5;
	end
		
endmodule