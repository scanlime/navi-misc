#!/usr/bin/env python
#
# An encapsulation of the PIC's USART, and a loopback test
#

import rcpod

class USART:
    def __init__(self, pic=None):
        if not pic:
            pic = rcpod.Device()
        self.pic = pic
        self.reset()

    def reset(self):
        
