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

  net.start(3, 2, 1, 0, 5, @cmd)

  repeat
    'debug.out(0)
    
    repeat i from 1 to $20
      cmd := i
      repeat while cmd
      'debug.hex(r, 2)
      debug.out("-")

      'cmd := net#MACLCON1
      'repeat while cmd
      'debug.hex(r, 2)
      'debug.out(" ")

    'waitcnt(cnt + clkfreq/100)