{

rfid-lf - Minimalist software-only reader for low-frequency RFID tags
─────────────────────────────────────────────────────────────────────

I. Supported Tags

  Tested with the EM4095-compatible RFID tags sold by Parallax, and with
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
  
  INPUT_PIN        = 5  ' Input signal
  SHIELD_PIN       = 4  ' Shield for INPUT, driven to ground.
  THRESHOLD_PIN    = 3  ' PWM output for detection threshold
  DEBUG_PIN        = 2  ' For the 'scope
  CARRIER_POS_PIN  = 1  ' Carrier wave pin
  CARRIER_NEG_PIN  = 0  ' Carrier wave pin
  
VAR
  long cog
  long buffer[128]
  
PUB start | period, okay

  ' Fundamental timing parameters: Default carrier wave
  ' drive frequency, and the period of the carrier in clock
  ' cycles.

  init_frqa := fraction(CARRIER_HZ, clkfreq)
  period := clkfreq / CARRIER_HZ

  ' Derived timing parameters: Timeouts and thresholds that
  ' are based on the carrier frequency but not directly tied to it.

  timeout := period * 4        ' How long to wait for a pulse?
  pulse_target := period / 3   ' What is our 'center' pulse width?

  okay := cog := cognew(@entry, @buffer) + 1
     
PUB stop
  if cog
    cogstop(cog~ - 1)
  
PUB read(addr)
  longmove(addr, @buffer, 8)
  
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

                        mov     r0, par
                        wrlong  frqa, r0
                        add     r0, #4
                        wrlong  frqb, r0
                        add     r0, #4
                        wrlong  pulse_count, r0
                        

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

                        jmp     #mainLoop

                        
 
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


'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

r0            res       1

high_cnt      res       1
low_cnt       res       1
pulse_len     res       1
pulse_count   res       1
prev_frqb     res       1

                        fit


{

packet_loop             mov     buf_offset, #0
                        mov     num_ones, #0
word_loop               mov     bit_count, #32

                        ' Measure the time between falling edges, ignoring short gaps.         
bit_loop

:edge_wait              mov     prev_edge, this_edge
                        waitpeq input_mask, input_mask  ' Wait for another pulse edge...
                        or      outa, #|<DEBUG_PIN      ' XXX: Debug high
                        waitpne input_mask, input_mask
                        mov     this_edge, cnt
                        andn    outa, #|<DEBUG_PIN      ' XXX: Debug low

                        mov     r0, this_edge           ' How long was that?
                        sub     r0, prev_edge                        
                        cmp     r0, debounce wc
              if_c      jmp     #:edge_wait             ' Too short. Look for another edge.
                        
                        ' Now the difference between first_edge and this_edge
                        ' is the period of the FSK modulated signal that the RFID
                        ' tag is generating. Compare the signal's period against our
                        ' bit threshold, to recover bits.

                        mov     r0, this_edge
                        sub     r0, first_edge
                        cmp     r0, bit_threshold wc
              if_c      add     num_ones, #1
              if_nc     mov     num_ones, #0
                        rcl     shift_reg, #1

                        ' If we've received a long run of ones, sync to the beginning of the packet.
                        cmp     num_ones, #16 wz
              if_z      jmp     #packet_loop

                        mov     first_edge, this_edge   ' Start a new bit

                        djnz    bit_count, #bit_loop

                        ' If we finished a word, write it to hub memory.

                        add     buf_offset, #4
                        and     buf_offset, #$7F
                        mov     r0, par
                        add     r0, buf_offset
                        wrlong  shift_reg, r0

                        jmp     #word_loop


init_frqb     long      DEFAULT_THRESHOLD     ' Default threshold
adj_gain      long      $0                    ' Automatic threshold adjustment gain

first_edge    long      0
prev_edge     long      0
this_edge     long      0

buf_offset    long      0
bit_count     long      0
num_ones      long      0
shift_reg     long      0

r0            res       1
}

