{{

BluetoothConduit
----------------

This object occupies a single cog, and implements a high-speed
conduit which allows reading and writing various kinds of shared
memory objects over a Bluetooth link. We support the BGB203-SPP 
Bluetooth module/firmware used by the Spark Fun Bluetooth module.

This module configures the Bluetooth module (setting baud rate,
name, etc) using the FullDuplexSerial object, then it switches
to a built-in assembly language driver which implements serial
I/O and the data protocol in a single cog.

At the lowest level, the conduit protocol is a simple bytecode
which can read and write regions of hub memory. Each region has
an ID (small positive number), a base address, and a size.

At a slightly higher level, by convention we use some regions to
describe other regions. Each even-numbered region is a descriptor,
and each odd-numbered region is a data area. For example, region
0 is a descriptor for region 1. It tells the PC software what
region 1 is used for.

At connect time, the PC can read all even-numbered regions in order
to enumerate the available memory regions on the device.


Bytecode Format
---------------

We use the RFCOMM and SPP protocols, which emulates a serial port
over Bluetooth. Our serial protocol consists of a sequence of
bytecodes which allow the PC to control the conduit cog:

 • 0xF0 <ID>

   Set the read/write pointer to the beginning of region <ID>.

 • 0xE1 <count>

   Read <count> longs, and advance the pointer. Reads that extend
   past the bounds of the memory region return undefined data.

 • 0xD2 <count> <data...>

   Write <count> longs. Writes that extend past the bounds of the
   memory region are ignored.

 • 0xC3 <offset8>

   Reposition the read/write pointer forward or backward by the
   specified number of longs, as an 8-bit signed number.

 • 0xB4 <offset16>

   Reposition the read/write pointer forward or backward by the
   specified number of longs, as a 16-bit signed number.

 • 0x00

   Guaranteed to be a no-op. If the connection is in an unknown
   state (just after connecting, for example) the PC must send
   up to 255*4 no-op bytes in order to guarantee that the last
   command has been completed.
   
All other bytecodes are currently undefined. Undefined bytecodes
are ignored by the interpreter.

All numbers are in little-endian byte order. 


Descriptor Format
-----------------

Descriptors are even-numbered regions of hub memory, which serve
to describe odd-numbered regions. Descriptors have a fixed format:

  Offset │ Size │ Description
 ────────┼──────┼─────────────────────────────────────
       0 │    4 │ Size of the described region, in longs
       4 │    N │ ASCII name of region
     4+N │    1 │ NUL byte (0)

     
┌──────────────────────────────────────────────────────┐
│ Copyright (c) 2008 Micah Dowty                       │               
│ See end of file for terms of use.                    │
│                                                      │
│ Uses code from the FullDuplexSerial object, written  │
│ by Chip Gracey and Copyright (c) 2006 Parallax, Inc. │ 
│                                                      │
│ The FullDuplexSerial code is released under the      │
│ same terms of use.                                   │
└──────────────────────────────────────────────────────┘

}}

OBJ
  ' We implement our own high-speed serial driver, but
  ' FullDuplexSerial is used by our Spin code during initialization.

  serial : "FullDuplexSerial"

CON
  ' ASCII characters
  QUOTE = 34
  CR = 13
  LF = 10

  MAX_REGIONS = 32              ' Must be a power of two
  TX_RING_SIZE = 128

  ' XXX: Configuration on startup is disabled for now.
  '      While testing this code, I got my BGB203 stuck
  '      in Data mode, so now configuration won't work.

  CONFIGURE_BT_MODULE = 0

DAT

        ' Baud rates supported by the Bluetooth adapter.
        '
        ' The BGB203 data sheet claims to support rates up through 1 MBit,
        ' but the Spark Fun bluetooth adapter's docs only claim to support
        ' rates up to 115200. I haven't been able to get rates higher than
        ' 115200 to work, but I'm not sure whether the problem is with the
        ' bluetooth adapter or with FullDuplexSerial...
        '
        ' All rates in this zero-terminated list are searched when we're
        ' initializing in auto-baud mode. The first rate in the list is
        ' our preferred baud rate. We'll try to switch to it during
        ' initialization.

baudTable      long      115200, 9600, 14400, 19200, 38400, 57600, 0
  
VAR
  long  txiPin, rxoPin          ' Serial pins
  long  baud                    ' Current baud rate                                           

  long cog
  long nextId

  long rx_pin, tx_pin, bit_ticks
  long regionAddrs[MAX_REGIONS]
  long regionSizes[MAX_REGIONS]
  
PUB start(rtsInputPin, rxOutputPin, txInputPin, ctsOutputPin, btName) : okay
  '' Initialize the Bluetooth module, and start our driver cog.

  ' We don't use CTS yet. Drive it high.
  outa[ctsOutputPin]~~
  dira[ctsOutputPin]~~

  rx_pin := txInputPin
  tx_pin := rxOutputPin

  if not CONFIGURE_BT_MODULE
    baud := baudTable
  else    
  
    ' Try to initialize communications with the Bluetooth device.
    okay := initSerial
    if not okay
      return

    ' Set our preferred baud rate, and re-initialize.
    okay := setBaudRate(baudTable)
    if not okay
      return
  
    ' Set the Bluetooth device name 
    serial.str(string("AT+BTLNM: ", QUOTE))
    serial.str(btName)
    serial.str(string(QUOTE, CR))
    okay := waitForReply
    if not okay
      return

    ' Set automatic connection mode, so we stay in data mode rather than
    ' command mode after initialization.
  '  serial.str(string("AT+BTAUT=1,0", CR))
  '  okay := waitForReply
  '  if not okay
  '    return

    ' Disable escape sequences. We want to stay in data mode.
  '  serial.str(string("AT+BTCFG=4", CR))
  '  okay := waitForReply
  '  if not okay
  '    return

    ' Listen for incoming connections and switch to data mode.
  '  serial.str(string("AT+BTSRV=1", CR))
  '  okay := waitForReply
  '  if not okay
  '    return
  
  serial.stop
  resetRegions
  
  ' Start the driver cog
  bit_ticks := clkfreq / baud
  cog := cognew(@entry, @rx_pin)
  if cog < 0
    stop
    return

  okay := 1

PUB defineRegion(desc, addr)
  '' Define a new memory region. 'desc' is a hub address pointing to
  '' the descriptor for this new memory region, and 'addr' is the hub
  '' address of the region itself.
  ''
  '' The size of the descriptor is calculated automatically by reading
  '' it, and the size of the described region is retrieved from the
  '' descriptor.

  if nextId => MAX_REGIONS
    ' Out of regions!
    return

  regionAddrs[nextId] := desc
  regionSizes[nextId] := getDescSize(desc)
  nextId++
  
  regionAddrs[nextId] := addr
  regionSizes[nextId] := LONG[desc]
  nextId++

PUB resetRegions | i
  '' Undefine all regions

  nextId := 0

  repeat i from 0 to MAX_REGIONS-1
    regionAddrs[i] := @nullRegionDesc
    regionSizes[i] := 0
  
PRI getDescSize(desc) : size | byteSize, c
  '' Calculate the size of a descriptor, in longs

  ' Skip the region size long
  byteSize := 4

  ' Find the name's NUL terminator
  repeat
    c := BYTE[desc + byteSize]
    byteSize++
    if c == 0
      quit

  ' Round up to the next long
  size := (byteSize + 3) >> 2  

PRI initSerialOnce : okay | baudPtr
  ' Initialize the serial port, with auto-baud.

  baudPtr := @baudTable
  repeat
    baud := LONG[baudPtr]
    baudPtr += 4

    if baud == 0
      okay := 0
      return

    serial.start(rx_pin, tx_pin, %0000, baud)

    ' Enter command mode with the default escape sequence,
    ' then issue a no-op command.
    
    serial.str(string("+++AT",CR))
    okay := waitForReplyTimeout(100)
    if okay
      return

PRI setBaudRate(newBaud) : okay
  ' Switch baud rates, and re-initialize.
    
  serial.str(string("AT+BTURT="))
  serial.dec(newBaud)
  serial.str(string(",8,0,1,0", CR))

  ' The reply to +BTURT comes back at the old baud rate.
  okay := waitForReply
  if not okay
    return

  ' Switch to the new rate.
  okay := initSerial

PRI initSerial : okay
  ' Initialize the serial port, with auto-baud and retry.
  
'  repeat 5  
  repeat
    okay := initSerialOnce
    if okay
      return
  
PRI waitForReply : okay
  ' Wait for an "OK" reply, with the default timeout.

  okay := waitForReplyTimeout(1000)
    
PRI waitForReplyTimeout(charTimeout) : okay | c, s
  ' Wait for an "OK" reply, with the specified timeout.
  ' On timeout (or on error reply), returns with okay=0.

  s := 0
  okay := 0

  repeat
    c := serial.rxtime(charTimeout)

    if c < 0
      ' Timeout
      return

    if c == LF
      ' Ignore LFs
      next

    ' String matching state machine      
    case s
      0:
        if c == CR
          s := 1
        else
          s := 0
      1:
        if c == "O" or c == "o"
          s := 2
        elseif c == CR
          s := 1
        else
          s := 0
      2:
        if c == "K" or c == "k"
          s := 3
        elseif c == CR
          s := 1
        else
          s := 0
      3:
        if c == CR
          okay := 1
          return
        else
          s := 0
       
PUB stop
  '' Immediately stop the driver cog.

  if cog > 0
    cogstop(cog)
  cog := -1

DAT

nullRegionDesc          long 0, 0


'==============================================================================
' Driver Cog
'==============================================================================

                        org

                        '======================================================
                        ' Initialization
                        '======================================================

entry                   mov     t1,par                'get structure address

                        rdlong  t2,t1                 'get rx_pin
                        mov     rxmask,#1
                        shl     rxmask,t2

                        add     t1,#4                 'get tx_pin
                        rdlong  t2,t1
                        mov     txmask,#1
                        shl     txmask,t2

                        add     t1,#4                 'get bit_ticks
                        rdlong  bitticks,t1

                        or      outa,txmask           
                        or      dira,txmask           

                        mov     txhead, #txring       'empty the TX ring buffer.
                        mov     txtail, #txring

                        mov     txcode, #transmit     ' Initialize receive/transmit multitasking


                        '======================================================
                        ' Receive thread
                        '======================================================

receive                 jmpret  rxcode,txcode         'run a chunk of transmit code, then return

                        test    rxmask,ina      wc    'wait for start bit on rx pin
        if_c            jmp     #receive

                        mov     rxbits,#9             'ready to receive byte
                        mov     rxcnt,bitticks
                        shr     rxcnt,#1
                        add     rxcnt,cnt                          

:bit                    add     rxcnt,bitticks        'ready next bit period

:wait                   jmpret  rxcode,txcode         'run a chunk of transmit code, then return

                        mov     t1,rxcnt              'check if bit receive period done
                        sub     t1,cnt
                        cmps    t1,#0           wc
        if_nc           jmp     #:wait

                        test    rxmask,ina      wc    'receive bit on rx pin
                        rcr     rxdata,#1
                        djnz    rxbits,#:bit

                        shr     rxdata,#32-9          'justify and trim received byte
                        and     rxdata,#$FF

bcState                 jmp     #s_default            ' Enter bytecode state machine
              
                 
                        '======================================================
                        ' Bytecode state machine
                        '======================================================

cmd_done                movs    bcState, #s_default
                        jmp     #receive

s_default               cmp     rxdata, #$F0 wz
              if_z      jmp     #cmd_F0
                        cmp     rxdata, #$E1 wz
              if_z      jmp     #cmd_E1
                        cmp     rxdata, #$D2 wz
              if_z      jmp     #cmd_D2
                        cmp     rxdata, #$C3 wz
              if_z      jmp     #cmd_C3
                        cmp     rxdata, #$B4 wz
              if_z      jmp     #cmd_B4
                        jmp     #cmd_done

cmd_F0                  movs    bcState, #s_cmd_F0_arg0
                        jmp     #receive

cmd_E1                  movs    bcState, #s_cmd_E1_arg0
                        jmp     #receive

cmd_D2                  movs    bcState, #s_cmd_D2_arg0
                        jmp     #receive

cmd_C3                  movs    bcState, #s_cmd_C3_arg0
                        jmp     #receive

cmd_B4                  movs    bcState, #s_cmd_B4_arg0
                        jmp     #receive

                        
                        ''
                        '' Set read/write pointer to beginning of memory region.
                        ''
s_cmd_F0_arg0           and     rxdata, #(MAX_REGIONS - 1)    ' Calculate address of array entry
                        shl     rxdata, #2
                        add     rxdata, par                   ' Point to regionAddrs
                        add     rxdata, #(3*4)
                        rdlong  bcAddress, rxdata             ' Store min/current addresses
                        mov     bcAddrMin, bcAddress
                        add     rxdata, #(MAX_REGIONS * 4)    ' Point to regionSizes
                        rdlong  bcAddrMax, rxdata
                        shl     bcAddrMax, #2
                        add     bcAddrMax, bcAddress          ' Store max address (plus one)
                        jmp     #cmd_done

                        
                        ''
                        '' Begin reading longs.
                        ''
s_cmd_E1_arg0           mov     t1, txhead                    ' Prepare the FIFO write addresses
                        movd    :storeAddr, t1
                        add     t1, #1
                        movd    :storeCount, t1
                        add     t1, #1
                        cmp     t1, #txring_end wz
              if_z      mov     t1, #txring
                        
:storeAddr              mov     0, bcAddress                  ' Write to the FIFO
:storeCount             mov     0, rxdata

                        mov     txhead, t1                    ' Atomically update the FIFO head pointer

                        shl     rxdata, #2                    ' Autoincrement address
                        add     bcAddress, rxdata

                        jmp     #cmd_done

                        ''
                        '' Begin writing longs.
                        '' 
s_cmd_D2_arg0           mov     bcCount, rxdata
                        movs    bcState, #s_cmd_D2_data0
                        jmp     #receive

                        ''
                        '' Shift in data to write
                        '' 
s_cmd_D2_data0          mov     bcData, rxdata
                        movs    bcState, #s_cmd_D2_data1
                        jmp     #receive

s_cmd_D2_data1          shl     rxdata, #8
                        or      bcData, rxdata
                        movs    bcState, #s_cmd_D2_data2
                        jmp     #receive

s_cmd_D2_data2          shl     rxdata, #16
                        or      bcData, rxdata
                        movs    bcState, #s_cmd_D2_data3
                        jmp     #receive

                        ''
                        '' Write a data word
                        '' 

s_cmd_D2_data3          shl     rxdata, #24
                        or      bcData, rxdata

                        sub     bcAddress, bcAddrMin nr,wc   ' Bounds check bcAddress
              if_c      jmp     #:out_of_bounds
                        sub     bcAddress, bcAddrMax nr,wc
              if_nc     jmp     #:out_of_bounds

                        wrlong  bcData, bcAddress

:out_of_bounds          add     bcAddress, #4

                        sub     bcCount, #1 wz               ' Next data word
              if_z      jmp     #cmd_done
                        movs    bcState, #s_cmd_D2_data0
                        jmp     #receive


                        ''
                        '' 8-bit address offset
                        '' 
s_cmd_C3_arg0           and     rxdata, #$80 nr,wz           ' Sign extend from 8-bit to 32-bit
              if_nz     or      rxdata, :sextMask
                        shl     rxdata, #2
                        add     bcAddress, rxdata
                        jmp     #cmd_done
:sextMask               long    $FFFFFF00


                        ''
                        '' 16-bit address offset
                        '' 
s_cmd_B4_arg0           mov     bcData, rxdata
                        movs    bcState, #s_cmd_B4_arg1
                        jmp     #receive

s_cmd_B4_arg1           shl     rxdata, #8
                        or      rxdata, bcData
                        and     rxdata, :sextTest nr,wz       ' Sign extend from 16-bit to 32-bit
              if_nz     or      rxdata, :sextMask
                        shl     rxdata, #2
                        add     bcAddress, rxdata
                        jmp     #cmd_done
:sextTest               long    $00008000
:sextMask               long    $FFFF0000


                        '======================================================
                        ' Transmit thread
                        '======================================================

transmit                jmpret  txcode,rxcode         'run a chunk of receive code, then return

                        cmp     txhead, txtail  wz    ' Is there a string to transmit?
        if_z            jmp     #transmit

                        movs    :fetchAddr, txtail    ' Fetch TX address and count from ring buffer
                        add     txtail, #1
                        movs    :fetchCount, txtail
                        add     txtail, #1
                        cmp     txtail, #txring_end wz
        if_z            mov     txtail, #txring
:fetchAddr              mov     txaddr, 0
:fetchCount             mov     txcount, 0

:long                   cmp     txcount, #0 wz        ' Is this TX string empty?
        if_z            jmp     #transmit

                        rdlong  txlong, txaddr        ' Read the next long to transmit
                        add     txaddr, #4
                        sub     txcount, #1

                        mov     txbytecnt, #4         ' Loop over the 4 bytes in this long
:byte                   mov     txdata, txlong
                        shr     txlong, #8            ' Little endian: LSB first
                        
                        and     txdata,#$FF
                        or      txdata,#$100          'ready byte to transmit
                        shl     txdata,#2
                        or      txdata,#1
                        mov     txbits,#11
                        mov     txcnt,cnt

:bit                    shr     txdata,#1       wc    'output bit on tx pin
                        muxc    outa, txmask       
                        add     txcnt,bitticks        'ready next cnt

:wait                   jmpret  txcode,rxcode         'run a chunk of receive code, then return

                        mov     t1,txcnt              'check if bit transmit period done
                        sub     t1,cnt
                        cmps    t1,#0           wc
        if_nc           jmp     #:wait

                        djnz    txbits,#:bit          ' Another bit to transmit?
                        djnz    txbytecnt, #:byte     ' Another byte?
                        jmp     #:long                ' Next long...
                        
'------------------------------------------------------------------------------
' Uninitialized Data
'------------------------------------------------------------------------------

t1                      res     1
t2                      res     1

                        ' Software UART

bitticks                res     1

rxmask                  res     1
rxbits                  res     1
rxcnt                   res     1
rxdata                  res     1
rxcode                  res     1

txmask                  res     1
txbits                  res     1
txcnt                   res     1
txdata                  res     1
txcode                  res     1

                        ' Ring buffer of hub memory strings to transmit

txhead                  res     1
txtail                  res     1
txring                  res     TX_RING_SIZE
txring_end

txaddr                  res     1
txlong                  res     1
txcount                 res     1
txbytecnt               res     1

                        ' Bytecode interpreter state

bcAddress               res     1
bcAddrMin               res     1               ' Half-open interval
bcAddrMax               res     1
bcCount                 res     1
bcData                  res     1

                        fit
                        
{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                   TERMS OF USE: MIT License                                                  │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
}}   