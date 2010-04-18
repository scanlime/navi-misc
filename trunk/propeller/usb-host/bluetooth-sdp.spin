{{

 bluetooth-sdp  ver 0.1
──────────────────────────────────────────────────────────────────

Service Discovery Protocol support for the Propeller Bluetooth stack.

The latest version of this file should always be at:
http://svn.navi.cx/misc/trunk/propeller/usb-host/bluetooth-sdp.spin

 ┌────────────────────────────────────────────────┐
 │ Copyright (c) 2010 Micah Dowty <micah@navi.cx> │               
 │ See end of file for terms of use.              │
 └────────────────────────────────────────────────┘

}}

CON

  ' Local error codes

  E_END_OF_BUFFER   = -1
  E_DE_UNSUPPORTED  = -2   ' Unsupported type in RxDataElement

  ' Limits maximum PDU size.
  ' (Currently only needs to be as big as our L2CAP MTU)

  BUFFER_SIZE       = 64 - 8

  ' Limit the number of services found in a ServiceSearchRequest.
  ' For all practical purposes, this also limits the number of
  ' supported services.

  MAX_SERVICES      = 8

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
  DE_Sequence8    = DE_Sequence | DE_Size8
  
DAT

transaction   word  0
uuidPattern   word  0   ' Pointer to the current UUID search pattern
attrIDList    word  0   ' Pointer to the current attribute ID list
continuation  word  0   ' Continuation state

rxCurrent     word  0   ' Current pointer for Rx functions
rxEnd         word  0   ' Real end of receive buffer

txBegin       word  0
txCurrent     word  0
txEnd         word  0
txSkip        word  0   ' Number of transmit bytes to skip

svcCount      word  0   ' Number of services found
svcList       word  0[MAX_SERVICES]

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
  result := Rx16

PRI Tx8(x)
  ' Append 8 bits to the transmit buffer. Stops when we hit txEnd.
  ' If txSkip is nonzero, we count down skipped bytes instead of
  ' actually writing out anything.

  if txSkip
    txSkip--
  else
    if txCurrent => txEnd
      abort E_END_OF_BUFFER
    BYTE[txCurrent++] := x

PRI Tx16(x)
  ' Append 16 bits to the transmit buffer.

  Tx8(x >> 8)
  Tx8(x)

PRI Tx32(x)
  ' Append 32 bits to the transmit buffer

  Tx16(x >> 16)
  Tx16(x)

PRI RxDataElementSize(descriptor) : size
  ' Read the size for a given data element. If the size
  ' is part of the descriptor, return that. Otherwise,
  ' read it from the receive buffer.
  
  if descriptor
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

PRI RxSizedValue(size) : value | endMarker
  ' Read a value between 0 and 4 bytes. Longer sizes will
  ' read the first 32 bits.

  endMarker := rxCurrent + size

  case size
    0:      value~
    1:      value := Rx8
    2,3:    value := Rx16
    other:  value := Rx32
  
  rxCurrent := endMarker 

PRI RxDataElement : value | descriptor, size
  ' Read a generic numerical Data Element.
  '
  ' Returns the result as a 32-bit number. Signed numbers will be
  ' sign-extended. 128-bit UUIDs will be truncated, only the high
  ' 32 bits will be returned (we don't validate the low 96 bits
  ' against the Bluetooth Base UUID.)
  '
  ' Lists won't return a meaningful value, but they will
  ' be skipped.
                                              
  size := RxDataElementSize(descriptor := Rx8)
  value := RxSizedValue(size)

  if (descriptor & DE_TYPE) == DE_Signed
    case size
      1: ~value
      2: ~~value

PRI RxEnterSequence : endMarker | descriptor
  ' Read a Data Element. Verify that it's a sequence.
  ' If so, return a pointer to the first byte after the end of the
  ' sequence, and advance rxCurrent to the first byte of the sequence's contents.
  '
  ' (Whereas RxDataElement would skip a sequence, this traverses into it.)

  endMarker := RxDataElementSize(descriptor := Rx8)
  endMarker += rxCurrent
  
  case descriptor & DE_TYPE

    DE_Sequence, DE_Alternative:
      return

    other:
      abort E_DE_UNSUPPORTED

PRI RxContinuation
  ' Read a continuation value, if we have one.
  ' If present, it's always 16 bits.
  '
  ' This also signfies the end of the receive packet,
  ' so we disable bounds checking in the Rx code. This
  ' is required so we can use the same code to read
  ' data elements from the service database.

  if Rx8
    continuation := Rx16
  else
    continuation~

  rxEnd := $FFFF

PRI TxContinuation
  ' Send a continuation value

  if continuation
    Tx8(2)
    Tx16(continuation)
  else
    Tx8(0)
  
DAT
''
''
''==============================================================================
'' SDP Server
''==============================================================================
''

PUB HandlePDU | pduId, size
  '' Handle one incoming Protocol Data Unit (PDU) and
  '' write an outgoing PDU to the transmit buffer.

  ' Receive the PDU header: PDU ID, Transaction ID, Length. 

  pduId := Rx8
  transaction := Rx16
  size := Rx16
  rxEnd <#= rxCurrent + size
               
  case pduId

    ' There are three ways to request SDP data:
    ' SearchRequest, AttributeRequest, and SearchAttributeRequest.
    ' The latter combines the functionality of the first two.
    '
    ' Windows (Microsoft stack), Linux (BlueZ), and Mac OS all seem
    ' to use the SearchAttributeRequest exclusively. I did find
    ' one debug tool that uses SearchRequest, so I've implemented
    ' that- but I haven't implemented AttributeRequest yet because
    ' I haven't found a way to test it.
    '
    ' If you're short on space, you can probably remove the
    ' SearchRequest implementation.

    SDP_SearchRequest:
      if \HandleSearchRequest
        SendErrorResponse(SE_BadSyntax)

    SDP_SearchAttrRequest:
      if \HandleSearchAttrRequest
        SendErrorResponse(SE_BadSyntax)

    other:
      SendErrorResponse(SE_BadSyntax)

PRI SendErrorResponse(errorCode)
  ' Discard the current contents of the transmit buffer,
  ' and fill it with an error response packet.
  
  txCurrent := txBegin
  Tx8(SDP_ErrorResponse)
  Tx16(transaction)
  Tx16(2) ' Size
  Tx16(errorCode)

PRI HandleSearchRequest | i
  ' Handle an SDP ServiceSearchRequest PDU.
  '
  ' NOTE: This request is not commonly used. The only tool I could find
  '       that would issue it is test/sdptest.c in BlueZ 4.51. So if you're
  '       running low on space, you can probably delete it.

  ' Parse the incoming packet, saving our UUID pattern.

  uuidPattern := rxCurrent
  RxDataElement
  Rx16             ' Ignore maximum attribute byte count
  RxContinuation

  ' Search for servies that match the UUID pattern
  ServiceSearch

  ' Send the response. We currently require that this fits in
  ' one PDU. We support few enough services that this should
  ' be fine.

  Tx8(SDP_SearchResponse)
  Tx16(transaction)
  Tx16(5 + (svcCount << 2))   ' PDU Size
  Tx16(svcCount)              ' Total record count
  Tx16(svcCount)              ' Current record count

  repeat i from 0 to svcCount - 1
    Tx32(svcList[i])          ' Send each 32-bit service ID

  continuation~               ' Never need a continuation
  TxContinuation

PRI HandleSearchAttrRequest | i, listSize, totalSize, packetSize
  ' Handle an SDP ServiceSearchAttributeRequest PDU.
  ' This is equivalent to ServiceSearch and a ServiceAttribute
  ' combined into a single transaction.

  ' Parse the incoming packet.
  ' We save the UUID pattern and attribute list for later,
  ' so we can use them when searching our database.

  uuidPattern := rxCurrent
  RxDataElement
  Rx16             ' Ignore maximum attribute byte count
  attrIDList := rxCurrent
  RXDataElement
  RxContinuation
   
  ' Search for servies that match the UUID pattern
  ServiceSearch

  ' How big is the full response? Measure the length of each
  ' attribute list, without actually writing it to the packet.
  ' This will tell us the payload size for the outermost list.

  txSkip := $FFFF
  repeat i from 0 to svcCount - 1
    'AttributeSearch(svcList[i])
  listSize := $FFFF - txSkip

   
  ' Send a smaller chunk of data in this PDU.

  'packetSize := listSize #<
                           
  ' Send back an attribute list
  Tx8(SDP_SearchAttrResponse)
  Tx16(transaction)
  
  'AttributeSearch

  
  '
  ' XXX: Implement some sensible way of storing the
  '      service discovery data, and filtering it by
  '      UUID and by attribute ID.
  '
  '      Alternatively, is it legal for us to just
  '      unconditionally send all the info we have?
  '      Seems like this could work for ServiceSearchAttrRequest,
  '      but not for the other SDP request types.
  '
  '      Info about each service record:
  '        ID
  '        UUIDs
  '        Attribute list
  '
  '      ID and UUIDs will also occur inside the attribute list.
  '      We can standardize on including the ID first, and UUIDs
  '      can be located anywhere in the service record.


  'Tx16(@attrList_end - @attrList_begin + 2 + 1) 
  
  'Tx16(@attrList_end - @attrList_begin)
  'repeat ptr from @attrList_begin to @attrList_end - 1
  '  Tx8(BYTE[ptr])
  'Tx8(0)  ' Empty continuation state
  
  
DAT
''
''
''==============================================================================
'' SDP Service Database
''==============================================================================
''

' Services are stored in a linked list. Each service begins with a word
' which points to the next service. After that word is a NIL-terminated
' list of Data Elements which are alternating attributes and values.
' The ServiceRecordHandle must always be the first attribute in the list.
'
' We initially populate the service database with a record for the SDP
' service itself. Other modules can advertise their own services by adding
' them to this linked list.
'
' To debug service records, try using BlueZ's "sdptool" to retrieve and dump
' them. The --raw option will show the raw identifiers that you can use in
' this table.

service_database
        word  0                                   ' Next Service

        byte  DE_Uint16, $00,$00                  ' ServiceRecordHandle
        byte    DE_Uint16, "SD"                   '   (Arbitrary unique value)

        byte  DE_Uint16, $00,$01                  ' ServiceClassIDList
        byte    DE_Sequence8, @tail1 - @head1     '   <sequence>
head1   byte      DE_UUID16, $10,$00              '     ServiceDiscoveryServerServiceClassID
tail1

        byte  DE_Uint16, $00,$04                  ' ProtocolDescriptorList
        byte    DE_Sequence8, @tail2 - @head2     '   <sequence>
head2   byte      DE_Sequence8, @tail3 - @head3   '     <sequence>
head3   byte        DE_UUID16, $01,$00            '       L2CAP
        byte        DE_Uint16, $00,$01            '       Standard L2CAP PSM for SDP
tail3   byte      DE_Sequence8, @tail4 - @head4   '     <sequence>
head4   byte        DE_UUID16, $00,$01            '       SDP
tail4
tail2

        byte  DE_Uint16, $00,$05                  ' BrowseGroupList
        byte    DE_Sequence8, @tail5 - @head5     '   <sequence>
head5   byte      DE_UUID16, $10,$02              '     PublicBrowseGroup
tail5

        byte  DE_Nil

PUB AddService(service)
  '' Link a new service into the service database. The service data
  '' must be in a very specific format- see the example SDP service
  '' record in this object.

  WORD[service] := WORD[@service_database]
  WORD[@service_database] := service
  
PRI GetServiceHandle(service)
  ' Given a pointer to the service database record for a service.
  ' Overwrites rxCurrent.

  rxCurrent := service + 2
  RxDataElement         ' Service ID
  return RxDataElement
  
PRI ServiceSearch | service, handle
  ' Locate all services that match every UUID in the uuidPattern.
  ' The resulting services will be in the svcList.

  service := @service_database
  svcCount~

  repeat while service
    handle := GetServiceHandle(service)
    if ServiceMatchesUUIDPattern(service)
      svcList[svcCount++] := handle      
    service := WORD[service]

PRI ServiceMatchesUUIDPattern(service) | pattern, end, uuid
  ' Test whether a single service matches every UUID in the uuidPattern

  rxCurrent := uuidPattern
  end := RxEnterSequence
  pattern := rxCurrent

  repeat while pattern < end
    rxCurrent := pattern
    uuid := RxDataElement
    pattern := rxCurrent

    if not ServiceContainsUUID(service, uuid)
      return 0

  return 1

PRI ServiceContainsUUID(service, uuid) | descriptor, size, value
  ' Test whether the service contains the specified UUID,
  ' searching inside each sequence.

  rxCurrent := service + 2

  repeat
    size := RxDataElementSize(descriptor := Rx8)
    case descriptor & DE_TYPE

      DE_Nil:
        return 0

      DE_Sequence:
        ' Enter the sequence

      DE_UUID:
        if uuid == RxSizedValue(size)
          return 1

      other:
        RxSizedValue(size)

  return 0

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