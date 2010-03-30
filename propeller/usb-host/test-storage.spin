
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000

OBJ
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"
  
VAR
  byte  buf[2048]

PUB main | value, error, i
  term.Start(115200)
  hc.Start

  repeat
    error := hc.Enumerate

    term.str(string(term#CS))
    
    if error
      term.str(string(term#NL, term#NL, "Can't enumerate device ("))
      term.dec(error)
      term.str(string(" "))
      term.hex(error, 8)
      term.str(string(")"))
    else
      term.str(string(term#NL, term#NL, "Found device "))
      term.hex(hc.GetVendorID, 4)
      term.str(string(":"))
      term.hex(hc.GetProductID, 4)
                                      
    term.str(string(term#NL, term#NL, "Device Descriptor:", term#NL))    
    repeat i from 0 to hc#DEVDESC_LEN - 1
      term.str(string(" "))
      term.hex(BYTE[hc.GetDeviceDescriptor + i], 2)

    term.str(string(term#NL, term#NL, "SetConfiguration: "))
    error := hc.SetConfiguration(1)
    term.dec(error)

    term.str(string(term#NL, term#NL, "Bulk Write: "))
    error := hc.BulkWrite(2, 32, @cbw, 31)
    term.dec(error)
    term.str(string(term#NL))

    repeat 5
      term.str(string(term#NL, term#NL, "Bulk Read: "))
      error := hc.BulkRead(1, 32, @buf, 32)
      term.dec(error)
      term.str(string(term#NL))
      repeat i from 0 to 64
        term.str(string(" "))
        term.hex(BYTE[buf + i], 2)
 
    waitcnt(cnt + clkfreq/4)

DAT

cbw     long  $43425355
        long  12345       ' tag
        long  8           ' length
        byte  $80         ' flags
        byte  0           ' lun
        byte  1           ' cbLength
        byte  $25         ' READ_CAPACITY  