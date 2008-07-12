"""
SVGPath.py

Objects for manipulating path data from SVG files.
Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

"""

import xml.sax
import xml.sax.handler
import math
import re


def parseUnits(value):
    """Given a numeric value with SVG units, return a plain floating
       point value in 'user coordinates', as defined by the SVG spec.
       
       For convenience, always returns None if value is None.
       """

    if value is None:
        return None

    value = value.lower().replace(' ', '').replace(',', '')

    # Fast path for values with no units
    try:
        return float(value)
    except ValueError:
        pass

    # Simplistic/fake 72dpi unit table.
    for unit, multiplier in (
        ('px', 1),
        ('pt', 1),
        ('pc', 15),
        ('in', 72),
        ('cm', 72 * 2.54),
        ('mm', 72 * 25.4),
        ):
        if value.find(unit) >= 0:
            try:
                return float(value.replace(unit, '')) * multiplier
            except ValueError:
                break
    raise ValueError("Unparseable units in %r" % value)


class TransformStack(object):
    """A matrix transformation stack, in 3D homogeneous
       coordinates. We only use 2D vectors and 3x2 matrices. (The Z
       component of vectors is assumed to be 1, and the 3rd row of our
       matrices will always be (0, 0, 1). All operations have
       SVG-compatible semantics.

       >>> ts = TransformStack()
       >>> ts.translate((50, 90))
       >>> ts.push()
       >>> ts.rotate((-45,))
       >>> ts.translate((130, 160))
       >>> ts
       <Transform [ 0.7071 -0.7071 0.7071 0.7071 255.0610 111.2132 ]>
       >>> ts.pop()
       >>> ts
       <Transform [ 1.0000 0.0000 0.0000 1.0000 50.0000 90.0000 ]>

       >>> ts = TransformStack()
       >>> ts.parse("translate(50, 90), rotate(-45) translate(130, 160)")
       >>> ts
       <Transform [ 0.7071 -0.7071 0.7071 0.7071 255.0610 111.2132 ]>

       """
    def __init__(self):
        self.stack = []
        self.top = (1, 0,
                    0, 1,
                    0, 0)

    def push(self):
        self.stack.append(self.top)

    def pop(self):
        self.top = self.stack.pop()

    def __repr__(self):
        return ("<Transform [ " + "%.04f " * 6 + "]>") % self.top

    def transform(self, p):
        """Transform a point using the current matrix,
           returning a new point. Input and output points
           are 2-tuples.
           """
        t = self.top
        return (t[0]*p[0] + t[2]*p[1] + t[4],
                t[1]*p[0] + t[3]*p[1] + t[5])

    def matrix(self, m):
        """Multiply a new matrix by the current top-of-stack matrix."""
        t = self.top
        self.top = (t[0]*m[0] + t[2]*m[1],
                     t[1]*m[0] + t[3]*m[1],
                     t[0]*m[2] + t[2]*m[3],
                     t[1]*m[2] + t[3]*m[3],
                     t[0]*m[4] + t[2]*m[5] + t[4],
                     t[1]*m[4] + t[3]*m[5] + t[5])

    def rotate(self, p):
        a = math.radians(p[0])
        s = math.sin(a)
        c = math.cos(a)

        m = ( c, s,
             -s, c,
              0, 0)

        if len(p) > 1:
            self.translate((p[1], p[2]))
            self.matrix(m)
            self.translate((-p[1], -p[2]))
        else:
            self.matrix(m)

    def scale(self, p):
        if len(p) < 2:
            p = p + p
        self.matrix(( p[0], 0,
                      0,    p[1],
                      0,    0 ))

    def translate(self, p):
        if len(p) < 2:
            p = p + (0,)
        self.matrix(( 1,    0,
                      0,    1,
                      p[0], p[1] ))

    def skewX(self, p):
        a = math.radians(p[0])
        self.matrix(( 1, 0,
                      math.tan(a), 1,
                      0, 0 ))

    def skewY(self, p):
        a = math.radians(p[0])
        self.matrix(( 1, math.tan(a),
                      0, 1,
                      0, 0 ))

    _parseRegex = re.compile(r"""
        ( matrix |
          rotate |
          scale |
          translate |
          skewX |
          skewY |
          [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)? )
        """, re.VERBOSE)

    def parse(self, s):
        """Parse an SVG transform string, and apply
           it to the top of the stack.

           This is a very simple parser. Mostly this is out of
           laziness, but it also helps uphold the "Be lenient about
           what you accept, strict about what you produce" mantra.
        
           For convenience, has no effect if s is None.
           """
        if s is None:
            return
        
        fn = None
        args = []

        for match in self._parseRegex.finditer(s):
            token = match.group(0)
            if token in self.__class__.__dict__:
                if fn:
                    fn(args)
                fn = getattr(self, token)
                args = []
            else:
                args.append(float(token))
        if fn:
            fn(args)


class Path(object):
    """A single closed or open curve. Path objects are specified
       entirely in SVG semantics. Each command is a Python tuple. The
       first element of the tuple is a one-character string
       identifying the command.

       Each Path() object is an SVG path, but not all SVG paths are
       Path objects.  We normalize SVG paths such that:

          - No relative (lowercase) commands are present.
          - If a 'Z' (closepath) is present, it must be the last command.
          - The first command must be 'M' (moveto), and no other 'M'
            commands may be present.
          - Each command must use its singular form. For example, every
            'L' command must have exactly 2 parameters.
          - The shorthand commands 'H', 'V', 'S', and 'T' are not allowed.

       """
    def __init__(self, data=[]):
        assert len(data) >= 1
        assert data[0][0] == 'M'
        self.data = data

    def transform(self, ts):
        """Transform this path, using the topmost matrix on the
           supplied transform stack.
           """
        pass


class PathParser(object):
    """Converts SVG path strings to Path objects, while enforcing
       all of the normalization requirements in Path's docstring.
       One SVG path string may turn into multiple Path objects.
       When a Path is completed, it is appended to the provided list.
       """

    _pathDataRegex = re.compile(r"""
        ( (?P<cmd> [MmZzLlHhVvCcSsQqTtAa]) |
          (?P<arg> [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?) )
        """, re.VERBOSE)

    _cmdArgsTable = {
        'M': 2,
        'Z': 0,
        'L': 2,
        'H': 1,
        'V': 1,
        'C': 6,
        'S': 4,
        'Q': 4,
        'T': 2,
        'A': 7,
        }

    def __init__(self, paths, transformStack=None):
        self.ts = transformStack or TransformStack()
        self.paths = paths
        self.pathData = []

    def parse(self, s):
        """Feed string data (whole commands, but not necessarily
           complete paths) to the parser.
           """
        cmd = None
        args = []

        for match in self._pathDataRegex.finditer(s):
           nextCmd = match.group('cmd')
           nextArg = match.group('arg')

           if nextCmd is not None:
               cmd = nextCmd
               args = []

           if nextArg is not None:
               args.append(float(nextArg))
               if self._cmdArgsTable[cmd.upper()] == len(args):
                   # Command is done
                   self.parseCmd(cmd, args)
                   args = []
        
    def finishPath(self):
        """Finish the current path. If non-empty, turn it into a Path object."""
        if self.pathData:
            p = Path(self.pathData)
            p.transform(self.ts)
            self.paths.append(p)
            self.pathData = []

    def parseCmd(self, cmd, args):
        """Try to interpret the current command and arguments."""

        # Lowercase and uppercase 'Z' are equivalent.
        if cmd == 'z':
            cmd = 'Z'

        if cmd == 'M':
            self.finishPath()

        # XXX: No motivation to support relative or shorthand
        #      commands yet, since Inkscape doesn't generate them :P
        #      To implement these commands, this class should use a
        #      function table to apply relative->Absolute and
        #      un-shorthandification transforms before adding
        #      commands to the pathList.

        if cmd not in 'MZzLCQA':
            raise NotImplementedError("SVG path command %r not yet supported" % cmd)

        self.pathData.append((cmd,) + tuple(args))


class SVGParser(xml.sax.handler.ContentHandler):
    """SAX content handler for parsing SVG documents into path data.
       Paths are accumulated into a sequence in the 'paths' attribute.
       Each path is a single closed or open curve, represented as a Path
       object.
       """
    def startDocument(self):
        self.paths = []
        self.width = None
        self.height = None
        self.ts = TransformStack()

    def startElement(self, name, attrs):
        # Generic transform support. Transforms apply to this node,
        # and to all children.
        self.ts.push()
        self.ts.parse(attrs.get('transform'))        

        # Call a begin_<name> handler.
        fn = getattr(self, 'begin_' + name, None)
        if fn:
            fn(attrs)

    def endElement(self, name):
        # Call an end_<name> handler.
        fn = getattr(self, 'end_' + name, None)
        if fn:
            fn()

        # Remove transform
        self.ts.pop()

    def __repr__(self):
        return "<SVG %dx%d, %d paths>" % (
            self.width, self.height, len(self.paths))

    def begin_svg(self, attrs):
        self.width = parseUnits(attrs.get('width'))
        self.height = parseUnits(attrs.get('height'))

    def begin_path(self, attrs):
        parser = PathParser(self.paths, self.ts)
        parser.parse(attrs['d'])
        parser.finishPath()


def parseSVG(filenameOrStream):
    """Parse a file or file-like object, returning a populated
       SVGParser object.  All groups in the SVG file are flattened,
       and all transforms are applied. A finished list of paths can be
       found in the parser's 'paths' list.
       """
    parser = SVGParser()
    xml.sax.parse(filenameOrStream, parser)
    return parser


def _test():
    import doctest
    doctest.testmod()

if __name__ == '__main__':
    #_test()

    import sys
    
    for path in parseSVG(sys.argv[1]).paths:
        print path
        print path.data
