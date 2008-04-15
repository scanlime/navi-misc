CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

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

  
OBJ
  serial        : "FullDuplexSerial"
  ow            : "OneWire"
  fp            : "FloatString"
  f             : "FloatMath"
  vcm           : "VoiceCoil"

VAR
  long  vcmCogStack[32]
  
PUB main | tempC, tempF

  dira[X_LED_PIN]~~
 
  dira[BT_CTS_I_PIN]~~
  outa[BT_CTS_I_PIN]~~

  serial.start(BT_TX_O_PIN, BT_RX_I_PIN, %0000, 9600)
  ow.start(X_ONEWIRE_PIN)
  vcm.start(X_DIR_PIN, X_PWM_PIN)  

  cognew(vcmCog, @vcmCogStack)
  
  repeat
    serial.str(string(10, 13, "Reading temperature...", 10, 13, 10, 13))

    tempC := getTemperature                             ' get temperature in celsius
    tempF := f.FAdd(f.FMul(tempC, 1.8), 32.0)           ' fahrenheit = (celsius * 1.8) + 32 
    serial.str(fp.FloatToString(tempC))
    serial.str(string(" C", 10, 13))
    serial.str(fp.FloatToString(tempF))
    serial.str(string(" F", 10, 13))

PRI vcmCog | char, period, freq
  freq := 10
  period := clkfreq / freq

  repeat
    char := serial.rxcheck
    repeat while char > 0
      if char == "["
        freq++
      elseif char == "]"
        freq--
      char := serial.rxcheck
      period := clkfreq / freq

    vcm.pulse(100)
    waitcnt(cnt + period)    
    vcm.pulse(-100)
    waitcnt(cnt + period)    
    
PRI getTemperature : temp
  '' Read the temperature from a DS18B20 sensor.
  '' Blocks while the conversion is taking place. Returns
  '' a floating point value, in Celsius.
  ''
  '' Assumes there is only one device on the bus.

  ow.reset
  ow.writeByte(SKIP_ROM)
  ow.writeByte(CONVERT_T)

  ' Wait for the conversion
  outa[X_LED_PIN]~~
  repeat
    waitcnt(cnt+clkfreq/1000*25)
    if ow.readBits(1)
      quit
  outa[X_LED_PIN]~

  ow.reset
  ow.writeByte(SKIP_ROM)
  ow.writeByte(READ_SCRATCHPAD)
  temp := ow.readByte + ow.readByte << 8                ' read temperature
  temp := F.FDiv(F.FFloat(temp), 16.0)                  ' convert to floating point