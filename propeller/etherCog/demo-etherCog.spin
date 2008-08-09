''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

OBJ
  netDrv  : "etherCog-enc28j60"
  sock1   : "etherCog-udp-socket"
  sock2   : "etherCog-udp-socket"
  debug   : "TV_Text"

VAR
  long  x
  
PUB main | i

  debug.start(12)
  debug.out(0)

  netDrv.start(3, 2, 1, 0)

  sock1.init(0)
  sock2.init(0)

  sock2.link(sock1.ptr)  
  netDrv.link(sock2.ptr)

  debug.hex(LONG[sock1.ptr], 8)
  debug.out(13)
  debug.hex(LONG[sock2.ptr], 8)
  repeat 