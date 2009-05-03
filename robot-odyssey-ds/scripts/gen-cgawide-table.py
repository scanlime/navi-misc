#
# Generate the lookup table for CGA widepixels -> 16 color
#

# Mapping from a pair of CGA colors to a palette entry.
# See VideoConvert::palette.

pairMap = {
    (0,0): 0,
    (0,1): 4,
    (0,2): 5,
    (0,3): 6,
    (1,0): 4,
    (1,1): 1,
    (1,2): 7,
    (1,3): 8,
    (2,0): 5,
    (2,1): 7,
    (2,2): 2,
    (2,3): 9,
    (3,0): 6,
    (3,1): 8,
    (3,2): 9,
    (3,3): 3,
    }

def cgaUnpack(byte):
    return ((byte >> 6) & 3,
            (byte >> 4) & 3,
            (byte >> 2) & 3,
            (byte >> 0) & 3)

def pack16(a, b):
    return (a | (b << 4))

for row in range(32):
    for col in range(8):
        byte = col | (row << 3)

        a0, a1, b0, b1 = cgaUnpack(byte)
        a = pairMap[(a0, a1)]
        b = pairMap[(b0, b1)]

        print "0x%02x," % pack16(a, b),
    print
