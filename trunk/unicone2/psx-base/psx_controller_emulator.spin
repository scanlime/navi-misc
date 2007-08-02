{{
  This object emulates a Playstation controller, with support
  for all known Dual Shock controller commands.

  This emulator is based on my own reverse engineering notes:
  http://docs.google.com/View?docid=ddbmmwds_5cw4pk3

  Each controller emulator can have several input/output
  buffers attached to it:

    - At most one actuator buffer, for force-feedback data received from the PSX

    - At most NUM_STATE_BUFFERS state buffers, holding controller states to
      read when we're polled by the PSX. If multiple buffers are attached, their 
      data is mixed. Button state is OR'ed, axes and pressure-sensitive buttons
      are summed and clamped.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  ' Pin assignments, starting with base_pin. These
  ' are the same as the pin assignments on the Unicone2
  ' controller connectors from pin 1 to pin 5.

  PIN_DAT = 0
  PIN_CMD = 1
  PIN_SEL = 2
  PIN_CLK = 3
  PIN_ACK = 4

  STATE_BUFFER_LEN = 18
  ACTUATOR_BUFFER_LEN = 2
  NUM_STATE_BUFFERS = 4

  ' ACK timings, based on values observed on a real Dual Shock controller
  ACK_DELAY_US = 8
  ACK_WIDTH_US = 3

  PADDING_BYTE = $5A

  DIGITAL_MODE = $40
  ANALOG_MODE = $70
  ESCAPE_MODE = $F0
  MODE_MASK = $F0

  REPLY_LEN_ESCAPE = $3
  REPLY_LEN_DIGITAL = $1
  REPLY_LEN_ANALOG = $3

  RESULT_FORMAT_ESCAPE = $0000003F
  RESULT_FORMAT_DIGITAL = $00000003
  RESULT_FORMAT_ANALOG = $0000003F
  
  ' Offsets within the cog communication area
  _base_pin = 0
  _actuators_ptr = 4
  _ack_delay_ticks = 8
  _ack_width_ticks = 12
  _state_ptr_list = 16


VAR

  long  cog

  ' Communication area between cog and object
  long  base_pin
  long  actuators_ptr
  long  ack_delay_ticks
  long  ack_width_ticks
  long  state_ptr_list[NUM_STATE_BUFFERS]
        

PUB start(basepin) : okay

  '' Start a cog running a new controller emulator.
  '' The emulator initially has no attached buffers.
  ''
  '' The controller must be connected via 5 pins starting at basepin.

  base_pin := basepin
  actuators_ptr := 0
  ack_delay_ticks := ACK_DELAY_US * (clkfreq / 1000000)
  ack_width_ticks := ACK_WIDTH_US * (clkfreq / 1000000)

  longfill(@state_ptr_list, 0, NUM_STATE_BUFFERS)

  okay := cog := cognew(@entry, @base_pin) + 1


PUB stop

  '' Frees a cog.

  if cog
    cogstop(cog~ - 1)


PUB set_actuator_buffer(buffer)

  '' Change this controller emulator's attached actuator buffer.
  '' If 'buffer' is zero, the controller does not write actuator data.
  '' If 'buffer' is nonzero, it must point to an area of at least
  '' ACTUATOR_BUFFER_LEN bytes where we'll write any actuator data as
  '' it arrives from the PSX.

  actuators_ptr := buffer

PUB add_state_buffer(buffer) : okay | i

  '' Add an additional state buffer for this controller to read.
  '' 'buffer' must point to an area of at least STATE_BUFFER_LEN bytes.
  ''
  '' Returns TRUE on success, or FALSE if NUM_STATE_BUFFERS buffers
  '' are already attached.

  repeat i from 0 to NUM_STATE_BUFFERS - 1    
    if state_ptr_list[i] == 0
      state_ptr_list[i] := buffer
      okay := TRUE
      quit
  

PUB get_debug_buffer
  return @base_pin


PUB remove_state_buffer(buffer) : okay | i

  '' Remove a state buffer which was previously attached with
  '' add_state_buffer. Note that, since the controller emulator
  '' runs asynchronously, there is no guarantee that 'buffer'
  '' will not be accessed after this method returns.
  ''
  '' Returns TRUE on success, or FALSE if 'buffer' is not
  '' attached to the emulator.

  repeat i from 0 to NUM_STATE_BUFFERS - 1
    if state_ptr_list[i] == buffer
      state_ptr_list[i] := 0
      okay := TRUE
      quit

  
DAT

                        org

        '------------------------------------------------------
        ' Entry point.

entry                   mov     t1, par                 ' Make local copies of all parameters

                        rdlong  t2, t1                  ' Make a set of pin masks
                        mov     dat_mask, #1
                        shl     dat_mask, t2

                        mov     cmd_mask, dat_mask
                        shl     cmd_mask, #1

                        mov     sel_mask, dat_mask
                        shl     sel_mask, #2

                        mov     clk_mask, dat_mask
                        shl     clk_mask, #3

                        mov     ack_mask, dat_mask
                        shl     ack_mask, #4

                        mov     clk_sel_mask, clk_mask
                        or      clk_sel_mask, sel_mask


        '------------------------------------------------------
        ' Packet receive loop

receive_packet
                        waitpne zero, sel_mask          ' Make sure SEL is high (previous packet is ended)

                        mov     dira, #0                ' All outputs high-Z
                        mov     outa, #0
                        mov     byte_index, #0

                        waitpeq zero, sel_mask          ' Wait for a falling edge on SEL
                        
                        call    #txrx_byte              ' Read the address byte
                        xor     rx_data, #$01 nr,wz     ' Is this packet for a controller on slot 1?
              if_nz     jmp     #receive_packet         ' If not, ignore the rest of the packet.
                        call    #send_ack               ' Acknowledge the address byte
                        or      dira, dat_mask          ' Enable output driver

                        mov     tx_data, mode_byte      ' Send current mode and reply length
                        call    #txrx_byte              ' Receive command byte

                        xor     rx_data, #$40           ' All commands have 4 as their high nybble
                        and     rx_data, #$F0 nr,wz
              if_nz     jmp     #receive_packet         ' Not between $40 and $4F? Don't respond.

                        add     rx_data, #:cmd_table    ' Jump via the cmd_table
                        jmp     rx_data

:cmd_table              jmp     #cmd_init_pressure                              ' $40
                        jmp     #cmd_get_available_poll_results                 ' $41
                        jmp     #cmd_poll                                       ' $42
                        jmp     #cmd_escape                                     ' $43
                        jmp     #cmd_set_major_mode                             ' $44
                        jmp     #cmd_read_ext_status_1                          ' $45
                        jmp     #cmd_read_const_1                               ' $46
                        jmp     #cmd_read_const_2                               ' $47
                        jmp     #receive_packet                                 ' $48 (invalid)
                        jmp     #receive_packet                                 ' $49 (invalid)
                        jmp     #receive_packet                                 ' $4a (invalid)
                        jmp     #receive_packet                                 ' $4b (invalid)
                        jmp     #cmd_read_const_3                               ' $4c
                        jmp     #cmd_set_poll_cmd_format                        ' $4d
                        jmp     #receive_packet                                 ' $4e (invalid)
                        jmp     #cmd_set_poll_result_format                     ' $4f
                        
        
        '------------------------------------------------------
        ' Byte receive/transmit subroutine
        '
        ' The Playstation bus is full-duplex. We always transmit
        ' a byte concurrently with receiving a byte. This routine
        ' sends 'tx_data' while receiving 'rx_byte'.
        '
        ' Multiple entry points exist for various data widths.
        '
        ' Note that this routine does not enable the output driver
        ' for DAT. The caller is responsible for doing so only when
        ' this device has been addressed. The original PSX shares
        ' the same physical bus for controller and memory card
        ' accesses, so we must be sure not to create bus contention
        ' with the memory card.
        '
        ' Bits are output on the falling edge of CLK, input on the
        ' rising edge of CLK. Each byte begins with a falling edge.
        '
        ' The last rising edge is timestamped, for send_ack.

txrx                    test    sel_mask, ina wc        ' Abort if SEL went high
              if_c      jmp     #receive_packet

                        mov     t1, #8
:bit
                        test    tx_data, #1 wc
                        waitpne clk_mask, clk_sel_mask  ' Wait for CLK- or SEL+
                        muxc    outa, dat_mask          ' Write tx_data[0]
                        ror     tx_data, #1             ' Shift out tx_data[0]

                        test    sel_mask, ina wc        ' Abort if SEL goes high
              if_c      jmp     #receive_packet
                        
                        waitpne zero, clk_sel_mask      ' Wait for CLK+ or SEL+
                        test    cmd_mask, ina wc        ' Read CMD bit
                        mov     clk_posedge_cnt, cnt    ' Timestamp this rising edge                        
                        rcr     rx_data, #1             ' Shift into rx_data[31]

                        test    sel_mask, ina wc        ' Abort if SEL went high
              if_c      jmp     #receive_packet

                        djnz    t1, #:bit               ' Next bit...
txrx_ret                ret

txrx_byte               call    #txrx
                        rol     rx_data, #8             ' Right-justify and trim rx_byte               
                        and     rx_data, #$FF                          
txrx_byte_ret           ret

txrx_16                 call    #txrx
                        call    #send_ack
                        call    #txrx
                        rol     rx_data, #16                                                           
                        and     rx_data, mask_16bit
txrx_16_ret             ret

txrx_32                 call    #txrx                        
                        call    #send_ack
                        call    #txrx
                        call    #send_ack
                        call    #txrx
                        call    #send_ack
                        call    #txrx
txrx_32_ret             ret
                        

        '------------------------------------------------------
        ' send_ack subroutine.
        '
        ' This routine sends a timed ACK pulse based on ack_delay_ticks,
        ' ack_width_ticks, and the timestamp saved by the last txrx_byte.

send_ack
                        mov     t1, par
                        add     t1, #_ack_delay_ticks
                        rdlong  t2, t1                  ' Read ack_delay_ticks

                        mov     t1, par
                        add     t1, #_ack_width_ticks
                        rdlong  t3, t1                  ' Read ack_width_ticks

                        add     t2, clk_posedge_cnt     ' Add ack_delay_ticks to the +CLK timestamp

                        waitcnt t2, t3                  ' Wait for the ACK pulse to start, and update t2
                        or      dira, ack_mask          ' Pull ACK low

                        waitcnt t2, #0                  ' Wait for the ACK pulse to end
                        xor     dira, ack_mask          ' Float ACK

send_ack_ret            ret


        '------------------------------------------------------
        ' Command prologues
        '

        ' This is the prologue to normal non-escape commands.
        ' It acknowledges the command byte and sends a padding byte.
begin_cmd
                        call    #send_ack
                        mov     tx_data, #PADDING_BYTE
                        call    #txrx_byte
                        call    #send_ack
begin_cmd_ret           ret

        ' Prologue for escape commands. If we're not in escape mode,
        ' reject this command.
begin_escape_cmd
                        mov     t1, mode_byte
                        and     t1, #MODE_MASK
                        xor     t1, #ESCAPE_MODE
              if_nz     jmp     #receive_packet

                        call    #begin_cmd
begin_escape_cmd_ret    ret


        '------------------------------------------------------
        ' Command: INIT_PRESSURE
        '

cmd_init_pressure       call    #begin_escape_cmd
                        'XXX: Implement me!
                        jmp     #receive_packet


        '------------------------------------------------------
        ' Command: GET_AVAILABLE_POLL_RESULTS
        '
        ' Returns:
        '    - 32-bit available_results flags (1 bit for each available byte of POLL data)
        '    - One more byte of flags, unused (0)
        '    - One padding byte

cmd_get_available_poll_results
                        call    #begin_escape_cmd

                        mov     tx_data, available_results
                        call    #txrx_32
                        call    #send_ack

                        mov     tx_data, #0
                        call    #txrx_byte
                        call    #send_ack

                        mov     tx_data, #PADDING_BYTE
                        call    #txrx_byte
                        call    #send_ack

                        jmp     #receive_packet


        '------------------------------------------------------
        ' Command: POLL/ESCAPE
        '
        ' The POLL and ESCAPE commands are substantially the same.
        ' This routine implements them both, by swapping in different
        ' rx_data handlers.
        '
        ' Arguments:
        '    - For ESCAPE, the first command-specific argument selects
        '      whether we're entering or exiting escape mode.
        '
        '    - For POLL, each argument may be mapped to an
        '      actuator by SET_POLL_CMD_FORMAT
        '
        ' Results:
        '    - For each bit set in result_format, returns one byte of data
        '      aggregated from all state buffers present in state_ptr_list.

cmd_escape              movs    poll_rx_callback, #pollcb_escape
                        jmp     #cmd_poll_or_escape                        
cmd_poll                movs    poll_rx_callback, #pollcb_poll
cmd_poll_or_escape      call    #begin_cmd

                        call    #send_ack
                        mov     tx_data, #PADDING_BYTE
                        call    #txrx_byte

                        mov     byte_index, #0          ' Iterate over result bytes
                        mov     result_iter, result_format
poll_byte_loop          test    result_iter, #1 wc      ' C = 1, output this byte
              if_nc     jmp     #poll_skip_byte
              
                        ' Perform poll result mixing in various ways, depending on
                        ' what type of byte this is. The first two are buttons, the
                        ' next four are analog axes, and the rest are pressure sensors.

                        sub     byte_index, #2 nr,wc
              if_c      jmp     #mix_button_byte
                        sub     byte_index, #6 nr,wc
              if_c      jmp     #mix_axis_byte
                        jmp     #mix_pressure_byte
mix_ret
                        
                        call    #txrx_byte

                        ' Act on the received data via a callback that changes
                        ' depending on whether this is POLL or ESCAPE.

poll_rx_callback        jmp     #0
poll_rx_callback_ret

                        xor     result_iter, #1 nr,wz   ' Z = 1, this is the last byte
              if_nz     call    #send_ack               ' Ack all bytes except the last

poll_skip_byte          add     byte_index, #1          ' Next byte...
                        shr     result_iter, #1 wz      ' Z = 1, this was the last byte
              if_nz     jmp     #poll_byte_loop
              
                        jmp     #receive_packet


                        '--------------------------------------
                        ' Poll RX callback: POLL command
                        '
                        ' This callback is responsible for storing rx_byte
                        ' in any applicable actuator buffers.
                        '
                        ' This also unconditionally kicks us out of
                        ' escape mode, if we were in it.

pollcb_poll
                        mov     mode_byte, preescape_mode_byte
                        mov     result_format, preescape_result_format

                        ' XXX: Implement actuators

                        jmp     #poll_rx_callback_ret


                        '--------------------------------------
                        ' Poll RX callback: ESCAPE command
                        '
                        ' This callback switches into or out of escape mode,
                        ' depending on the value of the byte at byte_index 0.

pollcb_escape
                        xor     byte_index, #0 nr,wz
              if_nz     jmp     #poll_rx_callback_ret   ' Not byte_index 0, ignore it

                        xor     rx_data, #0 nr,wz

                        ' Exiting escape mode
              if_z      mov     mode_byte, preescape_mode_byte
              if_z      mov     result_format, preescape_result_format
              
                        ' Entering escape mode
              if_nz     mov     mode_byte, #(ESCAPE_MODE | REPLY_LEN_ESCAPE)                  
              if_nz     mov     result_format, #RESULT_FORMAT_ESCAPE
              
                        jmp     #poll_rx_callback_ret              
              

                        '--------------------------------------
                        ' Poll result mixer: Buttons.
                        '
mix_button_byte
                        mov     tx_data, #$FF           ' Default value, all buttons released
                        movs    mixer_callback, #button_mix_callback
                        jmp     #mixer

button_mix_callback     and     tx_data, t1             ' Logically OR the inverted buttons
                        jmp     #mixer_callback_ret


                        '--------------------------------------
                        ' Poll result mixer: Axes.
                        '
mix_axis_byte
                        mov     tx_data, #$80           ' Default value, centered
                        movs    mixer_callback, #axis_mix_callback
                        jmp     #mixer

axis_mix_callback       add     tx_data, t1             ' Add axes
                        sub     tx_data, #$80           ' Subtract the duplicate offset
                        maxs    tx_data, #$FF           ' Clamp
                        mins    tx_data, #$00
                        jmp     #mixer_callback_ret


                        '--------------------------------------
                        ' Poll result mixer: Pressure sensors.
                        '
mix_pressure_byte
                        mov     tx_data, #$00           ' Default value, no pressure
                        movs    mixer_callback, #pressure_mix_callback
                        jmp     #mixer

pressure_mix_callback   add     tx_data, t1             ' Combine pressure
                        maxs    tx_data, #$FF           ' Clamp
                        jmp     #mixer_callback_ret


                        '--------------------------------------
                        ' Generic poll result mixer. Expects the
                        ' default value to be set already. This
                        ' iterates t1 over each value to be mixed,
                        ' using a mixer callback routine pointed to
                        ' by the jmp instruction at mixer_callback.
                        '

mixer                   mov     t2, par                 ' Point at the first state buffer
                        add     t2, #_state_ptr_list
                        mov     t3, #NUM_STATE_BUFFERS  ' Count the remaining state buffers

mixer_buffer_loop       rdlong  t4, t2 wz               ' Read the state buffer pointer
                        
                        add     t4, byte_index          ' Offset by the current byte index
              if_nz     rdbyte  t1, t4                  ' Get the current byte at this buffer

mixer_callback
              if_nz     jmp #0                          ' Allow the callback to operate on this byte
mixer_callback_ret

                        add     t2, #4                  ' Next state buffer...
                        djnz    t3, #mixer_buffer_loop
                        jmp     #mix_ret


        '------------------------------------------------------
        ' Command: SET_MAJOR_MODE
        '
        ' Sets analog mode if the first byte is nonzero, digital mode
        ' otherwise. All reply bytes are zero. This command also resets
        ' various controller state, such as the actuator and reply byte
        ' mappings.

cmd_set_major_mode      call    #begin_escape_cmd

                        mov     tx_data, #0
                        call    #txrx_byte
                        call    #send_ack

                        xor     rx_data, #0 nr,wz

                        ' Digital mode
              if_z      mov     preescape_mode_byte, #(DIGITAL_MODE | REPLY_LEN_DIGITAL)
              if_z      mov     preescape_result_format, #RESULT_FORMAT_DIGITAL

                        ' Analog mode
              if_nz     mov     preescape_mode_byte, #(ANALOG_MODE | REPLY_LEN_ANALOG)
              if_nz     mov     preescape_result_format, #RESULT_FORMAT_ANALOG
                        muxnz   ext_status, extstatus_analog

                        call    #txrx_byte      ' Send 5 more zeroes
                        call    #send_ack
                        call    #txrx_32

                        jmp     #receive_packet


        '------------------------------------------------------
        ' Command: READ_EXT_STATUS_1
        '
        ' Response data:
        '   - 0x03 (?) on Dual Shock controller, 0x01 (?) for Guitar Hero controller
        '   - 0x02 (?)
        '   - 0x01 if the "Analog" light is on, 0x00 if it's off.
        '   - 0x02 (?)
        '   - 0x01 (?)
        '   - 0x00 (?)

cmd_read_ext_status_1   call    #begin_escape_cmd

                        mov     tx_data, ext_status
                        call    #txrx_32
                        call    #send_ack

                        mov     tx_data, #$0001
                        call    #txrx_16         

                        jmp     #receive_packet


        '------------------------------------------------------
        ' Command: READ_CONST_1/2/3
        '
        ' These commands returns unknown constant data. The
        ' first command byte is always an address offset. The
        ' first two response bytes are always zero.

cmd_read_const_1        movs    const_ptr, #constdata_1
                        jmp     #cmd_read_const
cmd_read_const_2        movs    const_ptr, #constdata_2
                        jmp     #cmd_read_const
cmd_read_const_3        movs    const_ptr, #constdata_3
cmd_read_const          call    #begin_escape_cmd

                        mov     tx_data, #0             ' First byte is const address
                        call    #txrx_byte
const_ptr               add     rx_data, #0             ' This is filled in with a constdata_* address
                        movs    :indirect, rx_data

                        call    #send_ack               ' Unused (zero) byte
                        call    #txrx_byte
                        call    #send_ack
 
:indirect               mov     tx_data, 0              ' Load tx_data from the computed pointer
                        call    #txrx_32                ' .. and send the constant word.

                        jmp     #receive_packet

                        
        '------------------------------------------------------
        ' Command: SET_POLL_CMD_FORMAT
        '
        ' Sets cmd_format, the mapping from poll bytes to actuator indices.
        ' Our return value is the previous cmd_format.
        '
        ' We only use the first 32 bits. The last two actuator bytes are ignored.

cmd_set_poll_cmd_format call    #begin_escape_cmd

                        mov     tx_data, cmd_format
                        call    #txrx_32
                        call    #send_ack
                        mov     cmd_format, rx_data

                        mov     tx_data, mask_16bit
                        call    #txrx_16

                        jmp     #receive_packet

                        
        '------------------------------------------------------
        ' Command: SET_POLL_RESULT_FORMAT
        '
        ' Sets result_format, the bitmask of poll result bytes that the
        ' console wishes to receive. This command is typically used to
        ' enable and disable pressure-sensitive buttons.
        '
        ' Note that the new result length doesn't take effect until we leave
        ' escape mode. (Escape mode has a fixed result format, in order to
        ' match the fixed reply length.)

cmd_set_poll_result_format
                        call    #begin_escape_cmd

                        mov     tx_data, #0             ' Always 00 00 00 00
                        call    #txrx_32
                        call    #send_ack
                        mov     preescape_result_format, rx_data

                        mov     tx_data, #0             ' Always 00
                        call    #txrx_byte
                        call    #send_ack

                        mov     tx_data, #PADDING_BYTE
                        call    #txrx_byte              

                        jmp     #receive_packet


        '------------------------------------------------------
        ' Constant data.

zero                    long    0
mask_16bit              long    $FFFF

available_results       long    $0003FFFF
extstatus_analog        long    $00010000

constdata_1             long    $0a000201
                        long    $14010101
constdata_2             long    $00010002
constdata_3             long    $00000400
                        long    $00000700

        '------------------------------------------------------
        ' Initialized controller state.

mode_byte               long    DIGITAL_MODE | REPLY_LEN_DIGITAL
preescape_mode_byte     long    DIGITAL_MODE | REPLY_LEN_DIGITAL
result_format           long    RESULT_FORMAT_DIGITAL
preescape_result_format long    RESULT_FORMAT_DIGITAL
ext_status              long    $02000203
cmd_format              long    $00000000

        '------------------------------------------------------
        ' Uninitialized data.

t1                      res     1
t2                      res     1
t3                      res     1
t4                      res     1

tx_data                 res     1
rx_data                 res     1
clk_posedge_cnt         res     1
byte_index              res     1
result_iter             res     1

dat_mask                res     1               ' Output, high-z when not addressed
cmd_mask                res     1               ' Input
clk_mask                res     1               ' Input
sel_mask                res     1               ' Input
ack_mask                res     1               ' Output, open-collector
clk_sel_mask            res     1

                        fit
                        