
OBJ
  prox[2] : "OpticalProximity"
  vcm[2] : "VoiceCoilServo"
  bt : "BluetoothConduit"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  PATTERN_SIZE = 2048
  PATTERN_STRIDE = 2
  
VAR
  ' Params
  word pattern[PATTERN_SIZE]                        


DAT

pattern_desc  long      PATTERN_SIZE * PATTERN_STRIDE / 4
              byte      "pattern", 0      

pos_x_desc    long      1
              byte      "pos_x", 0

pos_y_desc    long      1
              byte      "pos_y", 0

params_x_desc long      vcm#NUM_PARAMS
              byte      "params_x", 0                 

params_y_desc long      vcm#NUM_PARAMS
              byte      "params_y", 0                 
              
  
PUB main | okay, p

  okay := bt.start(20, 21, 22, 23, string("Test Device"))
  if not okay
    repeat

  prox[0].start(4,5)
  prox[1].start(16,17)
  vcm[0].start(6,8, prox[0].getOutputAddress)
  vcm[1].start(2,0, prox[1].getOutputAddress)

  p := vcm[0].getParams
  LONG[p + vcm#PARAM_CENTER * 4] := 70000
  LONG[p + vcm#PARAM_P * 4] := $2000000
  
  bt.defineRegion(@pattern_desc, @pattern)
  bt.defineRegion(@pos_x_desc, prox[0].getOutputAddress)
  bt.defineRegion(@pos_y_desc, prox[1].getOutputAddress)
  bt.defineRegion(@params_x_desc, vcm[0].getParams)
  bt.defineRegion(@params_y_desc, vcm[1].getParams)
  