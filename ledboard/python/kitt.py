#!/usr/bin/env python
import ledboard, time
led = ledboard.Server()
colors = list(ledboard.colors)

numFadesBetweenMovement = 8
delay = 0.03
ledgroup = led.select({'group': 'bars'})

brightLED = 0
direction = 1
fadeCounter = 0

while True:
    # ping-pong the bright LED
    fadeCounter += 1
    if fadeCounter > numFadesBetweenMovement:
        if brightLED == 0:
            direction = 1
        if brightLED == 7:
            direction = -1
        brightLED += direction
        fadeCounter = 0

    led.fade(ledgroup)
    led.set(led.select({'x': brightLED}, ledgroup), ledboard.full_bright)
    led.blit()
    time.sleep(delay)
