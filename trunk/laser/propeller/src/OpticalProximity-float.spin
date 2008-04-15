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

     
┌───────────────────────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty                    │               
│                                                   │
│ Includes portions of the Float32 library by       │
│ Cam Thompson, Micromega Corporation.              │
│                                                   │
│ Float32 is Copyright (c) 2006-2007 Parallax, Inc. │
│                                                   │
│ See end of file for terms of use.                 │
└───────────────────────────────────────────────────┘

}}

CON

  ' Floating point value to scale converted readings by before
  ' storing the result as an integer.

  DEFAULT_OUTPUT_SCALE = 1e12

  ' Default maximum update rate, in Hz

  DEFAULT_UPDATE_RATE = 1000
  
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
  
VAR
  long samplerCog, mathCog
  long lastReadTimestamp

  ' Data area shared by assembly-language driver
  
  long timestamp      ' [ 0] OUT,   cnt value for latest sample
  long output         ' [ 4] OUT,   integer sample
  long outputScale    ' [ 8] IN,    floating point scale value for future samples
  long minCycles      ' [12] IN,    minimum # of clock cycles per reading
  long _p1            ' [16] TEMP,  Integer value of period 1
  long _p2            ' [20] TEMP,  Integer value of period 2
  long _numSamples    ' [24] TEMP,  Integer sample count
  long _timestamp     ' [28] TEMP,  Hand-off timestamp from sampler cog to math cog
  long ctraValue      ' [32] CONST, initialization value for CTRA
  long outPinMask     ' [36] CONST, pin mask for TSL230R output
  long ledPinMask     ' [40] CONST, pin mask for LED
  
PUB start(freqPin, ledPin) : okay
  '' Initialize the sensor, start its driver cogs, and wait for it
  '' to take a single sample. This uses two cogs: one to sample the
  '' frequency input, and one to do all the floating-point math.
  ''
  '' freqPin must be the pin connected to the TSL230R's output, and
  '' ledPin must be connected to the reference LED.

  ' Initialize cog parameters
  timestamp~
  ctraValue := PWM_MODE + ledPin
  outPinMask := |< freqPin
  ledPinMask := |< ledPin
  setOutputScale(DEFAULT_OUTPUT_SCALE)
  setUpdateRate(DEFAULT_UPDATE_RATE)

  samplerCog := cognew(@samplerEntry, @timestamp) 
  mathCog := cognew(@mathEntry, @timestamp) 

  if samplerCog < 0 or mathCog < 0
    stop
    okay := 0
  else
    ' Wait for the first sample
    repeat until timestamp
    okay := 1
     
PUB stop
  '' Immediately turn off the LED and stop the driver cogs.

  if samplerCog >= 0
    cogstop(samplerCog)
    samplerCog := -1

  if mathCog
    cogstop(mathCog)
    mathCog := -1

PUB setUpdateRate(hz)
  '' Set the maximum update rate, in Hz.
  '' Lower update rates provide better accuracy, but of course they're slower.

  minCycles := clkfreq / hz

PUB setOutputScale(s)
  '' Set the floating point value used to convert a frequency
  '' difference, in Hz, to the integer output value returned
  '' by the read function.

  outputScale := s

PUB read : result | stamp
  '' Take a ratiometric proximity reading. If no new data is
  '' available since the last call, this will block until a new
  '' reading is available.
  ''
  '' The result varies linearly with the amount of light reaching
  '' the sensor from the LED, after subtracting all ambient light.
  ''
  '' The result's units aren't particularly meaningful. It is a
  '' difference of two frequencies, in Hz, scaled by the value set
  '' using setOutputScale(). The relationship between frequency
  '' and light level is linear, but it will depend on the individual
  '' TSL230R device in use.

  repeat while timestamp == lastReadTimestamp
  lastReadTimestamp := timestamp
  result := output

PUB getOutputAddress : ptr
  ptr := @output

  
DAT

'==============================================================================
' Sampler Cog
'==============================================================================

                        org

samplerEntry            mov     scT1, par
                        add     scT1, #32
                        rdlong  ctra, scT1                ' Program CTRA
                        add     scT1, #4
                        rdlong  outMask, scT1                        
                        add     scT1, #4
                        rdlong  ledMask, scT1

                        mov     dira, ledMask
                        mov     frqa, pwm_freq_value

resetSampler
                        mov     p1, #0
                        mov     p2, #0
                        mov     numSamples, #0
                        
samplerLoop
                        ' Timestamp the rising and falling edges.
                        ' We use the low half of the waveform to measure
                        ' light with the LED fully on, and the high half
                        ' to take a measurement with the LED at 50% power.  

                        mov     prevRisingEdge, risingEdge

                        mov     scT1, #0                  ' Wait for falling edge
                        waitpeq scT1, outMask
                        mov     fallingEdge, cnt

                        mov     outa, ledMask           ' Bright LED (On)
                        
                        mov     scT1, #0                  ' Wait for rising edge
                        waitpne scT1, outMask
                        mov     risingEdge, cnt

                        mov     phsa, #0                ' Synchronize the PWM with the sensor frequency, to reduce periodic noise
                        mov     outa, #0                ' Dim LED (PWM)

                        ' Add this pair of pulses to the p1/p2 accumulators.

                        mov     scT1, risingEdge          ' Bright: p1 += risingEdge - fallingEdge
                        sub     scT1, fallingEdge
                        add     p1, scT1
                        
                        mov     scT1, fallingEdge         ' Dim: p2 += fallingEdge - prevRisingEdge
                        sub     scT1, prevRisingEdge
                        add     p2, scT1
                        
                        add     numSamples, #1
                        
                        ' Have we been sampling long enough to take a single reading?

                        mov     scT1, par                 ' Load minCycles
                        add     scT1, #12
                        rdlong  scT2, scT1

                        mov     scT1, p1                  ' Compare against p1+p2
                        add     scT1, p2

                        cmp     scT1, scT2 wc
              if_b      jmp     #samplerLoop                                                

                        ' Yes, we've completed one reading. Write it back to hub memory.

                        mov     scT1, par
                        add     scT1, #16
                        wrlong  p1, scT1                  ' Period 1
                        add     scT1, #4
                        wrlong  p2, scT1                  ' Period 2
                        add     scT1, #4
                        wrlong  numSamples, scT1          ' Number of samples
                        add     scT1, #4
                        wrlong  risingEdge, scT1          ' Timestamp
                        
                        jmp     #resetSampler

'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

pwm_freq_value          long    PWM_FREQ
                        
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

scT1                    res     1
scT2                    res     1

outMask                 res     1
ledMask                 res     1

fallingEdge             res     1
risingEdge              res     1
prevRisingEdge          res     1

p1                      res     1
p2                      res     1
numSamples              res     1

                        fit

'==============================================================================
' Math Cog
'==============================================================================

                        org

mathEntry                        
mathLoop

                        mov     t1, par                 ' Wait for a new timestamp
                        add     t1, #28
tsWait                  rdlong  mcTimestamp, t1 wz            
              if_z      jmp     #tsWait
                        mov     t2, #0
                        wrlong  t2, t1

                        mov     t1, par                 ' Read the rest of the inputs from the sampler cog
                        add     t1, #16
                        rdlong  mcP1, t1
                        add     t1, #4
                        rdlong  mcP2, t1
                        add     t1, #4
                        rdlong  mcNumSamples, t1

                        ' Convert p1 and p2 to floating point, and divide by numSamples

                        mov     fnumA, mcNumSamples
                        call    #_FFloat
                        mov     fnumB, fnumA
                        
                        mov     fnumA, mcP1
                        call    #_FFloat       
                        call    #_FDiv
                        mov     mcP1, fnumA

                        mov     fnumA, mcP2
                        call    #_FFloat       
                        call    #_FDiv
                        mov     mcP2, fnumA

                        ' With this sensor, light is proportional to frequency.
                        ' We have two period lengths now, but we want to calculate
                        ' the absolute difference in light irradiance between
                        ' these two measurements. We calculate the difference in
                        ' frequency:
                        '
                        '  f1 - f2 = (1 / p1) - (1 / p2) = (p2 - p1) / p1 / p2
                        '
                        ' Doing this calculation using integer math is rather
                        ' unreliable, since the magnitudes of the numbers can
                        ' vary so much. We use the Float32 library functions to
                        ' perform the calculation using floating point intermediate
                        ' values, then we multiply by a user-supplied scale factor
                        ' and convert the result back to integer. 

                        mov     fnumA, mcP2             ' p2 - p1
                        mov     fnumB, mcP1
                        call    #_FSub

                        mov     fnumB, mcP1             ' / p1
                        call    #_FDiv
                        mov     fnumB, mcP2             ' / p2
                        call    #_FDiv

                        mov     t1, par                 ' Multiply by outputScale
                        add     t1, #8
                        rdlong  fnumB, t1
                        call    #_FMul

                        call    #_FRound                ' Round and store the result
                        mov     t1, par
                        add     t1, #4
                        wrlong  fnumA, t1
                                                
                        mov     t1, par                 ' Store timestamp
                        wrlong  mcTimestamp, t1

                        jmp     #mathLoop

'------------------------------------------------------------------------------
' _FAdd    fnumA = fnumA + fNumB
' _FAddI   fnumA = fnumA + {Float immediate}
' _FSub    fnumA = fnumA - fNumB
' _FSubI   fnumA = fnumA - {Float immediate}
' changes: fnumA, flagA, expA, manA, fnumB, flagB, expB, manB, t1
'------------------------------------------------------------------------------

_FSubI                  movs    :getB, _FSubI_ret       ' get immediate value
                        add     _FSubI_ret, #1
:getB                   mov     fnumB, 0

_FSub                   xor     fnumB, Bit31            ' negate B
                        jmp     #_FAdd                  ' add values                                               

_FAddI                  movs    :getB, _FAddI_ret       ' get immediate value
                        add     _FAddI_ret, #1
:getB                   mov     fnumB, 0

_FAdd                   call    #_Unpack2               ' unpack two variables                    
          if_c_or_z     jmp     #_FAdd_ret              ' check for NaN or B = 0

                        test    flagA, #SignFlag wz     ' negate A mantissa if negative
          if_nz         neg     manA, manA
                        test    flagB, #SignFlag wz     ' negate B mantissa if negative
          if_nz         neg     manB, manB

                        mov     t1, expA                ' align mantissas
                        sub     t1, expB
                        abs     t1, t1
                        max     t1, #31
                        cmps    expA, expB wz,wc
          if_nz_and_nc  sar     manB, t1
          if_nz_and_c   sar     manA, t1
          if_nz_and_c   mov     expA, expB        

                        add     manA, manB              ' add the two mantissas
                        cmps    manA, #0 wc, nr         ' set sign of result
          if_c          or      flagA, #SignFlag
          if_nc         andn    flagA, #SignFlag
                        abs     manA, manA              ' pack result and exit
                        call    #_Pack  
_FSubI_ret
_FSub_ret 
_FAddI_ret
_FAdd_ret               ret      

'------------------------------------------------------------------------------
' _FMul    fnumA = fnumA * fNumB
' changes: fnumA, flagA, expA, manA, fnumB, flagB, expB, manB, t1, t2
'------------------------------------------------------------------------------

_FMul                   call    #_Unpack2               ' unpack two variables
          if_c          jmp     #_FMul_ret              ' check for NaN

                        xor     flagA, flagB            ' get sign of result
                        add     expA, expB              ' add exponents
                        mov     t1, #0                  ' t2 = upper 32 bits of manB
                        mov     t2, #32                 ' loop counter for multiply
                        shr     manB, #1 wc             ' get initial multiplier bit 
                                    
:multiply if_c          add     t1, manA wc             ' 32x32 bit multiply
                        rcr     t1, #1 wc
                        rcr     manB, #1 wc
                        djnz    t2, #:multiply

                        shl     t1, #3                  ' justify result and exit
                        mov     manA, t1                        
                        call    #_Pack 
_FMul_ret               ret

'------------------------------------------------------------------------------
' _FDiv    fnumA = fnumA / fNumB
' changes: fnumA, flagA, expA, manA, fnumB, flagB, expB, manB, t1, t2
'------------------------------------------------------------------------------

_FDiv                   call    #_Unpack2               ' unpack two variables
          if_c_or_z     mov     fnumA, NaN              ' check for NaN or divide by 0
          if_c_or_z     jmp     #_FDiv_ret
        
                        xor     flagA, flagB            ' get sign of result
                        sub     expA, expB              ' subtract exponents
                        mov     t1, #0                  ' clear quotient
                        mov     t2, #30                 ' loop counter for divide

:divide                 shl     t1, #1                  ' divide the mantissas
                        cmps    manA, manB wz,wc
          if_z_or_nc    sub     manA, manB
          if_z_or_nc    add     t1, #1
                        shl     manA, #1
                        djnz    t2, #:divide

                        mov     manA, t1                ' get result and exit
                        call    #_Pack                        
_FDiv_ret               ret

'------------------------------------------------------------------------------
' _FFloat  fnumA = float(fnumA)
' changes: fnumA, flagA, expA, manA
'------------------------------------------------------------------------------
         
_FFloat                 mov     flagA, fnumA            ' get integer value
                        mov     fnumA, #0               ' set initial result to zero
                        abs     manA, flagA wz          ' get absolute value of integer
          if_z          jmp     #_FFloat_ret            ' if zero, exit
                        shr     flagA, #31              ' set sign flag
                        mov     expA, #31               ' set initial value for exponent
:normalize              shl     manA, #1 wc             ' normalize the mantissa 
          if_nc         sub     expA, #1                ' adjust exponent
          if_nc         jmp     #:normalize
                        rcr     manA, #1                ' justify mantissa
                        shr     manA, #2
                        call    #_Pack                  ' pack and exit
_FFloat_ret             ret

'------------------------------------------------------------------------------
' _FTrunc  fnumA = fix(fnumA)
' _FRound  fnumA = fix(round(fnumA))
' changes: fnumA, flagA, expA, manA, t1 
'------------------------------------------------------------------------------

_FTrunc                 mov     t1, #0                  ' set for no rounding
                        jmp     #fix

_FRound                 mov     t1, #1                  ' set for rounding

fix                     call    #_Unpack                ' unpack floating point value
          if_c          jmp     #_FRound_ret            ' check for NaN
                        shl     manA, #2                ' left justify mantissa 
                        mov     fnumA, #0               ' initialize result to zero
                        neg     expA, expA              ' adjust for exponent value
                        add     expA, #30 wz
                        cmps    expA, #32 wc
          if_nc_or_z    jmp     #_FRound_ret
                        shr     manA, expA
                                                       
                        add     manA, t1                ' round up 1/2 lsb   
                        shr     manA, #1
                        
                        test    flagA, #signFlag wz     ' check sign and exit
                        sumnz   fnumA, manA
_FTrunc_ret
_FRound_ret             ret

'------------------------------------------------------------------------------
' input:   fnumA        32-bit floating point value
'          fnumB        32-bit floating point value 
' output:  flagA        fnumA flag bits (Nan, Infinity, Zero, Sign)
'          expA         fnumA exponent (no bias)
'          manA         fnumA mantissa (aligned to bit 29)
'          flagB        fnumB flag bits (Nan, Infinity, Zero, Sign)
'          expB         fnumB exponent (no bias)
'          manB         fnumB mantissa (aligned to bit 29)
'          C flag       set if fnumA or fnumB is NaN
'          Z flag       set if fnumB is zero
' changes: fnumA, flagA, expA, manA, fnumB, flagB, expB, manB, t1
'------------------------------------------------------------------------------

_Unpack2                mov     t1, fnumA               ' save A
                        mov     fnumA, fnumB            ' unpack B to A
                        call    #_Unpack
          if_c          jmp     #_Unpack2_ret           ' check for NaN

                        mov     fnumB, fnumA            ' save B variables
                        mov     flagB, flagA
                        mov     expB, expA
                        mov     manB, manA

                        mov     fnumA, t1               ' unpack A
                        call    #_Unpack
                        cmp     manB, #0 wz             ' set Z flag                      
_Unpack2_ret            ret

'------------------------------------------------------------------------------
' input:   fnumA        32-bit floating point value 
' output:  flagA        fnumA flag bits (Nan, Infinity, Zero, Sign)
'          expA         fnumA exponent (no bias)
'          manA         fnumA mantissa (aligned to bit 29)
'          C flag       set if fnumA is NaN
'          Z flag       set if fnumA is zero
' changes: fnumA, flagA, expA, manA
'------------------------------------------------------------------------------

_Unpack                 mov     flagA, fnumA            ' get sign
                        shr     flagA, #31
                        mov     manA, fnumA             ' get mantissa
                        and     manA, Mask23
                        mov     expA, fnumA             ' get exponent
                        shl     expA, #1
                        shr     expA, #24 wz
          if_z          jmp     #:zeroSubnormal         ' check for zero or subnormal
                        cmp     expA, #255 wz           ' check if finite
          if_nz         jmp     #:finite
                        mov     fnumA, NaN              ' no, then return NaN
                        mov     flagA, #NaNFlag
                        jmp     #:exit2        

:zeroSubnormal          or      manA, expA wz,nr        ' check for zero
          if_nz         jmp     #:subnorm
                        or      flagA, #ZeroFlag        ' yes, then set zero flag
                        neg     expA, #150              ' set exponent and exit
                        jmp     #:exit2
                                 
:subnorm                shl     manA, #7                ' fix justification for subnormals  
:subnorm2               test    manA, Bit29 wz
          if_nz         jmp     #:exit1
                        shl     manA, #1
                        sub     expA, #1
                        jmp     #:subnorm2

:finite                 shl     manA, #6                ' justify mantissa to bit 29
                        or      manA, Bit29             ' add leading one bit
                        
:exit1                  sub     expA, #127              ' remove bias from exponent
:exit2                  test    flagA, #NaNFlag wc      ' set C flag
                        cmp     manA, #0 wz             ' set Z flag
_Unpack_ret             ret       

'------------------------------------------------------------------------------
' input:   flagA        fnumA flag bits (Nan, Infinity, Zero, Sign)
'          expA         fnumA exponent (no bias)
'          manA         fnumA mantissa (aligned to bit 29)
' output:  fnumA        32-bit floating point value
' changes: fnumA, flagA, expA, manA 
'------------------------------------------------------------------------------

_Pack                   cmp     manA, #0 wz             ' check for zero                                        
          if_z          mov     expA, #0
          if_z          jmp     #:exit1

:normalize              shl     manA, #1 wc             ' normalize the mantissa 
          if_nc         sub     expA, #1                ' adjust exponent
          if_nc         jmp     #:normalize
                      
                        add     expA, #2                ' adjust exponent
                        add     manA, #$100 wc          ' round up by 1/2 lsb
          if_c          add     expA, #1

                        add     expA, #127              ' add bias to exponent
                        mins    expA, Minus23
                        maxs    expA, #255
 
                        cmps    expA, #1 wc             ' check for subnormals
          if_nc         jmp     #:exit1

:subnormal              or      manA, #1                ' adjust mantissa
                        ror     manA, #1

                        neg     expA, expA
                        shr     manA, expA
                        mov     expA, #0                ' biased exponent = 0

:exit1                  mov     fnumA, manA             ' bits 22:0 mantissa
                        shr     fnumA, #9
                        movi    fnumA, expA             ' bits 23:30 exponent
                        shl     flagA, #31
                        or      fnumA, flagA            ' bit 31 sign            
_Pack_ret               ret


'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

NaN                     long    $7FFF_FFFF
Minus23                 long    -23
Mask23                  long    $007F_FFFF
Bit29                   long    $2000_0000
Bit31                   long    $8000_0000
                        
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

t1                      res     1
t2                      res     1

mcP1                    res     1
mcP2                    res     1
mcNumSamples            res     1
mcTimestamp             res     1

status                  res     1                       ' last compare status

fnumA                   res     1                       ' floating point A value
flagA                   res     1
expA                    res     1
manA                    res     1

fnumB                   res     1                       ' floating point B value
flagB                   res     1
expB                    res     1
manB                    res     1

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