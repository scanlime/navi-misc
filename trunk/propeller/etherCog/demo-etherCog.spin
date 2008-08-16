''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  BUFFER_SIZE  = 16

VAR
  long  gen
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock1   : "etherCog-udp-socket"
  sock2   : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

PUB main | i

  debug.start(12)

  netDrv.start(3, 2, 1, 0)                   
  bufq.initFromList(netDrv.getRecycledBuffers(BUFFER_SIZE))

  netDrv.link(sock1.init(128))
  netDrv.link(sock2.init(256))

  sock1.rxQueueInit(bufq.getN(5))
  sock2.rxQueueInit(bufq.getN(5))

  repeat
    if i := sock1.rxQueueGet
      sock1.rxQueuePut(i)

    if i := sock2.rxQueueGet
      sock2.rxQueuePut(i)

    showState
  
PUB showState | j
  debug.out(1)

  debug.hex(gen++, 8)
  debug.out(13)
  debug.out(13)

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

  debug.hex(WORD[sock1.ptr + 20], 4)
  debug.out("-")
  debug.hex(WORD[sock1.ptr + 22], 4)
  debug.out("-")
  debug.hex(WORD[sock1.ptr + 24], 4)
  debug.out(" ")
  debug.hex(WORD[sock1.ptr + 26], 4)
  debug.out("-")
  debug.hex(WORD[sock1.ptr + 28], 4)
  debug.out("-")
  debug.hex(WORD[sock1.ptr + 30], 4)
  debug.out(" ")
{
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
  }