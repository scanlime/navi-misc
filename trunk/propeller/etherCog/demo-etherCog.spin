''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  BUFFER_SIZE  = 16
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock1   : "etherCog-udp-socket"
  sock2   : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

PUB main | i, j

  debug.start(12)

  netDrv.start(3, 2, 1, 0)                   
  bufq.initFromList(netDrv.getRecycledBuffers(BUFFER_SIZE))

  netDrv.link(sock1.init(128))
  netDrv.link(sock2.init(256))

  sock1.rxPutN(bufq.getN(4))
  sock2.rxPutN(bufq.getN(4))

  repeat
    if i := sock1.rxGet
      sock1.rxPut(i)

    if i := sock2.rxGet
      sock2.rxPut(i)

    debug.out(1)
    debug.hex(LONG[sock1.ptr], 8)
    debug.out(13)
    debug.hex(LONG[sock1.ptr + 4], 8)
    debug.out(13)
    debug.hex(LONG[sock1.ptr + 8], 8)
    debug.out(13)
    debug.hex(j := LONG[sock1.ptr + 12], 8)
    debug.out(" ")
    debug.hex(LONG[j], 8)
    debug.hex(LONG[j+4], 8)
    debug.out(13)
    debug.hex(LONG[sock1.ptr + 16], 8)
    debug.out(13)
    debug.out(13)
    debug.hex(LONG[sock2.ptr], 8)
    debug.out(13)
    debug.hex(LONG[sock2.ptr + 4], 8)
    debug.out(13)
    debug.hex(LONG[sock2.ptr + 8], 8)
    debug.out(13)
    debug.hex(j := LONG[sock2.ptr + 12], 8)
    debug.out(" ")
    debug.hex(LONG[j], 8)
    debug.hex(LONG[j+4], 8)
    debug.out(13)
    debug.hex(LONG[sock2.ptr + 16], 8)
