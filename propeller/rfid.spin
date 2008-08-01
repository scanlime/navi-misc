''
'' Minimalist proof-of-concept RFID Reader
''
'' XXX: Doesn't work yet, currently this is just a toy to generate a carrier wave
''      and perhaps sample some bits.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  CARRIER_HZ = 125_000

  CARRIER_POS_PIN  = 0  ' Carrier wave pin, to coil
  CARRIER_NEG_PIN  = 7  ' Carrier wave pin, to capacitor
  THRESHOLD_PIN    = 1  ' XXX
  INPUT_PIN        = 4  ' Conditioned input signal
  DEBUG_PIN        = 2
  
OBJ
  text : "TV_Text"

VAR
  long debug
  
PUB main
  text.start(12)
  text.str(string("Minimalist RFID reader", 13, "Micah Dowty <micah@navi.cx>", 13))
  start

  repeat
    repeat 50
      text.hex(debug, 3)
      text.out(" ")
    waitcnt(cnt + clkfreq/2)
  
PUB start
  carrier_cnt := clkfreq / constant(CARRIER_HZ * 2)
  cognew(@entry, @debug)

DAT
              org

entry         mov       dira, init_dira
              mov       outa, init_outa
              mov       ctra, init_ctra         ' CTRA generates the comparator threshold voltage
              mov       frqa, init_frqa
              mov       ctrb, init_ctrb         ' CTRB counts rising edges on the input pin
              mov       frqb, #1 

              mov       timer, cnt
              add       timer, carrier_cnt

carrierLoop   waitcnt   timer, carrier_cnt      ' LOW half-cycle
              xor       outa, carrier_mask
              waitcnt   timer, carrier_cnt      ' HIGH half-cycle
              xor        outa, carrier_mask

              mov       t1, phsb                ' Measure delta PHSB
              mov       delta_phsb, t1
              sub       delta_phsb, last_phsb
              mov       last_phsb, t1

              test      delta_phsb, delta_phsb wz
        if_nz add       frqa, pos_gain          ' Software peak detector      
        if_z  sub       frqa, neg_gain
              muxnz     outa, debug_mask
              add       fm_cnt, #1              ' Measure the current FM frequency
        if_nz mov       fm_cnt, #0
        if_z  wrlong    fm_cnt, par
   
              jmp       #carrierLoop

init_dira     long      (|< CARRIER_POS_PIN) | (|< CARRIER_NEG_PIN) | (|< THRESHOLD_PIN) | (|< DEBUG_PIN)
init_outa     long      |< CARRIER_NEG_PIN
init_frqa     long      $20000000
init_ctra     long      (%00110 << 26) | THRESHOLD_PIN
init_ctrb     long      (%01010 << 26) | INPUT_PIN

pos_gain      long      $800
neg_gain      long      $800

debug_mask    long      |< DEBUG_PIN

fm_cnt        long      0
carrier_cnt   long      0
carrier_mask  long      (|< CARRIER_POS_PIN) | (|< CARRIER_NEG_PIN)

t1            res       1
delta_phsb    res       1
last_phsb     res       1
timer         res       1

              fit        