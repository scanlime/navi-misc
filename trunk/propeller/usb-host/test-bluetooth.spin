' Bluetooth HCI Test

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000
  
OBJ
  bt : "usb-bluetooth"
  hc : "usb-fs-host"
  term : "tv_text"  

VAR
  byte localAddr[6]
  byte remoteAddr[6]
  
PUB main
  term.start(12)

  repeat
    testBT
    waitcnt(cnt + clkfreq)

PUB setupBT

  bt.Init

  term.out(0)
  term.str(string("Local BDAddress: ", $C, $83))
  bytemove(@localAddr, bt.ReadBDAddress, 6)
  term.str(bt.AddressToString(@localAddr))
  term.str(string($D, $C, $80))

  bt.WriteClassOfDevice($000100)
  bt.WriteLocalName(string("Propeller"))
  bt.SetDiscoverable

PRI testBT | i

  if showError(\bt.Enumerate, string("Can't enumerate device"))
    return         

  if not bt.Identify
    term.str(string("NOT a bluetooth device!", 13))
    return

  if showError(\setupBT, string("Error initializing Bluetooth device"))
    return

  showError(\initWiimote, string("Error testing wiimote"))

  term.str(string(13, "Done"))
  repeat while hc.GetPortConnection == hc#PORTC_FULL_SPEED

PRI initWiimote | cHandle

  repeat
    term.dec(\bt.HCIevt_WaitMS(1000))
    term.out(" ")

  term.str(string("Looking for wiimote.. "))
  bt.FindDeviceByClass($002504, @remoteAddr, 10)
  term.str(bt.AddressToString(@remoteAddr))

  ' PIN is the first 3 bytes of our BDADDR.
  bt.SendPIN(@remoteAddr, @localAddr, 3)

  term.str(string(13, "Connecting... "))
  cHandle := bt.Connect(@remoteAddr)
  term.dec(cHandle)

  
  repeat 100
    bt.Write(cHandle)


PRI inquiry
  term.str(string("Inquiry...", 13))
  if not showError(\bt.Inquiry(1), string("Error sending inquiry"))
    repeat while bt.Inquiry_Next
      term.str(bt.AddressToString(bt.Inquiry_BDAddress))
      term.str(string(" class="))
      term.hex(bt.Inquiry_Class, 6)
      term.out(13)
    term.str(string("Done!", 13))
  
PRI hexDump(buffer, bytes) | x, y, b
  ' A basic 16-byte-wide hex/ascii dump

  repeat y from 0 to ((bytes + 15) >> 4) - 1
    term.hex(y << 4, 4)
    term.str(string(": "))

    repeat x from 0 to 15
      term.hex(BYTE[buffer + x + (y<<4)], 2)
      term.out(" ")

    term.out(" ")

    repeat x from 0 to 15
      b := BYTE[buffer + x + (y<<4)]
      case b
        32..126:
          term.out(b)
        other:
          term.out(".")

    term.out(13)
    
PRI showError(error, message) : bool
  if error < 0
    term.str(message)
    term.str(string(" (Error "))
    term.dec(error)
    term.str(string(")", 13))
    return 1
  return 0

PRI showPerfCounters | i
  term.str(string("Perf:"))
  repeat i from 0 to bt#PERFMAX-1
    term.out(" ")
    term.dec(LONG[(i<<2) + bt.GetPerfCounters])
  term.out(13)
 