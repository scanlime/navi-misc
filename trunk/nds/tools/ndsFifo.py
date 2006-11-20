import struct

class FIFO:
    """Command IDs for the Nintendo DS graphics FIFO"""
    NOP               = chr(0x00)
    COLOR             = chr(0x20)
    NORMAL            = chr(0x21)
    TEX_COORD         = chr(0x22)
    VERTEX16          = chr(0x23)
    VERTEX10          = chr(0x24)
    VERTEX_XY         = chr(0x25)
    VERTEX_XZ         = chr(0x26)
    VERTEX_YZ         = chr(0x27)
    VERTEX_DIFF       = chr(0x28)
    POLY_FORMAT       = chr(0x29)
    TEX_FORMAT        = chr(0x2a)
    PAL_FORMAT        = chr(0x2b)
    DIFFUSE_AMBIENT   = chr(0x30)
    SPECULAR_EMISSION = chr(0x31)
    LIGHT_VECTOR      = chr(0x32)
    LIGHT_COLOR       = chr(0x33)
    SHININESS         = chr(0x34)
    BEGIN             = chr(0x40)
    END               = chr(0x41)
    FLUSH             = chr(0x50)
    VIEWPORT          = chr(0x60)
    BOX_TEST          = chr(0x70)
    STATUS            = chr(0x80)
    CUTOFF_DEPTH      = chr(0x84)

class GL:
    """Faux-OpenGL constants and parameter packing functions.
       These form arguments to the above FIFO commands.
       All FIFO arguments are represented as a list of packed
       
       """
    TRIANGLES       = struct.pack("<I", 0)
    QUADS           = struct.pack("<I", 1)
    TRIANGLE_STRIP  = struct.pack("<I", 2)
    QUAD_STRIP      = struct.pack("<I", 3)

    @classmethod
    def Color(cls, r, g, b):
        """An RGB15 color, packed into a 32-bit word"""
        return struct.pack(
            "<I",
            (min(31, int(round(r * 32.0))) |
             (min(31, int(round(g * 32.0))) << 5) |
             (min(31, int(round(b * 32.0))) << 10)))

    @classmethod
    def Vertex4v16(cls, x, y, z=0, w=0):
        """Up to four 4.12 fixed-point vertices, packed into two 32-bit words"""
        return struct.pack("<hhhh",
                           max(-32767, min(32767, int(round(x * 4096.0)))),
                           max(-32767, min(32767, int(round(y * 4096.0)))),
                           max(-32767, min(32767, int(round(z * 4096.0)))),
                           max(-32767, min(32767, int(round(w * 4096.0)))))

    @classmethod
    def Vertex3v10(cls, x, y, z=0, w=0):
        """Up to three 1.9 fixed-point vertices, packed into one 32-bit word.
           This is the format used for vertex normals.
           """
        return struct.pack(
            "<I",
            (max(-511, min(511, int(round(x * 512.0)))) & 0x3FF) |
            ((max(-511, min(511, int(round(y * 512.0)))) & 0x3FF) << 10) |
            ((max(-511, min(511, int(round(z * 512.0)))) & 0x3FF) << 20))

class DisplayList:
    """Maintains a list of FIFO commands, and generates a binary
       representation compatible with the Nintendo DS's hardware
       and with libnds's glCallList().
       """
    def __init__(self):
        self.commands = []

    def append(self, command, arg=""):
        self.commands.append((command, arg))

    def serialize(self):
        # Pad the command list to a multiple of four
        while len(self.commands) & 3:
            self.append(FIFO.NOP)

        # Commands and operands are interleaved in groups
        # of four. Pack four FIFO commands into a single word,
        # followed by all operands needed by those four
        # commands.
        parts = []
        for quad in xrange(0, len(self.commands), 4):
            for part in (0, 1):
                for index in xrange(quad, quad+4):
                    parts.append(self.commands[index][part])

        joined = "".join(parts)
        assert (len(joined) & 3) == 0
        wordCount = len(joined) / 4
        return struct.pack("<I", wordCount) + joined


if __name__ == "__main__":
    #
    # Simple Wavefront .obj to NDS display list converter
    #

    import sys
    dl = DisplayList()

    vertices = []
    normals = []
    faces = []

    for line in open(sys.argv[1]):
        tok = line.split()

        if tok[0] == 'v':
            vertices.append(map(float, tok[1:]))

        elif tok[0] == 'vn':
            normals.append(map(float, tok[1:]))

        elif tok[0] == 'f':
            points = tok[1:]

            if len(points) == 3:
                dl.append(FIFO.BEGIN, GL.TRIANGLES)
            elif len(points) == 4:
                dl.append(FIFO.BEGIN, GL.QUADS)
            else:
                assert 0

            for point in points:
                pointIndices = point.split("/")
                vertex = vertices[int(pointIndices[0]) - 1]
                normal = normals[int(pointIndices[2]) - 1]

                dl.append(FIFO.NORMAL, GL.Vertex3v10(*normal))
                dl.append(FIFO.VERTEX16, GL.Vertex4v16(*vertex))

            dl.append(FIFO.END)

    print dl.serialize()
