'
' To do:
'
'   VCM: Nothing. Works great!
'
'   Position sensor: ?
'
'   Control system:
'      http://en.wikipedia.org/wiki/PID_controller
'
' LaserProp Test Harness
'
' 1. TSL230R S0   BLK     P0
' 2. TSL230R S1   WHT     P1
' 3. TSL230R S2   GRY     P2
' 4. TSL230R S3   PUR     P3
' 5. +5v          BLU
' 6. TSL230R OUT  GRN     P4
' 7. GND          YEL
' 8. LED Anode    ORG     P5 (100 ohm series)
' 9. VCM-         RED
' 10. VCM+        BRN     P6 dir, P7 brake, P8 pwm (10k pulldown)
'

OBJ
  prox : "OpticalProximity"
  vcm : "VoiceCoil"
  vp : "ViewPortConduit_20"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  EDGE_MIN = 10_000_000     ' Below this value, we apply emergency corrective gain
  USABLE_MIN = 15_500_000   ' Bottom of normal operating range
  USABLE_MAX = 30_000_000   ' Top of normal operating range
  EDGE_MAX = 35_000_000     ' Above this value, we apply emergency corrective gain

  LASER_PIN = 9
  
  INTEGRATION_WINDOW_SIZE = 128
  
VAR
  long pos, error, iE, P, I, D, edgeGain, rampSpeed, velocity, predictor, accelWinTotal, corrWinTotal
  long target
  long correctionTotal
  
  long iWindow[INTEGRATION_WINDOW_SIZE]
  long iWindowPos

  long pAccelWindow[512]
  long pCorrectionWindow[512]
  long pWindowPos

  long controlLoopStack[32]
  long predictorStack[32]
  
PUB main | wavePos

  vp.general(string("mode=dso"))
  vp.share(@pos, @corrWinTotal)
  vp.channels(string("pos,error,iE,P,I,D,edgeGain,rampSpeed,velocity,predictor,accelWinTotal,corrWinTotal"))
  vp.view(string("pos,error,velocity,predictor,accelWinTotal,corrWinTotal"))
  vp.edit(string("P=10000(0..10000),I=0(0..100000),D=0(0..1000000),rampSpeed=22(0..500),edgeGain=30000"))
  
  P := 10_000
  I := 0
  D := 0
  iE := 0
  edgeGain := 30_000
  rampSpeed := 22
  
  cognew(predictorLoop, @predictorStack)
  cognew(controlLoop, @controlLoopStack)
  
  vp.start

  wavePos := $8000
  repeat
    wavePos := (wavePos + rampSpeed) & $FFFF
    
    ' Sawtooth wave    
    target := wavePos

    ' Square wave
    'if wavePos > $8000
    '  target := $A000
    'else
    '  target := $6000  

PRI predictorLoop | iter, ppos, lastPos, lastCorr, nextCorr, lastVel, accel, correction
  prox.start(4,5)

  repeat
    lastPos := ppos  
    ppos := prox.read
    lastVel := velocity
    velocity := ppos - lastPos
    accel := velocity - lastVel

    nextCorr := correctionTotal
    correction := nextCorr - lastCorr
    lastCorr := nextCorr

    if velocity < 0
      -velocity
      -correction

    accelWinTotal += accel - pAccelWindow[pWindowPos]      
    pAccelWindow[pWindowPos] := accel

    corrWinTotal += correction - pCorrectionWindow[pWindowPos]
    pCorrectionWindow[pWindowPos] := correction

    pWindowPos := (pWindowPos + 1) & 511

    
    predictor := accelWinTotal * correction / corrWinTotal
  
    
PRI controlLoop | target32, prevError, scaledError, absCorrection, correction, dE, loopCnt, period, proxAddr
  vcm.start(6,8)

  loopCnt := cnt
  period := clkfreq / 1000
  proxAddr := prox.getOutputAddress
  
  repeat
    pos := long[proxAddr]

    ' ***************** Error Calculation *****************

    ' Scale the target
    target32 := target * constant((USABLE_MAX - USABLE_MIN) / $10000) + USABLE_MIN

    prevError := error               
    error := pos - target32
    
    if pos < EDGE_MIN or pos > EDGE_MAX

      ' ******************* Edge control loop *******************      
      
      correction := error ** edgeGain

    else
      ' ******************* PID control loop ********************
      
      scaledError := error / $100
      iE += scaledError - iWindow[iWindowPos]
      iWindow[iWindowPos] := scaledError
      iWindowPos := (iWindowPos + 1) & constant(INTEGRATION_WINDOW_SIZE - 1)

      dE := error - prevError

      correction := error ** P + iE ** I + dE ** D

    vcm.pulse(-correction)
    correctionTotal += correction
    
    waitcnt(loopCnt += period)       
    