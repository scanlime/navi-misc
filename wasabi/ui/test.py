#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, HUD, GLOrtho
from BZEngine import Event, Animated
from Wasabi import Logos, Icon

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi Test")
view = ThreeDRender.View(viewport)


class IconTest(Sequencer.Page):
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)

        overlay = self.viewport.region(self.viewport.rect)
        self.background = HUD.Image(overlay,
                                    "brushed_metal.png",
                                    overlay.size)

        self.icon = Icon.Icon('navi512.png', 'Hello Navi', imageAspect=1.623)
        overlay.onDrawFrame.observe(self.drawFrame)

        self.size = Animated.Value(Animated.SineFunction(range=(200,300)))

    def drawFrame(self):
        self.size.integrate(self.time.step())

        GLOrtho.setup()
        GLOrtho.translate(self.viewport.size[0] * 0.5,
                          self.viewport.size[1] * 0.5)
        self.icon.draw(Icon.LargeIconStyle, self.size.value)


mainBook = Sequencer.CyclicBook(view, [
    # Cycle through wasabi logos until user intervention, then fade out
    Sequencer.FadeOut(0.2, (1,1,1), Sequencer.UserPageInterrupter(Logos.getLogoSubBook())),

    # Icon test
    Sequencer.FadeIn(0.2, (1,1,1), Sequencer.FadeOut(1, (0,0,0), Sequencer.UserPageInterrupter(IconTest)))
    ])

# Exit our main loop once the mainBook finishes
mainBook.onFinish.observe(loop.stop)
loop.run()
