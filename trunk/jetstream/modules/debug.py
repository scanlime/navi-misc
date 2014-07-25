#
# debug.py - Utilities to help with interactive debugging
#
# Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import PicoGUI, engine, math


class DebugPanel(PicoGUI.Widget):
    "A base panel widget for debugging objects"
    def __init__(self, name="Debug Panel"):
        self.app = engine.globals.app
        PicoGUI.Widget.__init__(self, self.app.server, self.app.server.register(name), self.app)
        self.app.link(self.closeHandler, self, 'close')

    def closeHandler(self, ev, widget):
        self.destroy()

    def destroy(self):
        self.app.delWidget(self)


class CameraControl(DebugPanel):
    "A set of debugging widgets for controlling the camera"
    def __init__(self, name="Camera Control Panel"):
        DebugPanel.__init__(self,name)
        cam = engine.world.camera

        self.flythrough = Flythrough(self.addWidget('tabpage','inside'))

        self.bearingPage = self.flythrough.page.addWidget('tabpage')
        self.bearingPage.text = 'Fluid bearing model'
        self.bearingPage.thobj = 0
        self.bearingPage.app = self.app
        r = cam.getRotation()
        d = cam.getDisplacement()
        self.thrust       = DictAttributeAdjuster(self.bearingPage, r, 'thrust',     1000)
        self.friction     = DictAttributeAdjuster(self.bearingPage, r, 'friction',   100 )
        self.hysteresis   = DictAttributeAdjuster(self.bearingPage, r, 'hysteresis', 10  )
        self.displacement =    VectorAdjuster(self.bearingPage, d.getTarget(),   1000, 'displacement')
        self.target       =     AngleAdjuster(self.bearingPage, r.getTarget())

    def destroy(self):
        self.thrust.destroy()
        self.friction.destroy()
        self.hysteresis.destroy()
        self.target.destroy()
        self.displacement.destroy()
        self.app.delWidget(self.bearingPage)
        self.flythrough.destroy()
        DebugPanel.destroy(self)


class DetailControl(DebugPanel):
    "A set of debugging widgets for controlling the global LOD parameters"
    def __init__(self, name="Global Level-of-detail Control Panel"):
        DebugPanel.__init__(self,name)
        d = engine.globals.engine.getDetail()
        self.adjusters = [DictAttributeAdjuster(self, d, 'geomorphDuration', 5, 'bottom'),
                          DictAttributeAdjuster(self, d, 'minTesselationErrorSquared', 20, 'bottom'),
                          DictAttributeAdjuster(self, d, 'maxTesselationErrorSquared', 20, 'bottom')
                          ]

    def destroy(self):
        for i in self.adjusters:
            i.destroy()
        DebugPanel.destroy(self)


class Flythrough:
    """
    Flythrough model:
    Mouse position controls rotational velocity, left mouse button
    accelerates forward, right mouse button enables rotation, middle
    mouse button accelerates backwards.
    """
    def __init__(self, page):
        self.page = page
        self.app = engine.globals.app

        self.page.text = 'Flythrough model'
        self.page.thobj = 0

        # Create a canvas for our flight control, scribbling some alignment marks on it
        self.canvas = self.page.addWidget('canvas','inside')
        self.canvas.side = 'all'
        self.canvas.triggermask = 0x7FFFFFFF
        c = PicoGUI.Canvas(self.canvas)
        c.setlgop('alpha')
        c.setcolor(0x80000000 | 0xFFFFFF | 0x50000000)
        c.setmapping(0,0,1,1,'scale')
        c.frame(0,0,1,1)
        c.setmapping(0,0,20,20,'squarescale')
        c.slab(9,10,2)
        c.bar(10,9,2)

        self.toolbar = self.page.addWidget('toolbar','inside')
        self.resetButton = self.toolbar.addWidget('button','inside')
        self.resetButton.text = 'Reset'

        self.thrust = 2.5
        self.page.app = self.app
        self.velocityAdjuster = DictAttributeAdjuster(self.page, self, 'thrust', 5, 'left', 'logarithmic scale')

        self.app.link(self.mouseMove, self.canvas, 'pntr move')
        self.app.link(self.mouseUp, self.canvas, 'pntr up')
        self.app.link(self.mouseDown, self.canvas, 'pntr down')
        self.app.link(self.scrollWheel, self.canvas, 'scrollwheel')
        self.app.link(self.reset, self.resetButton, 'activate')
        self.buttons = 0
        self.rotation = (0,0)

    def mouseMove(self, ev, widget):
        maxAcceleration = 400
        deadZone = 0.05

        r = engine.world.camera.getRotation()
        r.thrust = 0
        w = self.canvas.width
        h = self.canvas.height
        if w>0 and h>0:
            x = float(ev.x) / w * 2 - 1
            y = float(ev.y) / h * 2 - 1
        if x < deadZone and x > -deadZone:
            x = 0
        if y < deadZone and y > -deadZone:
            y = 0

        self.rotation = (y * maxAcceleration, x * maxAcceleration)
        if self.buttons & 4:
            r.getAcceleration().set(self.rotation[0], self.rotation[1], 0)

    def reset(self, ev, widget):
        cam = engine.world.camera
        p = cam.getPosition()
        r = cam.getRotation()
        p.thrust = 0
        cam.forwardThrust = 0
        p.set(0,0,0)
        p.getAcceleration().set(0,0,0)
        r.set(0,0,0)
        r.getAcceleration().set(0,0,0)

    def mouseDown(self, ev, widget):
        self.buttons |= ev.buttons
        self.updateButtons()
        self.app.server.focus(self.canvas.handle)

    def mouseUp(self, ev, widget):
        self.buttons &= ~ev.buttons
        self.updateButtons()

    def scrollWheel(self, ev, widget):
        self.velocityAdjuster.adjust.value = max(0,self.velocityAdjuster.adjust.value + ev.y * 3)
        self.updateButtons()

    def updateButtons(self):
        cam = engine.world.camera
        p = cam.getPosition()
        r = cam.getRotation()
        p.thrust = 0
        cam.forwardThrust = 0

        r.thrust = 0
        if self.buttons & 4:
            r.getAcceleration().set(self.rotation[0], self.rotation[1], 0)
        else:
            r.getAcceleration().set(0,0,0)

        thrust = math.pow(10, self.thrust)
        if self.buttons & 1:
            cam.forwardThrust += thrust
        if self.buttons & 2:
            cam.forwardThrust += -thrust
        if cam.forwardThrust == 0:
            p.getAcceleration().set(0,0,0)

    def destroy(self):
        self.velocityAdjuster.destroy()
        self.app.delWidget(self.resetButton)
        self.app.delWidget(self.toolbar)
        self.app.delWidget(self.page)
        self.app.delWidget(self.canvas)


class VectorAdjuster:
    "A box with sliders for adjusting a vector"
    def __init__(self,panel,vector,maxAxisMagnitude=1,name='vector',resolution=5000):
        self.panel = panel
        self.vector = vector
        self.resolution = resolution
        self.maxAxisMagnitude = maxAxisMagnitude
        self.box = panel.addWidget('box','inside')
        self.box.side = 'left'
        self.z = self.axisScroll()
        self.y = self.axisScroll()
        self.x = self.axisScroll()
        self.label = self.box.addWidget('label','inside')
        self.label.text = name
        self.value = self.box.addWidget('label','inside')
        self.value.side = 'bottom'

    def handler(self, ev, widget):
        x = ((self.x.value/float(self.resolution)) - 0.5) * 2 * self.maxAxisMagnitude
        y = ((self.y.value/float(self.resolution)) - 0.5) * 2 * self.maxAxisMagnitude
        z = ((self.z.value/float(self.resolution)) - 0.5) * 2 * self.maxAxisMagnitude
        self.vector.set(x,y,z)
        self.value.text = "%.2f,%.2f,%.2f" % (x,y,z)

    def axisScroll(self):
        s = self.box.addWidget('scroll','inside')
        s.side = 'left'
        s.size = self.resolution
        s.value = self.resolution/2
        self.panel.app.link(self.handler,s,'activate')
        return s

    def destroy(self):
        self.panel.app.delWidget(self.x)
        self.panel.app.delWidget(self.y)
        self.panel.app.delWidget(self.z)
        self.panel.app.delWidget(self.label)
        self.panel.app.delWidget(self.value)
        self.panel.app.delWidget(self.box)


class AngleAdjuster:
    "A set of sliders to control an angle, arranged at the edges of the panel"
    def __init__(self,panel,vector):
        self.panel = panel
        self.vector = vector
        self.z = self.degreeScroll('top')
        self.x = self.degreeScroll('bottom')
        self.y = self.degreeScroll('left')

    def handler(self, ev, widget):
        self.vector.set((self.y.value-1800)/10.0,
                        (self.x.value-1800)/10.0,
                       (-self.z.value+1800)/10.0)

    def degreeScroll(self, side):
        s = self.panel.addWidget('scroll','inside')
        s.side = side
        s.size = 3600
        s.value = 1800
        self.panel.app.link(self.handler,s,'activate')
        return s

    def destroy(self):
        self.panel.app.delWidget(self.x)
        self.panel.app.delWidget(self.y)
        self.panel.app.delWidget(self.z)


class ScalarAdjuster:
    "A slider to adjust one scalar parameter"
    def __init__(self, panel, name, range, side='right', axislabel='', resolution=5000):
        self.panel = panel
        self.resolution = resolution
        default = self.get()
        self.box = panel.addWidget('box','inside')
        self.box.side = side
        self.label = self.box.addWidget('label','inside')
        self.label.text = name
        self.label.align = 'left'
        self.value = self.label.addWidget('label')
        self.value.side = 'bottom'
        self.value.align = 'left'
        self.adjust = self.value.addWidget('scroll')
        self.adjust.side = side
        self.adjust.size = resolution
        self.axislabel = self.adjust.addWidget('label')
        self.axislabel.text = axislabel
        self.axislabel.side = 'all'
        self.axislabel.direction = 'vertical'
        if self.box.side == 'bottom' or self.box.side == 'top':
            self.adjust.value = default * resolution / range
        else:
            self.adjust.value = resolution-(default * resolution / range)
        self.range = range
        panel.app.link(self.handler,self.adjust,'activate')

    def handler(self, ev, widget):
        if self.box.side == 'bottom' or self.box.side == 'top':
            v = widget.value * self.range / float(self.resolution)
        else:
            v = (self.resolution-widget.value) * self.range / float(self.resolution)
        self.set(v)
        self.value.text = str(v)

    def destroy(self):
        self.panel.app.delWidget(self.axislabel)
        self.panel.app.delWidget(self.adjust)
        self.panel.app.delWidget(self.value)
        self.panel.app.delWidget(self.label)
        self.panel.app.delWidget(self.box)


class DictAttributeAdjuster(ScalarAdjuster):
    def __init__(self, panel, object, attr, range, side='right', axislabel='', resolution=5000):
        self.attr = attr
        self.object = object
        ScalarAdjuster.__init__(self,panel,attr,range,side,axislabel,resolution)

    def get(self):
        return getattr(self.object,self.attr)

    def set(self, value):
        setattr(self.object,self.attr,value)


class JetCOWFloatAdjuster(ScalarAdjuster):
    def __init__(self, panel, object, attr, range, side='right', axislabel='', resolution=5000):
        self.attr = attr
        self.object = object
        ScalarAdjuster.__init__(self,panel,attr,range,side,axislabel,resolution)

    def get(self):
        return self.object.getAttrFloat(self.attr)

    def set(self, value):
        self.object.setAttrFloat(self.attr, value)


class JetCOWIntAdjuster(ScalarAdjuster):
    def __init__(self, panel, object, attr, range, side='right', axislabel=''):
        self.attr = attr
        self.object = object
        ScalarAdjuster.__init__(self,panel,attr,range,side,axislabel,range)

    def get(self):
        return self.object.getAttrInt(self.attr)

    def set(self, value):
        self.object.setAttrInt(self.attr, int(value))
