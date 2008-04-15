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
  light : "TSL230R"
  serial : "FullDuplexSerial"
  fm : "FloatMath" 
  
CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  ' At the moment, we have no need to use sensitivity and divisor
  ' values other than the defaults. We get lower jitter by integrating
  ' over multiple periods in software.

  SENSITIVITY = 100
  DIVISOR = 1
  INTEGRATION_PERIODS = 16
                    
VAR
  long lightCogStack[16]
  long motorCogStack[16]
  long ledDiff            
  long calMin, calMax
  
PRI lightCog | p1, p2
  ' LED output
  dira[5]~~

  light.start(0)

  light.setSensitivity(SENSITIVITY)
  light.setDivisor(DIVISOR)

  repeat
    p1 := p2 := 0
  
    outa[5]~~
    light.discontinuity    
    repeat INTEGRATION_PERIODS
      p1 += light.measurePeriod

    outa[5]~
    light.discontinuity    
    repeat INTEGRATION_PERIODS
      p2 += light.measurePeriod

    ' With this sensor, light is proportional to frequency. We have two periods,
    ' but we want to calculate the absolute difference in light irradiance between
    ' these two measurements. Calculate the difference in frequency:
    '
    ' f1 = 1 / p1
    ' f2 = 1 / p2
    ' freqDiff = f1 - f2 = (1 / p1) - (1 / p2) = (p2 - p1) / (p1 * p2)
    '
    ' This turns out to be a pretty small number, so we represent it
    ' using 0.32 fixed-point notation.
     
    ledDiff := (p2 - p1) / (p1 ** p2)

                 
PRI vcmInit
  dira[6]~~
  dira[7]~~
  dira[8]~~

PRI vcmPark
  outa[8]~
  outa[6]~
  outa[7]~~  ' Brakes on

PRI vcmHomeMinimum
  outa[6]~~
  outa[7]~

  outa[8]~~
  waitcnt(clkfreq/20 + cnt)
   
PRI vcmHomeMaximum
  outa[6]~
  outa[7]~

  outa[8]~~
  waitcnt(clkfreq/20 + cnt)

PRI vcmOff
  outa[8]~

PRI calibrate
  vcmHomeMaximum
  ledDiff := 0
  repeat until ledDiff
  calMax := ledDiff
  vcmOff

  vcmHomeMinimum
  ledDiff := 0
  repeat until ledDiff
  calMin := ledDiff
  vcmPark
  
PUB main | lp1, lp2, diff

  serial.start(31, 30, 0, 115200)
  cognew(lightCog, @lightCogStack)

  vcmInit
  
  repeat
'     calibrate
 
'     serial.dec(calMin)
'     serial.str(string(", "))
'     serial.dec(calMax)

     repeat ledDiff / 1000
        serial.tx("*")

     serial.str(string(10, 13))         
  