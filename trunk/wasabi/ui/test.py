#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, HUD
from BZEngine import Event
from Wasabi import Logos

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)


class IconTest(Sequencer.Page):
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)
        height = self.viewport.size[1] / 2
        self.title = HUD.Image(self.viewport.region(self.viewport.rect),
                               "navi512.png",
                               (1.623 * height, height),
                               alignment = (0.5, 0.5))


mainBook = Sequencer.CyclicBook(view, [
    # Cycle through wasabi logos until user intervention, then fade out
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),

    # Icon test
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), Sequencer.UserPageInterrupter(IconTest)))
    ])

# Exit our main loop once the mainBook finishes
mainBook.onFinish.observe(loop.stop)
loop.run()
