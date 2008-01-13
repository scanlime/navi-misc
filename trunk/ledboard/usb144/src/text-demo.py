import pygame.font

pygame.font.init()

fontSize = 9
yOffset = 2
f = pygame.font.Font("fonts/Vera.ttf", fontSize)

text = "Hello World, in 12 point Bitstream Vera with antialiasing."
surf = f.render(text, True, (15, 15, 15, 255), (0, 0, 0, 255))

while True:
    for xOffset in range(-16, surf.get_width()):
        pixels = []
        for y in range(9):
            for x in reversed(range(16)):
                try:
                    pixels.append("%x" % surf.get_at((
                        x + xOffset, y + yOffset))[0])
                except IndexError:
                    pixels.append('0')
        print ''.join(pixels)
