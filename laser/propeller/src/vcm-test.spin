
OBJ
  vcm : "VoiceCoil"

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
                    
PUB main | i
'  vcm.start(6,8)  'X
  vcm.start(2,0)  'Y

  repeat
      vcm.pulse(100)
      vcm.pulse(-100)
      waitcnt(cnt + clkfreq/100)

  