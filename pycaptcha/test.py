#!/usr/bin/env python
from Captcha.Visual import Text, Backgrounds, Distortions, ImageCaptcha
from Captcha import Words, Factory

class PseudoGimpy(ImageCaptcha):
    def getLayers(self):
        word = Words.defaultWordList.pick()
        self.addSolution(word)
        return [
            #Backgrounds.Grid(),
            Backgrounds.TiledImage(),
            Text.TextLayer(word),
            Distortions.WigglyBlocks(),
            ]

g = Factory(PseudoGimpy).new()
i = g.render()
i.save("output.png")
i.show()
print g.solutions
