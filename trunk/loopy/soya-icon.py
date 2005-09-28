#
# Demo of an icon with drop shadow.
#
# Note that the icon used is an original work Copyright (C) 2005 Micah Dowty.
# This example is public domain code, but please don't use icons without asking :)
#

import os, math, random, sys
import loopy, soya, Numeric
import soya.pudding as pudding

class Icon(pudding.control.Image):
    def __init__(self, parent, image, aspect=1):
        self.image = image
        pudding.control.Image.__init__(self, parent, soya.Material(self.image))
        self.width = int(self.image.width * aspect)
        self.height = self.image.height

    def on_resize(self):
        self.left = (self.parent.width - self.width) / 2
        self.top = (self.parent.height - self.height) / 2

class Shadow(pudding.control.Image):
    def setIcon(self, icon):
        self.icon = icon
        self.material = soya.Material(makeShadow(icon.image))

    def on_resize(self):
        self.width = self.icon.width
        self.height = self.icon.height
        self.left = self.icon.left + 10
        self.top = self.icon.top + 10

def image_to_array(image):
    array = Numeric.fromstring(image.pixels, Numeric.UnsignedInt8)
    array.shape = (image.width, image.height, image.nb_color)
    return array

def array_to_image(array):
    if len(array.shape) == 3:
        return soya.Image(array.tostring(), *array.shape)
    else:
        assert len(array.shape) == 2
        return soya.Image(array.tostring(), array.shape[0], array.shape[1], 1)

def blur3(a):
    """Perform a gaussian blur with a 3x3 kernel"""
    b = a * 4
    b[1:,:] += a[:-1,:]
    b[:-1,:] += a[1:,:]
    b[:,1:] += a[:,:-1]
    b[:,:-1] += a[:,1:]
    return (b / 8).astype(a.typecode())

def makeShadow(source):
    alpha = image_to_array(source)[:,:,-1]
    for i in xrange(50):
        alpha = blur3(alpha)
    rgba = Numeric.zeros(alpha.shape + (4,), alpha.typecode())
    rgba[:,:,3] = alpha / Numeric.array(2, alpha.typecode())
    return array_to_image(rgba)

class Overlay(loopy.Overlay):
    def createWidgets(self):
        root = pudding.core.RootWidget()

        shadow = Shadow(root)
        icon = Icon(root, soya.Image.get("icon_ps2.png"), aspect=0.983)
        shadow.setIcon(icon)

        root.resize(0, 0, *self.resolution)
        shadow.on_resize()
        return root

    def resized(self):
        soya.set_video(self.resolution[0], self.resolution[1], False, True)
        self.root.resize(0, 0, *self.resolution)

    def setup(self):
        soya.init("Loopy Demo", self.resolution[0], self.resolution[1])
        soya.path.append(os.path.join(os.path.dirname(sys.argv[0]), "data"))
        pudding.init()
        self.root = self.createWidgets()

    def render(self):
        self.root.render()

if __name__ == "__main__":
    loopy.overlays.append(Overlay())
