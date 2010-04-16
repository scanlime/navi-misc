' Bluetooth HCI Test

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000
  
OBJ
  bt : "usb-bluetooth"
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"

VAR
  byte addr[6]
  
PUB main
  term.Start(115200)

  repeat
    testBT
    waitcnt(cnt + clkfreq)

PUB setupBT

  bt.Init

  term.str(string("Bluetooth Address: "))
  term.str(bt.AddressToString(bt.ReadBDAddress))
  term.char(term#NL)

  term.str(string("Setting class of device", term#NL))
  bt.WriteClassOfDevice($000100)

  term.str(string("Setting local name", term#NL))
  bt.WriteLocalName(string("Propeller"))

  term.str(string("Setting device as discoverable", term#NL))
  bt.SetDiscoverable

PRI testBT | i

  term.char(term#CS)

  if showError(\bt.Enumerate, string("Can't enumerate device"))
    return         

  if bt.Identify
    term.str(string("Identified as Bluetooth HCI", term#NL))
  else
    term.str(string("NOT a bluetooth device!", term#NL))
    return

  if showError(\setupBT, string("Error initializing Bluetooth device"))
    return

  showPerfCounters
  showError(\inquiry, string("Error in inquiry"))
  showPerfCounters
  
  term.str(string("Looking for Wiimote... "))
  if not showError(\bt.FindDeviceByClass($002504, @addr, 5), string("Error in inquiry"))
    term.str(bt.AddressToString(@addr))
  term.char(term#NL)

  repeat while hc.GetPortConnection == hc#PORTC_FULL_SPEED
    
    
PRI inquiry
  term.str(string("Inquiry...", term#NL))
  if not showError(\bt.Inquiry(1), string("Error sending inquiry"))
    repeat while bt.Inquiry_Next
      term.str(bt.AddressToString(bt.Inquiry_BDAddress))
      term.str(string(" class="))
      term.hex(bt.Inquiry_Class, 6)
      term.char(term#NL)
    term.str(string("Done!", term#NL))
  
PRI hexDump(buffer, bytes) | x, y, b
  ' A basic 16-byte-wide hex/ascii dump

  repeat y from 0 to ((bytes + 15) >> 4) - 1
    term.hex(y << 4, 4)
    term.str(string(": "))

    repeat x from 0 to 15
      term.hex(BYTE[buffer + x + (y<<4)], 2)
      term.char(" ")

    term.char(" ")

    repeat x from 0 to 15
      b := BYTE[buffer + x + (y<<4)]
      case b
        32..126:
          term.char(b)
        other:
          term.char(".")

    term.char(term#NL)
    
PRI showError(error, message) : bool
  if error < 0
    term.str(message)
    term.str(string(" (Error "))
    term.dec(error)
    term.str(string(")", term#NL))
    return 1
  return 0

PRI showPerfCounters | i
  term.str(string("Perf:"))
  repeat i from 0 to bt#PERFMAX-1
    term.char(" ")
    term.dec(LONG[(i<<2) + bt.GetPerfCounters])
  term.char(term#NL)
 