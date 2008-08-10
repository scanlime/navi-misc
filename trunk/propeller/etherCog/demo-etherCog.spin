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
  long  bd1[2]
  long  bd2[2]
  byte  buf1[32]
  byte  buf2[32]
  
PUB main | i, j

  debug.start(12)

  netDrv.start(3, 2, 1, 0)

  netDrv.link(sock1.init(128))
  netDrv.link(sock2.init(256))

  sock1.recv($14b5, 1472, @bd1)     ' XXX: Receive into the text framebuffer
  sock2.recv(@buf2 | 1, 32, @bd2)

  repeat 'XXX

  repeat
    debug.out(1)

    debug.str(string("Socket 1: "))
  
    i := sock1.ptr
    repeat 5
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "BD 1: "))

    i := @bd1
    repeat 2
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "Buf 1: "))

    i := @buf1
    repeat 32
      debug.out(BYTE[i++] #> " ")

    debug.str(string(13, "Socket 2: "))
  
    i := sock2.ptr
    repeat 5
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "BD 2:" ))

    i := @bd2
    repeat 2
      debug.hex(LONG[i], 8)
      i += 4
      debug.out(" ")

    debug.str(string(13, "Buf 2: "))

    i := @buf2
    repeat 32
      debug.out(BYTE[i++] #> " ")

    waitcnt(cnt + clkfreq/4) 