/*
 * led_hex.v - Hexadecimal display driver for a 4-digit 7-segment multiplexed LED display.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
module led_hex(sclk, reset, number, led_c, led_a);
    input sclk, reset;
    input [15:0] number;
    output [7:0] led_c;
    output [3:0] led_a;
    
    reg [7:0] led_c;
    reg [3:0] led_a;
    reg [1:0] digit;
    
    wire [3:0] hex = (digit == 0 ? number[15:12] :
                      digit == 1 ? number[11:8] :
                      digit == 2 ? number[7:4] :
                      digit == 3 ? number[3:0] : 4'hX);

    wire [3:0] anode = (digit == 0 ? 4'b1110 :
                        digit == 1 ? 4'b1101 :
                        digit == 2 ? 4'b1011 :
                        digit == 3 ? 4'b0111 : 4'hX);

    wire [6:0] seg = (hex == 4'h0 ? 7'b1000000 :
                      hex == 4'h1 ? 7'b1111001 :
                      hex == 4'h2 ? 7'b0100100 :
                      hex == 4'h3 ? 7'b0110000 :
                      hex == 4'h4 ? 7'b0011001 :
                      hex == 4'h5 ? 7'b0010010 :
                      hex == 4'h6 ? 7'b0000010 :
                      hex == 4'h7 ? 7'b1111000 :
                      hex == 4'h8 ? 7'b0000000 :
                      hex == 4'h9 ? 7'b0010000 :
                      hex == 4'hA ? 7'b0001000 :
                      hex == 4'hB ? 7'b0000011 :
                      hex == 4'hC ? 7'b1000110 :
                      hex == 4'hD ? 7'b0100001 :
                      hex == 4'hE ? 7'b0000110 :
                      hex == 4'hF ? 7'b0001110 : 7'hXX);
        
    always @(posedge sclk or posedge reset)
        if (reset) begin
            digit <= 0;
            led_a <= 4'b1111;
            led_c <= 8'b11111111;
        end
        else begin
            digit <= digit + 1;
            led_a <= anode;
            led_c <= {1'b1, seg};
        end
endmodule