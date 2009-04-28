columns = [[] for i in range(256)]

def scale(x):
    return x * 256 / 320.0

def plotSubpixel(x, i, coverage):
    columns[x].append((i, coverage))

def plotRange(i, x, y):
    while x < y:
        xi = int(x)
        width = min(1, y - xi)
        plotSubpixel(xi, i, width - (x - xi))
        x = xi + 1

for x in range(320):
    plotRange(x, scale(x), scale(x + 1))

def palette(r, g, b, coverage):
    r = int((r >> 3) * coverage + 0.5)
    g = int((g >> 3) * coverage + 0.5)
    b = int((b >> 3) * coverage + 0.5)
    assert r < 32
    assert g < 32
    assert b < 32
    return (b << 10) | (g << 5) | r

for column in columns:
    # Each column gets three samples.
    # Each sample has four 16-bit words:
    #   1. The original column number to sample
    #   2. Scaled palette color 1
    #   3. Scaled palette color 2
    #   4. Scaled palette color 3
    #
    # For no-op samples, all four words are zero.

    words = []
    assert len(column) == 2

    for x, coverage in column:
        words.extend([x,
                      palette(0x55, 0xFF, 0xFF, coverage),
                      palette(0xFF, 0x55, 0xFF, coverage),
                      palette(0xFF, 0xFF, 0xFF, coverage)])

    print ''.join(["0x%04x, " % w for w in words])

    # Test the scaling table.
    sum = 0
    for sample in range(2):
        sum += words[3 + sample*4]
    assert sum == 0x7FFF
