{{
  Low-level test application for psx_remote_port.
  Displays a continuously updating hex dump of the receive buffer.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000


VAR
  long  buffer_ptr
  
OBJ

  text : "TV_Text"
  remote : "psx_remote_port"


PUB Main | addr

  text.start(12)

  remote.start(2, 1)
  buffer_ptr := remote.get_controller_data(0) - 2

  repeat
    remote.poll
  
    text.out(1)  ' Home

    addr := buffer_ptr
    repeat 4
      repeat 10
        text.hex(BYTE[addr++], 2)
        text.out(" ")
      
      text.out(13)  ' Newline                        