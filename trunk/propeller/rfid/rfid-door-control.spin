{

Access control example using rfid-lf
────────────────────────────────────

This is a functioning example for the rfid-lf module.
It scans for RFID cards that match a list of accepted codes.
When one is found, we activate a relay momentarily.
A bi-color LED indicates when access is allowed or denied.

See the rfid-lf module for the RFID reader schematic.

Other parts include a reed relay (don't forget the protection
diode) and a bi-color LED with two current limiting resistors.

There is also a TV output on pin 12, for debugging.

Micah Dowty <micah@navi.cx>

 ┌───────────────────────────────────┐
 │ Copyright (c) 2008 Micah Dowty    │               
 │ See end of file for terms of use. │
 └───────────────────────────────────┘

}

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  TV_PIN           = 12
  RED_LED_PIN      = 23 ' Active high (bi-color LED)
  GREEN_LED_PIN    = 22
  RELAY_PIN        = 17 ' Active low

OBJ
  debug : "TV_Text"
  rfid  : "rfid-lf"

VAR
  long  buffer[16]
  
PUB main | i, format, isMatch
  debug.start(TV_PIN)
  hardwareInit
  rfid.start

  debug.out(0)
  debug.str(string("rfid-lf test:", 13))

'  cognew(@foo, 0)
  
  repeat
    if format := rfid.read(@buffer)

      if isMatch := matchCode(format, @buffer)
        LED_Green
      else
        LED_Red

      repeat i from 0 to (format & $FFFF) - 1
        debug.hex(buffer[i], 8)
        debug.out(" ")
      debug.out(13)

      if isMatch
        ' Pulse the relay, and wait for the door to actuate.
        ' After we're done, flush the RFID buffer so we don't
        ' immediately read another matching code.
        Relay_Pulse
        rfid.read(@buffer)

      LED_Off

  
PRI matchCode(format, bufPtr) : isMatch | pBuf, pTable, tFormat, len
  ' Check a received code against a table of authorized codes.
  ' The table consists of a zero-terminated list of longs. Each
  ' code has one long for its format code, followed by a variable
  ' number of longs for the code data itself.

  isMatch~
  pTable := @code_table

  repeat while tFormat := LONG[pTable]
    len := tFormat & $FFFF
    pTable += 4

    if tFormat == format and longCompare(bufPtr, pTable, len)
      isMatch~~
      return

    pTable += len << 2

PRI longCompare(bufA, bufB, count) : equal
  repeat count
    if LONG[bufA] <> LONG[bufB]
      equal~
      return
    bufA += 4
    bufB += 4
  equal~~
  return 
      
PRI hardwareInit
  LED_Off
  Relay_Off
  dira[RED_LED_PIN]~~
  dira[GREEN_LED_PIN]~~
  dira[RELAY_PIN]~~

PRI LED_Red
  outa[RED_LED_PIN]~~
  outa[GREEN_LED_PIN]~

PRI LED_Green
  outa[RED_LED_PIN]~
  outa[GREEN_LED_PIN]~~

PRI LED_Yellow
  outa[RED_LED_PIN]~~
  outa[GREEN_LED_PIN]~~

PRI LED_Off
  outa[RED_LED_PIN]~
  outa[GREEN_LED_PIN]~

PRI Relay_On
  outa[RELAY_PIN]~

PRI Relay_Off
  outa[RELAY_PIN]~~

PRI Relay_Pulse
  Relay_On
  waitcnt(clkfreq + cnt)
  Relay_Off
  waitcnt(clkfreq * 5 + cnt)

DAT

code_table
              long      rfid#FORMAT_EM4102, $17, $7F012B
              long      rfid#FORMAT_EM4102, $36, 7819960
              long      0
  
DAT
                        org
foo
                        waitpeq in_mask, in_mask
                        waitpne in_mask, in_mask
                        waitpeq in_mask, in_mask
                        waitpne in_mask, in_mask
                        waitpeq in_mask, in_mask
                        waitpne in_mask, in_mask
                        waitpeq in_mask, in_mask
                        waitpne in_mask, in_mask
                        xor     dira, out_mask
                        jmp     #foo

in_mask       long      |<0
out_mask      long      |<7      