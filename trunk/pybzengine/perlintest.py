#!/usr/bin/env python
#
# Test the perlin noise generator
#
from BZFlag.UI import Viewport
from BZFlag import Event, Noise
from Numeric import *
import pygame

# Test the perlin noise in a few different dimensions on individual vectors
for dimension in xrange(1, 10):
    print Noise.PerlinNoise(dimension).get( [1.2] * dimension )

# Create a viewport for displaying animated perlin noise
loop = Event.EventLoop()
viewport = Viewport.PygameViewport(loop, (96,96), resizable=False)

n = Noise.PerlinNoise(3)

# Set up the input array for perlin noise with x and y values
# proportional to the pixel locations. We'll vary the z value every frame.
i = indices(viewport.size) / 20.0
input = zeros(viewport.size + (3,), Float)
input[:,:,0] = i[0]
input[:,:,1] = i[1]

def drawFrame():
    # Vary the Z axis of the noise with time so the noise will
    # continuously change and appear to never repeat.
    input[:,:,2] += 0.1

    # Render some noise such that -1 and 1 get mapped to black and white, respectively.
    noise = n.get(input) * 127 + 127

    # Convert to grayscale and blit to the screen
    img = repeat(noise.astype(UInt8)[...,NewAxis], 3, -1)
    pygame.surfarray.blit_array(viewport.screen, img)

viewport.onDrawFrame.observe(drawFrame)
loop.run()
