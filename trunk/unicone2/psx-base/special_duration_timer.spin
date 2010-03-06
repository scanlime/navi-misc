{{
  Special-purpose firmware for the Unicone2 base station:
  Accurately time out fixed-duration button presses.

  We use only port 1 on the base unit, and ports 1 and 2 on the
  first remote port. The first port is a normal controller, the
  second one operates the one-shot timer.

  The timer output is the circle button. Controls on port 2:
     Circle: Hold to manually time out a button press
     X:      Replay a circle press with the same duration as the last one
  
  -- Micah Dowty <micah@navi.cx>
}}

CON

  _CLKMODE = XTAL1 + PLL16X
  _XINFREQ = 5_000_000

  AUTOREPEAT_SEC = 3
  DEFAULT_DURATION = 36_500_000
            
OBJ

  Rem : "psx_remote_port"
  Emu : "psx_controller_emulator"
  Term : "Parallax Serial Terminal"

VAR
  long  output_buf[Emu#STATE_BUFFER_LEN]
                     
PUB Main | in_buttons, prev_in, changed, down_ts, state, autorepeat_en, autorepeat_deadline, saved_len

  Term.start(115200)

  Rem.start(0, 1)
  Emu.start(8, 4, Emu#CONTROLLER_DUAL_SHOCK)
 
  Emu.add_state_buffer(Rem.get_state_buffer(0))
  Emu.init_state_buffer(@output_buf)
  Emu.add_state_buffer(@output_buf)

  saved_len := DEFAULT_DURATION
  
  repeat
    prev_in := in_buttons
    in_buttons := !WORD[Rem.get_state_buffer(1)]
    changed := prev_in ^ in_buttons
  
    if changed & Rem#BTN_CIRCLE
      if in_buttons & Rem#BTN_CIRCLE
        ' Circle down
        down_ts := cnt
        state := Rem#BTN_CIRCLE
        saved_len := $10000000
      else
        ' Circle up
        saved_len := cnt - down_ts
        state := 0
        Term.Str(String(Term#NL, "New saved length: "))
        Term.Dec(saved_len)      

    if in_buttons & Rem#BTN_UP
      ' autorepeat on
      autorepeat_en := 1
      autorepeat_deadline := cnt + (clkfreq * AUTOREPEAT_SEC)
    if in_buttons & Rem#BTN_DOWN
      ' autorepeat off
      autorepeat_en := 0
      
    if (changed & Rem#BTN_X) and (in_buttons & Rem#BTN_X)
      ' X pressed: Trigger
      state := Rem#BTN_CIRCLE
      down_ts := cnt

    if autorepeat_en and (cnt - autorepeat_deadline) => 0
      ' Autorepeat reached: Trigger 
      state := Rem#BTN_CIRCLE
      down_ts := cnt
      autorepeat_deadline := cnt + (clkfreq * AUTOREPEAT_SEC)
      
    if state and (cnt - down_ts) => saved_len
      ' Release
      state := 0
       
    WORD[@output_buf] := !state
       