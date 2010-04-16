' Bluetooth HCI Test

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000
  
OBJ
  bt : "usb-bluetooth"
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"

PUB main
  term.Start(115200)

  repeat
    testBT
    waitcnt(cnt + clkfreq)

PUB setupBT

  bt.Init

  term.str(string("Bluetooth Address: "))
  term.str(bt.BDAddressString)
  term.char(term#NL)
  showPerfCounters

  term.str(string("Setting class of device", term#NL))
  bt.SetClassOfDevice($000100)
  showPerfCounters

  term.str(string("Setting local name", term#NL))
  bt.SetLocalName(string("Propeller"))
  showPerfCounters

  term.str(string("Setting device as discoverable", term#NL))
  bt.SetDiscoverable
  showPerfCounters

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

  term.str(string("Bluetooth Address: "))
  term.str(bt.BDAddressString)
  term.char(term#NL)
  
  bt.HCIcmd_Begin(bt#CB_ReadClassOfDevice)
  if not showError(\bt.HCIcmd_Wait, string("Error sending cmd"))
    term.str(string("Read Class : "))
    term.dec(bt.HCIevt_ParamSize)
    term.char(term#NL)
    hexDump(bt.HCIevt_Buffer, bt.HCIevt_ParamSize)

  bt.HCIcmd_Begin(bt#CB_ReadLocalName)
  if not showError(\bt.HCIcmd_Wait, string("Error sending cmd"))
    term.str(string("Read local name : "))
    term.dec(bt.HCIevt_ParamSize)
    term.char(term#NL)
    hexDump(bt.HCIevt_Buffer, bt.HCIevt_ParamSize)

  repeat while hc.GetPortConnection == hc#PORTC_FULL_SPEED

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
 