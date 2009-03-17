import math, random, sys


class WaveDisplay:
    height = 16
    width = 9

    taps = [i / float(width - 1) for i in range(width)]

    def __init__(self):
        self.rows = [[0] * self.width] * self.height

    def gauss(self, center, width=0.1, brightness=15):
        numBins = len(self.rows[-1])
        maxBin = numBins - 1
        bins = [0] * numBins

        for iter in xrange(1000):
            i = int(maxBin * random.gauss(center, width) + 0.5)
            if i >= 0 and i <= maxBin:
                bins[i] += 1
                if bins[i] >= brightness:
                    break

        for i, v in enumerate(bins):
            self.rows[-1][i] += bins[i]

    def render(self):
        pixels = []
        for x in range(self.width):
            for y in range(self.height):
                p = self.rows[y][x]
                p = min(15, max(0, int(0.5 + p)))
                pixels.append("%1x" % p)
        return ''.join(pixels)

    def nextRow(self):
        self.rows = self.rows[1:] + [[0] * self.width]


if __name__ == "__main__":
    disp = WaveDisplay()

    palette = {
        '-': dict(width=0.0, brightness=15),
        '+': dict(width=0.1, brightness=5),
        '*': dict(width=0.2, brightness=15),
        }

    while True:
        for row in [
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -    +  ",
            "     +   ",
            "      +  ",
            " -       ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -    +  ",
            "     +   ",
            "      +  ",
            " -       ",

            "         ",
            "         ",
            "    -    ",
            "         ",
            "         ",
            "    -    ",
            "         ",
            "         ",
            " -     + ",
            "  -   +  ",
            "    -    ",
            "  +   -  ",
            " +     - ",

            "         ",
            "         ",
            "    -    ",
            "         ",
            "         ",
            "    -    ",
            "         ",
            "         ",
            "*        *",
            "         ",
            "         ",
            " *       ",
            "         ",
            "         ",
            "       * ",
            "         ",
            "         ",
            "    *    ",
            "         ",
            "    *    ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "      *  ",
            "         ",
            "         ",
            " -       ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "      *  ",
            "         ",
            "         ",
            "      *  ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "      *  ",
            "     *   ",
            "    *    ",
            "   *     ",


            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "   *     ",
            "    *    ",
            "     *   ",
            "      *  ",

            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "       - ",
            "         ",
            "         ",
            " -       ",
            "         ",
            "         ",
            "       - ",

            "         ",
            "         ",
            "       - ",
            "         ",
            "         ",
            "       - ",
            "         ",
            "         ",
            " *       ",
            "         ",
            " *       ",
            "       - ",


            ]:

            for i in range(6):
                for j, c in enumerate(row):
                    if c in palette:
                        center = j / float(len(row) - 1)
                        disp.gauss(center, **palette[c])

                print disp.render()
                disp.nextRow()
