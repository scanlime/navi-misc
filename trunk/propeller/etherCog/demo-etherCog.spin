''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1' + pll16x
  _xinfreq = 5_000_000

OBJ
  net   : "etherCog"
  debug : "TV_Text"

VAR
  long  cmd, r
  
PUB main | i

  debug.start(12)
  debug.out(0)

  net.start(3, 2, 1, 0)

  debug.hex($beef, 8)

