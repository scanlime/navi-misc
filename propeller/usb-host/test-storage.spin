
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000

OBJ
  hc : "usb-fs-host"
  term : "Parallax Serial Terminal"
  
VAR
  byte  buf[2048]
  long  epIn, epOut

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
      term.hex(hc.VendorID, 4)
      term.str(string(":"))
      term.hex(hc.ProductID, 4)
                                      
    term.str(string(term#NL, term#NL, "Device Descriptor:", term#NL))    
    repeat i from 0 to hc#DEVDESC_LEN - 1
      term.str(string(" "))
      term.hex(BYTE[hc.DeviceDescriptor + i], 2)

    term.str(string(term#NL, term#NL, "Configure: "))
    error := hc.Configure
    term.dec(error)

    i := hc.FindInterface(8)
    epIn := hc.NextEndpoint(i)
    epOut := hc.NextEndpoint(epIn)
      
    term.str(string(term#NL, term#NL, "Bulk Write: "))
    error := hc.BulkWrite(epOut, @cbw, 31)
    term.dec(error)
    term.str(string(term#NL))

    bulkRead
    bulkRead
         
    waitcnt(cnt + clkfreq / 4)

pub bulkRead | value, i
  term.str(string(term#NL, term#NL, "Bulk Read: "))
  bytefill(@buf, $42, 64)
  value := hc.BulkRead(epIn, @buf, 64)
  term.dec(value)
  term.char(term#NL)

  repeat i from 0 to 64
    term.char(" ")
    term.hex(BYTE[@buf + i], 2)

  term.char(term#NL)

  repeat i from 0 to 64
    value := BYTE[@buf + i]
    if value < 32 or value > 127
      value := "."
    term.char(value)


DAT

cbw     long  $43425355
        long  1           ' tag
        long  $24         ' length
        byte  $80         ' flags
        byte  0           ' lun
        byte  $6          ' cbLength

        byte  $12, 0, 0, 0, $24, 0  ' INQUIRY
        'byte  $0, 0, 0, 0, 0, 0  ' TEST_UNIT_READY
        
        byte  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 