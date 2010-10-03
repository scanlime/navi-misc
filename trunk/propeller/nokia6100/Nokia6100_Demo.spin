{{

Demonstration of the Nokia 6100 LCD driver.

Micah Elizabeth Scott <micah@navi.cx>

}}

OBJ
  lcd : "Nokia6100"

CON
  #3, PIN_LED, PIN_RST, PIN_DATA, PIN_SCLK, PIN_CS

PUB main

  lcd.start(PIN_RST, PIN_DATA, PIN_SCLK, PIN_CS, lcd#EPSON)
  dira[PIN_LED]~~

  lcd.background(lcd#BLUE)
  lcd.clear
  lcd.on
  
  lcd.color(lcd#YELLOW)
  lcd.text(5, 80, string("Hello World! <3"))

  lcd.color(lcd#WHITE)
  lcd.box(1, 0, 100, 9)
  lcd.color(lcd#CYAN)
  lcd.box(21, 10, 80, 9)
  lcd.color(lcd#BLUE)
  lcd.box(41, 20, 60, 9)
  lcd.color(lcd#GREEN)
  lcd.box(61, 30, 40, 9)
  lcd.color(lcd#RED)
  lcd.box(81, 40, 20, 9)
  
  repeat    