#!/usr/bin/env python

import Image, ImageDraw, ImageFont, random, string, math

im = Image.new("L", (192, 96))
im.paste("white", (0, 0) + im.size)
draw = ImageDraw.Draw(im)
font = ImageFont.truetype("data/fonts/vera/VeraBd.ttf", 30)

def readDict(filename="data/words/basic-english"):
    d = []
    for line in open(filename):
        line = line.strip()
        if len(line) == 5:
            d.append(line)
    return d

def outlinedText(x, y, txt,
                 border = 2,
                 borderColor = "black",
                 fill = "white"):
    draw.text((x-border,y-border), txt, font=font, fill=borderColor)
    draw.text((x+border,y+border), txt, font=font, fill=borderColor)
    draw.text((x,y), txt, font=font, fill=fill)

def randomText(txt):
    txtSize = font.getsize(txt)
    outlinedText(random.uniform(0, im.size[0] - txtSize[0]),
                 random.uniform(0, im.size[1] - txtSize[1]),
                 txt)

def warpy(iters=2, blockSize=4, sigma=0.4):
    for i in xrange(iters):
        for y in xrange(im.size[1]/blockSize):
            for x in xrange(im.size[0]/blockSize):
                rx = random.normalvariate(0, sigma)
                ry = random.normalvariate(0, sigma)
                square = im.crop((x*blockSize,
                                  y*blockSize,
                                  (x+1)*blockSize-1,
                                  (y+1)*blockSize-1))
                im.paste(square, (int(math.floor(x*blockSize+rx)),
                                  int(math.floor(y*blockSize+ry))))

def randomSquares(numBlocks=400, blockSize=3):
    for i in xrange(numBlocks):
        x = int(random.uniform(0, im.size[0]))
        y = int(random.uniform(0, im.size[0]))
        im.paste("black", (x, y, x+blockSize, y+blockSize))

def twistyText():
    randomSquares()
    word = random.choice(readDict())
    print word
    randomText(word)
    warpy()

twistyText()
im.save("output.png")
