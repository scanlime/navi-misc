CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  cols = 40
  rows = 13

  screensize = cols * rows
  lastrow = screensize - cols

  tv_count = 14

OBJ
  bt : "BluetoothConduit"
  tv : "tv"
  
VAR

  long  col, row, color, flag
  
  word  screen[screensize]
  long  colors[8 * 2]

  long  foo[16]

  long  tv_status     '0/1/2 = off/invisible/visible              read-only   (14 longs)
  long  tv_enable     '0/non-0 = off/on                           write-only
  long  tv_pins       '%pppmmmm = pin group, pin group mode       write-only
  long  tv_mode       '%tccip = tile,chroma,interlace,ntsc/pal    write-only
  long  tv_screen     'pointer to screen (words)                  write-only      
  long  tv_colors     'pointer to colors (longs)                  write-only                            
  long  tv_ht         'horizontal tiles                           write-only                            
  long  tv_vt         'vertical tiles                             write-only                            
  long  tv_hx         'horizontal tile expansion                  write-only                            
  long  tv_vx         'vertical tile expansion                    write-only                            
  long  tv_ho         'horizontal offset                          write-only                            
  long  tv_vo         'vertical offset                            write-only                            
  long  tv_broadcast  'broadcast frequency (Hz)                   write-only                            
  long  tv_auralcog   'aural fm cog                               write-only                            

PUB main | okay, w, c, cl

  wordfill(@screen, $3278, screensize)                  ' Red "x"es
  tvStart(12)

  okay := bt.start(20, 21, 22, 23, string("Test Device"))
  if not okay
    repeat

  bt.defineRegion(@screen_desc, @screen)
  bt.defineRegion(@foo_desc, @foo)

  wordfill(@screen, $220, screensize)                   ' Blue

  repeat
    foo[0]++
  

PUB tvStart(basepin) : okay

'' Start terminal - starts a cog
'' returns false if no cog available

  setcolors(@palette)
  
  longmove(@tv_status, @tv_params, tv_count)
  tv_pins := (basepin & $38) << 1 | (basepin & 4 == 4) & %0101
  tv_screen := @screen
  tv_colors := @colors
  
  okay := tv.start(@tv_status)  

PRI setcolors(colorptr) | i, fore, back

'' Override default color palette
'' colorptr must point to a list of up to 8 colors
'' arranged as follows:
''
''               fore   back
''               ------------
'' palette  byte color, color     'color 0
''          byte color, color     'color 1
''          byte color, color     'color 2
''          ...

  repeat i from 0 to 7
    fore := byte[colorptr][i << 1]
    back := byte[colorptr][i << 1 + 1]
    colors[i << 1]     := fore << 24 + back << 16 + fore << 8 + back
    colors[i << 1 + 1] := fore << 24 + fore << 16 + back << 8 + back

DAT

foo_desc      long      16
              byte      "Foo", 0

screen_desc   long      screensize/2
              byte      "Screen", 0

tv_params               long    0               'status
                        long    1               'enable
                        long    0               'pins
                        long    %10010          'mode
                        long    0               'screen
                        long    0               'colors
                        long    cols            'hc
                        long    rows            'vc
                        long    4               'hx
                        long    1               'vx
                        long    0               'ho
                        long    0               'vo
                        long    0               'broadcast
                        long    0               'auralcog


                        '       fore   back
                        '       color  color
palette                 byte    $07,   $0A    '0    white / dark blue
                        byte    $07,   $BB    '1    white / red
                        byte    $9E,   $9B    '2   yellow / brown
                        byte    $04,   $07    '3     grey / white
                        byte    $3D,   $3B    '4     cyan / dark cyan
                        byte    $6B,   $6E    '5    green / gray-green
                        byte    $BB,   $CE    '6      red / pink
                        byte    $3C,   $0A    '7     cyan / blue