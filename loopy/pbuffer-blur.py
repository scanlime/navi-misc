from __future__ import division
import loopy, pygame, os, sys, math
from OpenGL import GL

datadir = os.path.join(os.path.dirname(sys.argv[0]), "data")

def loadTexture(name):
    texturefile = os.path.join(datadir, name)
    textureSurface = pygame.image.load(texturefile)
    textureData = pygame.image.tostring(textureSurface, "RGBX", 1)
    texture = GL.glGenTextures(1)
    GL.glBindTexture(GL.GL_TEXTURE_2D, texture)
    GL.glTexImage2D(GL.GL_TEXTURE_2D, 0, GL.GL_RGBA,
                    textureSurface.get_width(), textureSurface.get_height(), 0,
                    GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, textureData )
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR)
    GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR)
    return texture

class Overlay(loopy.Overlay):
    def resized(self):
        GL.glViewport(0, 0, *self.resolution)
        GL.glMatrixMode(GL.GL_PROJECTION)
        GL.glLoadIdentity()
        GL.glMatrixMode(GL.GL_MODELVIEW)
        GL.glLoadIdentity()

    def setup(self):
        self.src = loadTexture("images/icon_ps2.png")
        self.filter = GaussianFilterPyramid()

        self.square = GL.glGenLists(1)
        GL.glNewList(self.square, GL.GL_COMPILE)
        GL.glBegin(GL.GL_QUADS)
        GL.glTexCoord2f(0.0, 0.0); GL.glVertex2f(-1.0, -1.0)
        GL.glTexCoord2f(0.0, 1.0); GL.glVertex2f(-1.0,  1.0)
        GL.glTexCoord2f(1.0, 1.0); GL.glVertex2f( 1.0,  1.0)
        GL.glTexCoord2f(1.0, 0.0); GL.glVertex2f( 1.0, -1.0)
        GL.glEnd()
        GL.glEndList()

        # Loopy disables color buffer clearing in our context, by default
        self.glState.clearMask = -1
        GL.glEnable(GL.GL_TEXTURE_2D)
        GL.glEnable(GL.GL_BLEND)

        self.resized()

    def drawIcons(self, offset=(0,0), scale=(0.3, 0.4), shadow=False):
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.src)
        GL.glColor(1,1,1,1)
        if shadow:
            GL.glBlendFunc(GL.GL_ZERO, GL.GL_ONE_MINUS_SRC_ALPHA)
        else:
            GL.glBlendFunc(GL.GL_SRC_ALPHA, GL.GL_ONE_MINUS_SRC_ALPHA)

        GL.glLoadIdentity()
        GL.glTranslatef(offset[0], offset[1], 0)
        GL.glScalef(scale[0], scale[1], 1)
        GL.glCallList(self.square)

    def drawShadowMask(self, buffer):
        """In the given buffer, draw a mask which will eventually
           be multiplied by the image to generate a shadow.
           """
        GL.glViewport(0, 0, buffer.w, buffer.h)
        buffer.lock()

        GL.glClearColor(1,1,1,1)
        GL.glClear(GL.GL_COLOR_BUFFER_BIT)

        self.drawIcons()#shadow=True)

        buffer.unlock()
        GL.glViewport(0, 0, *self.resolution)


    def drawBlurredShadowMask(self, h):
        self.filter.setRadius(h)
        self.drawShadowMask(self.filter.outputTarget)
        self.filter.run()
        GL.glViewport(0, 0, *self.resolution)

    t = 0
    def render(self):
        #self.filter.outputTarget.unlock()

        self.t += 1
        h = (math.sin(self.t * 0.1) + 1) * 0.15
        self.drawBlurredShadowMask(h)

        # Blend the completed shadow mask
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.filter.outputTarget.texture)
        #GL.glBlendFunc(GL.GL_DST_COLOR, GL.GL_ZERO)
        GL.glBlendFunc(GL.GL_ONE, GL.GL_ZERO)
        GL.glColor(1,1,1,1)
        GL.glLoadIdentity()
        GL.glCallList(self.square)

        #self.drawIcons(offset=(-h, h))
        #self.filter.outputTarget.lock()

class SeparableConvolutionFilter:
    """A separable convolution filter- that is, one that is applied
       once in the horizontal then once in the vertical. The output
       is rendered into a RenderTarget. By default, the input comes
       from that same RenderTarget- but it may instead be any texture.
       If no RenderTargets are specified, new ones are allocated at
       the specified size.

       The kernel functions must be supplied. Separate kernels can
       be used for horizontal and vertical, or one function may be
       used for both. The kernel is specified as a list of
       (x, y) tuples. 'x' is spatial (positive or negative, in texels)
       while 'y' is a gain between 0 and 1. 'y' may be a single scalar,
       or a 4-tuple of separate RGBA gains. The kernel may be any
       function, even a generator. Speed isn't critical if the kernel
       isn't changed often, since it is evaluated only once while
       caching display lists.
       """
    def __init__(self, outputTarget=None, inputTexture=None,
                 tempTarget=None, size=None):
        if outputTarget is not None:
            if size is None:
                size = (outputTarget.w, outputTarget.h)
            elif size != (outputTarget.w, outputTarget.h):
                raise ValueError("outputTarget does not match the specified size")
        if tempTarget is not None:
            if size is None:
                size = (tempTarget.w, tempTarget.h)
            elif size != (tempTarget.w, tempTarget.h):
                raise ValueError("outputTarget and tempTarget sizes do not match")
        if size is None:
            raise ValueError("No size was specified")

        if outputTarget is None:
            outputTarget = loopy.RenderTarget(*size)
        if outputTarget.texture is None:
            outputTarget.bind(GL.glGenTextures(1))
        else:
            outputTarget.bind(outputTarget.texture)
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP)
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP)

        if tempTarget is None:
            tempTarget = loopy.RenderTarget(*size)
        if tempTarget.texture is None:
            tempTarget.bind(GL.glGenTextures(1))
        else:
            tempTarget.bind(tempTarget.texture)
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_CLAMP)
        GL.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_CLAMP)

        if inputTexture is None:
            inputTexture = outputTarget.texture

        self._kernelLists = GL.glGenLists(2)
        self._kernelValid = False
        self._kernels = None
        self.outputTarget = outputTarget
        self.inputTexture = inputTexture
        self.tempTarget = tempTarget
        self.size = size
        self.majorAxis = max(*size)
        self.minorAxis = min(*size)

    def setKernel(self, xkernel, ykernel=None):
        """Set the kernel function(s). If only one kernel is supplied, this
           is a symmetric convolution.
           """
        if ykernel is None:
            ykernel = xkernel
        self._kernels = (xkernel, ykernel)
        self._kernelValid = False

    def run(self):
        """Perform the convolution. OpenGL state modified: viewport, clear color,
           blend function, and modelview matrix.
           """
        GL.glViewport(0, 0, self.outputTarget.w, self.outputTarget.h)
        GL.glClearColor(0,0,0,0)
        GL.glBlendFunc(GL.GL_ONE, GL.GL_ONE)
        GL.glLoadIdentity()

        for axis, target, texture in (
            (0, self.tempTarget, self.inputTexture),
            (1, self.outputTarget, self.tempTarget.texture),
            ):
            target.lock()
            GL.glClear(GL.GL_COLOR_BUFFER_BIT)
            GL.glBindTexture(GL.GL_TEXTURE_2D, texture)
            if self._kernelValid:
                GL.glCallList(self._kernelLists + axis)
            else:
                GL.glNewList(self._kernelLists + axis, GL.GL_COMPILE_AND_EXECUTE)
                for x, y in self._kernels[axis]:
                    if type(y) is float:
                        GL.glColor(y,y,y,y)
                    else:
                        GL.glColor(*y)
                    if axis:
                        GL.glBegin(GL.GL_QUADS)
                        GL.glTexCoord2f(0.0, x+0.0); GL.glVertex2f(-1.0, -1.0)
                        GL.glTexCoord2f(0.0, x+1.0); GL.glVertex2f(-1.0,  1.0)
                        GL.glTexCoord2f(1.0, x+1.0); GL.glVertex2f( 1.0,  1.0)
                        GL.glTexCoord2f(1.0, x+0.0); GL.glVertex2f( 1.0, -1.0)
                        GL.glEnd()
                    else:
                        GL.glBegin(GL.GL_QUADS)
                        GL.glTexCoord2f(x+0.0, 0.0); GL.glVertex2f(-1.0, -1.0)
                        GL.glTexCoord2f(x+0.0, 1.0); GL.glVertex2f(-1.0,  1.0)
                        GL.glTexCoord2f(x+1.0, 1.0); GL.glVertex2f( 1.0,  1.0)
                        GL.glTexCoord2f(x+1.0, 0.0); GL.glVertex2f( 1.0, -1.0)
                        GL.glEnd()
                GL.glEndList()
            target.unlock()
        self._kernelValid = True

class GaussianFilter(SeparableConvolutionFilter):
    """Extends the SeparableConvolutionFilter to know about gaussian kernels"""
    def setRadius(self, xradius, yradius=None):
        if yradius is None:
            self.setKernel(self.createGaussianKernel(xradius))
        else:
            self.setKernel(self.createGaussianKernel(xradius),
                           self.createGaussianKernel(yradius))

    def createGaussianKernel(self, textureRadius, softness=1.5):
        """Create a kernel sequence for a gaussian blur. The
           radius is specified in texture space (0 to 1),
           the number of quantization steps is determined
           automatically. 'softness' determines how far out
           we evaluate the gaussian distribution.
           """
        # Figure out how many quantization steps
        # we need to render this texture radius on
        # our particular output resolution. Note that
        # besides being slow, too many quantization steps
        # will kill our color resolution. We cap it
        # arbitrarily, to avoid this problem. If this
        # routinely gets too big, the caller should really be
        # using a lower output resolution.
        stepRadius = min(10, int(textureRadius * self.majorAxis + 1))

        # Compute one tail of the normal distribution
        t = []
        s = 0
        z = softness / stepRadius
        for i in xrange(stepRadius):
            x = (1.0 + i) * z
            y = math.exp(-x*x)
            t.append(y)
            s += y

        # Normalize the whole thing- we'll have two copies
        # of the tail, plus a central '1'.
        a = 1.0 / (1 + 2*s)

        kernel = [(0, a)]
        z = textureRadius / stepRadius
        x = 0
        for y in t:
            x += z
            kernel.append((x, y*a))
            kernel.append((-x, y*a))
        return kernel

class GaussianFilterPyramid:
    """This class contains GaussianFilters created at multiple
       texture resolutions. It selects the best one to use for
       a particular blur level in order to minimize the number
       of quantization steps used by GaussianFilter. This improves
       speed and color quality.
       """
    def __init__(self, resolutions=(32, 64, 128, 256, 512)):
        resolutions = list(resolutions)
        resolutions.sort()
        self.filters = []
        for r in resolutions:
            self.filters.append((r, GaussianFilter(size=(r,r))))

    def setRadius(self, xradius, yradius=None):
        if yradius is None:
            minRadius = xradius
        else:
            minRadius = min(xradius, yradius)

        if minRadius > 0:
            # Figure out the 'optimum' texture size for this blur.
            # This is another fudge factor, but it roughly equates
            # to the largest number of quantization steps we tolerate.
            size = 10.0 / minRadius
            for r, f in self.filters:
                if size < r:
                    print size, r
                    f.setRadius(xradius, yradius)
                    self.outputTarget = f.outputTarget
                    self.run = f.run
                    return

        # Make do with our highest resolution target
        f = self.filters[-1][1]
        f.setRadius(xradius, yradius)
        self.outputTarget = f.outputTarget
        self.run = f.run

if __name__ == "__main__":
    loopy.overlays.append(Overlay())

