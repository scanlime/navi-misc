
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 6_000_000

OBJ
  storage : "usb-storage"
  term : "Parallax Serial Terminal"

VAR
  byte  replyBuf[1024]
  
PUB main
  term.Start(115200)

  repeat
    testStorage
    waitcnt(cnt + clkfreq)

PRI testStorage

  term.char(term#CS)

  if showError(\storage.Enumerate, string("Can't enumerate device"))
    return         

  if storage.Identify
    term.str(string("Identified as USB storage", term#NL))
  else
    term.str(string("NOT a USB storage device!", term#NL))
    return

  if showError(\storage.Init, string("Error initializing storage device"))
    return

  ' Show the information that our usb-storage driver collected about the device
  
  term.str(string(term#NL, "Sector Size: "))
  term.dec(storage.SectorSize)
  term.str(string(term#NL, "Number of Sectors: "))
  term.hex(storage.NumSectors, 8)

  ' Calculate disk capacity in megabytes

  term.str(string(" ("))
  term.dec(storage.NumSectors / ($100000 / storage.SectorSize))
  term.str(string(" MB)", term#NL))
  
  ' Use the low-level SCSI interface to send an INQUIRY packet.
  
  term.str(string(term#NL, "SCSI INQUIRY:", term#NL))
  storage.SCSI_CB_Begin(storage#INQUIRY, 6)
  showError(\storage.SCSI_Command(@replyBuf, $24, storage#DIR_IN, storage#DEFAULT_TIMEOUT), string("Inquiry"))
  hexDump(@replyBuf, $24)

  ' Read and hexdump a disk sector
  showSector($01)  

PRI showSector(num)
  term.str(string(term#NL, "Disk sector "))
  term.hex(num, 8)
  term.str(string(":", term#NL))
  showError(\storage.ReadSectors(@replyBuf, num, 1), string("ReadSectors"))
  hexDump(@replyBuf, storage.SectorSize)

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