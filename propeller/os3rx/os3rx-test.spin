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
  SERVO_PIN  = 1
  LED_PIN    = 2

  BAUD       = 19200
  
OBJ
  rx : "os3rx"
  serial : "FullDuplexSerial"
  servo : "Servo32v3"
  
VAR
  long  pkt_bits
  long  pkt_data[4]
  byte  servo_state
  
PUB main | temp

  servo.Set(SERVO_PIN, 1000)
  servo.Start

  rx.start(RX_PIN, @pkt_bits)
  serial.start(31, 30, 0, BAUD)

  serial.str(string("Ready, waiting for packets", 13, 10))
      
  dira[LED_PIN]~~
  
  repeat
    repeat until pkt_bits > 0 AND pkt_bits =< 128
    outa[LED_PIN]~~

    serial.str(string("Packet, "))
    serial.dec(pkt_bits)
    serial.str(string("bits ["))
    bin_dump(@pkt_data, pkt_bits)
    serial.str(string("] ", 13, 10))

    toggle
    
    waitcnt(clkfreq/2 + cnt)

    outa[LED_PIN]~
    pkt_bits~

PRI bin_dump(ptr, len) | r
  if len > 32
    bin_dump(ptr + 4, len - 32)
    bin_dump(ptr, 32)
  else
    r := LONG[ptr] >< len
    repeat len
      if r & 1
        serial.tx("1")
      else
        serial.tx("0")
      r >>= 1

PRI toggle 
  servo.Set(SERVO_PIN, 2000)
  waitcnt(clkfreq/2 + cnt)
  servo.Set(SERVO_PIN, 1000)
  