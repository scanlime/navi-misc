{{
  Special-purpose firmware for the Unicone2 base station:
  A script to cheat at the basketball minigame in Final Fantasy 7

  This was prototyped with the special_duration_timer firmware, but this
  is a simpler script that just replays a preset sequence of events to
  play one game of basketball.
  
  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000

  SHOOT_DURATION = 36_500_000
  TAP_DURATION   =  8_000_000
            
OBJ

  Rem : "psx_remote_port"
  Emu : "psx_controller_emulator"
  Term : "Parallax Serial Terminal"

VAR
  long  output_buf[Emu#STATE_BUFFER_LEN]
                     
PUB Main | in_buttons, prev_in, changed, down_ts, state, autorepeat_en, autorepeat_deadline, saved_len

  term.start(115200)

  Rem.start(0, 1)
  Emu.start(8, 4, Emu#CONTROLLER_DUAL_SHOCK)
 
  Emu.add_state_buffer(Rem.get_state_buffer(0))
  Emu.init_state_buffer(@output_buf)
  Emu.add_state_buffer(@output_buf)

  repeat until term.CharIn == term#NL
    term.str(String(term#CS, "Ready to play. Press enter to start.", term#NL))

  repeat
    PlayGame

PUB PlayGame | dunkCount

  term.str(string("Starting game", term#NL))
  Select

  WaitMS(1000)    ' "Basketball Game", 1 game 200 gil
  Select

  WaitMS(1000)    ' Hold down the circle button...
  Select

  WaitMS(1000)    ' "Try it"
  Select

  WaitMS(3500)    ' Wait to get into position

  dunkCount~

  ' Four doubling-up rounds are possible
  repeat 4
  
    ' Every 10 shots it lets us double up
    repeat 10
      term.str(string("Dunk #"))
      term.dec(++dunkCount)
      term.char(term#NL)

      Dunk
      WaitMS(5600)

    term.str(string("Double up!", term#NL))
    Select         ' Double chance!
    WaitMS(1500)   ' Double your current GP...
    Select
    WaitMS(1500)   ' You may cotinue after you successfully double..
    Select
    WaitMS(500)    ' Yes/No
    Select

    WaitMS(3000)
    Dunk
    WaitMS(5600)

    Select         ' Double up!
    WaitMS(5600)

  Select           ' Give up
  WaitMS(4000)

  Select           ' Received 300 GP
  WaitMS(4000)


PUB Dunk
  term.str(string("-- dunk", term#NL))
  PressButton(Rem#BTN_CIRCLE, SHOOT_DURATION)
  
PUB Select
  term.str(string("-- select", term#NL))
  PressButton(Rem#BTN_X, TAP_DURATION)

PUB WaitMS(ms)
  term.str(string("-- wait "))
  term.dec(ms)
  term.char(term#NL)
  waitcnt(cnt + clkfreq/1000*ms)
 
PUB PressButton(button, duration)
  WORD[@output_buf] := !button
  waitcnt(cnt + duration)
  WORD[@output_buf] := $FFFF
      