#!/usr/bin/env python
import gtk, rtgraph, sys, fcntl, os

def main():
    channels = [rtgraph.Channel (name="Parameter block %d" % i) for i in xrange (8)]

    graph = rtgraph.GraphUI (channels,
                             rtgraph.HScrollLineGraph(range=(0,255)))

    win = gtk.Window (gtk.WINDOW_TOPLEVEL)
    vbox = gtk.VBox ()
    win.add (vbox)
    win.set_border_width (5)
    vbox.pack_start (graph, gtk.TRUE)
    win.set_default_size (400,400)
    win.show_all ()
    win.connect ("destroy", gtk.mainquit)

    stdin = sys.stdin.fileno()

    flags = fcntl.fcntl (stdin, fcntl.F_GETFL)
    fcntl.fcntl (stdin, fcntl.F_SETFL, flags | os.O_NONBLOCK)

    while True:
        try:
            values = os.read(stdin, 4096).split(' ')
            for i in xrange(8):
                channels[i].value = float(values[i])
        except:
            pass
        while gtk.events_pending():
            gtk.main_iteration()

if __name__ == "__main__":
    main()
