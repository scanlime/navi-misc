{{

OpticalProximity
----------------

This spin object tries to robustly and quickly detect the relative
distance/angle between an LED and a light sensor. It does its best
to work across a wide range of ambient light levels, by modulating
the LED and ratiometrically comparing the light levels when the LED
is at full power and 1/2 power.

The LED is any typical LED that we can modulate with high-frequency
PWM. It could even be infrared. The light sensor is a TSL230R
programmable light-to-frequency converter by Texas Advanced
Optoelectronic Solutions (TAOS).

Recommended schematic for low-noise operation:

      +5v
       ┬
       │
        220 Ω                                               
       │                                                      
       ┣─┳──────────┳──────────────┐                                                    
       │ │          │ TSL230R      │               
       │ │          │ ┌──────────┐ │               
      │ 0.1 µF   ┣─┤ S0    S3 ├─┼─┐                      
       │ │          └─┤ S1    S2 ├─┫ │ 1.8 kΩ                    
       │22 µF    ┌─┤ /OE  OUT ├─┼─┼────── freqPin                  
       │ │          ┣─┤ GND  Vdd ├─┘ │                     
       │ │          │ └──────────┘   │                         
       ┣─┻──────────┻────────────────┘           
                                    
                         
               100 Ω      
      ledPin ──────┐ 
                         

The TSL230R can be hard-wired for maximum (100x) sensitivity, and
the 2x frequency divisor. Low power supply noise is very important,
hence the RC filter. You should be sure to use separate ground return
wires for the TSL230R and the LED.
      
For best results, the mechanical environment is also quite important.
The absolute level of ambient light does not affect this object's
output, but any varying ambient light (like a fluorescent lamp) can
introduce noise. Additionally. it's best to use a diffuse surface
between the sensor and the LED in order to reduce nonlinearity in
response to changes in angle.

I use the following mechanical setup:

   
   ┌──┐1
  2│┌─┐└──┤
   ││S│   │3
   │└─┘┌──┤
   └──┘

  (S) TSL230R, in transparent 8-pin DIP package.

  (1) Front of TSL230R covered with 3/8" black heat shrink tubing.

  (2) Back of TSL230R wrapped in black electrical tape, carefully
      folded around the IC's legs. Overlap it with the edges of the
      heat shrink tubing.

  (3) End of heat shrink tubing covered with a sheet of masking
      tape, as a diffuser. If necessary, epoxy it to the heat
      shrink tubing.

  (1a) Alternatively, use heat shrink tubing of the same color as
       your LED, and crimp the end closed while the tubing is still
       warm. This will do a better job at filtering out ambient
       light of other colors.

     
┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

CON

  ' Default maximum update rate, in Hz

  DEFAULT_UPDATE_RATE = 4000
  
  ' PWM frequency for the LED. The higher the better, but if it's
  ' too high we'll never overcome the parasitic capacitance of the
  ' LED and its wiring.
  '
  ' To tune PWM_FREQ, monitor the LED voltage with an oscilloscope
  ' and increase this parameter until you see a triangle wave instead
  ' of a square wave.
  '
  ' The higher this is relative to the TSL230R frequency, the less
  ' jitter we'll see in dimPeriod.

  PWM_MODE = %00100 << 26
  PWM_FREQ = $240000

  ' Float32 constants
  
  SignFlag      = $1
  ZeroFlag      = $2
  NaNFlag       = $8

  ' cogdata offsets

  COG_TIMESTAMP   = 0    'OUT,   cnt value for latest sample
  COG_OUTPUT      = 1    'OUT,   integer sample
  COG_MINCYCLES   = 2    'IN,    minimum # of clock cycles per reading
  COG_CTRA        = 3    'CONST, initialization value for CTRA
  COG_FREQMASK    = 4    'CONST, pin mask for TSL230R frequency output
  COG_LEDMASK     = 5    'CONST, pin mask for LED
  COG_DATA_SIZE   = 6    '(Must be last)

VAR
  long cog
  long lastReadTimestamp
  long cogdata[COG_DATA_SIZE]
  
PUB start(freqPin, ledPin) : okay
  '' Initialize the sensor, start its driver cogs, and wait for it
  '' to take a single sample. This uses two cogs: one to sample the
  '' frequency input, and one to do all the floating-point math.
  ''
  '' freqPin must be the pin connected to the TSL230R's output, and
  '' ledPin must be connected to the reference LED.

  ' Initialize cog parameters
  longfill(@cogdata, 0, COG_DATA_SIZE)
  cogdata[COG_CTRA] := PWM_MODE + ledPin
  cogdata[COG_FREQMASK] := |< freqPin
  cogdata[COG_LEDMASK] := |< ledPin
  setUpdateRate(DEFAULT_UPDATE_RATE)

  cog := cognew(@entry, @cogdata) 

  if cog < 0
    stop
    okay := 0
  else
    ' Wait for the first sample
    repeat until cogdata[COG_TIMESTAMP]
    okay := 1
     
PUB stop
  '' Immediately turn off the LED and stop the driver cogs.

  if cog > 0
    cogstop(cog)
  cog := -1

PUB setUpdateRate(hz)
  '' Set the maximum update rate, in Hz.
  '' Lower update rates provide better accuracy, but of course they're slower.

  cogdata[COG_MINCYCLES] := clkfreq / hz

PUB read : result | stamp
  '' Take a ratiometric proximity reading. If no new data is
  '' available since the last call, this will block until a new
  '' reading is available.
  ''
  '' The result varies linearly with the amount of light reaching
  '' the sensor from the LED, after subtracting all ambient light.
  ''
  '' The result's units aren't particularly meaningful. It is a
  '' difference of two frequencies, in Hz, scaled by an arbitrary
  '' constant. (See the calculations in the driver cog's source.)
  ''
  '' The relationship between frequency and light level is linear,
  '' but it will depend on the individual TSL230R device in use.
  '' As a result, the caller should have a way to calibrate the
  '' expected readings for a particular hardware setup.

  repeat while cogdata[COG_TIMESTAMP] == lastReadTimestamp
  lastReadTimestamp := cogdata[COG_TIMESTAMP]
  result := cogdata[COG_OUTPUT]

PUB getOutputAddress : ptr
  '' Return the address of a long at which the latest reading
  '' will always be available. This is useful for using the sensor
  '' output asynchronously from another assembly-language cog.

  ptr := @cogdata[COG_OUTPUT]
  
DAT

'==============================================================================
' Driver Cog
'==============================================================================

                        org

                        '======================================================
                        ' Initialization
                        '======================================================

entry                   mov     t1, par
                        add     t1, #4*COG_CTRA
                        rdlong  ctra, t1                ' Program CTRA

                        mov     t1, par
                        add     t1, #4*COG_FREQMASK
                        rdlong  freqMask, t1                        

                        mov     t1, par
                        add     t1, #4*COG_LEDMASK
                        rdlong  ledMask, t1                        

                        mov     dira, ledMask
                        mov     frqa, pwm_freq_value
                        
                        '======================================================
                        ' Period Sampler
                        '======================================================

beginSampler
                        mov     p1, #0
                        mov     p2, #0
                        mov     numSamples, #0
                        mov     totalCycles, #0
                        
samplerLoop
                        ' Timestamp the rising and falling edges.
                        ' We use the low half of the waveform to measure
                        ' light with the LED fully on, and the high half
                        ' to take a measurement with the LED at 50% power.  

                        mov     prevRisingEdge, risingEdge

                        mov     t1, #0                  ' Wait for falling edge
                        waitpeq t1, freqMask
                        mov     fallingEdge, cnt

                        mov     outa, ledMask           ' Bright LED (On)
                        
                        mov     t1, #0                  ' Wait for rising edge
                        waitpne t1, freqMask
                        mov     risingEdge, cnt

                        mov     phsa, #0                ' Synchronize the PWM with the sensor frequency, to reduce jitter
                        mov     outa, #0                ' Dim LED (PWM)

                        ' Add this pair of pulses to the p1/p2 accumulators.

                        mov     t1, risingEdge          ' Bright: p1 += risingEdge - fallingEdge
                        sub     t1, fallingEdge
                        add     p1, t1
                        add     totalCycles, t1
                        
                        mov     t1, fallingEdge         ' Dim: p2 += fallingEdge - prevRisingEdge
                        sub     t1, prevRisingEdge
                        add     p2, t1
                        add     totalCycles, t1
                        
                        add     numSamples, #1
                        
                        ' Have we been sampling long enough to take a single reading?

                        mov     t1, par                 ' Load minCycles
                        add     t1, #4*COG_MINCYCLES
                        rdlong  t2, t1
                        cmp     totalCycles, t2 wc      ' Compare against totalCycles
              if_b      jmp     #samplerLoop


                        '======================================================
                        ' Calculations
                        '======================================================

calculations
                        ' With this sensor, light is proportional to frequency.
                        ' We have two period lengths now, but we want to calculate
                        ' the absolute difference in light irradiance between
                        ' these two measurements. We calculate the difference in
                        ' frequency:
                        '
                        '   f1 - f2 = (1 / p1) - (1 / p2) = (p2 - p1) / (p1 * p2)
                        '
                        ' Our p1 and p2 are actually multiplied by some integral
                        ' number of samples, so we need to include a divide by
                        ' numSamples. We also want to scale the result up by
                        ' outputScale. The new equation is:
                        '                        
                        '   ((numSamples / p1) - (numSamples / p2)) * outputScale
                        '   (numSamples * p2 - numSamples * p1) / (p1 * p2) * outputScale
                        '   numSamples * outputScale * (p2 - p1) / (p1 * p2)
                        '
                        ' Useful values of outputScale will be very large. Let's
                        ' hardcode it as 2^32. In this case, the calculation turns into
                        ' a 64-bit division, where the upper word of the dividend is
                        ' (numSamples * (p2 - p1)) and the divisor is (p1 * p2).

                        ' t2 = p2 - p1

                        mov     t2, p2
                        sub     t2, p1

                        ' Prepare the numerator. We can assume numSamples fits in
                        ' 16 bits, so this is a 16x32 multiply:
                        '   divA_high = numSamples * t2
                        '   divA_low = 0

                        mov     divA_low, #0
                        mov     divA_high, #0           ' Init result to zero
                        mov     t1, #16                 ' Loop over 16 bits
mul1_loop               shr     numSamples, #1 wc
              if_c      add     divA_high, t2
                        shl     t2, #1
                        djnz    t1, #mul1_loop     
                        
                        ' Prepare the denominator. p1*p2 could overflow 32 bits,
                        ' so we need to do a full 32x32 multiply and keep the high
                        ' and low parts of the result.

                        mov     divB_high, #0
                        mov     t1, #32
                        shr     p2, #1 wc
mul2_loop     if_c      add     divB_high, p1 wc
                        rcr     divB_high, #1 wc
                        rcr     p2, #1 wc
                        djnz    t1, #mul2_loop
                        mov     divB_low, p2

                        ' Now, a 64-bit by 64-bit divide with 32-bit result:
                        '
                        '   output = divA_high:divA_low / divB_high:divB_low
                        
                        mov     output, #0
                        mov     divT_low, #0
                        mov     divT_high, #0
                        mov     t1, #64                        ' Loop over 64 bits
                        
div_loop                shl     divA_low, #1 wc                ' Rotate a bit from divA into divT
                        rcl     divA_high, #1 wc
                        rcl     divT_low, #1 wc
                        rcl     divT_high, #1 wc

                        shl     output, #1

                        cmp     divT_high, divB_high wc,wz     ' 64-bit comparison                 
              if_z      cmp     divT_low, divB_low wc
              if_c      jmp     #:next_bit                     ' Skip if divB > divA

                        add     output, #1                     ' Increment output if divB fits in divA
                        sub     divT_low, divB_low wc          ' 64-bit subtract
                        subx    divT_high, divB_high

:next_bit               djnz    t1, #div_loop

                        '======================================================
                        ' Write results
                        '======================================================

                        mov     t1, par
                        add     t1, #4*COG_OUTPUT
                        wrlong  output, t1  

                        mov     t1, par
                        add     t1, #4*COG_TIMESTAMP
                        wrlong  risingEdge, t1  

                        jmp     #beginSampler


'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

pwm_freq_value          long    PWM_FREQ
                        
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

t1                      res     1
t2                      res     1

freqMask                res     1
ledMask                 res     1

fallingEdge             res     1
risingEdge              res     1
prevRisingEdge          res     1
output                  res     1

p1                      res     1
p2                      res     1
numSamples              res     1
totalCycles             res     1

divA_high               res     1
divA_low                res     1
divB_high               res     1
divB_low                res     1
divT_high               res     1
divT_low                res     1

                        fit

{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}   