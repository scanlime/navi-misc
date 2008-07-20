{{

NESPort
-------

This object polls an NES controller port, with support for normal
gamepads as well as the Zapper light gun. The results are stored
in a shared memory area which can be polled by other cogs, or even
by a PC via BluetoothConduit.

   - Game pad data (8 bits) is polled periodically

   - Every time the light gun trigger is pulled (falling edge),
     we increment a trigger counter.

   - Measure current, min, and max periods from the light sensor.

   - Every time the light sensor crosses a configurable brightness
     threshold, we latch the current laser scan position and increment
     a pulse counter.

References:
   http://ca.geocities.com/jefftranter@rogers.com/xgs/zapper.html
   http://www.zero-soft.com/HW/USB_ZAPPER/
   http://yikescode.blogspot.com/2007/12/nes-controller-arduino.html

Pin layout:   
                  ┌────┐
   (BLK) 1. Data  │•  •│ 4. Trigger (YEL)
   (ORG) 2. Latch │•  •│ 5. Light Sensor (GRN)
   (RED) 3. Clock │•  •│    +5V (WHT)
   (BLU)    Gnd   │• ┌─┘
                  └──┘

  All pins must be connected consecutively, starting at a specified
  base pin, in the order defined above. Data pins must have a 10K
  pull-up resistor to 5V.

Zapper mod:

  I originally wrote this module for use with an unmodified Zapper
  controller. It was possible to trigger the Zapper using the laser
  projector, but sensitivity left much to be desired. The light sensor
  in the Zapper insn't particularly sensitive, but more importantly it
  has a band-pass filter on it which makes it even harder to trigger
  using laser light that's pulsed at frequencies we can produce.

  The mod works by installing a TSL230R light-to-frequency converter
  in front of the original light sensor board. The only modifications
  necessary to the original circuit board are to cut one trace and
  add three wires.

  First, prep the TSL230R with a series resistor (to limit output
  slew rate and current) and a decoupling capacitor. I did mine
  "dead bug" style, with the capacitor and resistor soldered
  directly onto the back of the chip after folding the leads in.
  
         +5v
           ┬
           │
           ┣──┳──────────────┐                                                    
           │  │ TSL230R      │               
           │  │ ┌──────────┐ │               
    0.1 µF ┣─┤ S0    S3 ├─┼─┐                      
           │  └─┤ S1    S2 ├─┫ │  100 Ω                    
           │  ┌─┤ /OE  OUT ├─┼─┼────── OUT                  
           │  ┣─┤ GND  Vdd ├─┘ │                     
           │  │ └──────────┘   │                         
           ┣──┻────────────────┘           
           

  Solder three wires onto the TSL230R assembly, for GND, +5v, and
  OUT. I used 30 gauge wire-wrap wire.

  Hot glue the TSL230R just behind the rearmost circular partition
  in the Zapper barrel. The chip in the TSL230R should be centered
  in the hole in this partition.

  Now attach it to the original Zapper circuit board. This is a
  bottom view of the board, with a corresponding portion of the
  schematic:

   ┌────────── ─ ─       +5v
   │        BC            ┬             
   │  •     •••           │          
   │ A•                    100 Ω       ┌─ A 
   │  •   •••  •D      D ┫          D ┫ 
   │  •    ••            10 µF        
   │                   B ┫             │
   └────────── ─ ─                     

  This shows the original Zapper's power supply filter, and its
  open-collector drive transistor for the original light detector
  output. To attach the new TSL230R assembly:

    - Cut the trace between point A and C
    - Connect the TSL230 ground to point D
    - Connect TSL230 power to point D (utilizing the original power filter)
    - Connect the TSL230 output (with 100 Ω resistor) to point A

  That's it! Test the sensor with an oscilloscope, then reassemble. The
  modified Zapper should not damage an NES, but it won't work correctly
  of course. If you want to undo the mod, just unglue (pull off) the TSL230R
  itself, unsolder its wires from the Zapper board, and run a jumper wire
  between points D and A.

┌───────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty    │               
│ See end of file for terms of use. │
└───────────────────────────────────┘

}}

CON

  ' Relative pin numbers  
  DATA_PIN    = 0
  LATCH_PIN   = 1
  CLOCK_PIN   = 2
  TRIGGER_PIN = 3
  LIGHT_PIN   = 4

  ' Button bit numbers
  BUTTON_A      = 0
  BUTTON_B      = 1
  BUTTON_SELECT = 2
  BUTTON_START  = 3
  BUTTON_UP     = 4
  BUTTON_DOWN   = 5
  BUTTON_LEFT   = 6
  BUTTON_RIGHT  = 7
  
  ' Public cog data
  PARAM_BUTTONS      = 0   'OUT,    Game pad buttons
  PARAM_TRIGGER_CNT  = 1   'OUT,    Counts trigger presses
  PARAM_LIGHT        = 2   'OUT,    Current light sensor period, in clock ticks
  PARAM_LIGHT_MIN    = 3   'IN/OUT, Minimum detected light sensor period
  PARAM_LIGHT_MAX    = 4   'IN/OUT, Maximum detected light sensor period
  PARAM_LIGHT_THRESH = 5   'IN,     Light sensor pulse threshold
  PARAM_LIGHT_CNT    = 6   'OUT,    Light sensor pulse counter
  PARAM_LIGHT_X      = 7   'OUT,    Last light sensor X position
  PARAM_LIGHT_Y      = 8   'OUT,    Last light sensor Y position
  NUM_PARAMS         = 9

  ' Private cog data
  COG_PIN            = 9   'CONST, base pin number
  COG_POS_X          = 10  'CONST, X position address
  COG_POS_Y          = 11  'CONST, Y position address
  COG_CLOCK_CNT      = 12  'CONST, cnt ticks per NES clock (6us)
  COG_FRAME_CNT      = 13  'CONST, cnt ticks per button poll (60 Hz)
  COG_DATA_SIZE      = 14  '(Must be last)

VAR
  long cog
  long cogdata[COG_DATA_SIZE]
  
PUB start(basePin, xAddr, yAddr) : okay
  '' Initialize the controller port, and start its driver cog.
  '' The controller must be connected to five pins, starting with 'basePin'.
  '' The supplied X and Y addresses will be sampled when the light sensor is triggered.

  ' Initialize cog parameters
  longfill(@cogdata, 0, COG_DATA_SIZE)
  cogdata[COG_PIN] := basePin
  cogdata[COG_POS_X] := xAddr
  cogdata[COG_POS_Y] := yAddr
  cogdata[COG_CLOCK_CNT] := clkfreq * 6 / 1000000
  cogdata[COG_FRAME_CNT] := clkfreq / 60

  okay := (cog := cognew(@entry, @cogdata)) + 1
     
PUB stop
  if cog > 0
    cogstop(cog)
  cog := -1

PUB getParams : addr
  '' Get the address of our parameter block (public PARAM_* values)

  addr := @cogdata
  
DAT

'==============================================================================
' Driver Cog
'==============================================================================

                        org

                        '======================================================
                        ' Initialization
                        '======================================================

entry                   mov     t1, par                 ' Read COG_PIN
                        add     t1, #4*COG_PIN
                        rdlong  t2, t1

                        mov     t1, t2                  ' Calculate pin masks
                        add     t1, #CLOCK_PIN
                        shl     clock_mask, t1

                        mov     t1, t2
                        add     t1, #LATCH_PIN
                        shl     latch_mask, t1

                        mov     t1, t2
                        add     t1, #DATA_PIN
                        shl     data_mask, t1

                        mov     t1, t2
                        add     t1, #TRIGGER_PIN
                        shl     trigger_mask, t1

                        mov     t1, t2
                        add     t1, #LIGHT_PIN
                        shl     light_mask, t1

                        mov     t1, par                 ' Read X/Y position addresses
                        add     t1, #4*COG_POS_X
                        rdlong  addr_pos_x, t1

                        mov     t1, par
                        add     t1, #4*COG_POS_Y
                        rdlong  addr_pos_y, t1

                        mov     addr_buttons, par       ' Calculate parameter addresses
                        add     addr_buttons, #4*PARAM_BUTTONS

                        mov     addr_trigger_cnt, par
                        add     addr_trigger_cnt, #4*PARAM_TRIGGER_CNT

                        mov     addr_light, par
                        add     addr_light, #4*PARAM_LIGHT

                        mov     addr_light_cnt, par
                        add     addr_light_cnt, #4*PARAM_LIGHT_CNT

                        mov     addr_light_x, par
                        add     addr_light_x, #4*PARAM_LIGHT_X

                        mov     addr_light_y, par
                        add     addr_light_y, #4*PARAM_LIGHT_Y

                        mov     t1, par                 ' Load other const data
                        add     t1, #4*COG_CLOCK_CNT
                        rdlong  clock_cnt, t1

                        mov     t1, par
                        add     t1, #4*COG_FRAME_CNT
                        rdlong  frame_cnt, t1

                        mov     outa, #0                ' All pins are open-collector
                        mov     dira, #0

                        or      dira, latch_mask        ' Clock and latch are low by default
                        or      dira, clock_mask

                        mov     button_deadline, cnt    ' Set up the button polling counter
                        add     button_deadline, frame_cnt

                        mov     last_light_cnt, cnt

                        
                        '======================================================
                        ' Button polling
                        '======================================================

                        ' In the outer polling loop, we scan the controller
                        ' buttons at 60 Hz. Any time this poll loop needs to wait,
                        ' we jump to the lightgun poll loop which runs until it
                        ' hits the button_deadline.
                        '
                        ' We also do slow polling here, for the trigger button.
                        ' The trigger button is very noisy, so we don't consider
                        ' it pressed until we see a "100" pattern when polling it.
                        
buttonLoop
                        ' Poll and debounce the trigger

                        test    trigger_mask, ina wc
                        rcl     trigger_debounce, #1
                        and     trigger_debounce, #%111
                        cmp     trigger_debounce, #%100 wz
              if_z      call    #trigger_edge                                                                        
                        
                        ' Poll the button shift register

                        xor     dira, latch_mask        ' 12us latch pulse
                        call    #clock_delay
                        call    #clock_delay
                        or      dira, latch_mask

                        mov     buttons, #0             ' Sample 8 bits, LSB first
                        mov     bit_count, #8

:bit
                        call    #clock_delay
                        test    data_mask, ina wc       ' Sample before the rising edge
                        rcr     buttons, #1 

                        xor     dira, clock_mask        ' Rising edge
                        call    #clock_delay
                        or      dira, clock_mask        ' Falling edge

                        djnz    bit_count, #:bit        ' Next bit..
                        shr     buttons, #24            ' Justify buttons

                        wrlong  buttons, addr_buttons   ' Save buttons

                        add     button_deadline, frame_cnt
                        call    #wait_for_deadline      ' Wait for the next frame...

                        jmp     #buttonLoop             ' Next button loop

clock_delay             add     button_deadline, clock_cnt
                        call    #wait_for_deadline      ' Utility for a 6us clock delay
clock_delay_ret         ret

                        '======================================================
                        ' Light gun polling
                        '======================================================

                        ' While we're waiting for button polling deadlines, poll
                        ' for light sensor pulses.
                        
wait_for_deadline       mov     in_prev, in_latch       ' Compare current and previous inputs
                        mov     in_latch, ina
                        test    light_mask, in_latch wz ' Rising edge on the light sensor?
              if_nz     testn   light_mask, in_prev wz
              if_nz     call    #light_edge                                                                        

                        mov     t1, cnt                 ' Are we done yet?
                        sub     t1, button_deadline
                        rcl     t1, #1 wc               ' (cnt - button_deadline) >= 0
              if_nc     jmp     #wait_for_deadline                        
wait_for_deadline_ret   ret

                        '======================================================
                        ' Light gun edge handlers
                        '======================================================

                        ' When we detect a light gun pulse, measure the duration
                        ' since the last pulse and write out our updated period info.
                        ' If the light is brighter than our threshold, count this
                        ' pulse and latch the laser position.

light_edge
                        ' Measure the light sensor period
                        mov     t1, cnt
                        mov     light_period, t1
                        sub     light_period, last_light_cnt
                        mov     last_light_cnt, t1

                        ' Write output
                        wrlong  light_period, addr_light

                        ' Update min
                        mov     t1, par
                        add     t1, #4*PARAM_LIGHT_MIN
                        rdlong  t2, t1
                        max     t2, light_period
                        wrlong  t2, t1  

                        ' Update max
                        mov     t1, par
                        add     t1, #4*PARAM_LIGHT_MAX
                        rdlong  t2, t1
                        min     t2, light_period
                        wrlong  t2, t1  

                        ' Read the threshold. If the light is too dim (light_period too high), skip the rest.            
                        mov     t1, par
                        add     t1, #4*PARAM_LIGHT_THRESH
                        rdlong  t2, t1
                        sub     t2, light_period wc     '
              if_c      jmp     #light_edge_ret

                        ' Increment the counter       
                        add     light_cnt, #1
                        wrlong  light_cnt, addr_light_cnt

                        ' Latch laser position
                        rdlong  light_x, addr_pos_x
                        rdlong  light_y, addr_pos_y
                        wrlong  light_x, addr_light_x
                        wrlong  light_y, addr_light_y
light_edge_ret          ret

                        ' Falling edge on the trigger?
                        testn   trigger_mask, in_latch wz
              if_nz     test    trigger_mask, in_prev wz
              if_nz     call    #trigger_edge                                                                        

                        ' When we detect a trigger pulse, count it.
                        
trigger_edge            add     trigger_cnt, #1
                        wrlong  trigger_cnt, addr_trigger_cnt
trigger_edge_ret        ret


'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

hFFFFFFFF               long    $FFFFFFFF

clock_mask              long    1
latch_mask              long    1
data_mask               long    1
trigger_mask            long    1
light_mask              long    1

buttons                 long    0
trigger_cnt             long    0
light_cnt               long    0
light_x                 long    0
light_y                 long    0
in_latch                long    0
trigger_debounce        long    0
                   
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

t1                      res     1
t2                      res     1

bit_count               res     1
button_deadline         res     1
in_prev                 res     1

last_light_cnt          res     1
light_period            res     1

addr_buttons            res     1
addr_trigger_cnt        res     1
addr_light              res     1
addr_light_cnt          res     1
addr_light_x            res     1
addr_light_y            res     1
addr_pos_x              res     1
addr_pos_y              res     1

clock_cnt               res     1
frame_cnt               res     1

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