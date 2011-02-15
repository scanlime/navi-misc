CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  TV  = 12
  SCL = 4
  LED = 6

  ' Registers
  CTRL          = $00
  CONFIG        = $01
  CAP_RED       = $06
  CAP_GREEN     = $07
  CAP_BLUE      = $08
  CAP_CLEAR     = $09
  INT_RED_LO    = $0A
  INT_RED_HI    = $0B
  INT_GREEN_LO  = $0C
  INT_GREEN_HI  = $0D
  INT_BLUE_LO   = $0E
  INT_BLUE_HI   = $0F
  INT_CLEAR_LO  = $10
  INT_CLEAR_HI  = $11
  DATA_RED_LO   = $40
  DATA_RED_HI   = $41
  DATA_GREEN_LO = $42
  DATA_GREEN_HI = $43
  DATA_BLUE_LO  = $44
  DATA_BLUE_HI  = $45
  DATA_CLEAR_LO = $46
  DATA_CLEAR_HI = $47
  OFFSET_RED    = $48
  OFFSET_GREEN  = $49
  OFFSET_BLUE   = $4A
  OFFSET_CLEAR  = $4B

  ' Register bits
  CTRL_GSSR     = 1 << 0
  CTRL_GOFS     = 1 << 1
  CONFIG_TOFS   = 1 << 0
  CONFIG_SLEEP  = 1 << 1
  CONFIG_EXTCLK = 1 << 2  
  
OBJ
  text : "tv_text"
  i2c : "Basic_I2C_Driver"
  term : "Parallax Serial Terminal"

PUB start | i, color[4]
  text.start(TV)
  term.start(115200)
  i2c.initialize(SCL)
  dira[LED]~~
  outa[LED]~~

  repeat
    differential(4, $1FF, @color)
    
    text.str(string(1, "Color:"))
    repeat i from 0 to 3
      text.out(13)
      text.hex(color[i], 8)
      term.char(" ")
      term.hex(color[i], 8)
    term.char(10)

      
PRI differential(reps, time, buffer) | fg[4], bg[4], i, settle_delay
  ' Take a differential reading, once with the light off and once with it on,
  ' in order to measure the reflected color of an object in front of the sensor.

  longfill(buffer, 0, 4)

  repeat reps
    outa[LED]~~
    integrate(time, @fg)

    outa[LED]~
    integrate(time, @bg)

    repeat i from 0 to 3
      LONG[buffer][i] += fg[i] - bg[i]

  repeat i from 0 to 3      
    LONG[buffer][i] #>= 0

PRI integrate(time, buffer) | t
  ' Integrate the incident light for a given number of timeslots

  longfill(buffer, 0, 4) 
  repeat while time > 0
  
    t := time <# $FFF
    time -= t
    
    regWrite16(INT_RED_LO, t)
    regWrite16(INT_GREEN_LO, t)
    regWrite16(INT_BLUE_LO, t)
    regWrite16(INT_CLEAR_LO, t)

    regWrite(CTRL, CTRL_GSSR)
    repeat while regRead(CTRL)  
    
    LONG[buffer] += regRead16(DATA_RED_LO)
    LONG[buffer][1] += regRead16(DATA_GREEN_LO)
    LONG[buffer][2] += regRead16(DATA_BLUE_LO)
    LONG[buffer][3] += regRead16(DATA_CLEAR_LO)
  
PRI regWrite(addr, data)
  i2c.start(SCL)
  i2c.write(SCL, $E8)
  i2c.write(SCL, addr)
  i2c.write(SCL, data)
  i2c.stop(SCL)

PRI regRead(addr)
  i2c.start(SCL)
  i2c.write(SCL, $E8)      ' Write address
  i2c.write(SCL, addr)
  i2c.start(SCL)           ' Repeated start condition
  i2c.write(SCL, $E9)
  result := i2c.read(SCL, 0)
  i2c.stop(SCL)

PRI regWrite16(addr, data)
  regWrite(addr, data & $FF)
  regWrite(addr + 1, data >> 8)

PRI regRead16(addr)
  result := regRead(addr) | (regRead(addr + 1) << 8)
