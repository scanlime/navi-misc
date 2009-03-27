
import pygame.font

pygame.font.init()

fontSize = 9
yOffset = 2
f = pygame.font.Font("fonts/Vera.ttf", fontSize)

text = "Quack!"
surf = f.render(text, True, (15, 15, 15, 255), (0, 0, 0, 255))

while True:
    for xOffset in range(-16, surf.get_width()):
        pixels = []
        for y in range(9):
            for x in range(16):
                try:
                    pixels.append("%x" % surf.get_at((
                        x + xOffset, y + yOffset))[0])
                except IndexError:
                    pixels.append('0')
        print ''.join(pixels)
