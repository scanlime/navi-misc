#!/usr/bin/env python
import rasterwand
import Image, ImageDraw, ImageFont
import time, math

dev = rasterwand.Device()
frame = Image.new("1", (40,8))
draw = ImageDraw.Draw(frame)
font = ImageFont.load("data/helvetica8.pil")

t0 = time.time()
while True:
    t = time.time() - t0
    frame.paste(0)

    draw.rectangle((0,0,39,7), outline=1)
    draw.text((2,-2), time.strftime("%H:%M:%S"), fill=1, font=font)

    #theta = t*10
    #radius = 4
    #draw.line((20 + math.cos(theta)*radius, 3.99 + math.sin(theta)*radius,
    #           20 - math.cos(theta)*radius, 3.99 - math.sin(theta)*radius),
    #          fill=1)

    dev.writeImage(frame)
