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

  NUM_REMOTES = 1
  NUM_EMULATORS = 2
  NUM_TOTAL_SLOTS = NUM_REMOTES * Rem#NUM_SLOTS
  
  HOTKEY = Rem#BTN_R3 | Rem#BTN_L3 | Rem#BTN_R1 | Rem#BTN_L1
  HOTKEY_TIMEOUT_SECONDS = 5
  EMU_SELECT_BUTTONS = Rem#BTN_SQUARE | Rem#BTN_CIRCLE | Rem#BTN_X | Rem#BTN_TRIANGLE | Rem#BTN_START

  EMU_FLAG_ATTACHED = 1 << 1
  EMU_FLAG_DUALSHOCK = 1 << 2


OBJ

  Rem[NUM_REMOTES] : "psx_remote_port"
  Emu[NUM_EMULATORS] : "psx_controller_emulator"


VAR

  byte  slot_mappings[NUM_TOTAL_SLOTS]
  byte  emu_cflags[NUM_EMULATORS]


PUB Main | remote, slot, emulator

  ' Start all remote connections and emulator cores

  Rem[0].start(2, 1)
  Emu[0].start(4, Emu#CONTROLLER_DUAL_SHOCK)
  Emu[1].start(9, Emu#CONTROLLER_DUAL_SHOCK)

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

        ' Check for the hotkey on each remote
        if Rem[remote].get_buttons(slot) & HOTKEY == HOTKEY
          handle_hotkey(remote, slot)

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


PRI poll_all_remotes | remote

    repeat remote from 0 to NUM_REMOTES-1
      Rem[remote].poll
          

PRI handle_hotkey(remote, slot) | buttons, deadline, emulator, i

  ' Our hotkey was just pressed on a particular physical controller.
  ' Give the user a chance to change the controller's emulator assignment.

  ' Disconnect this controller, so further input doesn't affect the game
  set_slot_mapping(remote, slot, -1)

  ' Visual indication that we're waiting for input
  Rem[remote].set_led_char(slot, Rem#LED_CHAR_DASH + Rem#LED_EFFECT_BLINK)

  ' Wait a few seconds for input
  deadline := cnt + clkfreq * HOTKEY_TIMEOUT_SECONDS
  
  repeat
    if cnt - deadline > 0
      quit

    poll_all_remotes
    buttons := Rem[remote].get_buttons(slot)

    ' Debug feature: Pressing Select in hotkey mode will
    ' show the low nybble of each controller's flags.
    if buttons & Rem#BTN_SELECT
      repeat i from 0 to Rem#NUM_SLOTS-1
        Rem[remote].set_led_char(i, Rem[remote].get_controller_flags(i) + Rem#LED_EFFECT_BLINK)
    
  until buttons & EMU_SELECT_BUTTONS <> 0

  if buttons & Rem#BTN_SQUARE
    emulator := 0
  elseif buttons & Rem#BTN_TRIANGLE
    emulator := 1
  elseif buttons & Rem#BTN_CIRCLE
    emulator := 2
  elseif buttons & Rem#BTN_X
    emulator := 3
  else
    emulator := -1

  set_slot_mapping(remote, slot, emulator)


PRI set_slot_mapping(remote, slot, emulator) | slot_ptr, old_emulator

  ' Change the emulator attached to a particular slot.
  ' This adds/removes buffers as necessary, and updates
  ' the slot's LED display. "emulator" is zero-based.
  ' If "emulator" is -1 or out of range, this
  ' disconnects the slot from all emulators.

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
                       