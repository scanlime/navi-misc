#!/usr/bin/env python
from Captcha.Visual.Tests import PseudoGimpy
from Captcha import Factory

g = Factory(PseudoGimpy).new()
i = g.render()
i.save("output.png")
i.show()
print g.solutions
