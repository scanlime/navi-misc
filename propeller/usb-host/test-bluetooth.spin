' Bluetooth HCI Test

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000
  
OBJ
  bt : "bluetooth-host"
  term : "tv_text"
  
PUB main
  term.start(12)

  term.str(string("Starting Bluetooth... "))
  if showError(\bt.Start, string("Can't start Bluetooth host"))
    return

  bt.SetName(string("Propeller"))
  bt.SetClass(bt#COD_Computer)
  bt.SetDiscoverable
  bt.SetFixedPIN(string("zugzug"))
  bt.AddService(@mySerialService)
  bt.AddService(@myService2)
  
  term.str(string("Done.", $D, "Local Address: ", $C, $85, " "))
  term.str(bt.AddressToString(bt.LocalAddress))
  term.str(string(" ", $C, $80, $D))

  'showConnections
  'showDiscovery
  debug

PRI debug | ptr
  repeat
    term.out($a)
    term.out(0)
    term.out($b)
    term.out(3)

    ptr := $4000
    repeat 16
      term.hex(LONG[ptr], 8)
      ptr += 4
      term.out(" ")
  
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


DAT
mySerialService

    word  0                                ' Link
    byte  bt#DE_Seq8, @t0 - @h0            ' <sequence>
h0      

    byte    bt#DE_Uint16, $00,$00          '   ServiceRecordHandle
    byte    bt#DE_Uint32, $00,$01,$00,$02  '     (Arbitrary unique value)

    byte    bt#DE_Uint16, $00,$01          '   ServiceClassIDList
    byte    bt#DE_Seq8, @t1 - @h1          '   <sequence>
h1  byte      bt#DE_UUID16, $11,$01        '     SerialPort
t1

    byte    bt#DE_Uint16, $00,$04          '   ProtocolDescriptorList
    byte    bt#DE_Seq8, @t2 - @h2          '   <sequence>
h2  byte      bt#DE_Seq8, @t3 - @h3        '     <sequence>
h3  byte        bt#DE_UUID16, $01,$00      '       L2CAP
t3  byte      bt#DE_Seq8, @t4 - @h4        '     <sequence>
h4  byte        bt#DE_UUID16, $00,$03      '       RFCOMM
    byte        bt#DE_Uint8, $03           '       Channel
t4
t2

    byte    bt#DE_Uint16, $00,$05          '   BrowseGroupList
    byte    bt#DE_Seq8, @t5 - @h5        '   <sequence>
h5  byte      bt#DE_UUID16, $10,$02        '     PublicBrowseGroup
t5

    byte    bt#DE_Uint16, $00,$09          '   BluetoothProfileDescriptorList
    byte    bt#DE_Seq8, @t7 - @h7          '   <sequence>
h7  byte      bt#DE_Seq8, @t8 - @h8        '     <sequence>
h8  byte      bt#DE_UUID16, $11,$01        '       SerialPort
    byte      bt#DE_Uint16, $01,$00        '       Version 1.0
t8
t7              

    byte    bt#DE_Uint16, $01,$00          '   ServiceName + Language Base
    byte    bt#DE_Text8, @t9 - @h9
h9  byte      "Propeller Virtual Serial Port"
t9

t0

myService2

    word  0                                ' Link
    byte  bt#DE_Seq8, @tx - @hx            ' <sequence>
hx      

    byte    bt#DE_Uint16, $01,$00          '   ServiceName + Language Base
    byte    bt#DE_Text8, @ty - @hy
hy  byte      "This is another long string widget, for testing continuation records and stuff. Woo......."
ty

tx