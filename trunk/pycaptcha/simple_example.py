#!/usr/bin/env python
from Captcha.Visual.Tests import PseudoGimpy

g = PseudoGimpy()
i = g.render()
i.save("output.png")
i.show()
print g.solutions
