{{
  Low-level interface to the Unicone2 Playstation controller remote
  unit. The remote unit communicates over an asymmetric RS-422 serial
  link: A 19200-baud channel transmits LED and force-feedback info
  to the remote, and a 250kbps channel transmits controller data
  back from the remote unit.

  For transmitting LED and force feedback data, this object provides
  blocking methods written in Spin. For receiving controller data,
  this object has an assembly language driver which updates controller
  state information in shared memory.
  
  This module requires at least an 80 MHz system clock. 

  -- Micah Dowty <micah@navi.cx>
}}

CON
  NUM_SLOTS = 2

  TX_BAUD = 19200
  RX_BAUD = 250000

  ' Short watchdog timeout, in milliseconds.
  ' If we don't receive a byte in this amount of time,
  ' we restart at the beginning of the packet.
  WDT_TIMEOUT_MS = 5

  ' Long watchdog timeout, in milliseconds.
  ' If we don't receive a valid packet in this amount of
  ' time, reset the entire receive buffer. This lets us
  ' recover from transient glitches, and it lets us detect
  ' when the remote unit disconnects.
  LONG_WDT_TIMEOUT_MS = 100 

  RX_MAX_PACKET_LEN = 20

  ' Special bytes in the TX protocol
  TX_START_OF_PACKET = $5A
  TX_ESCAPE = $FE
  
  ' Offsets within the TX packet
  TX_LEDS = 0
  TX_FF_1 = 1
  TX_FF_2 = 2
  TX_SLOT_LEN = 3

  TX_PACKET_LEN = TX_SLOT_LEN * NUM_SLOTS

  ' Controller state flags
  CSTATE_CONNECTED = $01
  CSTATE_ANALOG = $02
  CSTATE_DUALSHOCK = $04
  CSTATE_POWERDOWN = $08

  ' Controller buttons at WORD[get_controller_data]
  BTN_SELECT = $0001
  BTN_L3 = $0002
  BTN_R3 = $0004
  BTN_START = $0008
  BTN_UP = $0010
  BTN_RIGHT = $0020
  BTN_DOWN = $0040
  BTN_LEFT = $0080
  BTN_L2 = $0100
  BTN_R2 = $0200
  BTN_L1 = $0400
  BTN_R1 = $0800
  BTN_TRIANGLE = $1000
  BTN_CIRCLE = $2000
  BTN_X = $4000
  BTN_SQUARE = $8000

  ' Controller axes in BYTE[get_controller_data + axis]
  AXIS_RX = 2
  AXIS_RY = 3
  AXIS_LX = 4
  AXIS_LY = 5

  ' Pressure-sensitive buttons in BYTE[get_controller_data + button]
  PBTN_R2 = 6
  PBTN_L2 = 7
  PBTN_R1 = 8
  PBTN_L1 = 9
  PBTN_SQUARE = 10
  PBTN_X = 11
  PBTN_CIRCLE = 12
  PBTN_TRIANGLE = 13
  PBTN_DOWN = 14
  PBTN_UP = 15
  PBTN_LEFT = 16
  PBTN_RIGHT = 17

  ' LED special effect flags (From 0x20 to 0x80)
  LED_EFFECT_BLINK = $20
  LED_EFFECT_SPIN = $40
  
  ' Special LED characters (From 0x10 to 0x1F)
  LED_CHAR_BLANK = $10
  LED_CHAR_DASH = $11
  
  
OBJ
  tx : "Simple_Serial"


VAR

  long  rx_cog

  ' Communication area for RX driver startup
  long  rx_pin
  long  rx_bit_ticks
  long  rx_buffer_ptr
  long  wdt_timeout_ticks
  long  new_packet_flag

  long  last_packet_timestamp
  long  long_wdt_timeout_ticks
  byte  tx_buffer[TX_PACKET_LEN]
  byte  rx_buffers[RX_MAX_PACKET_LEN * NUM_SLOTS]
  byte  led_chars[NUM_SLOTS]


PUB start(rxpin, txpin) : okay

  '' Initializes the remote port. Starts a cog.

  bytefill(@tx_buffer, 0, TX_PACKET_LEN)
  reset_rx_packets
  tx.start(-1, txpin, TX_BAUD)

  wdt_timeout_ticks := (clkfreq / 1000) * WDT_TIMEOUT_MS
  long_wdt_timeout_ticks := (clkfreq / 1000) * LONG_WDT_TIMEOUT_MS
  
  rx_pin := rxpin
  rx_bit_ticks := clkfreq / RX_BAUD
  rx_buffer_ptr := @rx_buffers
  new_packet_flag := 0
  okay := rx_cog := cognew(@entry, @rx_pin) + 1


PUB stop

  '' Frees a cog.

  tx.stop

  if rx_cog
    cogstop(rx_cog~ - 1)


PUB poll

  '' Perform slow polling tasks which aren't handled by our dedicated cog.
  '' This includes transmitting updated LED and force-feedback state, as well
  '' as trimming incoming packets. (Resetting unused portions of the receive
  '' buffer to their default values).

  update_leds
  tx_packet
  trim_rx_packets


PUB get_buttons(slot) : buttons

  '' Returns a 16-bit mask of pressed buttons on the specified slot.

  buttons := $FFFF ^ WORD[get_state_buffer(slot)]

  
PUB set_led_char(slot, char)

  '' Set the character to display on a slot's LED display.
  '' The character can be a hexadecimal digit, or it can be
  '' one of the LED_CHAR_* constants. Additionally, it may
  '' have LED_EFFECT_* flags set.

  led_chars[slot] := char


PUB set_actuator(slot, actuator, value)

  '' Set the value of a force feedback actuator on a particular slot.
  '' The meaning of the value byte is actuator-dependent. Playstation
  '' Dual Shock controllers support two actuators.

  tx_buffer[slot * TX_SLOT_LEN + TX_FF_1 + actuator] := value
  

PUB get_actuator_buffer(slot) : address

  '' Retrieve the address of a controller slot's raw actuator buffer.

  address := @tx_buffer[slot * TX_SLOT_LEN + TX_FF_1]

  
PUB get_state_buffer(slot) : address

  '' Retrieve the address of a controller slot's raw polling results

  address := @rx_buffers[RX_MAX_PACKET_LEN * slot + 2]


PUB get_controller_flags(slot) : state_flags

  '' Returns state flags indicating the controller's hardware capabilities,
  '' and even whether a controller is connected at all. The return value
  '' consists of one or more CSTATE_* flags.

  state_flags := rx_buffers[RX_MAX_PACKET_LEN * slot + 1]


PRI is_new_packet : new

  '' Return 1 if a new packet has arrived since the last call
    
  new := new_packet_flag
  new_packet_flag := 0


PRI update_leds | slot, leds, char

  ' Recalculate values for TX_LEDS based on led_chars.

  ' "leds" is the raw LED state passed on to the remote unit.
  ' Bit positions:
  '        
  '       5
  '      ---
  '    6| 7 |4
  '      ---
  '    0|   |2
  '      ---  
  '       1
  '
  ' (The dot cannot be controlled remotely. Bit 3 is ignored.)

  repeat slot from 0 to NUM_SLOTS-1
    char := led_chars[slot]

    if char & LED_EFFECT_SPIN
      leds := BYTE[@led_spin_table + (cnt >> 22) // 6]
    else
      leds := BYTE[@led_char_table + (char & $1F)]

    if (char & LED_EFFECT_BLINK) and ((cnt >> 24) & 1)
      leds := 0

    tx_buffer[slot * TX_SLOT_LEN + TX_LEDS] := leds


PRI get_rx_packet_len(flags) : length

  '' Given a controller's state flags, return the expected number of
  '' bytes (after the 2-byte header) to be stored for that controller.

  if flags & CSTATE_CONNECTED
    length += 2

  if flags & CSTATE_ANALOG
    length += 4

  if flags & CSTATE_DUALSHOCK
    length += 12

    
PRI trim_rx_packets | slot, addr, packet_len

  ' Reset unused portions of the receive buffers to our default values.
  ' This lets other objects use values in the receive buffer without
  ' first checking whether they're applicable to the attached controller
  ' type.
  '
  ' This function also implements the long watchdog timer. If we haven't
  ' seen a new packet in LONG_WDT_TIMEOUT_MS, reset the entire receive buffer.

  if is_new_packet
    last_packet_timestamp := cnt

  if (cnt - last_packet_timestamp) > long_wdt_timeout_ticks
    reset_rx_packets
                                                                   
  addr := @rx_buffers
  repeat slot from 0 to NUM_SLOTS-1
    packet_len := get_rx_packet_len(BYTE[addr + 1]) + 2    
    bytemove(addr + packet_len, @default_rx_data + packet_len, RX_MAX_PACKET_LEN - packet_len)
    addr += RX_MAX_PACKET_LEN


PRI reset_rx_packets

  ' Initialize rx_buffers with our default_rx_data. This is called once during
  ' init, and it's called again by trim_rx_packets any time the long WDT expires.

  bytemove(@rx_buffers, @default_rx_data, RX_MAX_PACKET_LEN)
  bytemove(@rx_buffers[RX_MAX_PACKET_LEN], @default_rx_data, RX_MAX_PACKET_LEN)

    
PRI tx_packet | i, value

  ' Send an outgoing packet to the remote. This function must be called
  ' for any changes to LED or force feedback state to take effect. If it
  ' is not called at least once a second or so, the remote unit's watchdog
  ' timer will cause all LEDs and force feedback actuators to be turned off.

  tx.tx(TX_START_OF_PACKET)

  repeat i from 0 to TX_PACKET_LEN - 1
    value := tx_buffer[i]
    if value == TX_START_OF_PACKET or value == TX_ESCAPE
      tx.tx(TX_ESCAPE)
    tx.tx(value)
  
  
DAT

        '------------------------------------------------------
        ' LED Character Table
        
led_char_table          byte    $77             ' 0
                        byte    $14             ' 1
                        byte    $B3             ' 2
                        byte    $B6             ' 3
                        byte    $D4             ' 4
                        byte    $E6             ' 5
                        byte    $E7             ' 6
                        byte    $34             ' 7
                        byte    $F7             ' 8
                        byte    $F4             ' 9
                        byte    $F5             ' A
                        byte    $C7             ' B
                        byte    $63             ' C
                        byte    $97             ' D
                        byte    $E3             ' E
                        byte    $E1             ' F
                        byte    $00             ' 10: Blank
                        byte    $80             ' 11: Dash
                        byte    $00             ' 12: (unused)
                        byte    $00             ' 13: (unused)
                        byte    $00             ' 14: (unused)
                        byte    $00             ' 15: (unused)
                        byte    $00             ' 16: (unused)
                        byte    $00             ' 17: (unused)
                        byte    $00             ' 18: (unused)
                        byte    $00             ' 19: (unused)
                        byte    $00             ' 1A: (unused)
                        byte    $00             ' 1B: (unused)
                        byte    $00             ' 1C: (unused)
                        byte    $00             ' 1D: (unused)
                        byte    $00             ' 1E: (unused)
                        byte    $00             ' 1F: (unused)

led_spin_table          byte    $40
                        byte    $20
                        byte    $10
                        byte    $04
                        byte    $02
                        byte    $01
        
                        org

        '------------------------------------------------------
        ' Entry point.

entry                   mov     t1, par                 ' Make local copies of all parameters

                        rdlong  t2, t1
                        mov     rx_mask, #1
                        shl     rx_mask, t2

                        add     t1, #4
                        rdlong  rx_bit_ticks_, t1

                        add     t1, #4
                        rdlong  rx_buffer_ptr_, t1

                        add     t1, #4
                        rdlong  wdt_timeout_ticks_, t1

                        mov     wdt_timestamp, cnt      ' Init the watchdog timestamp

receive_packet
                        call    #restart_packet         ' Init the packet state
                        

        '------------------------------------------------------
        ' Receive a byte.
        '
        ' This will wait indefinitely for a new byte to arrive
        ' successfully. When finished, a valid byte will be in rx_data.
        '
        ' For each bit, we do a simple majority-detect in order
        ' to be more resistant to transient noise.
        '
        ' At 250 kbps and 80 MHz, we have 80 4-cycle instructions
        ' per bit.  This implementation takes 9 samples. If at least
        ' 4 of them are HIGH, the resulting bit is HIGH.

receive_byte
                        mov     rx_data, #0             ' Clear rx_data
                        mov     rx_bits, #10            ' Prepare to receive start bit, 8 data bits, and stop bit.
                        mov     next_bit, rx_bit_ticks_

                        mov     t1, #0                  ' Wait for falling edge (start bit)
                        waitpeq t1, rx_mask
                        add     next_bit, cnt
                        
:bit
                        mov     rx_accum, #0

                        test    rx_mask, ina wc         ' Sample 1
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 2
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 3
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 4
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 5
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 6
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 7
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 8
                        addx    rx_accum, #0
                        test    rx_mask, ina wc         ' Sample 9
                        addx    rx_accum, #0

                        ror     rx_data, #1             ' Shift in next bit
                        test    rx_accum, #$C wz        ' At least 4 HIGH samples?
              if_nz     or      rx_data, #$100
              
                        waitcnt next_bit, rx_bit_ticks_ ' Sleep until the next bit
                        djnz    rx_bits, #:bit
                        
                        xor     rx_data, #$100          ' Toggle stop bit.
                        test    rx_data, start_stop_mask wz
              if_nz     jmp     #receive_packet         ' Start/stop bits must now both be zero


        '------------------------------------------------------
        ' Packet watchdog timer.
        '
        ' If it's been too long since the last successfully received byte,
        ' start a fresh packet. This prevents getting permanently out of
        ' sync in the event of a corrupted packet.
        
                        mov     t1, cnt
                        sub     t1, wdt_timestamp       ' t1 = cnt - wdt_timestamp
                        sub     t1, wdt_timeout_ticks_ wc

                        mov     wdt_timestamp, cnt      ' Reset watchdog, since we have a good rx_data byte
                        
              if_nc     call    #restart_packet         ' Restart packet if wdt_timeout_ticks < t1


        '------------------------------------------------------
        ' Process incoming bytes
        '
        ' Is this byte part of the packet header, or part of the controller
        ' data? Parse the header bytes and store the rest in shared RAM.

                        xor     byte_index, #0 wz,nr    ' First header byte
              if_z      jmp     #header_byte_1

                        xor     byte_index, #1 wz,nr    ' Second header byte  
              if_z      jmp     #header_byte_2

data_byte                                               ' Normal data byte
                        wrbyte  rx_data, rx_pointer     ' Store it.

                        add     rx_pointer, #1          ' Next byte
                        add     byte_index, #1
                        sub     bytes_remaining, #1 wz
              if_nz     jmp     #receive_byte           ' Still not done with this packet...

                        mov     t1, par                 ' Done with this packet!
                        add     t1, #4*4                '   Set new_packet_flag
                        mov     t2, #1
                        wrlong  t2, t1

                        jmp     #receive_packet

        ' The first header byte. Top nybble must be 5. Bottom nybble
        ' specifies the slot number. Since we only support two slots,
        ' the only valid values are $50 and $51.
header_byte_1
                        xor     rx_data, #$50 wz,nr     ' If it's $50, we're done
              if_z      jmp     #data_byte

                        xor     rx_data, #$51 wz,nr     ' If it's not $51, keep looking for the beginning of the packet.
              if_nz     jmp     #receive_byte

                        add     rx_pointer, #RX_MAX_PACKET_LEN
                        jmp     #data_byte              ' It's $51. Point at slot 2.


        ' The second header byte. This contains various flags that
        ' describe the controller's state, and also imply the length
        ' of the rest of the packet.
header_byte_2
                        test    rx_data, #CSTATE_CONNECTED wc
              if_c      add     bytes_remaining, #2

                        test    rx_data, #CSTATE_ANALOG wc
              if_c      add     bytes_remaining, #4

                        test    rx_data, #CSTATE_DUALSHOCK wc
              if_c      add     bytes_remaining, #12
                                                           
                        jmp     #data_byte
        

        '------------------------------------------------------
        ' restart_packet subroutine.
        '
        ' This subroutine resets all per-packet state, starting us back
        ' at the beginning of the header. It should be called in the
        ' event of a receive error, watchdog timer expiration, or upon
        ' reaching the end of a packet's expected payload.
        
restart_packet
                        mov     byte_index, #0
                        mov     bytes_remaining, #2     ' All packets include at least the 2-byte header
                        mov     rx_pointer, rx_buffer_ptr_                        
restart_packet_ret      ret


        '------------------------------------------------------
        ' Local data.

start_stop_mask         long    $80000100

default_rx_data         byte    $00, $00                ' Invalid header, no flag bits set
                        byte    $FF, $FF                ' No buttons pressed
                        byte    $80, $80, $80, $80      ' All analog axes centered
                        byte    $00, $00, $00, $00      ' No button pressure
                        byte    $00, $00, $00, $00
                        byte    $00, $00, $00, $00

t1                      res     1
t2                      res     1

rx_mask                 res     1
rx_bit_ticks_           res     1
rx_buffer_ptr_          res     1
wdt_timeout_ticks_      res     1
default_rx_data_ptr_    res     1

next_bit                res     1
rx_bits                 res     1
rx_accum                res     1
rx_data                 res     1

wdt_timestamp           res     1

byte_index              res     1
bytes_remaining         res     1
rx_pointer              res     1                                                                                                                                                                             

                        fit
                        