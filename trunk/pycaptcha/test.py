import Captcha.Visual.Text
import Captcha.Words


class BgLayer(Captcha.Visual.Layer):
    def render(self, image):
        image.paste("white")


class PseudoGimpy(Captcha.Visual.ImageCaptcha):
    def getLayers(self):
        word = Captcha.Words.defaultWordList.pick()
        self.addSolution(word)
        return [
            BgLayer(),
            Captcha.Visual.Text.TextLayer(word),
            ]

g = Captcha.Factory(PseudoGimpy).new()
g.render().show()
