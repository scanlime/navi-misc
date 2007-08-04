{{
  Low-level test for controller emulator.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000


OBJ

  remote : "psx_remote_port"
  emulator : "psx_controller_emulator"


PUB Main | slot, button_ptr
                                      
  remote.start(2, 1)
  emulator.start(4, -1, emulator#CONTROLLER_GUITAR_HERO)

  remote.set_led_char(0, 0)
  remote.set_led_char(1, 0)

  ' For testing, mix both remote controller ports onto a single emulated controller
  repeat slot from 0 to 1
    emulator.add_state_buffer(remote.get_state_buffer(slot))
    emulator.add_actuator_buffer(remote.get_actuator_buffer(slot))

  button_ptr := remote.get_state_buffer(0)
    
  repeat
    remote.poll

    if WORD[button_ptr] & remote#BTN_R3 == 0
      emulator.set_controller_type(emulator#CONTROLLER_DUAL_SHOCK)

    if WORD[button_ptr] & remote#BTN_L3 == 0
      emulator.set_controller_type(emulator#CONTROLLER_GUITAR_HERO)
      

    
                       