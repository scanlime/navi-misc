#!/usr/bin/env python
#
# Sample Raster Wand app, scrolls in lines of text read from stdin.
#
# -- Micah Dowty <micah@navi.cx>
#

import rwand, sys

rwdc = rwand.RwdClient()
rwdc.start()
try:
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        text = rwand.TextRenderer(unicode(line, 'utf-8').strip())
        rwdc.renderer = rwand.VScroll(rwdc.renderer, text)

    print 'done'

except KeyboardInterrupt:
    pass
finally:
    rwdc.stop()
    rwdc.join()
