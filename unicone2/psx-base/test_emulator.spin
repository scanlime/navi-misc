{{
  Low-level test for controller emulator.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000


OBJ

  text : "TV_Text"
  remote : "psx_remote_port"
  emulator : "psx_controller_emulator"


PUB Main | addr, buffer_ptr
                                      
  text.start(12)  
  remote.start(2, 1)
  emulator.start(4)

  ' For testing, mix both remote controller ports onto a single emulated controller
  emulator.add_state_buffer(remote.get_controller_data(0))
  emulator.add_state_buffer(remote.get_controller_data(1))
  
  buffer_ptr := emulator.get_debug_buffer
             
  repeat
    remote.poll
  
    text.out(1)  ' Home

    addr := buffer_ptr
    repeat 10
      repeat 8
        text.hex(BYTE[addr++], 2)
        text.out(" ")
      
      text.out(13)  ' Newline                       