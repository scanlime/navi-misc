{{

 usb-bluetooth  ver 0.1
──────────────────────────────────────────────────────────────────

USB Bluetooth HCI driver for the Parallax Propeller.

The latest version of this file should always be at:
http://svn.navi.cx/misc/trunk/propeller/usb-host/usb-bluetooth.spin

 ┌────────────────────────────────────────────────┐
 │ Copyright (c) 2010 Micah Dowty <micah@navi.cx> │               
 │ See end of file for terms of use.              │
 └────────────────────────────────────────────────┘

}}

OBJ
  hc : "usb-fs-host"

CON
  ' Negative error codes. Most functions in this library can call
  ' "abort" with one of these codes. The range from -100 to -150 is
  ' reserved for device drivers. (See usb-fs-host.spin)

  E_SUCCESS       = 0
  E_NOT_BLUETOOTH = -100        ' Not a Bluetooth USB device
  E_NO_ENDPOINT   = -101        ' Couldn't find all required endpoints
  E_INVALID_EVT   = -102        ' Malformed event header
  E_SHORT_EVT     = -103        ' Event response ended too soon
  E_UNEXPECT_EVT  = -104        ' Received an event that we didn't expect
  E_NO_RESPONSE   = -105        ' The device didn't respond to an HCI command
  E_CMD_MISMATCH  = -106        ' Command completion didn't match command
  E_CMD_FAIL      = -107        ' Command returned unsuccessful status

  ' Bluetooth "Class of Device" constants

  COD_Misc               = $000000
  COD_Computer           = $000100
  COD_Computer_Desktop   = $000104
  COD_Computer_Server    = $000108
  COD_Computer_Laptop    = $00010C
  COD_Computer_Handheld  = $000110
  COD_Computer_Palm      = $000114  
  COD_Phone              = $000200
  COD_Phone_Cellular     = $000204
  COD_Phone_Cordless     = $000208
  COD_Phone_Smart        = $00020C
  COD_Phone_Modem        = $000210  
  COD_LAN                = $000300
  COD_Audio              = $000400
  COD_Audio_Headset      = $000404
  COD_Peripheral         = $000500
  
  ' Bluetooth USB HCI class constants

  CLASS_WIRELESS   = $E0
  SUBCLASS_RF      = $01
  PROTOCOL_BT      = $01

  MAX_CMD_SIZE     = 256
  MAX_CMD_PARAMS   = MAX_CMD_SIZE - 3

  EVT_PACKET_SIZE  = 16
  MAX_EVT_SIZE     = 256
  MAX_EVT_PARAMS   = 256 - 2

  REQ_COMMAND      = $0020

  ' Automatic command retries

  CMD_TIMEOUT_MS   = 100
  CMD_RETRIES      = 50

  ' Performance counters

  PERF_CMD_SENT    = 0
  PERF_CMD_WAIT    = 1
  PERF_CMD_BYTES   = 2
  PERF_EVT_BYTES   = 3
  PERFMAX          = 4

DAT

perf                    long    0[PERFMAX]

intrIn                  word    0
bulkIn                  word    0
bulkOut                 word    0
evtPointer              word    0

' HCI Command Buffer

hciCommand
cmdOpcode               word    0
cmdNumParams            byte    0
cmdParams               byte    0[MAX_CMD_PARAMS]

' HCI Event Buffer

hciEvent
evtOpcode               byte    0
evtNumParams            byte    0
evtParams               byte    0[MAX_EVT_PARAMS]

DAT
''
''
''==============================================================================
'' Device Driver Interface
''==============================================================================

PUB Enumerate
  '' Enumerate the available USB devices. This is provided for the convenience
  '' of applications that use no other USB class drivers, so they don't have to
  '' directly import the host controller object as well.

  return hc.Enumerate

PUB Identify | ifp

  '' The caller must have already successfully enumerated a USB device.
  '' This function tests whether the device looks like it's compatible
  '' with this driver.
  ''
  '' This function is meant to be non-invasive: it doesn't do any setup,
  '' nor does it try to communicate with the device. If your application
  '' needs to be compatible with several USB device classes, you can call
  '' Identify on multiple drivers before committing to using any one of them.
  ''
  '' Returns 1 if the device is supported, 0 if not. Does not abort.

  ifp := hc.FindInterface(CLASS_WIRELESS)
  return ifp and BYTE[ifp + hc#IFDESC_bInterfaceSubClass] == SUBCLASS_RF and BYTE[ifp + hc#IFDESC_bInterfaceProtocol] == PROTOCOL_BT

PUB Init | epd

  '' (Re)initialize this driver. This must be called after Enumerate
  '' and Identify are both run successfully. All three functions must be
  '' called again if the device disconnects and reconnects, or if it is
  '' power-cycled.
  ''
  '' This function does communicate with the device, so it may abort with
  '' any driver or host controller error code.

  epd := hc.FindInterface(CLASS_WIRELESS)
  if not Identify
    abort E_NOT_BLUETOOTH
    
  ' Locate the device's bulk IN/OUT and interrupt IN endpoints

  bulkIn~
  bulkOut~
  intrIn~

  repeat while epd := hc.NextEndpoint(epd)
    case hc.EndpointType(epd)
      hc#TT_BULK:
        if hc.EndpointDirection(epd) == hc#DIR_IN
          bulkIn := epd
        else
          bulkOut := epd
      hc#TT_INTERRUPT:
        if hc.EndpointDirection(epd) == hc#DIR_IN
          intrIn := epd

  if not (bulkIn and bulkOut and intrIn)
    abort E_NO_ENDPOINT

  hc.Configure

DAT
''
''==============================================================================
'' Debugging Support
''==============================================================================

PUB GetPerfCounters
  '' Return a pointer to a buffer of performance counters,
  '' an array of longs with indices described by the PERF_*
  '' constants.

  return @perf

DAT
''
''==============================================================================
'' High-level Entry Points
''==============================================================================


PUB SetLocalName(str)
  '' Set the human-readable name of this Bluetooth device.
  '' This is what you'll see when scanning for this device on your
  '' computer or phone, if the device is discoverable.

  HCIcmd_Begin(CB_WriteLocalName)
  HCIcmd_String(str)
  HCIcmd_Wait

PUB SetClassOfDevice(class)
  '' Sets the "Class of Device" that we present to the outside
  '' world. This is what determines what "type" of device we're
  '' seen as. 'cod' is a 24-bit value that's constructed by OR'ing
  '' one or more COD_* constants.

  HCIcmd_Begin(CB_WriteClassOfDevice)
  HCIcmd_Word(class)
  HCIcmd_Byte(class >> 16)
  HCIcmd_Wait

PUB SetDiscoverable
  '' Make this device discoverable. It will appear when you scan
  '' for nearby Bluetooth devices on your computer or phone.

  HCIcmd_Begin(CB_WriteScanEnable)
  HCIcmd_Byte($03)
  HCIcmd_Wait

PUB BDAddress
  '' Read the adapter's Bluetooth Device Address.
  ''
  '' Returns a pointer to a 6-byte buffer containing
  '' the raw address. The buffer is only valid until the
  '' next event is received.

  HCIcmd_Begin(IP_ReadBDAddr)
  HCIcmd_Wait
  return HCIevt_Buffer

PUB BDAddressString | pIn, pOut, octet
  '' Read the adapter's Bluetooth Device Address,
  '' and format it as a human-readable string in the
  '' standard format ("XX:XX:XX:YY:YY:YY").
  ''
  '' Returns a pointer to a zero-terminated string.
  '' The returned buffer is only valid until the
  '' next command is issued.

  pIn := BDAddress + 5
  pOut := @hciCommand
  repeat 6
    octet := BYTE[pIn--] << 24
    repeat 2
         BYTE[pOut++] := lookupz((octet <-= 4) & $F : "0".."9", "A".."F")
    BYTE[pOut++] := ":"
  BYTE[--pOut] := 0
  return @hciCommand
  
DAT
''
''==============================================================================
'' HCI Command Interface
''==============================================================================

PUB HCIcmd_Begin(opcode)
  '' Begin a new HCI command, with the specified opcode. The command
  '' starts out with no parameters.

  cmdOpcode := opcode
  cmdNumParams~

PUB HCIcmd_Byte(b)
  '' Append a one-byte parameter to the current HCI command

  cmdParams[cmdNumParams++] := b

PUB HCIcmd_Word(w)
  '' Append a two-byte parameter to the current HCI command

  HCIcmd_Byte(w)
  HCIcmd_Byte(w >> 8)
  
PUB HCIcmd_String(str)
  '' Append a zero-terminated string to the current HCI command

  repeat strsize(str)
    HCIcmd_Byte(BYTE[str++])

PUB HCIcmd_Pad(len)
  '' Pad the current command with zeroes until the total
  '' number of parameter bytes are at least 'len'.

  repeat while cmdNumParams < len
    HCIcmd_Byte(0)

PUB HCIcmd_Send | bytes
  '' Send the current HCI command

  perf[PERF_CMD_SENT]++
  perf[PERF_CMD_BYTES] += bytes := 3 + cmdNumParams
  hc.ControlWrite(REQ_COMMAND, 0, 0, @hciCommand, bytes)

PUB HCIcmd_Wait | retries
  '' Send the current HCI command, and wait for a response.
  ''
  '' Handles events until a command completion arrives, or the
  '' command times out. If the command times out or an error occurs,
  '' automatically retries.
  ''
  '' If a completion event arrives, verifies that the command finished
  '' successfully. We consume the Command Completion header, and leave
  '' the event buffer pointing at the command's response.

  perf[PERF_CMD_WAIT]++
  retries := CMD_RETRIES
  repeat retries
    result := \HCIcmd_TryWait
    if result => 0
      return
  abort

PRI HCIcmd_TryWait | deadline
  ' One try in an HCIcmd_Wait.

  HCIcmd_Send

  case HCIevt_Wait(CMD_TIMEOUT_MS)

    EV_CommandComplete:
      ' Check and consume the event header

      ' Num_HCI_Command_Packets: Ignore. We don't pipeline commands.
      HCIevt_Byte

      ' Command opcode. This should match what we sent.
      if HCIevt_Word <> cmdOpcode
        abort E_CMD_MISMATCH
      
      ' The command status is technically not part of the
      ' event header, but it's common to all commands so let's
      ' consume and check it here too.
      if HCIevt_Byte <> 0
        abort E_CMD_FAIL

      ' Success!
      return

    0:
      abort E_NO_RESPONSE
    other:
      abort E_UNEXPECT_EVT


''
''==============================================================================
'' HCI Event Interface
''==============================================================================

PUB HCIevt_Wait(ms) | deadline
  '' Wait up to 'ms' milliseconds for any event.
  '' Returns an event opcode, or 0 if nothing was available.

  deadline := cnt + clkfreq/1000 * ms
  repeat while (cnt - deadline) < 0
    if result := HCIevt_Poll
      return  

PUB HCIevt_Poll | retval, length
  '' Check whether there is an HCI Event ready.
  '' If so, receive the whole event and return the event opcode.
  '' The parameters will be waiting in the event buffer.
  ''
  '' If no event is ready, returns 0.

  hc.FrameWait(1)
  evtPointer := @evtParams
  
  ' Poll for exactly one packet. This tells us how big the event is.
  retval := \hc.InterruptRead(intrIn, @hciEvent, EVT_PACKET_SIZE)
  if retval == hc#E_TIMEOUT
    return 0
  elseif retval < 0
    abort retval
  elseif retval < 2
    abort E_INVALID_EVT

  ' Now read the rest, if the event didn't fit entirely in one packet.
  ' This time we want to to a blocking read, and we have an exact
  ' upper bound on the number of bytes to read. (We're not relying on
  ' short packets to end the transfer)
  '
  ' So, oddly enough, now this looks just like a bulk transfer.
  ' Continue it as such.

  perf[PERF_EVT_BYTES] += length := evtNumParams + 2
  length -= retval
  if length > 0
    if hc.BulkRead(intrIn, @hciEvent + retval, length) <> length
      abort E_SHORT_EVT

  return evtOpcode  

PUB HCIevt_ParamSize
  '' Return the number of event parameter bytes remaining.
  '' This number will decrease as event parameters are consumed
  '' by the HCIevt_* functions.

  return evtNumParams

PUB HCIevt_Byte
  '' Consumes one byte from the event parameter list, and returns it.

  evtNumParams--
  return BYTE[evtPointer++] 

PUB HCIevt_Word
  '' Consumes a 16-bit word from the event parameter list, and returns it.

  result := HCIevt_Byte
  result |= HCIevt_Byte << 8

PUB HCIevt_Buffer
  '' Return a pointer to the current position in the event parameter buffer
  return evtPointer

   
CON

  ' Bluetooth HCI Commands

  LC_Inquiry                     = $0001 | ($01 << 10)  ' $0401
  LC_InquiryCancel               = $0002 | ($01 << 10)  ' $0402
  LC_PeriodicInquiryMode         = $0003 | ($01 << 10)  ' $0403
  LC_ExitPeriodicInquiryMode     = $0004 | ($01 << 10)  ' $0404
  LC_CreateConn                  = $0005 | ($01 << 10)  ' $0405
  LC_Disconnect                  = $0006 | ($01 << 10)  ' $0406
  LC_CreateConnCancel            = $0008 | ($01 << 10)  ' $0408
  LC_AcceptConnRequest           = $0009 | ($01 << 10)  ' $0409
  LC_RejectConnRequest           = $000a | ($01 << 10)  ' $040a
  LC_LinkKeyRequestReply         = $000b | ($01 << 10)  ' $040b
  LC_LinkKeyRequestNegativeReply = $000c | ($01 << 10)  ' $040c
  LC_PINCodeRequestReply         = $000d | ($01 << 10)  ' $040d
  LC_PINCodeRequestNegativeReply = $000e | ($01 << 10)  ' $040e
  LC_ChgConnPacketType           = $000f | ($01 << 10)  ' $040f
  LC_AuthenticationRequested     = $0011 | ($01 << 10)  ' $0411
  LC_SetConnEncryption           = $0013 | ($01 << 10)  ' $0413
  LC_ChgConnLinkKey              = $0015 | ($01 << 10)  ' $0415
  LC_MasterLinkKey               = $0017 | ($01 << 10)  ' $0417
  LC_RemNameRequest              = $0019 | ($01 << 10)  ' $0419
  LC_RemNameRequestCancel        = $001a | ($01 << 10)  ' $041a
  LC_ReadRemFeatures             = $001b | ($01 << 10)  ' $041b
  LC_ReadRemExtFeatures          = $001c | ($01 << 10)  ' $041c
  LC_ReadRemVersion              = $001d | ($01 << 10)  ' $041d
  LC_ReadClockOffset             = $001f | ($01 << 10)  ' $041f
  LC_ReadLMPHandle               = $0020 | ($01 << 10)  ' $0420
  LC_SetupSyncConn               = $0028 | ($01 << 10)  ' $0428
  LC_AcceptSyncConnRequest       = $0029 | ($01 << 10)  ' $0429
  LC_RejectSyncConnRequest       = $002a | ($01 << 10)  ' $042a
  LP_HoldMode                    = $0001 | ($02 << 10)  ' $0801
  LP_SniffMode                   = $0003 | ($02 << 10)  ' $0803
  LP_ExitSniffMode               = $0004 | ($02 << 10)  ' $0804
  LP_ParkState                   = $0005 | ($02 << 10)  ' $0805
  LP_ExitParkState               = $0006 | ($02 << 10)  ' $0806
  LP_QoSSetup                    = $0007 | ($02 << 10)  ' $0807
  LP_RoleDiscovery               = $0009 | ($02 << 10)  ' $0809
  LP_SwitchRole                  = $000b | ($02 << 10)  ' $080b
  LP_ReadLPSettings              = $000c | ($02 << 10)  ' $080c
  LP_WriteLPSettings             = $000d | ($02 << 10)  ' $080d
  LP_ReadDefaultLPSettings       = $000e | ($02 << 10)  ' $080e
  LP_WriteDefaultLPSettings      = $000f | ($02 << 10)  ' $080f
  LP_FlowSpec                    = $0010 | ($02 << 10)  ' $0810
  CB_SetEventMask                = $0001 | ($03 << 10)  ' $0c01
  CB_Reset                       = $0003 | ($03 << 10)  ' $0c03
  CB_SetEventFilter              = $0005 | ($03 << 10)  ' $0c05
  CB_Flush                       = $0008 | ($03 << 10)  ' $0c08
  CB_ReadPINType                 = $0009 | ($03 << 10)  ' $0c09
  CB_WritePINType                = $000a | ($03 << 10)  ' $0c0a
  CB_CreateNewUnitKey            = $000b | ($03 << 10)  ' $0c0b
  CB_ReadStoredLinkKey           = $000d | ($03 << 10)  ' $0c0d
  CB_WriteStoredLinkKey          = $0011 | ($03 << 10)  ' $0c11
  CB_DeleteStoredLinkKey         = $0012 | ($03 << 10)  ' $0c12
  CB_WriteLocalName              = $0013 | ($03 << 10)  ' $0c13
  CB_ReadLocalName               = $0014 | ($03 << 10)  ' $0c14
  CB_ReadConnAcceptTimeout       = $0015 | ($03 << 10)  ' $0c15
  CB_WriteConnAcceptTimeout      = $0016 | ($03 << 10)  ' $0c16
  CB_ReadPageTimeout             = $0017 | ($03 << 10)  ' $0c17
  CB_WritePageTimeout            = $0018 | ($03 << 10)  ' $0c18
  CB_ReadScanEnable              = $0019 | ($03 << 10)  ' $0c19
  CB_WriteScanEnable             = $001a | ($03 << 10)  ' $0c1a
  CB_ReadPageScanActivity        = $001b | ($03 << 10)  ' $0c1b
  CB_WritePageScanActivity       = $001c | ($03 << 10)  ' $0c1c
  CB_ReadInquiryScanActivity     = $001d | ($03 << 10)  ' $0c1d
  CB_WriteInquiryScanActivity    = $001e | ($03 << 10)  ' $0c1e
  CB_ReadAuthenticationEnable    = $001f | ($03 << 10)  ' $0c1f
  CB_WriteAuthenticationEnable   = $0020 | ($03 << 10)  ' $0c20
  CB_ReadEncryptionMode          = $0021 | ($03 << 10)  ' $0c21
  CB_WriteEncryptionMode         = $0022 | ($03 << 10)  ' $0c22
  CB_ReadClassOfDevice           = $0023 | ($03 << 10)  ' $0c23
  CB_WriteClassOfDevice          = $0024 | ($03 << 10)  ' $0c24
  CB_ReadVoiceSetting            = $0025 | ($03 << 10)  ' $0c25
  CB_WriteVoiceSetting           = $0026 | ($03 << 10)  ' $0c26
  CB_ReadAutomaticFlushTimeout   = $0027 | ($03 << 10)  ' $0c27
  CB_WriteAutomaticFlushTimeout  = $0028 | ($03 << 10)  ' $0c28
  CB_ReadNumBroadcastTrans       = $0029 | ($03 << 10)  ' $0c29
  CB_WriteNumBroadcastTrans      = $002a | ($03 << 10)  ' $0c2a
  CB_ReadHoldModeActivity        = $002b | ($03 << 10)  ' $0c2b
  CB_WriteHoldModeActivity       = $002c | ($03 << 10)  ' $0c2c
  CB_ReadTransmitPowerLevel      = $002d | ($03 << 10)  ' $0c2d
  CB_ReadSyncFlowControlEnable   = $002e | ($03 << 10)  ' $0c2e
  CB_WriteSyncFlowControlEnable  = $002f | ($03 << 10)  ' $0c2f
  CB_SetCtrlToHostFlowControl    = $0031 | ($03 << 10)  ' $0c31
  CB_HostBufferSize              = $0033 | ($03 << 10)  ' $0c33
  CB_HostNumCompletedPackets     = $0035 | ($03 << 10)  ' $0c35
  CB_ReadLinkSupervisionTimeout  = $0036 | ($03 << 10)  ' $0c36
  CB_WriteLinkSupervisionTimeout = $0037 | ($03 << 10)  ' $0c37
  CB_ReadNumSupportedIAC         = $0038 | ($03 << 10)  ' $0c38
  CB_ReadCurrentIACLAP           = $0039 | ($03 << 10)  ' $0c39
  CB_WriteCurrentIACLAP          = $003a | ($03 << 10)  ' $0c3a
  CB_ReadPageScanPeriodMode      = $003b | ($03 << 10)  ' $0c3b
  CB_WritePageScanPeriodMode     = $003c | ($03 << 10)  ' $0c3c
  CB_SetAFHHostChannelClass      = $003f | ($03 << 10)  ' $0c3f
  CB_ReadInquiryScanType         = $0042 | ($03 << 10)  ' $0c42
  CB_WriteInquiryScanType        = $0043 | ($03 << 10)  ' $0c43
  CB_ReadInquiryMode             = $0044 | ($03 << 10)  ' $0c44
  CB_WriteInquiryMode            = $0045 | ($03 << 10)  ' $0c45
  CB_ReadPageScanType            = $0046 | ($03 << 10)  ' $0c46
  CB_WritePageScanType           = $0047 | ($03 << 10)  ' $0c47
  CB_ReadAFHChannelAssignMode    = $0048 | ($03 << 10)  ' $0c48
  CB_WriteAFHChannelAssignMode   = $0049 | ($03 << 10)  ' $0c49
  IP_ReadLocalVersion            = $0001 | ($04 << 10)  ' $1001
  IP_ReadLocalSupportedCommands  = $0002 | ($04 << 10)  ' $1002
  IP_ReadLocalFeatures           = $0003 | ($04 << 10)  ' $1003
  IP_ReadLocalExtFeatures        = $0004 | ($04 << 10)  ' $1004
  IP_ReadBufferSize              = $0005 | ($04 << 10)  ' $1005
  IP_ReadBDAddr                  = $0009 | ($04 << 10)  ' $1009
  SP_ReadFailedContactCounter    = $0001 | ($05 << 10)  ' $1401
  SP_ResetFailedContactCounter   = $0002 | ($05 << 10)  ' $1402
  SP_ReadLinkQuality             = $0003 | ($05 << 10)  ' $1403
  SP_ReadRSSI                    = $0005 | ($05 << 10)  ' $1405
  SP_ReadAFHChannelMap           = $0006 | ($05 << 10)  ' $1406
  SP_ReadClock                   = $0007 | ($05 << 10)  ' $1407
  Test_ReadLoopbackMode          = $0001 | ($06 << 10)  ' $1801
  Test_WriteLoopbackMode         = $0002 | ($06 << 10)  ' $1802
  Test_EnableDeviceUnderTestMode = $0003 | ($06 << 10)  ' $1803

  ' Bluetooth HCI Events

  EV_InquiryComplete             = $01
  EV_InquiryResult               = $02
  EV_ConnComplete                = $03
  EV_ConnRequest                 = $04
  EV_DisconnectionComplete       = $05
  EV_AuthenticationComplete      = $06
  EV_RemNameRequestComplete      = $07
  EV_EncryptionChg               = $08
  EV_ChgConnLinkKeyComplete      = $09
  EV_MasterLinkKeyComplete       = $0a
  EV_ReadRemFeaturesComplete     = $0b
  EV_ReadRemVersionComplete      = $0c
  EV_QoSSetupComplete            = $0d
  EV_CommandComplete             = $0e
  EV_CommandStatus               = $0f
  EV_HardwareError               = $10
  EV_FlushOccurred               = $11
  EV_RoleChg                     = $12
  EV_NumberofCompletedPackets    = $13
  EV_ModeChg                     = $14
  EV_ReturnLinkKeys              = $15
  EV_PINCodeRequest              = $16
  EV_LinkKeyRequest              = $17
  EV_LinkKeyNotification         = $18
  EV_LoopbackCommand             = $19
  EV_DataBufferOverflow          = $1a
  EV_MaxSlotsChg                 = $1b
  EV_ReadClockOffsetComplete     = $1c
  EV_ConnPacketTypeChgd          = $1d
  EV_QoSViolation                = $1e
  EV_PageScanRepetitionModeChg   = $20
  EV_HCIFlowSpecComplete         = $21
  EV_InquiryResultWithRSSI       = $22
  EV_ReadRemExtFeaturesComplete  = $23
  EV_SyncConnComplete            = $2c
  EV_SyncConnChgd                = $2d

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