#!/usr/bin/env python
import gtk, gtk.gl, gtk.gdk, vte, os

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
            ( '/File/_New',                    None,                self.print_hello, 0, None ),
            ( '/File/_Open',                   None,                self.print_hello, 0, None ),
            ( '/File/sep1',                    None,                None,             0, '<Separator>' ),
            ( '/File/Open Setup',              None,                None,             0, None),
            ( '/File/Save Setup',              None,                None,             0, None),
            ( '/File/Close Setup',             None,                None,             0, None),
            ( '/File/sep2',                    None,                None,             0, '<Separator>' ),
            ( '/File/Open Scene',              None,                None,             0, None ),
            ( '/File/sep3',                    None,                None,             0, '<Separator>' ),
            ( '/File/Quit',                    None,                gtk.mainquit,     0, None ),
            ( '/Compile',                      None,                None,             0, '<Branch>' ),
            ( '/Compile/Compile',              None,                None,             0, None ),
            ( '/Compile/Configuration',        None,                None,             0, None ),
            ( '/Compile/Compile All',          '<control><shift>D', None,             0, None ),
            ( '/Compile/Go to Next Error',     None,                None,             0, None ),
            ( '/Render',                       None,                None,             0, '<Branch>' ),
            ( '/Render/Refresh',               None,                None,             0, None ),
            ( '/Render/Refresh After Compile', None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Refresh After Tweak',   None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Wireframe',             None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Wireframe When Moved',  None,                None,             0, '<ToggleItem>' ),
            ( '/Render/Draw World Axes',       None,                None,             0, '<ToggleItem>' ),
            ( '/Control',                      None,                None,             0, '<Branch>' ),
            ( '/Control/Animate Model',        None,                None,             0, '<ToggleItem>' ),
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

        self.quads = [
            gtk.Frame(label=None),  # upper left
            gtk.Frame(label=None),  # lower left
            gtk.Frame(label=None),  # upper right
            gtk.Frame(label=None),  # lower right
            ]

        self.left.pack1(self.quads[0], gtk.TRUE, gtk.FALSE)
        self.left.pack2(self.quads[1], gtk.TRUE, gtk.FALSE)
        self.right.pack1(self.quads[2], gtk.TRUE, gtk.FALSE)
        self.right.pack2(self.quads[3], gtk.TRUE, gtk.FALSE)

        self.main_vbox.pack_start(self.hpane, gtk.TRUE, gtk.TRUE, 0)

        self.window.show_all()

    def set(self, quad, item):
        self.quads[quad].add(item)
        item.show_all()

class SourceNotebook(gtk.Notebook):
    def __init__(self):
        gtk.Notebook.__init__(self)
        self.set_scrollable(gtk.TRUE)

class CompileOutput(gtk.TextView):
    def __init__(self):
        gtk.TextView.__init__(self)
        self.set_editable(gtk.FALSE)
        self.set_cursor_visible(gtk.FALSE)
        self.buffer = self.get_buffer()

    def set_text(self, string):
        self.buffer.set_text(string)

class ScrollArea(gtk.ScrolledWindow):
    def __init__(self, child):
        gtk.ScrolledWindow.__init__(self)
        self.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        self.add(child)

class GLView(gtk.gl.Area):
    def __init__(self):
        gtk.DrawingArea.__init__(self)

class Editor(vte.Terminal):
    def __init__(self):
        vte.Terminal.__init__(self)
        self.set_color_foreground(gtk.gdk.color_parse('Black'))
        self.set_color_background(gtk.gdk.color_parse('White'))
        editor = os.environ.get('EDITOR', 'vim')
        del os.environ['DISPLAY']
        self.fork_command('sh', ('sh', '-c', editor),
                          None, os.getcwd(),
                          gtk.FALSE, gtk.FALSE, gtk.FALSE)

main = MainWindow()

sources = SourceNotebook()
sources.append_page(Editor(), gtk.Label('Look Ma! A tab!'))
main.set(0, sources)

compiler_output = CompileOutput()
infile = open('pyg.py', 'r')
if infile:
    string = infile.read()
    infile.close()
    compiler_output.set_text(string)
main.set(1, ScrollArea(compiler_output))

view = GLView()
main.set(2, view)

main.set(3, gtk.Label('Right Bottom'))

gtk.main()
