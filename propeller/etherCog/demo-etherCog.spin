''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  net   : "etherCog"
  debug : "TV_Text"

VAR
  long  foo
  
PUB main

  debug.start(12)
  debug.str(string("etherCog - Fast network driver", 13))

  net.start(3, 2, 1, 0, 5, @foo)

  repeat
    debug.hex(foo, 8)
    debug.out(13)