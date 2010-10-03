{{

Demonstration of the Nokia 6100 LCD driver.

Micah Elizabeth Scott <micah@navi.cx>

}}

OBJ
  lcd : "Nokia6100"
  num : "Simple_Numbers"

PUB main | i, c

  lcd.start(4, 5, 6, 7, lcd#EPSON)
  dira[3]~~

  c := lcd#WHITE
  repeat i from 0 to 64
    lcd.color(c)
    lcd.box(i, i, lcd#WIDTH-i*2, lcd#height-i*2)
    c ^= lcd#WHITE

  lcd.background(lcd#BLUE)  
  lcd.color(lcd#YELLOW)
  lcd.clear
  lcd.text(0, 0, string("Hello World! <3 abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789  ▶◀←→↑↓‣•"))

  i~
  repeat
    i++
    lcd.text(20, 100, num.hex(i,8))
  