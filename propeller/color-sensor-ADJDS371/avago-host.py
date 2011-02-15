import serial
import pygame
import sys

port = serial.Serial(port="COM23", baudrate="115200")

pygame.init()
surface = pygame.display.set_mode((512, 512))

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit()

    color = [int(s, 16) for s in port.readline().split()]

    s = 0.5 
    whitePoint = [3910.0/3704, 1, 3910.0/2541]

    rgb = [color[i] * whitePoint[i] * s for i in range(3)]
    m = max(*rgb)
    if m > 255:
        rgb = [rgb[i] * 255.0 / m for i in range(3)]

    print color, rgb

    surface.fill(rgb)
    pygame.display.flip()
