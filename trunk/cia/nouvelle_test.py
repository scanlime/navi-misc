#!/usr/bin/env python
#
# A sandbox for testing the Nouvelle web framework used by CIA
#
import Nouvelle
from Nouvelle import tag, BaseHTTP

class Hello(BaseHTTP.Page):
    document = tag('html')[
                   tag('body') [
                       "Hello World!",
                   ],
               ]

if __name__ == "__main__":
    BaseHTTP.main(Hello())
