#!/usr/bin/env python
import ledboard, time, random
led = ledboard.Server()
colors = list(ledboard.colors)

led.vfd.write("Sparkly LEDs!")

while True:
    random.shuffle(colors)
    for color in colors:
        led.set(random.choice(led.select({'color': color})), 15)
        for i in range(5):
            led.blit()
            led.fade()
            time.sleep(0.02)
