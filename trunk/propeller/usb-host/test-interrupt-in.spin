' Quick EP1 IN test.
' I use this with a Logitech MX1100 mouse, since it's a full-speed HID device.

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000

OBJ
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"
  
VAR
  byte buf[64]

PUB main | value, error, i
  term.Start(115200)
  hc.Start

  repeat
    error := hc.Enumerate
    if error
      term.str(string(term#NL, term#NL, "Can't enumerate device ("))
      term.dec(error)
      term.str(string(" "))
      term.hex(error, 8)
      term.str(string(")"))
    else

      term.str(string(term#NL, term#NL, "SetConfiguration: "))
      error := hc.SetConfiguration(1)
      term.dec(error)

      if not error
        repeat while hc.GetPortConnection <> hc#PORTC_NO_DEVICE

          value := hc.InterruptRead(1, @buf, 64)
          if value <> hc#E_TIMEOUT
            term.str(string(term#NL, "EP1 IN ["))
            term.hex(value, 2)
            term.str(string("] "))
            repeat i from 0 to 31
              term.str(string(" "))
              term.hex(buf[i], 2)
  
          if value >= 0
            ' Just for fun, map the mouse buttons to the
            ' LEDs on the Propeller Demo Board
            dira := $FF << 16
            outa := buf[0] << 16
            
    