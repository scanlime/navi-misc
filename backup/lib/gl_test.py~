#!/usr/bin/python
import fieldbox, time, oss, os
from threading import *
from math import *

class fieldThread(Thread):
    def run(self):
        self.running = True
        sensor = fieldbox.sensor()
        while self.running:
            self.packet = sensor.read()

fthread = fieldThread()
fthread.packet = [0]*8
fthread.start()

sound = oss.open_audio("/dev/dsp", os.O_WRONLY)
sound.reset()
sound.stereo(1)
rate = 44100
sound.speed(rate)

try:
    t = 0
    freq = 1/44100.0
    while True:
        left = 0
        right = 0
        for i in range(0,4):
            right += sin(t*5000*pow(2,i))*fthread.packet[i]
            left += sin(t*5000*pow(2,i))*fthread.packet[i+4]
        sound.write(chr(left*127+127) + chr(right*127+127))
        t += 1.0/rate
finally:
    fthread.running = False
