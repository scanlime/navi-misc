#!/usr/bin/env python
import svm, efs, rtgraph, gtk

models = [svm.svm_model("../training/x.model"),
          svm.svm_model("../training/y.model"),
          svm.svm_model("../training/z.model")]

sensor = efs.FieldSensor()
sensor.initScan()
channel = rtgraph.Channel()

def update():
    reading = sensor.readAverages()
    channel.value = [model.predict(reading) for model in models]
    gtk.timeout_add(10, update)
update()

graph = rtgraph.IsometricVectorGraph(channels=[channel])

win = gtk.Window(gtk.WINDOW_TOPLEVEL)
win.add(graph)
win.show_all()
win.connect("destroy", gtk.mainquit)
gtk.main()
