#!/usr/bin/env python
import gtk

class MainWindow:
    def print_hello(self, w, data):
        print 'Hello, World!'

    def get_main_menu(self, window):
        accel_group = gtk.AccelGroup()
        item_factory = gtk.ItemFactory(gtk.MenuBar, '<main>', accel_group)
        item_factory.create_items(self.menu_items)
        window.add_accel_group(accel_group)
        self.item_factory = item_factory
        return item_factory.get_widget('<main>')

    def __init__(self):
        self.menu_items = (
            ( '/_File',         None,         None,             0, '<Branch>' ),
            ( '/File/_New',     '<control>N', self.print_hello, 0, None ),
            ( '/File/_Open',    '<control>O', self.print_hello, 0, None ),
            ( '/File/_Save',    '<control>S', self.print_hello, 0, None ),
            ( '/File/Save _As', None,         None,             0, None ),
            ( '/File/sep1',     None,         None,             0, '<Separator>' ),
            ( '/File/Quit',     '<control>Q', gtk.mainquit,     0, None ),
            ( '/_Options',      None,         None,             0, '<Branch>' ),
            ( '/Options/Test',  None,         None,             0, None ),
            ( '/_Help',         None,         None,             0, '<LastBranch>' ),
            ( '/_Help/About',   None,         None,             0, None ),
        )
        window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        window.connect('destroy', gtk.mainquit, 'WM destroy')
        window.set_title('pyg')
        window.set_size_request(300, 200)

        main_vbox = gtk.VBox(gtk.FALSE, 1)
        main_vbox.set_border_width(1)
        window.add(main_vbox)
        main_vbox.show()

        menubar = self.get_main_menu(window)
        main_vbox.pack_start(menubar, gtk.FALSE, gtk.TRUE, 0)

        hpane = gtk.HPaned()
        left = gtk.VPaned()
        hpane.pack1(left, gtk.TRUE, gtk.FALSE)
        right = gtk.VPaned()
        hpane.pack2(right, gtk.TRUE, gtk.FALSE)

        left.pack1(gtk.Label('  Left Top '), gtk.TRUE, gtk.FALSE)
        left.pack2(gtk.Label('Left Bottom'), gtk.TRUE, gtk.FALSE)
        right.pack1(gtk.Label(' Right Top  '), gtk.TRUE, gtk.FALSE)
        right.pack2(gtk.Label('Right Bottom'), gtk.TRUE, gtk.FALSE)

        main_vbox.pack_start(hpane, gtk.TRUE, gtk.TRUE, 0)

        window.show_all()

MainWindow()
gtk.main()
