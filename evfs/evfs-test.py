#!/usr/bin/env python
import evfs
import gtk, gobject, gtk.glade

class DemoUI:
    def __init__(self):
        self.xml = gtk.glade.XML("evfs-test.glade")
        self.metadata = self.initList("metadata", "Key", "Value")
        self.children = self.initList("children", "Repr")

        self.nameFormat = self.xml.get_widget("name").get_label()
        self.setName("")

        self.node = None

    def setName(self, name):
        self.xml.get_widget("name").set_markup(self.nameFormat % name)

    def setNode(self, node):
        if self.node is not None:
            self.session.removeListener(self.listener)

        self.node = node
        self.session = node.open(evfs.EventSession(self.listener))

        self.setName(node.name)

    def initList(self, viewName, *columns):
        view = self.xml.get_widget(viewName)

        model = gtk.ListStore(*([gobject.TYPE_STRING] * len(columns)))
        view.set_model(model)

        for i in xrange(len(columns)):
            column = gtk.TreeViewColumn(columns[i],
                                        gtk.CellRendererText(),
                                        text=i)
            column.set_sort_column_id(0)
            view.append_column(column)

        return model

    def listener(self, event):
        gtk.timeout_add(0, lambda: self.thunkedListener(event))

    def thunkedListener(self, event):
        print event

        if event.dataType == evfs.METADATA:
            if event.status in (evfs.EXISTS, evfs.ADDED):
                key, value = event.data
                row = self.metadata.append()
                self.metadata.set(row, 0, str(key), 1, str(value))

        if event.dataType == evfs.CHILD:
            if event.status in (evfs.EXISTS, evfs.ADDED):
                row = self.children.append()
                self.children.set(row, 0, repr(event.data))

if __name__ == "__main__":
    #node = evfs.TempNode("foo")
    #node.add(evfs.METADATA, ("uri", "foo.html"))
    #node.add(evfs.METADATA, ("type", "wobbly"))
    #node.add(evfs.METADATA, ("size", "0"))

    node = evfs.GaminNode("/navi/disk/0/media/audio/groups")

    ui = DemoUI()
    ui.setNode(node)

    #node.add(evfs.METADATA, ("content-type", "text/html"))

    gtk.threads_init()
    gtk.main()
