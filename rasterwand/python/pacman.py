#!/usr/bin/env python
import rasterwand
import time, math


class Pacman(rasterwand.AnimatedImage):
    def update(self, dt):
        self.updatePacman(dt)
        self.updateDots(dt)

    theta = 0
    def updatePacman(self, dt):
        self.theta += dt * 15
        mouth_angle = (math.sin(self.theta) * 0.5 + 0.5) * 160
        self.draw.pieslice((0,0,8,8), int(mouth_angle/2.0+0.5),
                           int(360-mouth_angle/2.0+0.5), fill=255)

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
    Pacman(rasterwand.Device()).run()

