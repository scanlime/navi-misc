{

rfid-lf - Minimalist software-only reader for low-frequency RFID tags
─────────────────────────────────────────────────────────────────────

I. Supported Tags

  Tested with the EM4012 compatible RFID tags sold by Parallax, and with
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
  THRESHOLD_GAIN     = 3            ' Log2 gain for threshold control loop
  CARRIER_RATE       = 6            ' Inverse Log2 rate for carrier frequency control loop
  CARRIER_GAIN       = 300          ' Gain for carrier frequency control loop  

  EM4012_RATE        = 32           ' Parallax RFID tags use 32 clocks per bit
  
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
     
PUB stop
  if cog
    cogstop(cog~ - 1)
  
PUB read(addr)
  longmove(addr, @em_buffer, 2)
  
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
                        rcl     baseband_reg, #1           ' Store in our baseband shift register
                        mov     next_hyst, pulse_target    ' Update hysteresis for the next bit
              if_nc     add     next_hyst, hyst_constant
              if_c      sub     next_hyst, hyst_constant

                        '
                        ' Our work here is done. Give each card-specific protocol decoder
                        ' a chance to find actual ones and zeroes.
                        '
                        
                        call    #rx_hid
                        call    #rx_em4012

                        jmp     #mainLoop


                        '======================================================
                        ' EM4012 Decoder 
                        '======================================================

                        ' The EM4012 chip actually supports multiple clock rates
                        ' and multiple encoding schemes: Manchester, Biphase, and PSK.
                        ' We only support one hardcoded clock frequency, and Manchester
                        ' encoding. These settings are compatible with the RFID tags
                        ' and reader sold by Parallax.

rx_em4012
                        add     em_clock, #1            ' Increment clock recovery counter
                        and     em_clock, #EM4012_RATE-1
                        
                        mov     r0, baseband_reg        ' Detect transitions in the baseband
                        shr     r0, #1
                        xor     r0, baseband_reg
                        and     r0, #1 wz
              if_nz     mov     em_clock, #0            ' Reset the clock on transitions

                        cmp     em_clock, #EM4012_RATE >> 1 wz
              if_nz     jmp     #rx_em4012_ret          ' Sample bits in the middle of em_clock

                        test    baseband_reg, #1 wc     ' Store manchester encoded bits
                        rcl     em_manchester, #1

                        ' Detect the 9-bit packet header. If we find it,
                        ' clear the manchester buffer so we don't re-trigger
                        ' if another sequence of 9 ones overlaps this one.
                        
                        and     em_manchester, em_header_m
                        cmp     em_manchester, em_header wz
              if_z      jmp     #:new_packet

                        ' Is a valid packet in progress?
                        cmp     em_bit_count, #(64-9)*2 wc,wz
              if_a      jmp     #rx_em4012_ret
                        add     em_bit_count, #1           

                        ' Ignore the first bit of each manchester pair
                        test    em_bit_count, #1 wz
              if_z      jmp     #rx_em4012_ret
                        
                        or      dira, #|<7              ' XXX Debug
                        xor     outa, #|<7

                        test    em_manchester, #1 wc    ' Receive, LSB first
                        rcr     em_data_high, #1 wc 
                        rcr     em_data_low, #1

                        cmp     em_bit_count, #17  wz ' (64-9)*2 wz
                        mov     r0, em_buf_ptr
              if_z      wrlong  em_data_high, r0
                        add     r0, #4
              if_z      wrlong  em_data_low, r0
              
                        jmp     #rx_em4012_ret
                        
:error                  neg     em_bit_count, #1        ' Stop receiving this packet
                        jmp     #rx_em4012_ret

:new_packet             mov     em_manchester, #0       ' Clear header match
                        mov     em_bit_count, #0        ' Empty the packet buffer
                        mov     em_data_low, #0
                        mov     em_data_high, #0
rx_em4012_ret           ret
                        

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

' Header for EM4012 packets: nine manchester encoded "1" bits.
em_header     long      %01_01_01_01_01_01_01_01_01
em_header_m   long      %11_11_11_11_11_11_11_11_11

em_buf_ptr    long      0


'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

r0            res       1

high_cnt      res       1
low_cnt       res       1
pulse_len     res       1
pulse_count   res       1
prev_frqb     res       1

hid_bit_len   res       1
hid_reg       res       1

em_clock      res       1
em_manchester res       1

em_data_low   res       1       ' EM4012 only holds 40 data bits, so we just use
em_data_high  res       1       '   a big shift register to hold them all.
em_bit_count  res       1

                        fit
