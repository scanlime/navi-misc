''
'' Simple test program for the etherCog object.
''
'' Micah Dowty <micah@navi.cx>
''

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  NUM_BUFFERS  = 10
  BUFFER_SIZE  = 8
  
OBJ
  netDrv  : "etherCog-enc28j60"
  sock1   : "etherCog-udp-socket"
  sock2   : "etherCog-udp-socket"
  bufq    : "etherCog-buffer-queue"
  
  debug   : "TV_Text"

VAR
  long  bufData[NUM_BUFFERS * BUFFER_SIZE / 4]
  long  bufBD[NUM_BUFFERS * bufq#BD_SIZE]
  
PUB main | i, j

  debug.start(12)

  bufq.initFromMem(NUM_BUFFERS, BUFFER_SIZE, @bufData, @bufBD)
  
  netDrv.start(3, 2, 1, 0)

  netDrv.link(sock1.init(128))
  netDrv.link(sock2.init(256))

  sock1.recv(bufq.get)
  sock2.recv(bufq.getN(8))

  repeat
    debug.out(1)

    debug.str(string("Socket 1: "))
  
    i := sock1.ptr
    repeat 5
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "Socket 2: "))
  
    i := sock2.ptr
    repeat 5
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "BD:", 13))

    i := @bufBD
    repeat NUM_BUFFERS
      debug.hex(LONG[i], 8)
      i += 4
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "Buf: ", 13))

    i := @bufData
    repeat 16
      debug.hex(BYTE[i++], 2)
      debug.out(" ")

    waitcnt(cnt + clkfreq/4) 