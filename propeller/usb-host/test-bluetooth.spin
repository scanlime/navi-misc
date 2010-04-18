' Bluetooth HCI Test

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000
  
OBJ
  bt : "bluetooth-host"
  hc : "usb-fs-host"
  term : "tv_text"
  
PUB main
  term.start(12)

  term.str(string("Starting Bluetooth... "))
  if showError(\bt.Start, string("Can't start Bluetooth host"))
    return

  bt.SetName(string("Propeller"))
  bt.SetClass(bt#COD_Computer)
  bt.SetDiscoverable
  
  term.str(string("Done.", $D, "Local Address: ", $C, $85, " "))
  term.str(bt.AddressToString(bt.LocalAddress))
  term.str(string(" ", $C, $80, $D))

  'showConnections
  showDiscovery

PRI showConnections | i
  repeat
    repeat i from 0 to 7
      term.str(string($A, 3, $B))
      term.out(4+i)
      term.hex(i, 4)
      term.out(" ")
      term.str(bt.AddressToString(\bt.ConnectionAddress(i)))

PRI showDiscovery | i, count
  bt.DiscoverDevices(30)
  repeat
    term.str(string($A, 1, $B, 2, "Devices found: "))
    term.dec(count := bt.NumDiscoveredDevices)
    if bt.DiscoveryInProgress
      term.str(string(" (Scanning...)"))
    else
      term.str(string("              "))
    
    if count
      repeat i from 0 to count - 1
        term.out($A)
        term.out(0)
        term.out($B)
        term.out(3+i)
        term.str(bt.AddressToString(bt.DiscoveredAddr(i)))
        term.out(" ")
        term.hex(bt.DiscoveredClass(i), 6)
  
PRI showError(error, message) : bool
  if error < 0
    term.str(message)
    term.str(string(" (Error "))
    term.dec(error)
    term.str(string(")", 13))
    return 1
  return 0
