{{

High-speed PWM control for a voice coil motor, using the LMD18200T
H-bridge chip from National Semiconductor.

This does not use the cog's counter modules, since their PWM would be too
fast for the H-bridge. The H-bridge requires 1us between any changes to
its input pins. Instead, we use a simple assembly language routine.

Since our actual PWM rate is controlled by the motor's servo loop, this
module is really just responsible for generating a train of timed pulses.
Since the H-bridge's minimum pulse length is 1us, we just keep an accumulator
that tracks the number of 1us pulses that need to be generated (in either
direction). The driver cog is responsible for asynchronously generating
these pulses.

TODO: Add a schematic. For now, see the LMD18200T data sheet.


┌──────────────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty           │               
│     See end of file for terms of use.    │               
└──────────────────────────────────────────┘

}}

VAR
  ' Data area shared by assembly-language driver.
  ' These values are all read-only by the driver and read/write by Spin.
  ' The driver never clears pulseCnt, it just keeps track of how much
  ' it's changed.

  long pulseCnt
  long pulseLen
  long pwmMask
  long dirMask
  
  long cog

  
PUB start(dirPin, pwmPin) | okay
  '' Prepare the VoiceCoil object for use.
  '' This allocates a cog for our PWM driver.

  ' Initialize cog parameters
  pulseCnt~
  pulseLen := clkfreq / 1000000
  pwmMask := |< pwmPin
  dirMask := |< dirPin 

  okay := cog := cognew(@entry, @pulseCnt) + 1
     
PUB stop
  '' Immediately de-energize the motor and stop the driver cog.

  pulseCnt := 0

  if cog
    cogstop(cog~ - 1)

PUB pulse(value)
  '' Begin generating the specified number of minimum-length pulses.
  '' May be positive or negative- negative numbers pulse the VCM in reverse.

  pulseCnt += value
  

DAT
                        org

                        ' Save parameters

entry                   mov     t1, par
                        rdlong  drvPulseCnt, t1
                        add     t1, #4
                        rdlong  drvPulseLen, t1
                        add     t1, #4
                        rdlong  drvPwmMask, t1                        
                        add     t1, #4
                        rdlong  drvDirMask, t1

                        ' Initialize I/O

                        mov     outa, #0
                        mov     dira, drvDirMask
                        or      dira, drvPwmMask
          
mainLoop                mov     t1, par                ' Read the newPulseCnt         
                        rdlong  newPulseCnt, t1

                        mov     pulseDiff, newPulseCnt 
                        sub     pulseDiff, drvPulseCnt
                        mov     drvPulseCnt, newPulseCnt
                        
                        abs     t1, pulseDiff wz,wc
              if_z      jmp     #mainLoop
                        muxnc   outa, drvDirmask

                        mov     time, cnt
                        or      outa, drvPwmMask
pulseLoop               add     time, drvPulseLen
                        djnz    t1, #pulseLoop
                        waitcnt time, #0
                        xor     outa, drvPwmMask
                        
                        jmp     #mainLoop

'
' Uninitialized data
'

t1                      res     1
t2                      res     1

time                    res     1
pulseDiff               res     1
newPulseCnt             res     1
drvPulseCnt             res     1
drvPulseLen             res     1
drvPwmMask              res     1
drvDirMask              res     1

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