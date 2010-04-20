{{

 bluetooth-sdp  ver 0.2
──────────────────────────────────────────────────────────────────

Service Discovery Protocol support for the Propeller Bluetooth stack.

I originally started this by writing a fairly complete-looking SDP
server, with a searchable service record database, DataElement
decoding and encoding, and a public function to link your own
new service records into our database.

But all of that functionality uses a lot of RAM, and hardly any of it
actaully gets used. Of the three transaction types described by SDP,
I've only seen PCs ever send one of them- and our service records
should be small enough that we have no real need to support searching
or filtering them. This SDP server just sends our entire service
database in response to any attribute request. It sure isn't spec
compliant, but it should work in practice, and it takes very little
memory to implement.

The latest version of this file should always be at:
http://svn.navi.cx/misc/trunk/propeller/usb-host/bluetooth-sdp.spin

 ┌────────────────────────────────────────────────┐
 │ Copyright (c) 2010 Micah Dowty <micah@navi.cx> │               
 │ See end of file for terms of use.              │
 └────────────────────────────────────────────────┘

}}

DAT
''
''
''==============================================================================
'' SDP Service Database
''==============================================================================
''

' This is our hardcoded SDP response, defining all services that this
' Bluetooth device advertises. If you want to add new services, they
' go here. See the Bluetooth SDP and GAP specifications for more info.
'
' Note that none of these services are required, and in each service,
' only the ServiceRecordHandle and ServiceClassIDList are mandated by
' the SDP spec.
'
' To debug service records, try using BlueZ's "sdptool" to retrieve and dump
' them. The --raw option will show the raw identifiers that you can use in
' this table.

serviceDatabase
        byte  DE_Sequence | DE_Size16
        byte  (@serviceDatabase_end - @serviceDatabase - 3) >> 8
        byte  (@serviceDatabase_end - @serviceDatabase - 3)

        '===============================
        ' SDP Service
        '===============================

        ' Note: The SDP service is included here as an example,
        ' and for completeness. But it isn't actually required, so
        ' I've disabled it by default to save space.
{       
        byte  DE_Seq8, @t1 - @h1                  ' <sequence>
h1      

        byte    DE_Uint16, $00,$00                '   ServiceRecordHandle
        byte    DE_Uint32, $00,$01,$00,$01        '     (Arbitrary unique value)

        byte    DE_Uint16, $00,$01                '   ServiceClassIDList
        byte    DE_Seq8, @t2 - @h2                '   <sequence>
h2      byte      DE_UUID16, $10,$00              '     ServiceDiscoveryServerServiceClassID
t2

        byte    DE_Uint16, $00,$04                '   ProtocolDescriptorList
        byte    DE_Seq8, @t3 - @h3                '   <sequence>
h3      byte      DE_Seq8, @t4 - @h4              '     <sequence>
h4      byte        DE_UUID16, $01,$00            '       L2CAP
        byte        DE_Uint16, $00,$01            '       Standard L2CAP PSM for SDP
t4      byte      DE_Seq8, @t5 - @h5              '     <sequence>
h5      byte        DE_UUID16, $00,$01            '       SDP
t5
t3

        byte    DE_Uint16, $00,$05                '   BrowseGroupList
        byte    DE_Seq8, @t6 - @h6                '   <sequence>
h6      byte      DE_UUID16, $10,$02              '     PublicBrowseGroup
t6

t1
}

        '===============================
        ' Serial Port service #1
        '===============================

        byte  DE_Seq8, @t10 - @h10                ' <sequence>
h10      

        byte    DE_Uint16, $00,$00                '   ServiceRecordHandle
        byte    DE_Uint32, $00,$01,$00,$02        '     (Arbitrary unique value)

        byte    DE_Uint16, $00,$01                '   ServiceClassIDList
        byte    DE_Seq8, @t11 - @h11              '   <sequence>
h11     byte      DE_UUID16, $11,$01              '     SerialPort
t11

        byte    DE_Uint16, $00,$04                '   ProtocolDescriptorList
        byte    DE_Seq8, @t12 - @h12              '   <sequence>
h12     byte      DE_Seq8, @t13 - @h13            '     <sequence>
h13     byte        DE_UUID16, $01,$00            '       L2CAP
t13     byte      DE_Seq8, @t14 - @h14            '     <sequence>
h14     byte        DE_UUID16, $00,$03            '       RFCOMM
        byte        DE_Uint8, $03                 '       Channel
t14
t12

        byte    DE_Uint16, $00,$05                '   BrowseGroupList
        byte    DE_Seq8, @t15 - @h15              '   <sequence>
h15     byte      DE_UUID16, $10,$02              '     PublicBrowseGroup
t15

        byte    DE_Uint16, $00,$06                '   LanguageBaseAttributeIDList
        byte    DE_Seq8, @t16 - @h16              '   <sequence>
h16     byte      DE_Uint16, $65,$6e              '     Language
        byte      DE_Uint16, $00,$6a              '     Encoding
        byte      DE_Uint16, $01,$00              '     Base attribute ID value
t16             

        ' Note: Naming the service is optional. If you don't want it to have
        ' a name, you can delete the LanguageBaseAttributeIDList attribute,
        ' as well as the service name attribute itself.
        '
        ' The service name is used in various ways by different operating
        ' systems. On Windows it appears in the properties for the Bluetooth
        ' device, alongside the COM port number. On Mac OS it's used to
        ' identify the service to the user in several places, plus it's truncated
        ' and used in the device node. (/dev/tty.DeviceName-ServiceName)
{
        byte    DE_Uint16, $00,$09                '   BluetoothProfileDescriptorList
        byte    DE_Seq8, @t17 - @h17              '   <sequence>
h17     byte      DE_Seq8, @t18 - @h18            '     <sequence>
h18     byte      DE_UUID16, $11,$01              '       SerialPort
        byte      DE_Uint16, $01,$00              '       Version 1.0
t18
t17              

        byte    DE_Uint16, $01,$00                '   ServiceName + Language Base
        byte    DE_Text8, @t19 - @h19
h19     byte      "Propeller Virtual Serial Port"
t19
}

t10
        '===============================
        ' End of services
        '===============================

serviceDatabase_end


CON

  ' Local error codes
  E_END_OF_BUFFER   = -1

  ' Limits maximum PDU size.
  ' (Currently only needs to be as big as our L2CAP MTU)
  BUFFER_SIZE       = 64 - 8

  ' SDP PDU Types

  SDP_ErrorResponse      = $01
  SDP_SearchRequest      = $02
  SDP_SearchResponse     = $03
  SDP_AttrRequest        = $04
  SDP_AttrResponse       = $05
  SDP_SearchAttrRequest  = $06
  SDP_SearchAttrResponse = $07

  ' SDP Errors

  SE_BadSDPVersion       = $0001
  SE_BadServiceRecord    = $0002
  SE_BadSyntax           = $0003
  SE_BadPDUSize          = $0004
  SE_BadContinuation     = $0005
  SE_NoResources         = $0006

  ' Data element types

  DE_TYPE         = %11111 << 3
  DE_Nil          = 0 << 3
  DE_Unsigned     = 1 << 3
  DE_Signed       = 2 << 3
  DE_UUID         = 3 << 3
  DE_Text         = 4 << 3
  DE_Boolean      = 5 << 3
  DE_Sequence     = 6 << 3
  DE_Alternative  = 7 << 3
  DE_URL          = 8 << 3

  ' Data element sizes

  DE_SIZE         = %111
  DE_1Byte        = 0
  DE_2Bytes       = 1
  DE_4Bytes       = 2
  DE_8Bytes       = 3
  DE_16Bytes      = 4
  DE_Size8        = 5  ' Subsequent byte(s) are a size field
  DE_Size16       = 6
  DE_Size32       = 7

  ' Common combinations of type and size

  DE_Uint8        = DE_Unsigned | DE_1Byte
  DE_Uint16       = DE_Unsigned | DE_2Bytes
  DE_Uint32       = DE_Unsigned | DE_4Bytes
  DE_Int8         = DE_Signed | DE_1Byte
  DE_Int16        = DE_Signed | DE_2Bytes
  DE_Int32        = DE_Signed | DE_4Bytes
  DE_UUID16       = DE_UUID | DE_2Bytes
  DE_UUID32       = DE_UUID | DE_4Bytes
  DE_UUID128      = DE_UUID | DE_16Bytes
  DE_Text8        = DE_Text | DE_Size8
  DE_Seq8         = DE_Sequence | DE_Size8
  
DAT

rxCurrent     word  0   ' Current pointer for Rx functions
rxEnd         word  0   ' Real end of receive buffer

txBegin       word  0
txCurrent     word  0
txEnd         word  0

rxBuffer      byte  0[BUFFER_SIZE]

DAT
''
''
''==============================================================================
'' Buffer Support
''==============================================================================
''

PUB FillReceiveBuffer(sourceData, length)
  '' Copy incoming data to the internal receive buffer,
  '' and point to the beginning of the buffer.

  bytemove(rxCurrent := @rxBuffer, sourceData, length)
  rxEnd := rxCurrent + length

PUB SetTransmitBuffer(ptr, length)
  '' Prepare the transmit buffer

  txBegin := txCurrent := ptr
  txEnd := txCurrent + length

PUB TransmitLength
  '' Return the current length of buffered transmit data

  return txCurrent - txBegin

PRI Rx8
  ' Extract 8 bits from the receive buffer. Stops when we hit rxEndMarker

  if rxCurrent => rxEnd
    abort E_END_OF_BUFFER

  return BYTE[rxCurrent++]

PRI Rx16
  ' Extract 16 bits, in big-endian byte order, from the receive buffer.

  result := Rx8 << 8
  result |= Rx8

PRI Rx32
  ' Extract 32 bits, in big-endian byte order, from the receive buffer.

  result := Rx16 << 16
  result |= Rx16
  
PRI Tx8(x)
  ' Append 8 bits to the transmit buffer. Stops when we hit txEnd.
  ' If txSkip is nonzero, we count down skipped bytes instead of
  ' actually writing out anything.

  if txCurrent => txEnd
    abort E_END_OF_BUFFER
  BYTE[txCurrent++] := x

PRI Tx16(x)
  ' Append 16 bits to the transmit buffer.

  Tx8(x >> 8)
  Tx8(x)
  
PRI RxSkipDataElement | descriptor, size
  ' Skip a single generic Data Element.
  ' Since we ignore the request data, we don't actually
  ' need to understand data elements- but we need to skip
  ' them to make it all the way to the continuation data.

  if descriptor := Rx8
    case descriptor & DE_SIZE
      DE_1Byte:    size := 1
      DE_2Bytes:   size := 2
      DE_4Bytes:   size := 4
      DE_8Bytes:   size := 8
      DE_16Bytes:  size := 16
      DE_Size8:    size := Rx8
      DE_Size16:   size := Rx16
      DE_Size32:   size := Rx32
  else
    ' Nil is a special case
    size~

  rxCurrent += size

DAT
''
''
''==============================================================================
'' SDP Server
''==============================================================================
''

PUB HandlePDU | pduId, transactionId, size
  '' Handle one incoming Protocol Data Unit (PDU) and
  '' write an outgoing PDU to the transmit buffer.

  ' Receive the PDU header: PDU ID, Transaction ID, Length. 

  pduId := Rx8
  transactionId := Rx16
  size := Rx16
  rxEnd <#= rxCurrent + size

  ' There are three ways to request SDP data:
  ' SearchRequest, AttributeRequest, and SearchAttributeRequest.
  ' The latter combines the functionality of the first two.
  '
  ' Windows (Microsoft stack), Linux (BlueZ), and Mac OS all seem
  ' to use the SearchAttributeRequest exclusively. So, to save
  ' memory, that's the only transaction type we implement.

  if pduId == SDP_SearchAttrRequest
    Tx8(SDP_SearchAttrResponse)
    Tx16(transactionId)
    if 0 == \HandleSearchAttrRequest
      return

  ' An error occurred: Discard the transmit buffer, and generate an error PDU.

  txCurrent := txBegin
  Tx8(SDP_ErrorResponse)
  Tx16(transactionId)
  Tx16(2) ' Size
  Tx16(SE_BadSyntax) 

PRI HandleSearchAttrRequest | contIn, contOut, listSize, pduSize
  ' Handle an SDP ServiceSearchAttributeRequest PDU.
  ' This is equivalent to ServiceSearch and a ServiceAttribute
  ' combined into a single transaction. We're supposed to find
  ' all service records which match the UUID pattern we were given,
  ' then return any listed attributes from those service records.
  '
  ' But we have so little service data (and 99% of the time, clients
  ' want it all anyway) so we cut corners and always send all service
  ' data, regardless of what request actually came in.
  '
  ' The only real SDP protocol feature we support are continuations,
  ' so that we can send more than one MTU's worth of data back.

  ' "Parse" the incoming packet. All we care about is the
  ' continuation data. If it's present, we assume it's 16 bits.

  RxSkipDataElement   ' Skip UUID pattern
  Rx16                ' Skip maximum attribute byte count
  RxSkipDataElement   ' Skip attribute ID list
  if contIn := Rx8
    contIn := Rx16

  ' How much data do we need to send?
  ' If we don't have enough room for it all, take the continuation record's
  ' size into account too.

  listSize := (constant(@serviceDatabase_end - @serviceDatabase) - contIn) #> 0
  pduSize := listSize <# constant(BUFFER_SIZE - 10)
  if pduSize == listSize
    contOut~                  ' Last PDU in the response
  else
    listSize := pduSize    ' Not the last PDU in the response 
    pduSize += 2           ' Continuation data
    contOut := contIn + listSize

  ' Write the output packet

  Tx16(pduSize + 3)        ' Include size of listSize, list itself, and continuation
  Tx16(listSize)           ' Includes only list size

  bytemove(txCurrent, @serviceDatabase + contIn, listSize)
  txCurrent += listSize
  
  if contOut
    Tx8(2)
    Tx16(contOut)
  else
    Tx8(0)

DAT
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