
OBJ
  prox[2] : "OpticalProximity"
  vcm[2] : "VoiceCoil"
  vp : "ViewPortConduit_20"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000
  
VAR
  long pos[2], P[2], target[2], center[2], DP[2], patSize
  long posAddress[2], lastPattern[2]
  long minPos[2], unstickGain[2]
  
PUB main | i, loopCnt, period, s, pattern

  vp.general(string("mode=dso"))
  vp.share(@pos, @patSize)
  vp.channels(string("pos_x,pos_y,P_x,P_y,target_x,target_y,center_x,center_y,DP_x,DP_y,patSize"))
  vp.view(string("pos_x,target_x,pos_y,target_y"))
  vp.edit(string("center_x=17000000(0..40000000),center_y=17000000(0..40000000),P_x=4000(0..100000),P_y=4000(0..100000),DP_x=0(0..100000),DP_y=0(0..100000),patSize=0(0..10000000)"))

  prox[0].start(4,5)
  prox[1].start(16,17)
  vcm[0].start(6,8)
  vcm[1].start(2,0)

  repeat i from 0 to 1
    posAddress[i] := prox[i].getOutputAddress
  
  dira[9]~~
  outa[9]~~

  unstickGain[0] := 0
  unstickGain[1] := 0
  minPos[0] := 0
  minPos[1] := 0

  P[0] := 0000
  P[1] := 0000
  center[0] := 19_022_218
  center[1] := 11_999_992
  DP[0] := 0
  DP[1] := 0
  patsize := 13_333_333

  vp.start


  period := clkfreq / 2000
  loopCnt := cnt + period
  s := @patternData

  repeat
    
    repeat i from 0 to 1
      pos[i] := long[posAddress[i]]
      if pos[i] < minPos[i]
        vcm[i].pulse(((minPos[i] - pos[i]) ** unstickGain[i]))
      else
        pattern := long[s] * patSize
        target[i] := center[i] + pattern       
        vcm[i].pulse(((target[i] - pos[i]) ** P[i]) + ((pattern - lastPattern[i]) ** DP[i]))

        lastPattern[i] := pattern

      s += 4

    waitcnt(loopCnt += period)

    if s == @patternData_end
      s := @patternData

DAT

patternData
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1
            long  1, -1

            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1
            long  -1, -1

            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1
            long  -1, 1

            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1
            long  1, 1                     
patternData_end
        
  