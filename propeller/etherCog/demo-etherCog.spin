''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  BUFFER_SIZE = 1472
  NUM_BUFFERS = 8
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock1   : "etherCog-udp-socket"
  sock2   : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

VAR
  long  bufMem[BUFFER_SIZE * NUM_BUFFERS / 4]
  long  bufBD[bufq#BD_SIZE * NUM_BUFFERS]
  
PUB main | i, c

  debug.start(12)

  netDrv.start(3, 2, 1, 0)                   

  netDrv.link(sock1.init(128))
  netDrv.link(sock2.init(256))

' bufq.initFromList(netDrv.getRecycledBuffers(16))  
  bufq.initFromMem(NUM_BUFFERS, BUFFER_SIZE, @bufMem, @bufBD)

  c := $100

  i := bufq.get
  WORD[i+2] := BUFFER_SIZE
  LONG[LONG[i+4]] := c++
  sock1.txQueuePut(i)

  i := bufq.get
  WORD[i+2] := BUFFER_SIZE
  LONG[LONG[i+4]] := c++
  sock1.txQueuePut(i)

  sock2.rxRingInit(bufq.getN(1))
  sock1.rxQueueInit(bufq.getAll)

  repeat
    if i := sock1.rxQueueGet
      sock1.rxQueuePut(i)

    if i := sock1.txQueueGet
      LONG[LONG[i+4]] := c++
      sock1.txQueuePut(i)

    showState
  
PUB showState
  debug.out(1)
  debugSocket(sock1.ptr)
  debug.out(13)
  debugSocket(sock2.ptr)

PUB debugSocket(sockPtr) | bd, data

  debug.hex(LONG[sockPtr], 8)
  debug.out(13)
  repeat 2
    debug.hex(LONG[sockPtr += 4], 8)
    debug.out(13)
  repeat 2
    debug.hex(bd := LONG[sockPtr += 4], 8)
    debug.out(" ")
    debug.hex(LONG[bd], 8)
    debug.hex(data := LONG[bd+4], 8)
    debug.out(" ")
    data &= !3
    repeat 12
      debug.out(BYTE[data++] #> " ")
    debug.out(13)
  