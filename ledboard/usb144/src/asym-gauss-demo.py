import math, random


class Histogram:
    height = 9
    width = 16

    def __init__(self):
        self.reset()

    def reset(self):
        self.buckets = [0] * (self.width * self.height)

    def plot(self, x, y):
        """Plot a point on the histogram. Returns the number
           of units of brightness contributed by this point:
           1 if it landed on the histogram and didn't saturate,
           0 if not.
           """
        x = int(x + 0.5)
        y = int(y + 0.5)

        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return 0

        i = x + y * self.width
        if self.buckets[i] < 15:
            self.buckets[i] += 1
            return 1
        else:
            return 0

    def gauss(self, x, y, width=1.5, aspect=1.0, angle=0, brightness=500):
        """Plot a gaussian distribution which contributes
           'brightness' total units of light, centered at
           (x, y), with a standard deviation of 'width'.
           """
        # Compute the asymmetric standard deviations for each axis
        xw = width * aspect
        yw = width / aspect

        # Precalculate sine/cosine of angle
        s = math.sin(angle)
        c = math.cos(angle)

        while brightness > 0:
            xg = random.gauss(0, xw)
            yg = random.gauss(0, yw)

            # Rotate and plot
            brightness -= self.plot(x + xg*c - yg*s,
                                    y + xg*s + yg*c)

    def render(self):
        return ''.join(["%1x" % min(15, p) for p in self.buckets])


if __name__ == "__main__":
    h = Histogram()

    frame = 0
    while True:
        frame += 1

        angle = frame * 0.01
        width = 2.0 + math.sin(frame * 0.1) * 1.5

        h.reset()
        h.gauss(4, 4, width=width, aspect=1, angle=angle, brightness=200)
        print h.render()
