#!/usr/bin/env python
from Captcha.Visual import Text, Backgrounds, Distortions, ImageCaptcha
from Captcha import Words, Factory

class PseudoGimpy(ImageCaptcha):
    def getLayers(self):
        word = Words.defaultWordList.pick()
        self.addSolution(word)
        return [
            Backgrounds.TiledImage(),
            Text.TextLayer(word, borderSize=1),
            Distortions.WigglyBlocks(),
            ]

g = Factory(PseudoGimpy).new()
i = g.render()
i.save("output.png")
i.show()
print g.solutions
