/************************************************************************
 *
 * Asynchronous serial receiver (8-N-1) with continuous majority-detect.
 *
 * Micah Dowty <micah@navi.cx>
 *
 ************************************************************************/


module serial_rx(clk, reset, serial_in,
		 parallel_out, parallel_out_strobe,
		 idle_timeout);

    parameter DIVISOR = 12000000 / 250000;      /* 250 kbaud */
    parameter IDLE_TIMEOUT = 12000000 / 1000;   /* Idle timeout = 1ms */

    parameter DIVISOR_BITS = 6;    
    parameter IDLE_TIMER_BITS = 14;

    input     clk;
    input     reset;
    input     serial_in;

    output [7:0] parallel_out;
    output 	 parallel_out_strobe;
    output 	 idle_timeout;

    reg [DIVISOR_BITS:0] divisor_timer;
    reg [DIVISOR_BITS:0] bit_accumulator;
    reg [IDLE_TIMER_BITS:0] idle_timer;

    wire received_bit = (bit_accumulator >= (DIVISOR/2));

    reg is_idle;
    reg [7:0] parallel_out;
    reg parallel_out_strobe;
    reg idle_timeout;
    reg [3:0] bit_count;
    wire serial_in_sync;

    d_flipflop_pair dff_serial_in(clk, reset, serial_in, serial_in_sync);
    
    always @(posedge clk or posedge reset)
      if (reset) begin
	  is_idle 		<= 1;
	  idle_timeout 		<= 0;
	  divisor_timer 	<= 0;
	  bit_accumulator 	<= 0;
	  idle_timer 		<= 0;
	  parallel_out 		<= 0;
	  parallel_out_strobe 	<= 0;
      end
      else if (is_idle) begin

	  /* Update the idle timer */
	  if (idle_timer == IDLE_TIMEOUT)
	    idle_timeout  <= 1;
	  else
	    idle_timer    <= idle_timer + {{IDLE_TIMER_BITS-1{1'b0}}, 1'b1};
	  
	  /* Wait for a start bit */
	  if (!serial_in_sync) begin
	      is_idle 		<= 0;
	      bit_count 	<= 0;
	      divisor_timer 	<= {{DIVISOR_BITS-1{1'b0}}, 1'b1};
	      bit_accumulator 	<= 0;   /* We just received a zero */
	  end

	  parallel_out_strobe 	<= 0;

      end
      else begin
	  /* We're actively receiving... */

	  if (divisor_timer == DIVISOR-1) begin
	      /* This is the last clock in the current bit */

	      divisor_timer 	<= 0;
	      bit_accumulator 	<= 0;

	      if (bit_count == 0) begin
		  /*
		   * Just received the start bit. If it was good, we can
		   * reset the idle timer. If not, back to idle state we go.
		   */

		  if (received_bit) begin
		      is_idle 	<= 1;
		  end
		  else begin
		      idle_timeout 	<= 0;
		      idle_timer 	<= 0;
		  end

		  bit_count 		<= bit_count + 4'h1;
		  parallel_out_strobe 	<= 0;

	      end
	      else if (bit_count 	 == 4'h9) begin
		  /*
		   * Just received the stop bit. If it was good, latch the
		   * finished byte and reset the idle timer. In either
		   * case, go to idle next. (We don't expect to see the next
		   * start bit immediately)
		   */

		  if (received_bit) begin
		      idle_timeout 	    <= 0;
		      idle_timer 	    <= 0;
		      parallel_out_strobe   <= 1;
		  end
		  else begin
		      parallel_out_strobe   <= 0;
		  end

		  is_idle 		    <= 1;
       		  bit_count 		    <= 0;
		  
	      end
	      else begin
		  /*
		   * Received a data byte. Shift it in.
		   */

		  parallel_out 		<= {received_bit, parallel_out[7:1]};
		  idle_timeout 		<= 0;
		  idle_timer 		<= 0;
		  bit_count 		<= bit_count + 4'h1;
		  parallel_out_strobe 	<= 0;		  
	      end
	  end
	  else begin
	      /* Add this sample to the accumulator... */

	      divisor_timer 	    <= divisor_timer + {{DIVISOR_BITS-1{1'b0}}, 1'b1};
	      parallel_out_strobe   <= 0;
	      if (serial_in_sync)
		bit_accumulator <= bit_accumulator + {{DIVISOR_BITS-1{1'b0}}, 1'b1};
	  end
      end
endmodule