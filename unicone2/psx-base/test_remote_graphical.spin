{{
  Graphical test application for psx_remote_port.

  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000

   _stack = ($3000 + $3000 + 100) >> 2   'accomodate display memory and stack

  x_tiles = 16
  y_tiles = 12
  x_pixels = x_tiles * 8
  y_pixels = y_tiles * 8

  paramcount = 14       
  bitmap_base = $2000
  display_base = $5000


VAR
  long  tv_status     '0/1/2 = off/visible/invisible           read-only
  long  tv_enable     '0/? = off/on                            write-only
  long  tv_pins       '%ppmmm = pins                           write-only
  long  tv_mode       '%ccinp = chroma,interlace,ntsc/pal,swap write-only
  long  tv_screen     'pointer to screen (words)               write-only
  long  tv_colors     'pointer to colors (longs)               write-only               
  long  tv_hc         'horizontal cells                        write-only
  long  tv_vc         'vertical cells                          write-only
  long  tv_hx         'horizontal cell expansion               write-only
  long  tv_vx         'vertical cell expansion                 write-only
  long  tv_ho         'horizontal offset                       write-only
  long  tv_vo         'vertical offset                         write-only
  long  tv_broadcast  'broadcast frequency (Hz)                write-only
  long  tv_auralcog   'aural fm cog                            write-only

  word  screen[x_tiles * y_tiles]
  long  colors[64]

  long  stack[32]
  
OBJ

  tv : "tv"
  gr : "graphics"
  remote : "psx_remote_port"
  update_rate : "rate_timer"

PUB Main | dx, dy, i, controller, buttons

  update_rate.init
  cognew(packet_poller, @stack)

  longmove(@tv_status, @tvparams, paramcount)
  tv_screen := @screen
  tv_colors := @colors
  tv.start(@tv_status)

  'init tile screen
  repeat dx from 0 to tv_hc - 1
    repeat dy from 0 to tv_vc - 1
      ' Use colors[0] for each tile
      screen[dy * tv_hc + dx] := display_base >> 6 + dy + dx * tv_vc

  '0: black, 1: white, 2: blue, 3: red
  colors[0] := $CCFC0602
      
  ' Start graphics
  gr.start
  gr.setup(x_tiles, y_tiles, x_pixels, y_pixels, bitmap_base)

  ' Start controller extender ports
  remote.start(2, 1)
  controller := remote.get_controller_data(0)

  repeat
    remote.poll
    gr.clear

    ' Right analog stick
    gr.colorwidth(3, 3)
    gr.plot(0, 0)
    gr.line((BYTE[controller][remote#AXIS_RX] - $80) / 2, -(BYTE[controller][remote#AXIS_RY] - $80) / 2)

    ' Left analog stick
    gr.colorwidth(1, 3)
    gr.plot(0, 0)
    gr.line((BYTE[controller][remote#AXIS_LX] - $80) / 2, -(BYTE[controller][remote#AXIS_LY] - $80) / 2)

    ' Analog buttons
    gr.colorwidth(1, 2)
    repeat i from 0 to 11
      gr.plot(-x_pixels, i * 8)
      gr.line(-x_pixels + BYTE[controller + 6 + i]/4, i * 8)

    ' Digital buttons
    gr.colorwidth(3, 8)
    buttons := WORD[controller]
    repeat i from 0 to 15
      ifnot buttons & (1 << i)
        gr.plot(-x_pixels + 10 + i * 10, -y_pixels + 10)

    gr.colorwidth(2, 14)
    gr.box(65, -90, 60, 16)
    gr.textmode(1, 1, 6, 5)
    gr.colorwidth(0, 0)
    gr.text(95, -82, string("Unicone2"))

    gr.colorwidth(1, 0)
    gr.text(0, -y_pixels+25, update_rate.hz_string)
    gr.finish
      
    gr.copy(display_base)


PRI packet_poller
  ' This is a simple Spin task which polls for new packets and
  ' counts how frequently they arrive using rate_timer.
  
  repeat
    repeat until remote.is_new_packet
    update_rate.sample    
  

DAT

tvparams                long    0               'status
                        long    1               'enable
                        long    %001_0101       'pins
                        long    %0000           'mode
                        long    0               'screen
                        long    0               'colors
                        long    x_tiles         'hc
                        long    y_tiles         'vc
                        long    10              'hx
                        long    1               'vx
                        long    0               'ho
                        long    0               'vo
                        long    0               'broadcast
                        long    0               'auralcog
                        