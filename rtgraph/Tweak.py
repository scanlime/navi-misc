""" Tweak

A system for easily connecting GUI widgets with python attributes
that can be tweaked in real-time.

  rtgraph real-time graphing package for PyGTK
  Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""
from __future__ import division
import gtk, threading

__version__ = "pre-0.71"


def Window(*controlLists):
    """Creates a gtk Window containing a Tweak.List.
       For convenience, the constructor's argument list can be one
       or more tweak controls or lists of tweak controls.
       """
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    list = List(*controlLists)
    win.set_border_width(8)
    list.show()
    win.add(list)
    win.show()
    return win


def run(runnable=None):
    """A utility function to create a Tweak.Thread, call runnable.run(), then
       safely tell the Tweak.Thread to stop. This is designed to be used with
       an event loop, but should suppport any runnable object.
       """
    if runnable:
        try:
            tweakThread = Thread()
            runnable.run()
        finally:
            tweakThread.stop()
    else:
        # If we have no runnable, no need to make a thread
        gtk.main()


class Thread(threading.Thread):
    """A thread to run gtk's main loop, used when Tweak.run() is called with an argument"""
    def __init__(self):
        threading.Thread.__init__(self)
        self.start()

    def run(self):
        """The thread's run function, started by start()"""
        gtk.threads_init()
        gtk.main()

    def stop(self):
        """Force the gtk event loop to terminate"""
        gtk.main_quit()


class List(gtk.Table):
    """A widget that holds tweak controls
       For convenience, the constructor's argument list can be one
       or more tweak controls or lists of tweak controls.
       """
    def __init__(self, *controlLists):
        # Expand controlLists
        self.controls = []
        for controlList in controlLists:
            try:
                for control in controlList:
                    self.controls.append(control)
            except TypeError:
                self.controls.append(controlList)

        gtk.Table.__init__(self, len(self.controls), 2)

        row = 0
        for control in self.controls:
            if control.name is not None:
                # Pack the widget in the right side, with a name label in the left
                l = gtk.Label(control.name + ": ")
                l.set_alignment(1, 0.5)
                self.attach(l, 0,1, row,row+1, xoptions=gtk.FILL, yoptions=gtk.FILL)
                l.show()
                self.attach(control.widget, 1,2, row,row+1, yoptions=gtk.FILL)
            else:
                # Stretch the widget all the way across
                self.attach(control.widget, 0,2, row,row+1, yoptions=gtk.FILL)

            control.widget.show()
            row += 1


class Control:
    """A control which can be placed in a Tweak.List"""
    def __init__(self, widget, name=None):
        self.name = name
        self.widget = widget


class Separator(Control):
    """A control that doesn't actually do anything, just provides visual separation"""
    def __init__(self):
        sep = gtk.HSeparator()
        sep.set_size_request(16, 16)
        Control.__init__(self, sep)


class AttributeControl(Control):
    """A control which operates on a vector or scalar object attribute"""
    def __init__(self, object, attribute, name=None):
        self.object = object
        self.attribute = attribute
        if not name:
            name = "%s.%s" % (object.__class__.__name__, attribute)

        # Get the subclass to add subwidgets for each individual value
        box = gtk.HBox(False, 2)
        self.initialValue = getattr(object, attribute)

        self.isVector = False
        if type(self.initialValue) != str:
            try:
                self.initialValue[0]
                self.isVector = True
            except TypeError:
                pass

        self.widgets = []
        if self.isVector:
            # Make sure our initialValue isn't mutable
            self.initialValue = tuple(self.initialValue)

            for index in xrange(len(self.initialValue)):
                def createUpdater(index):
                    # This is necessary so 'index' in the lambda binds with the value at this
                    # iteration, rather than the final value after the loop exits.
                    return lambda value: self.setValueIndex(value, index)
                w = self.addWidget(self.initialValue[index],  createUpdater(index))
                box.pack_start(w, True, True)
                w.show()
                self.widgets.append(w)
        else:
            # Nope, now assume it's a scalar
            w = self.addWidget(self.initialValue, self.setValue)
            box.pack_start(w, True, True)
            w.show()
            self.widgets.append(w)

        reset = gtk.Button("Reset")
        box.pack_end(reset, False, False)
        reset.connect("clicked", self.reset)
        reset.show()

        Control.__init__(self, box, name)

    def addWidget(self, initialValue, setFunction):
        """A hook for subclasses to handle adding individual values
           in the attribute. initialValue is the value at initialization,
           setFunction should be called to change it. This should return
           a widget, which will be packed into the control's HBox.
           """
        pass

    def setValue(self, value):
        setattr(self.object, self.attribute, value)

    def setValueIndex(self, value, index):
        try:
            getattr(self.object, self.attribute)[index] = value
        except TypeError:
            # If it's a tuple, it won't support setting indices directly, we have to build a new object
            l = list(getattr(self.object, self.attribute))
            l[index] = value
            setattr(self.object, self.attribute, l)

    def reset(self, widget=None, event=None):
        setattr(self.object, self.attribute, self.initialValue)
        self.refresh()

    def refresh(self):
        """Update the Tweak controls from the object being tweaked"""
        value = getattr(self.object, self.attribute)
        if self.isVector:
            for index in xrange(len(value)):
                self.setWidget(self.widgets[index], value[index])
        else:
            self.setWidget(self.widgets[0], value)


class Quantity(AttributeControl):
    """A control that lets you directly manipulate a scalar or
       vector object attribute, using the gtk Scale widget.
       """
    def __init__(self, object, attribute, range=(0,1), name=None, stepSize=None, pageSize=None):
        self.range = range
        if not stepSize:
            stepSize = (range[1] - range[0]) / 200
        if not pageSize:
            pageSize = (range[1] - range[0]) / 50
        self.stepSize = stepSize
        self.pageSize = pageSize
        AttributeControl.__init__(self, object, attribute, name)

    def addWidget(self, initialValue, setFunction):
        adj = gtk.Adjustment(initialValue, self.range[0], self.range[1], self.stepSize, self.pageSize)
        adj.connect("value_changed", lambda adj: setFunction(adj.value))
        scale = gtk.HScale(adj)
        scale.set_digits(3)
        return scale

    def setWidget(self, widget, newValue):
        widget.get_adjustment().set_value(newValue)


class Color(AttributeControl):
    """A control for manipulating a scalar or vector object attribute consisting of 4-tuples
       specifying RGBA colors in the range [0,1]
       """
    def addWidget(self, initialValue, setFunction):
        box = gtk.VBox(False, False)
        label = gtk.Label()
        label.show()
        box.pack_start(label)

        drawingArea = gtk.DrawingArea()
        drawingArea.set_events(gtk.gdk.BUTTON_PRESS_MASK)
        drawingArea.set_size_request(20,20)
        drawingArea.connect("event", self.event)
        drawingArea.box = box
        drawingArea.label = label
        drawingArea.colorDialog = None
        drawingArea.setFunction = setFunction
        drawingArea.color = initialValue
        drawingArea.show()
        box.pack_end(drawingArea)
        box.drawingArea = drawingArea

        self.setWidget(box, initialValue)
        return box

    def setColor(self, drawingArea, color, alpha):
        """Accepts a gdkcolor and a floating point alpha, showing their values in the control widgets
           and setting the attached object attribute to the new color value.
           """
        drawingArea.gdkColor = color
        drawingArea.modify_bg(gtk.STATE_NORMAL, color)
        drawingArea.colorTuple = (color.red   / 65535,
                                  color.green / 65535,
                                  color.blue  / 65535,
                                  alpha)

        # Show the color in hex, since floating point takes too much space
        drawingArea.label.set_text("#%02X%02X%02X%02X" % tuple(
            [int(c * 255) for c in drawingArea.colorTuple]))
        drawingArea.setFunction(drawingArea.colorTuple)

    def event(self, drawingArea, event):
        handled = False
        if event.type == gtk.gdk.BUTTON_PRESS:
            handled = True
            if not drawingArea.colorDialog:
                self.openColorDialog(drawingArea)
        return handled

    def openColorDialog(self, drawingArea):
        # Create our own window for the color selector. The
        # ColorSelectorDialog is modal, which is a Bad Thing.
        window = gtk.Window()
        drawingArea.colorDialog = window
        window.set_border_width(8)
        window.set_title(self.name)

        colorsel = gtk.ColorSelection()
        window.add(colorsel)
        colorsel.show()
        colorsel.drawingArea = drawingArea

        colorsel.set_has_opacity_control(True)
        colorsel.set_previous_color(drawingArea.gdkColor)
        colorsel.set_previous_alpha(int(drawingArea.colorTuple[3] * 65535))
        colorsel.set_current_color(drawingArea.gdkColor)
        colorsel.set_current_alpha(int(drawingArea.colorTuple[3] * 65535))
        colorsel.set_has_palette(True)
        colorsel.connect("color_changed", self.colorChanged)

        window.connect("delete_event", self.closeColorDialog)
        window.drawingArea = drawingArea
        window.show()

    def closeColorDialog(self, widget, event):
        widget.hide()
        widget.drawingArea.colorDialog = None

    def colorChanged(self, widget):
        self.setColor(widget.drawingArea,
                      widget.get_current_color(), widget.get_current_alpha()/65535)

    def setWidget(self, widget, newValue):
        """Called by AttributeControl to set our widgets' values to the observed values,
           to implement refresh(). This is also used during initialization.
           """
        drawingArea = widget.drawingArea
        color = [int(c * 65535) for c in newValue[:3]]
        gdkColor = drawingArea.get_colormap().alloc_color(*color)
        self.setColor(drawingArea, gdkColor, newValue[3])


class Text(AttributeControl):
    """A control that lets you directly manipulate strings using text entry boxes"""
    def addWidget(self, initialValue, setFunction):
        entry = gtk.Entry()
        self.setWidget(entry, initialValue)
        entry.connect("changed", lambda widget: setFunction(entry.get_text()))
        return entry

    def setWidget(self, widget, newValue):
        widget.set_text(newValue)


class Boolean(AttributeControl):
    """A control that manipulates a boolean value using a CheckButton widget"""
    def addWidget(self, initialValue, setFunction):
        entry = gtk.CheckButton()
        self.setWidget(entry, initialValue)
        entry.connect("toggled", lambda widget: setFunction(bool(entry.get_active())))
        return entry

    def setWidget(self, widget, newValue):
        widget.set_active(newValue)

### The End ###
