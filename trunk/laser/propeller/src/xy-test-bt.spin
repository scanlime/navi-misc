
OBJ
  prox[2] : "OpticalProximity"
  vcm[2] : "VoiceCoil"
  bt : "BluetoothConduit"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  PATTERN_SIZE = 2048
  PATTERN_STRIDE = 2
  
VAR
  long posAddr[2]
  long lastPattern[2], lastDpv[2], lastError[2]

  ' Params
  word pattern[PATTERN_SIZE]                        
  long P[2], DP[2], center[2], minPos[2], unstickGain, scale[2], reps, D[2]


DAT

pattern_desc  long      PATTERN_SIZE * PATTERN_STRIDE / 4
              byte      "pattern", 0      

pos_x_desc    long      1
              byte      "pos_x", 0

pos_y_desc    long      1
              byte      "pos_y", 0

params_desc   long      14
              byte      "params", 0                 
              
  
PUB main | i, r, loopCnt, period, s, pa, okay, pos, pv, e, de, dpv, target

  okay := bt.start(20, 21, 22, 23, string("Test Device"))
  if not okay
    repeat

  prox[0].start(4,5)
  prox[1].start(16,17)
  vcm[0].start(6,8)
  vcm[1].start(2,0)

  repeat i from 0 to 1
    posAddr[i] := prox[i].getOutputAddress
  
  dira[9]~~
  outa[9]~

  bt.defineRegion(@pattern_desc, @pattern)
  bt.defineRegion(@pos_x_desc, posAddr[0])
  bt.defineRegion(@pos_y_desc, posAddr[1])
  bt.defineRegion(@params_desc, @P)

  P[0] := 3839442
  P[1] := 3839442
  D[0] := 91273997
  D[1] := 67713787
  DP[0] := 0
  DP[1] := 0
  center[0] := 67190
  center[1] := 98604
  minPos[0] := 10000
  minPos[1] := 35253
  unstickGain := 200000000
  scale[0] := 307
  scale[1] := 321
  reps := 0
  
  period := clkfreq / 1000
  loopCnt := cnt + period
  s := @pattern[2]

  repeat

    repeat reps+1
  
      repeat i from 0 to 1
        pos := LONG[posAddr[i]]

        if pos < minPos[i]
         vcm[i].pulse(((minPos[i] - pos) ** unstickGain))
        else
         pv := ~~WORD[s+2*i] * scale[i]
         target := center[i] - pv
         dpv := pv - lastPattern[i]       

         e := target - pos
         de := e - lastError[i]
         lastError[i] := e
         
         vcm[i].pulse((e ** P[i]) + (de ** D[i]) + ((lastDpv[i] - dpv) ** DP[i]))

         lastPattern[i] := pv
         lastDpv[i] := dpv
      
      waitcnt(loopCnt += period)
      outa[9] := s => @pattern[10]

    s += 4
    if s => @pattern + (4 * WORD[@pattern])
      s := @pattern[2]
        
  