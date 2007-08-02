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


PUB Main | slot
                                      
  remote.start(2, 1)
  emulator.start(4)

  remote.set_led_digit(0, 0)
  remote.set_led_digit(1, 0)

  ' For testing, mix both remote controller ports onto a single emulated controller
  repeat slot from 0 to 1
    emulator.add_state_buffer(remote.get_state_buffer(slot))
    emulator.add_actuator_buffer(remote.get_actuator_buffer(slot))
    
  repeat
    remote.poll
                       