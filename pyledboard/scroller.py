#!/usr/bin/env python
import ledboard, time
led = ledboard.Server()

pattern = ledboard.GridPattern([
    "o   o                       o   o                                 ",
    "o   o       o  o            o   o            o     o   o   o      ",
    "o   o       o  o            o   o            o     o              ",
    "ooooo  ooo  o  o   ooo      o   o  ooo  o oo o     o     o        ",
    "o   o o   o o  o  o   o     o o o o   o oo   o   ooo              ",
    "o   o ooooo o  o  o   o     o o o o   o o    o  o  o   o   o      ",
    "o   o o     o  o  o   o      o o  o   o o    o  o  o    ooo       ",
    "o   o  ooo  oo oo  ooo       o o   ooo  o    oo  ooo              ",
    ])

led.vfd.write("Hello World!\nText scrolling demo")

while True:
    pattern.set(led)
    pattern.scrollLeft()
    led.blit()
    time.sleep(0.08)
