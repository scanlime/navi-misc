{

rfid-lf - Minimalist software-only reader for low-frequency RFID tags
─────────────────────────────────────────────────────────────────────

I. Supported Tags

  Tested with the EM4102 compatible RFID tags sold by Parallax, and with
  HID proximity cards typically issued for door security.

I. Theory of operation

  The Propeller itself generates the 125 kHz carrier wave, which excites
  an LC tank. The inductor in this tank is an antenna coil, which becomes
  electromagnetically coupled to the RFID tag when it comes into range.
  This carrier powers the RFID tag. The RFID sends back data by modulating
  the amplitude of this carrier. We detect this modulation via a peak
  detector and low-pass filter (D1, C2, R1). The detected signal is AC
  coupled via C3, and an adjustable bias is applied via R3 from a PWM signal
  which is filtered by R4 and C4. This signal is fed directly to an input
  pin via R2 (which limits high-frequency noise) where the pin itself acts
  as an analog comparator.

  The waveform which exits the peak detector is a sequence of narrow spikes
  at each oscillation of the carrier wave. Each of these spikes occurs when
  C2 charges quickly via D1, then discharges slowly via R1. We can use this
  to fashion a simple A/D converter by having the Propeller measure the width
  of the spike. The higher the carrier amplitude, the more charge was stored
  in C2. In order to keep the spikes from over-saturating in either direction,
  the "threshold" bias is adjusted in software in order to keep the
  comparator's threshold at a usable portion of the spike waveform.

  Now the Propeller has an analog sample corresponding to each carrier
  oscillation's amplitude. This sample is used directly in order to optimize
  the carrier frequency automatically.
  
II. Schematic

                (P5) IN ─┐    (P1) C+ ──┐    
                          │               │    
                       R2                 L1 
               R4         │  C3       D1  │    
   (P3) THR ────┳────┻────┳──┳────┫    
                   │  R3        │  │      │    
                C4        R1  C2  C1 
                                       │    
                                          │    
                               (P0) C- ──┘    

  C1,C3  1 nF
  C2     2.2 nF
  C4     1.5 nF
  R1     1 MΩ
  R2,R4  100 kΩ
  R3     1.8 MΩ
  D1     Some garden variety sigal diode from my junk drawer.
  L1     About 30 turns of magnet wire on a 5.5x7 inch wooden block.
         (Tune for 125 kHz resonance with C1.)
 
  Optional parts:

   - An amplifier for the carrier wave. Ideally it would be a very high
     slew rate op-amp or buffer which could convert the 3.3v signal from
     the Prop into a higher-voltage square wave for exciting the LC tank.
  
     I've tried a MAX233A, but it was unsuccessful due to the low slew rate
     which caused excessive harmonic distortion.

   - An external comparator for the input value. This makes the circuit
     easier to debug, but I'm not sure whether it actually improves
     performance yet.
        
III. License
       
 ┌───────────────────────────────────┐
 │ Copyright (c) 2008 Micah Dowty    │               
 │ See end of file for terms of use. │
 └───────────────────────────────────┘

}

CON
  CARRIER_HZ         = 125_000      ' Initial carrier frequency
  DEFAULT_THRESHOLD  = $80000000    ' Initial comparator frequency
  THRESHOLD_GAIN     = 7            ' Log2 gain for threshold control loop
  CARRIER_RATE       = 6            ' Inverse Log2 rate for carrier frequency control loop
  CARRIER_GAIN       = 300          ' Gain for carrier frequency control loop  

  INPUT_PIN          = 5            ' Input signal
  SHIELD_PIN         = 4            ' Optional, driven to ground to shield INPUT.
  THRESHOLD_PIN      = 3            ' PWM output for detection threshold
  DEBUG_PIN          = 2            ' Optional, for oscilloscope debugging
  CARRIER_POS_PIN    = 1            ' Carrier wave pin
  CARRIER_NEG_PIN    = 0            ' Carrier wave pin
  
VAR
  long cog
  long em_buffer[2]
  
PUB start | period, okay

  ' Fundamental timing parameters: Default carrier wave
  ' drive frequency, and the period of the carrier in clock
  ' cycles.

  init_frqa := fraction(CARRIER_HZ, clkfreq)
  period := clkfreq / CARRIER_HZ

  ' Derived timing parameters: Timeouts and thresholds that
  ' are based on the carrier frequency but not directly tied to it.

  timeout := period * 4           ' How long to wait for a pulse?
  pulse_target := period / 3      ' What is our 'center' pulse width?
  next_hyst := pulse_target
  hyst_constant := period / 100   ' Amount of pulse hysteresis

  ' Output buffers
  em_buf_ptr := @em_buffer
  
  okay := cog := cognew(@entry, 0) + 1

  em_buffer[0]~~
  em_buffer[1]~~
  
PUB stop
  if cog
    cogstop(cog~ - 1)
  
PUB read(addr) | a, b
  repeat
    a := em_buffer[0]
    b := em_buffer[1]
    if a <> $FFFFFFFF
      em_buffer[0]~~
      em_buffer[1]~~
      LONG[addr] := a
      LONG[addr+4] := b
      return
  
PRI fraction(a, b) : f
  a <<= 1
  repeat 32
    f <<= 1
    if a => b
      a -= b
      f++
    a <<= 1

DAT
'==============================================================================
' Driver Cog
'==============================================================================

                        '======================================================
                        ' Initialization
                        '======================================================

                        org

entry                   mov     dira, init_dira
                        mov     ctra, init_ctra         ' CTRA generates the carrier wave
                        mov     frqa, init_frqa
                        mov     ctrb, init_ctrb         ' CTRB generates a pulse threshold bias
                        mov     frqb, init_frqb


                        '======================================================
                        ' Main pulse A/D loop
                        '======================================================

mainLoop
                        add     pulse_count, #1         ' Global pulse counter, used below

                        '
                        ' Measure a low pulse, with timeout.
                        '
                        ' The length of this pulse is proportional to the amplitude of the carrier,
                        ' but we need to apply some automatic gain control by adjusting the threshold
                        ' value in FRQB.
                        '

                        mov     r0, timeout             ' Wait for low, with timeout 
:waitLow                test    input_mask, ina wz
              if_nz     djnz    r0, #:waitLow
                        mov     low_cnt, cnt

                        mov     r0, timeout             ' Wait for high, with timeout
:waitHigh               test    input_mask, ina wz
              if_z      djnz    r0, #:waitHigh
                        mov     high_cnt, cnt

                        mov     pulse_len, high_cnt
                        sub     pulse_len, low_cnt

                        '
                        ' Adjust the comparator threshold in order to achieve our pulse_target,
                        ' using a linear proportional control loop.
                        '

                        mov     r0, pulse_target
                        sub     r0, pulse_len
                        shl     r0, #THRESHOLD_GAIN
                        add     frqb, r0

                        '
                        ' We also want to dynamically tweak the carrier frequency, in order
                        ' to hit the resonance of our LC tank as closely as possible. The
                        ' value of frqb is actually a filtered representation of our overall
                        ' carrier amplitude, so we want to adjust frqa in order to maximize frqb.
                        '
                        ' Since we can't adjust frqa drastically while the RFID reader is
                        ' operating, we'll make one small adjustment at a time, and decide
                        ' whether or not it was an improvement. This process eventually converges
                        ' on the correct resonant frequency, so it should be enough to keep our
                        ' circuit tuned as the analog components fluctuate slightly in value
                        ' due to temperature variations.
                        '
                        ' This algorithm is divided into four phases, sequenced using two
                        ' bits from the pulse_count counter:
                        '
                        '   0. Store a reference frqb value, and increase frqa
                        '   1. Test the frqb value. If it wasn't an improvement, decrease frqa
                        '   2. Store a reference frqb value, and decrease frqa
                        '   3. Test the frqb value. If it wasn't an improvement, increase frqa
                        '

                        test    pulse_count, #(|< CARRIER_RATE) - 1 wz
        if_nz           jmp     #:skip_frqa
                        test    pulse_count, #|<(CARRIER_RATE + 0) wz
                        test    pulse_count, #|<(CARRIER_RATE + 1) wc
                        negc    r0, #CARRIER_GAIN
        if_nz           mov     prev_frqb, frqb
        if_nz           add     frqa, r0
        if_z            cmp     prev_frqb, frqb wc
        if_z_and_nc     sub     frqa, r0        
:skip_frqa

                        '
                        ' That takes care of all our automatic calibration tasks.. now to
                        ' receive some actual bits. Since our pulse length is proportional
                        ' to the amount of carrier attenuation, our demodulated bits (or
                        ' baseband FSK signal) are determined by the amount of pulse width
                        ' excursion from our center position.
                        '
                        ' We don't need to measure the center, since we're actively balancing
                        ' our pulses around pulse_target. A simple bit detector would just
                        ' compare pulse_len to pulse_target. We go one step further, and
                        ' include a little hysteresis.
                        '

                        cmp     next_hyst, pulse_len wc    
                        muxc    outa, #|<DEBUG_PIN         ' Output demodulated bit to debug pin

                        mov     next_hyst, pulse_target    ' Update hysteresis for the next bit
                        sumc    next_hyst, hyst_constant

              if_nc     add     baseband_sum, #1
                        rcl     baseband_reg, #1 wc        ' Store in our baseband shift register
              if_nc     sub     baseband_sum, #1           '   ... and keep a running total of the bits.              

                        '
                        ' Our work here is done. Give each card-specific protocol decoder
                        ' a chance to find actual ones and zeroes.
                        '
                        
                        call    #rx_hid
                        call    #rx_em4102

                        jmp     #mainLoop


                        '======================================================
                        ' EM4102 Decoder 
                        '======================================================

                        ' The EM4102 chip actually supports multiple clock rates
                        ' and multiple encoding schemes: Manchester, Biphase, and PSK.
                        ' Their "standard" scheme, and the one Parallax uses, is
                        ' Manchester encoding at 32 clocks per code (64 clocks per bit).
                        ' Our support for this format is hardcoded.
                        '
                        ' The EM4102's data packet consists of 40 payload bits (an 8-bit
                        ' manufacturer ID and 32-bit serial number), 9 header bits, 1 stop
                        ' bit, and 14 parity bits. This is a total of 64 bits. These bits
                        ' are manchester encoded into 128 baseband bits. 
                        '
                        ' We could decode this the traditional way- do clock/data recovery
                        ' on the Manchester signal using a DPLL, look for the header, do
                        ' manchester decoding on the rest of the packet, etc. But this is
                        ' software, and we can throw memory and CPU at the problem in order
                        ' to get a more noise-resistant decoding.
                        '
                        ' A packet in its entirety is 4096 clocks. This is 128 manchester
                        ' codes by 32 clocks per code. We can treat this as 32 possible phases
                        ' and 128 possible code alignments. In fact, it's a more convenient
                        ' to treat it as 64 possible phases and 128 possible bits. We get the
                        ' same result, and it's less data to move around.
                        '
                        ' Every time a code arrives, we shift it into a 128-bit shift register.
                        ' We have 32 total shift registers, which we cycle through. Every time
                        ' we shift in a new bit, we examine the entire shift register and test
                        ' whether it's a valid packet.
                        
rx_em4102
                        ' XXX
                        'test    baseband_reg, #1 wc
                        
                        cmp     baseband_sum, #16 wc    ' Low pass filter: Look at average of last 32 bits

                        or      dira, #|<7              ' XXX
                        muxc    outa, #|<7

:shift1                 rcl     em_bits+1, #1 wc        ' Shift in the new filtered bit
:shift2                 rcl     em_bits+0, #1
:load1                  mov     em_shift+0, em_bits+0   ' And save a copy in a static location
:load2                  mov     em_shift+1, em_bits+1

                        add     :shift1, dest_2         ' Increment em_bits pointers
                        add     :shift2, dest_2
                        add     :load1, #2
                        add     :load2, #2
                        cmp     :shift1, em_shift1_end wz
              if_z      sub     :shift1, dest_128       ' Wrap around     
              if_z      sub     :shift2, dest_128
              if_z      sub     :load1, #128     
              if_z      sub     :load2, #128     

                        '
                        ' At this point, the encoded packet should have the following format:
                        ' (Even bits in the manchester code are not shown.)
                        '
                        '   bits+0:  11111111_1ddddPdd_ddPddddP_ddddPddd                        
                        '   bits+1:  dPddddPd_dddPdddd_PddddPdd_ddPPPPP0
                        '
                        ' Where 'd' is a data bit and 'P' is a parity bit.
                        '

                        mov     r0, em_bits+0           ' Look for the header of nine "1" bits
                        shr     r0, #32-9
                        cmp     r0, #$1FF wz
              if_nz     jmp     #rx_em4102_ret

                        rcr     em_bits+1, #1 nr,wc     ' Look for a footer of one "0"
              if_c      jmp     #rx_em4102_ret

                        ' Looking good so far. Now loop over the 10 data rows...

                        mov     em_decoded, #0
                        mov     em_decoded+1, #0
                        mov     em_parity, #0
                        mov     r0, #10
:row
                        mov     r2, em_bits+0           ' Extract the next row's 5 bits
                        shr     r2, #18
                        and     r2, #%11111 wc          ' Check row parity
              if_c      jmp     #rx_em4102_ret

                        mov     r1, em_decoded+1        ' 64-bit left shift by 4
                        shl     em_decoded+1, #4
                        shl     em_decoded+0, #4
                        shr     r1, #32-4
                        or      em_decoded+0, r1 
                        
                        shr     r2, #1                  ' Drop row parity bit
                        xor     em_parity, r2           ' Update column parity
                        or      em_decoded+1, r2        ' Store 4 decoded bits

                        mov     r1, em_bits+1           ' 64-bit left shift by 5
                        shl     em_bits+1, #5
                        shl     em_bits+0, #5
                        shr     r1, #32-5
                        or      em_bits+0, r1 

                        djnz    r0, #:row

                        mov     r2, em_bits+0           ' Extract the next 4 bits
                        shr     r2, #19
                        and     r2, #%1111
                        xor     em_parity, r2 wc        ' Test column parity
              if_c      jmp     #rx_em4102_ret

                        mov     r0, em_buf_ptr
                        wrlong  em_decoded+0, em_buf_ptr
                        add     r0, #4
                        wrlong  em_decoded+1, r0

rx_em4102_ret           ret

em_shift1_end           rcl     em_bits+1+128, #1 wc    ' This is ":shift1" above, after we've
                                                        ' passed the end of the em_bits array.


                        '======================================================
                        ' HID Decoder
                        '======================================================

                        ' This is a data decoder for HID's 125 kHz access control cards.
                        '
                        ' These cards use a FSK scheme. Zeroes and ones appear to nominally
                        ' last 8 and 10 cycles, respectively. The code seems to consist of
                        ' a header (16 "1" bits) followed by a payload (512 data bits).
                        '
                        ' I don't have any documentation from HID, this is all written
                        ' based on a fairly cursory examination of the signal from a HID
                        ' card that I happened to have.

rx_hid                  add     hid_bit_len, #1         ' Count samples within one bit

                        mov     r0, baseband_reg
                        and     r0, #%11
                        cmp     r0, #%01 wz             ' Look for rising edges
              if_nz     jmp     #rx_hid_ret             ' Continue only if we found one

                        cmp     hid_bit_len, #9 wc      ' Compare against the bit threshold              
                        mov     hid_bit_len, #0         ' Reset bit counter

                        rcl     hid_reg, #1             ' Store this bit, LSB first.

rx_hid_ret              ret

                        
 
'------------------------------------------------------------------------------
' Initialized Data
'------------------------------------------------------------------------------

hFFFFFFFF     long      $FFFFFFFF
dest_2        long      2 << 9
dest_128      long      128 << 9

init_dira     long      (|< CARRIER_POS_PIN) | (|< CARRIER_NEG_PIN) | (|< THRESHOLD_PIN) | (|< DEBUG_PIN)
init_frqa     long      0
init_frqb     long      DEFAULT_THRESHOLD
init_ctra     long      (%00101 << 26) | (CARRIER_POS_PIN << 9) | CARRIER_NEG_PIN 
init_ctrb     long      (%00110 << 26) | THRESHOLD_PIN
input_mask    long      |< INPUT_PIN

timeout       long      0
pulse_target  long      0
next_hyst     long      0
hyst_constant long      0

baseband_reg  long      0
baseband_sum  long      32                      ' Number of zeroes in baseband_reg

em_buf_ptr    long      0


'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

r0            res       1
r1            res       1
r2            res       1

high_cnt      res       1
low_cnt       res       1
pulse_len     res       1
pulse_count   res       1
prev_frqb     res       1

hid_bit_len   res       1
hid_reg       res       1
                                                                         
em_bits       res       128     ' 64-bit shift register for each of the 64 phases
em_shift      res       2       ' Just the current shift register
em_decoded    res       2       ' Decoded 40-bit EM4102 packet       
em_parity     res       1       ' Column parity accumulator

                        fit