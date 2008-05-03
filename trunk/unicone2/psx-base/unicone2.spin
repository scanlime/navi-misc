{{
  Main program for Unicone2 base station with Playstation 2 support.

  This program runs multiple controller emulator cores and keeps
  connections to multiple remote units. We can arbitrarily attach
  each physical controller (a slot on a remote unit) to a particular
  emulator core.

  The attachments are edited by the user via a hotkey. When the
  hotkey is pressed (L3+R3+L1+R1), the next button (square, triangle,
  circle, or x) selects an emulator port. The LED displays indicate
  each controller's current port assignment.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000

  NUM_REMOTES = 2
  NUM_EMULATORS = 4
  NUM_TOTAL_SLOTS = NUM_REMOTES * Rem#NUM_SLOTS
  
  STANDARD_HOTKEY = Rem#BTN_R3 | Rem#BTN_L3 | Rem#BTN_R1 | Rem#BTN_L1
  GUITAR_HOTKEY = Rem#BTN_LEFT | Rem#BTN_SELECT | Rem#BTN_START

  HOTKEY_TIMEOUT_SECONDS = 3
  EMU_SELECT_MASK = !(Rem#BTN_LEFT | Rem#BTN_RIGHT | Rem#BTN_UP | Rem#BTN_DOWN)

  EMU_FLAG_ATTACHED = 1 << 1
  EMU_FLAG_DUALSHOCK = 1 << 2


OBJ

  Rem[NUM_REMOTES] : "psx_remote_port"
  Emu[NUM_EMULATORS] : "psx_controller_emulator"
  Debug : "Simple_Serial"
  Num : "Numbers"


VAR

  byte  slot_mappings[NUM_TOTAL_SLOTS]
  byte  emu_cflags[NUM_EMULATORS]
  long  debug_stack[64]


PUB Main | remote, slot, emulator, buttons

  ' Use the programming serial port for debug at runtime
  Debug.init(31, 30, 19200)
  cognew(debug_cog, @debug_stack)

  ' Start all remote connections and emulator cores

  Rem[0].start(0, 1)
  Rem[1].start(2, 3)

  Emu[0].start(8, 4, Emu#CONTROLLER_DUAL_SHOCK)
  Emu[1].start(13, 5, Emu#CONTROLLER_DUAL_SHOCK)
  Emu[2].start(18, 6, Emu#CONTROLLER_DUAL_SHOCK)
  Emu[3].start(23, 7, Emu#CONTROLLER_DUAL_SHOCK)

  ' Reset all slot mappings. The default is to
  ' have an identity mapping where slot N on each
  ' remote maps to emulator N.

  bytefill(@slot_mappings, -1, NUM_TOTAL_SLOTS)
  repeat remote from 0 to NUM_REMOTES-1
    repeat slot from 0 to Rem#NUM_SLOTS-1
      set_slot_mapping(remote, slot, slot)

  ' Main loop for this cog: poll all remote units.
  ' This is responsible for updating LEDs, transmitting
  ' force-feedback data, and handling hotkeys. All the
  ' latency-critical controller state updates are
  ' sent via shared memory directly from the remote
  ' to the emulator.

  repeat
    bytefill(@emu_cflags, 0, NUM_EMULATORS)
  
    repeat remote from 0 to NUM_REMOTES-1
      Rem[remote].poll

      repeat slot from 0 to Rem#NUM_SLOTS-1

        ' Check for the various hotkeys on each remote...
        buttons := Rem[remote].get_buttons(slot)

        if buttons == STANDARD_HOTKEY
          ' Normal controller hotkey: emulator select buttons form a clockwise circle.
          handle_hotkey(remote, slot, Rem#BTN_SQUARE, Rem#BTN_TRIANGLE, Rem#BTN_CIRCLE, Rem#BTN_X)

        elseif buttons == GUITAR_HOTKEY
          ' Guitar Hero controller: Fret buttons select an emulator.
          handle_hotkey(remote, slot, Rem#BTN_R2, Rem#BTN_CIRCLE, Rem#BTN_TRIANGLE, Rem#BTN_X)

        ' Set emu_cflags to be the union of all attached controllers' flags
        emulator := slot_mappings[remote * Rem#NUM_SLOTS + slot]
        if emulator < NUM_EMULATORS and emulator <> -1
          emu_cflags[emulator] |= Rem[remote].get_controller_flags(slot)
  
    ' Make sure each emulator is emulating the proper type of controller.
    ' Use CONTROLLER_DUAL_SHOCK if any attached controllers are Dual Shock,
    ' but use CONTROLLER_ANALOG if controllers are attached and none are
    ' Dual Shock. These rules are necessary since pressure sensitive buttons
    ' are only available with CONTROLLER_DUAL_SHOCK, and Guitar Hero requires
    ' CONTROLLER_ANALOG for the guitar controller.

    repeat emulator from 0 to NUM_EMULATORS-1
      if emu_cflags[emulator] & Rem#CSTATE_CONNECTED
        if emu_cflags[emulator] & Rem#CSTATE_DUALSHOCK
          Emu[emulator].set_controller_type(Emu#CONTROLLER_DUAL_SHOCK)
        else
          Emu[emulator].set_controller_type(Emu#CONTROLLER_ANALOG)


PRI debug_cog | remote, slot

  ' This function runs continuously in a dedicated cog, and outputs debug information
  ' including the current state of all buffers.

  Num.Init

  repeat
    waitcnt(cnt + clkfreq)
    Debug.str(string(10, 13, 10, 13))
  
    repeat remote from 0 to NUM_REMOTES-1
      Debug.str(string(10, 13, "Remote "))
      Debug.str(Num.ToStr(remote, Num#DEC))
      Debug.str(string(":"))
      repeat slot from 0 to Rem#NUM_SLOTS-1
        Debug.str(string(10, 13, "  Slot "))
        Debug.str(Num.ToStr(slot, Num#DEC))
        Debug.str(string(":", 10, 13, "    St:"))
        debug_hexdump_buffer(Rem[remote].get_state_buffer(slot) - Rem#RX_HEADER_LEN, Rem#RX_MAX_PACKET_LEN)
        Debug.str(string(10, 13, "    Ac:"))
        debug_hexdump_buffer(Rem[remote].get_actuator_buffer(slot) - Rem#TX_FF_1, Rem#TX_SLOT_LEN)


PRI debug_hexdump_buffer(address, len) | i

  ' Internal routine for debug_cog: hex dump a buffer of bytes.

  repeat i from 0 to len-1
    Debug.str(Num.ToStr(BYTE[address+i], Num#HEX3))
      

PRI poll_all_remotes | remote

  repeat remote from 0 to NUM_REMOTES-1
    Rem[remote].poll
          

PRI handle_hotkey(remote, slot, btn1, btn2, btn3, btn4) | buttons, deadline, emulator, i
                              
  ' Our hotkey was just pressed on a particular physical controller.
  ' Give the user a chance to change the controller's emulator assignment.

  ' Wait a few seconds for input...
  deadline := cnt + clkfreq * HOTKEY_TIMEOUT_SECONDS

  ' Disconnect this controller, so further input doesn't affect the game.
  ' Save the old emulator port, so we can cancel if the user doesn't
  ' select anything.
  emulator := set_slot_mapping(remote, slot, -1)

  ' Visual indication that we're waiting for input
  Rem[remote].set_led_char(slot, Rem#LED_CHAR_DASH + Rem#LED_EFFECT_BLINK)

  ' First, wait for the keys to be released
  repeat
    if cnt - deadline > 0
      return      
    poll_all_remotes
    buttons := Rem[remote].get_buttons(slot) & EMU_SELECT_MASK
  until buttons == 0

  ' Wait for a button press
  repeat
    if cnt - deadline > 0
      quit
    poll_all_remotes
    buttons := Rem[remote].get_buttons(slot) & EMU_SELECT_MASK
  until buttons <> 0

  if buttons == btn1
    emulator := 0
  elseif buttons == btn2
    emulator := 1
  elseif buttons == btn3
    emulator := 2
  elseif buttons == btn4
    emulator := 3

  elseif buttons == Rem#BTN_SELECT
    ' Debug feature: Pressing Select in hotkey mode will
    ' show the low nybble of each controller's flags.
    repeat i from 0 to Rem#NUM_SLOTS-1
      Rem[remote].set_led_char(i, Rem[remote].get_controller_flags(i) + Rem#LED_EFFECT_BLINK)

  set_slot_mapping(remote, slot, emulator)


PRI set_slot_mapping(remote, slot, emulator) : old_emulator | slot_ptr

  ' Change the emulator attached to a particular slot.
  ' This adds/removes buffers as necessary, and updates
  ' the slot's LED display. "emulator" is zero-based.
  ' If "emulator" is -1 or out of range, this
  ' disconnects the slot from all emulators.
  '
  ' Returns the old value of 'emulator'.

  ' First, remove the old buffers if any

  slot_ptr := @slot_mappings + remote * Rem#NUM_SLOTS + slot
  old_emulator := BYTE[slot_ptr]
  BYTE[slot_ptr] := emulator

  if old_emulator < NUM_EMULATORS and old_emulator <> -1
    Emu[old_emulator].remove_state_buffer(Rem[remote].get_state_buffer(slot))
    Emu[old_emulator].remove_actuator_buffer(Rem[remote].get_actuator_buffer(slot))

  if emulator < NUM_EMULATORS and emulator <> -1
    Emu[emulator].add_state_buffer(Rem[remote].get_state_buffer(slot))
    Emu[emulator].add_actuator_buffer(Rem[remote].get_actuator_buffer(slot))
    Rem[remote].set_led_char(slot, emulator + 1)
  else
    Rem[remote].set_led_char(slot, Rem#LED_CHAR_BLANK)
                       