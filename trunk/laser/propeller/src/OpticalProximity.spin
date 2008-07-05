{{

OpticalProximity
----------------

This is a simple proximity sensor based on a TSL230R light sensor
and two LEDs. One bright "reference" LED is fixed, and a dimmer
sensor LED is attached to a movable object. The reference LED
sets the minimum sampling frequency, and the moving sensor LED
affects our output reading.

This approach is very simplistic and includes no intrinsic immunity
to ambient light. I tried more complicated approaches, including
a pulsed reference LED with ratiometric sampling. In the end, these
approaches proved to be too slow and have too many numerical stability
problems. In the end, the simple approach wins.

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
      ledPin ──────┐   Sensor LED 
                         

               100 Ω      
         +5V ──────┐   Reference LED
                         


The TSL230R can be hard-wired for maximum (100x) sensitivity, and
the 2x frequency divisor. Low power supply noise is very important,
hence the RC filter. You should be sure to use separate ground return
wires for the TSL230R and the LEDs.
     
┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

CON
  MIN_DEFAULT = $FFFFFFFF
  MAX_DEFAULT = 0

  SCALE_BITS = 10

  ' cogdata offsets

  COG_OUTPUT      = 0    'OUT,   integer sample
  COG_OUT_MIN     = 1    'OUT,   minimum sample value so far
  COG_OUT_MAX     = 2    'OUT,   maximum sample value so far
  COG_FREQMASK    = 3    'CONST, pin mask for TSL230R frequency output
  COG_LEDMASK     = 4    'CONST, pin mask for LED
  COG_DATA_SIZE   = 5    '(Must be last)

VAR
  long cog
  long cogdata[COG_DATA_SIZE]
  
PUB start(freqPin, ledPin) : okay
  '' Initialize the sensor, start its driver cogs, and wait for it
  '' to take a single sample.
  ''
  '' freqPin must be the pin connected to the TSL230R's output, and
  '' ledPin must be connected to the sensor LED.

  ' Initialize cog parameters
  longfill(@cogdata, 0, COG_DATA_SIZE)
  cogdata[COG_FREQMASK] := |< freqPin
  cogdata[COG_LEDMASK] := |< ledPin
  resetMinMax

  cog := cognew(@entry, @cogdata) 

  if cog < 0
    stop
    okay := 0
  else
    ' Wait for the first sample
    repeat until cogdata[COG_OUTPUT]
    okay := 1
     
PUB stop
  '' Immediately turn off the LED and stop the driver cogs.

  if cog > 0
    cogstop(cog)
  cog := -1

PUB read : result | stamp
  '' Take a proximity reading. Never blocks.

  result := cogdata[COG_OUTPUT]

PUB resetMinMax
  '' Reset minimum/maximum counters

  cogdata[COG_OUT_MIN] := MIN_DEFAULT
  cogdata[COG_OUT_MAX] := MAX_DEFAULT
  
  
PUB readMin : result
  '' Return the minimum output value

  result := cogdata[COG_OUT_MIN]

PUB readMax : result
  '' Return the maximum output value

  result := cogdata[COG_OUT_MAX]

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
                        add     t1, #4*COG_FREQMASK
                        rdlong  freqMask, t1                        

                        mov     t1, par
                        add     t1, #4*COG_LEDMASK
                        rdlong  ledMask, t1                        

                        mov     dira, ledMask
                        mov     outa, ledMask           ' Bright LED (On)
                        
                        '======================================================
                        ' Period Sampler
                        '======================================================

samplerLoop
                        waitpeq zero, freqMask
                        mov     t1, cnt
                        waitpne zero, freqMask
                        waitpeq zero, freqMask
                        mov     output, cnt
                        sub     output, t1
                        shl     output, #SCALE_BITS

                        ' Write output
                        mov     t1, par
                        add     t1, #4*COG_OUTPUT
                        wrlong  output, t1

                        ' Update min
                        mov     t1, par
                        add     t1, #4*COG_OUT_MIN
                        rdlong  t2, t1
                        max     t2, output
                        wrlong  t2, t1  

                        ' Update max
                        mov     t1, par
                        add     t1, #4*COG_OUT_MAX
                        rdlong  t2, t1
                        min     t2, output
                        wrlong  t2, t1  

                        jmp     #samplerLoop
     

'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

zero                    long    0
                        
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

t1                      res     1
t2                      res     1

output                  res     1

freqMask                res     1
ledMask                 res     1

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