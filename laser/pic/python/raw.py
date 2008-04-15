#!/usr/bin/env python
import pyrcpod, Tweak
from laser import Projector
dev = pyrcpod.devices[0].open()

lp = Projector(dev)

Tweak.Window(
    Tweak.Quantity(lp, 'x', range=(0,255)),
    Tweak.Quantity(lp, 'y', range=(0,255)),
    Tweak.Boolean(lp, 'laser'),
    )

Tweak.run()
