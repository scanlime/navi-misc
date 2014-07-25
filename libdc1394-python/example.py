#!/usr/bin/env python
#
# Capture frames from the default camera, displaying them using pygame
#

import dc1394
import pygame

camera = dc1394.findCamera()
capture = dc1394.Capture(camera)

# Open a pygame window at the same size and color depth
# as the capture's framebuffer
screen = pygame.display.set_mode(capture.frame.shape[:2], 0,
                                 capture.frame.shape[2]*8)
while 1:
    pygame.surfarray.blit_array(screen, capture.nextFrame())
    pygame.display.flip()
