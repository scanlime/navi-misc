'
' Incomplete test program for decoding Oregon Scientific 3.0 wireless weather
' station packets. So far this can do some filtering and manchester decoding,
' and print out raw packets in hex.
'
' -- Micah Dowty <micah@navi.cx>
'
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  RX_PIN     = 0
  LED_PIN    = 2

  BAUD       = 19200
  
OBJ
  rx : "os3rx"
  serial : "FullDuplexSerial"

VAR
  long  pkt_bits
  long  pkt_data[4]
  
PUB main

  rx.start(RX_PIN, @pkt_bits)
  serial.start(31, 30, 0, BAUD)

  serial.str(string("Ready, waiting for packets", 13, 10))
  
  dira[LED_PIN]~~
  
  repeat
    repeat until pkt_bits <> 0
    outa[LED_PIN]~~

    serial.str(string("Packet ["))
    serial.dec(pkt_bits)
    serial.str(string(" bits] "))
    serial.hex(pkt_data[3], 8)
    serial.tx(" ")
    serial.hex(pkt_data[2], 8)
    serial.tx(" ")
    serial.hex(pkt_data[1], 8)
    serial.tx(" ")
    serial.hex(pkt_data[0], 8)
    serial.tx(" ")
    serial.str(string(13, 10))
    
    waitcnt(cnt + clkfreq/4)

    outa[LED_PIN]~
    pkt_bits~
