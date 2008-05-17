
OBJ
  prox[2] : "OpticalProximity"
  vcm[2] : "VoiceCoilServo"
  ow : "OneWire"
  vm : "VectorMachine"
  bt : "BluetoothConduit"
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  VECTOR_MEM_SIZE = 4096

  X_ONEWIRE_PIN = 1                                     ' Pins for axis actuators
  X_FREQ_PIN = 4
  X_LED_PIN = 5
  X_DIR_PIN = 6
  X_PWM_PIN = 8

  Y_ONEWIRE_PIN = 15
  Y_FREQ_PIN = 16
  Y_LED_PIN = 17
  Y_DIR_PIN = 2
  Y_PWM_PIN = 0

  BT_CTS_I_PIN = 23                                     ' Spark Fun bluetooth module
  BT_TX_O_PIN = 22
  BT_RX_I_PIN = 21
  BT_RTS_O_PIN = 20

  SKIP_ROM          = $CC                               ' 1-wire commands
  READ_SCRATCHPAD   = $BE
  CONVERT_T         = $44

VAR
  long  vector_mem[VECTOR_MEM_SIZE]
  long  therm[2]
  
DAT

vector_desc   long      VECTOR_MEM_SIZE
              byte      "vector_mem", 0      

vm_cmd_desc   long      1
              byte      "vm_cmd", 0

vm_out_desc   long      2
              byte      "vm_output", 0

pos_x_desc    long      1
              byte      "pos_x", 0

pos_y_desc    long      1
              byte      "pos_y", 0

params_x_desc long      vcm#NUM_PARAMS
              byte      "params_x", 0                 

params_y_desc long      vcm#NUM_PARAMS
              byte      "params_y", 0                 
              
therm_desc    long      2
              byte      "therm", 0

              
PUB main | okay, syncTime

  okay := bt.start(BT_RTS_O_PIN, BT_RX_I_PIN, BT_TX_O_PIN, BT_CTS_I_PIN, string("Test Device"))
  if not okay
    repeat

  bt.defineRegion(@vector_desc, @vector_mem)
  bt.defineRegion(@vm_cmd_desc, vm.getCmdAddress)
  bt.defineRegion(@vm_out_desc, vm.getOutputAddress(0))
  bt.defineRegion(@pos_x_desc, prox[0].getOutputAddress)
  bt.defineRegion(@pos_y_desc, prox[1].getOutputAddress)
  bt.defineRegion(@params_x_desc, vcm[0].getParams)
  bt.defineRegion(@params_y_desc, vcm[1].getParams)
  bt.defineRegion(@therm_desc, @therm)
  
  syncTime := cnt + clkfreq / 100

  vm.start(syncTime, vcm#LOOP_HZ, 9, @vector_mem, VECTOR_MEM_SIZE)
  prox[0].start(X_FREQ_PIN, X_LED_PIN)
  prox[1].start(Y_FREQ_PIN, Y_LED_PIN)
  vcm[0].start(X_DIR_PIN, X_PWM_PIN, syncTime, prox[0].getOutputAddress, vm.getOutputAddress(0))
  vcm[1].start(Y_DIR_PIN, Y_PWM_PIN, syncTime, prox[1].getOutputAddress, vm.getOutputAddress(1))

  repeat
    therm[0] := readTemperature(X_ONEWIRE_PIN)
    therm[1] := readTemperature(Y_ONEWIRE_PIN)
    

PUB readTemperature(pin) : temp
  '' Read the temperature from a DS18B20 sensor.
  '' Blocks while the conversion is taking place. Returns
  '' a floating point value, in Celsius.
  ''
  '' Assumes there is only one device on the bus.

  ow.start(pin)

  ow.reset
  ow.writeByte(SKIP_ROM)
  ow.writeByte(CONVERT_T)

  ' Wait for the conversion
  repeat
    waitcnt(cnt+clkfreq/1000*25)
    if ow.readBits(1)
      quit

  ow.reset
  ow.writeByte(SKIP_ROM)
  ow.writeByte(READ_SCRATCHPAD)
  temp := ow.readByte + ow.readByte << 8
