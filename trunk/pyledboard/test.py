#!/usr/bin/env python
import PicoGUI, ledboard

led = ledboard.Server()
app = PicoGUI.Application("LED Board Test")

def addScroll(app, container, criteria):
    def scrollHandler(ev, widget):
        criteria = widget.criteria
        value = 15 - widget.value
        led.set(led.select(criteria), value)
        led.blit()
        led.vfd.writeScreen("%s\n%d" % (str(criteria)[1:-1], value))
    box = container.addWidget('box','inside')
    box.side = 'left'
    l = box.addWidget('label','inside')
    l.text = str(criteria)
    s = l.addWidget('scroll')
    s.criteria = criteria
    s.size = 15
    s.value = 15
    app.link(scrollHandler, s, 'activate')

b = app.addWidget('box', 'inside')
b.size = 100
for color in ledboard.colors:
    addScroll(app, b, {'color': color})

b = app.addWidget('box', 'inside')
b.size = 100
for group in ledboard.groups:
    addScroll(app, b, {'group': group})

app.run()
