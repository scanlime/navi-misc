#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer
from BZEngine import Event
from Wasabi import Logos

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)

# Our main book, running only once
mainBook = Sequencer.Book(view, [
#    Sequencer.FadeOut(3, (0,0,0), Sequencer.UserPageInterrupter(Logos.getLogoSubBook()))
    Logos.getLogoSubBook()
    ])

# Exit our main loop once the mainBook finishes
mainBook.onFinish.observe(loop.stop)
loop.run()
