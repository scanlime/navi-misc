import math, random

class Metaball:
    def __init__(self, x, y, xv=0, yv=0, scale=1.0):
        self.x = x + 0.01
        self.y = y + 0.01
        self.xv = xv
        self.yv = yv
        self.scale = float(scale)

    def evaluate(self, x, y):
        return self.scale / ((x - self.x) ** 2 + (y - self.y) ** 2)

    def animate(self):
        self.x += self.xv
        self.y += self.yv

class MetaballSystem:
    def __init__(self):
        self.balls = []
        self.min = 0.05
        self.max = 0.4
        self.gamma = 2.5

    def evaluate(self, x, y):
        return sum([b.evaluate(x, y) for b in self.balls])

    def render(self, width=16, height=9, newline=''):
        for b in self.balls:
            b.animate()

        pixels = []
        for y in range(height):
            for x in range(width):
                v = self.evaluate(x, y)
                v = min(1, max(0, (v - self.min) / (self.max - self.min)))
                v = pow(v, self.gamma)
                pixels.append("%x" % int(v * 15 + 0.5))
            pixels.append(newline)
        return ''.join(pixels)

def populateRandomMetaballs(mbsys,
                            numBalls=10,
                            margins=5,
                            width=16, height=9,
                            maxSpeed=0.2,
                            minScale=0.5, maxScale=1.0):

    for ball in iter(mbsys.balls):
        if (ball.x < -margins or
            ball.y < -margins or
            ball.x > width + margins or
            ball.y > height + margins):
            mbsys.balls.remove(ball)
        
    while len(mbsys.balls) < numBalls:
        x, y = random.choice((
            (-margins, random.uniform(0, height)),
            (width + margins, random.uniform(0, height)),
            (random.uniform(0, width), -margins),
            (random.uniform(0, width), height + margins),
            ))
        
        newBall = Metaball(x, y,
                           random.uniform(-maxSpeed, maxSpeed),
                           random.uniform(-maxSpeed, maxSpeed),
                           random.uniform(minScale, maxScale))
        mbsys.balls.append(newBall)

if __name__ == "__main__":
    mbsys = MetaballSystem()

    while True:
        populateRandomMetaballs(mbsys)
        print mbsys.render()
