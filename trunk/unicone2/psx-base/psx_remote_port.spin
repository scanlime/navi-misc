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

  ' Watchdog timeout, in milliseconds
  WDT_TIMEOUT_MS = 5

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
  
  
OBJ
  tx : "Simple_Serial"


VAR

  long  rx_cog

  'Communication area for RX driver startup
  long  rx_pin
  long  rx_bit_ticks
  long  rx_buffer_ptr
  long  wdt_timeout_ticks
  long  default_rx_data_ptr

  byte  tx_buffer[TX_PACKET_LEN]
  byte  rx_buffers[RX_MAX_PACKET_LEN * NUM_SLOTS]


PUB start(rxpin, txpin) : okay

  bytefill(@tx_buffer, 0, TX_PACKET_LEN)

  ' The RX driver will reset unused parts of the rx_buffer
  ' to default values upon receiving each packet, but we
  ' still need to initialize the buffer.
  bytemove(@rx_buffers, @default_rx_data, RX_MAX_PACKET_LEN)
  bytemove(@rx_buffers[RX_MAX_PACKET_LEN], @default_rx_data, RX_MAX_PACKET_LEN)

  tx.start(-1, txpin, TX_BAUD)
  
  ' Start RX driver
  rx_pin := rxpin
  rx_bit_ticks := clkfreq / RX_BAUD
  wdt_timeout_ticks := (clkfreq / 1000) * WDT_TIMEOUT_MS
  rx_buffer_ptr := @rx_buffers
  default_rx_data_ptr := @default_rx_data
  okay := rx_cog := cognew(@entry, @rx_pin) + 1


PUB stop

  tx.stop

  if rx_cog
    cogstop(rx_cog~ - 1)


PUB tx_packet | i, value

  '' Send an outgoing packet to the remote. This function must be called
  '' for any changes to LED or force feedback state to take effect. If it
  '' is not called at least once a second or so, the remote unit's watchdog
  '' timer will cause all LEDs and force feedback actuators to be turned off.

  tx.tx(TX_START_OF_PACKET)

  repeat i from 0 to TX_PACKET_LEN - 1
    value := tx_buffer[i]
    if value == TX_START_OF_PACKET or value == TX_ESCAPE
      tx.tx(TX_ESCAPE)
    tx.tx(value)  


PUB set_led_state(slot, leds)

  '' Set a slot's raw 8-bit LED state.
  '' Bit positions:
  ''       
  ''      5
  ''     ---
  ''   6| 7 |4
  ''     ---
  ''   0|   |2
  ''     ---  
  ''      1
  ''
  '' (The dot cannot be controlled remotely. Bit 3 is ignored.)


  tx_buffer[slot * TX_SLOT_LEN + TX_LEDS] := leds


PUB set_led_digit(slot, digit)

  '' Display a hexadecimal digit on a particular slot's LED display.

  set_led_state(slot, lookupz(digit: $77, $14, $B3, $B6, $D4, $E6, $E7, $34, $F7, $F4, $F5, $C7, $63, $97, $E3, $E1))  


PUB set_actuator(slot, actuator, value)

  '' Set the value of a force feedback actuator on a particular slot.
  '' The meaning of the value byte is actuator-dependent. Playstation
  '' Dual Shock controllers support two actuators.

  tx_buffer[slot * TX_SLOT_LEN + TX_FF_1 + actuator] := value
  

PUB get_controller_data(slot) : address

  '' Retrieve the address of a controller slot's raw polling results

  address := @rx_buffers[RX_MAX_PACKET_LEN * slot + 2]


PUB get_controller_flags(slot) : state_flags

  '' Returns state flags indicating the controller's hardware capabilities,
  '' and even whether a controller is connected at all. The return value
  '' consists of one or more CSTATE_* flags.

  state_flags := rx_buffers[RX_MAX_PACKET_LEN * slot + 1]

  
DAT

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

                        add     t1, #4
                        rdlong  default_rx_data_ptr_, t1

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

        ' We just finished a packet. Reset any unused bytes to their default
        ' values, and prepare to receive another packet.

finish_packet
                        xor     byte_index, #RX_MAX_PACKET_LEN wz,nr
              if_z      jmp     #receive_packet

                        mov     t1, default_rx_data_ptr_
                        add     t1, byte_index
                        rdbyte  t2, t1
                        wrbyte  t2, rx_pointer

                        add     rx_pointer, #1          ' Next byte
                        add     byte_index, #1
                        jmp     #finish_packet


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

answer_byte             byte    $42, $99, $98, $87, $43                        
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
                        