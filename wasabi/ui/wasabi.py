#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer
from BZEngine import Event
from Wasabi import Logos

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)

book = Sequencer.CyclicBook(view, Logos.getLogoList())
loop.run()
