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
            ( '/_File',                        None,                None,             0, '<Branch>' ),
            ( '/File/_New',                    '<control>N',        self.print_hello, 0, None ),
            ( '/File/_Open',                   '<control>O',        self.print_hello, 0, None ),
            ( '/File/_Save',                   '<control>S',        self.print_hello, 0, None ),
            ( '/File/Save _As',                None,                None,             0, None ),
            ( '/File/Save All',                '<control><shift>S', self.print_hello, 0, None ),
            ( '/File/sep1',                    None,                None,             0, '<Separator>' ),
            ( '/File/Open Setup',              None,                None,             0, None),
            ( '/File/Save Setup',              None,                None,             0, None),
            ( '/File/Close Setup',             None,                None,             0, None),
            ( '/File/sep2',                    None,                None,             0, '<Separator>' ),
            ( '/File/Open Scene',              None,                None,             0, None ),
            ( '/File/sep3',                    None,                None,             0, '<Separator>' ),
            ( '/File/Quit',                    '<control>Q',        gtk.mainquit,     0, None ),
            ( '/_Edit',                        None,                None,             0, '<Branch>' ),
            ( '/Edit/Undo',                    '<control>Z',        None,             0, None ),
            ( '/Edit/Redo',                    '<control>Y',        None,             0, None ),
            ( '/Edit/Cut',                     '<control>X',        None,             0, None ),
            ( '/Edit/Copy',                    '<control>C',        None,             0, None ),
            ( '/Edit/Paste',                   '<control>V',        None,             0, None ),
            ( '/Compile',                      None,                None,             0, '<Branch>' ),
            ( '/Compile/Compile',              '<control>D',        None,             0, None ),
            ( '/Compile/Configuration',        None,                None,             0, None ),
            ( '/Compile/Compile All',          '<control><shift>D', None,             0, None ),
            ( '/Compile/Go to Next Error',     '<control>G',        None,             0, None ),
            ( '/Render',                       None,                None,             0, '<Branch>' ),
            ( '/Render/Refresh',               '<control>R',        None,             0, None ),
            ( '/Render/Refresh After Compile', None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Refresh After Tweak',   None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Wireframe',             None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Wireframe When Moved',  None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Draw World Axes',       None,                None,             0, '<ToggleItem>' ),
            ( '/Control',                      None,                None,             0, '<Branch>' ),
            ( '/Control/Animate Model',        '<control>A',        None,             0, '<ToggleItem>' ),
            ( '/_Help',                        None,                None,             0, '<LastBranch>' ),
            ( '/_Help/About',                  None,                None,             0, None ),
        )
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect('destroy', gtk.mainquit, 'WM destroy')
        self.window.set_title('pyg')
        self.window.set_size_request(300, 200)

        self.main_vbox = gtk.VBox(gtk.FALSE, 1)
        self.main_vbox.set_border_width(1)
        self.window.add(self.main_vbox)
        self.main_vbox.show()

        self.menubar = self.get_main_menu(self.window)
        self.main_vbox.pack_start(self.menubar, gtk.FALSE, gtk.TRUE, 0)

        self.hpane = gtk.HPaned()
        self.left = gtk.VPaned()
        self.hpane.pack1(self.left, gtk.TRUE, gtk.FALSE)
        self.right = gtk.VPaned()
        self.hpane.pack2(self.right, gtk.TRUE, gtk.FALSE)

        self.ul = gtk.Frame(label=None)
        self.left.pack1(self.ul, gtk.TRUE, gtk.FALSE)
        self.ll = gtk.Frame(label=None)
        self.left.pack2(self.ll, gtk.TRUE, gtk.FALSE)
        self.ur = gtk.Frame(label=None)
        self.right.pack1(self.ur, gtk.TRUE, gtk.FALSE)
        self.lr = gtk.Frame(label=None)
        self.right.pack2(self.lr, gtk.TRUE, gtk.FALSE)

        self.ul.add(gtk.Label('  Left Top  '))
        self.ur.add(gtk.Label(' Right Top  '))
        self.lr.add(gtk.Label('Right Bottom'))

        self.compiler_output = gtk.TextView(buffer=None)
        self.compiler_output.set_editable(gtk.FALSE)
        self.compiler_output.set_cursor_visible(gtk.FALSE)
        self.ll.add(self.compiler_output)

        self.main_vbox.pack_start(self.hpane, gtk.TRUE, gtk.TRUE, 0)

        self.window.show_all()

main = MainWindow()
gtk.main()
