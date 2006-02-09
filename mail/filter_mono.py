#!/usr/bin/env python
from filter_gnome import GnomeFilter

# Mono should be using the same format as gnome
class MonoFilter(GnomeFilter):
    project = 'mono'

if __name__ == '__main__':
    MonoFilter().main()
