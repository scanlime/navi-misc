#!/usr/bin/env python
import rasterwand
import Image, ImageDraw
import time, math


class Animation:
    def __init__(self, dev,
                 numColumns   = 40,
                 displayWidth = 0.6,
                 dutyCycle    = 0.2
                 ):
        self.dev = dev
        dev.params.display_width = 0xFFFF * displayWidth
        dev.params.duty_cycle = 0xFFFF * dutyCycle
        self.numColumns = numColumns
        self.image = Image.new("1", (numColumns, 8))
        self.draw = ImageDraw.Draw(self.image)

    def run(self):
        then = time.time()
        while 1:
            now = time.time()
            self.image.paste(0)
            self.update(now - then)
            then = now
            self.dev.writeImage(self.image)


class Pacman(Animation):
    def update(self, dt):
        self.updatePacman(dt)
        self.updateDots(dt)

    theta = 0
    def updatePacman(self, dt):
        self.theta += dt * 15
        mouth_angle = (math.sin(self.theta) * 0.5 + 0.5) * 160
        self.draw.pieslice((0,0,8,8), mouth_angle/2.0,
                           360-mouth_angle/2.0, fill=255)

    dot_offset = 0
    def updateDots(self, dt):
        self.dot_offset -= dt * 25
        dot_spacing = 6
        self.dot_offset %= dot_spacing
        for i in xrange(10):
            self.draw.rectangle((self.dot_offset+7, 3,
                                 self.dot_offset+8, 4), fill=255)
            self.dot_offset += dot_spacing


if __name__ == "__main__":
    dev = rasterwand.Device()
    Pacman(dev).run()

