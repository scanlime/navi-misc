""" HangmanGUI

A view for Hangman.  Creates menus, opens game files and controls
the style used for the gallows.  Gallows is a subclass of
gtk.DrawingArea that separates the aspects of drawing the hangman and
gallows from the GUI.
"""

#
# W. Evan Sheehan <evan@navi.picogui.org>
#

import sys, os, gtk, string
from Hangman import Hangman

__version__ = "0.2"
__all__ = ["HangmanGUI"]

class HangmanGUI:
  """ Acts as a view and controller for the Hangman class.  __init__ creates
      all the necessary widgets for the main window.  By creating an
      instance of HangmanGUI and starting gtk.main the game is all set up.
      """
  def __init__(self):
    # Game controller.
    self.controller = Hangman()

    # This keeps a reference to the last style menu widget selected so that it can be
    # set inactive when a new one is selected.
    self.currentStyle = None

    # Set up the window.
    self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    self.window.set_title("Hangman")

    # Default window size.
    self.window.set_default_size(640, 480)

    self.window.connect("delete_event", self.destroy)

    # Text entry field for entering guesses.
    self.guessField = gtk.Entry(1)
    self.guessField.connect("activate", self.GuessCheck, self.guessField)
    self.guessField.set_editable(gtk.FALSE)

    # Label to display the correctly guessed letters.
    self.correctText = gtk.Label(" ".join(self.controller.correct))
    self.correctText.set_justify(gtk.JUSTIFY_CENTER)

    # Label to display the incorrect guesses.
    guessFrame = gtk.Frame("Missed Guesses")
    self.guessedText = gtk.Label(None)
    self.guessedText.set_justify(gtk.JUSTIFY_CENTER)
    self.guessedText.set_line_wrap(gtk.TRUE)
    guessFrame.add(self.guessedText)
    self.guessedText.show()

    # Clue field.
    self.clueFrame = gtk.Frame("Clue")
    self.clue = gtk.Label(None)
    self.clue.set_line_wrap(gtk.TRUE)
    self.clueFrame.add(self.clue)
    self.clue.show()

    # New game button.
    self.newGame = gtk.Button("New Game for Justice",gtk.STOCK_NEW)
    self.newGame.connect("clicked", self.NewGame)

    # Open new file button.
    self.openButton = gtk.Button(stock=gtk.STOCK_OPEN)
    self.openButton.connect("clicked", self.openFile)

    # Quit button.
    self.quit = gtk.Button(stock=gtk.STOCK_QUIT)
    self.quit.connect("clicked", self.Quit)

    # Gallows.
    self.gallows = Gallows()
    self.gallowsFrame = gtk.AspectFrame(ratio=self.gallows.imageAspect,obey_child=gtk.FALSE)
    self.gallowsFrame.set_shadow_type(gtk.SHADOW_IN)
    self.gallowsFrame.add(self.gallows)
    self.gallows.show()
    self.gallowsFrame.show()

    # Boxes for window layout.

    # buttonBox: contains the New Game button to the left of the Quit button.
    buttonBox = self.Pack(gtk.HBox,
        [None, self.newGame, self.openButton, self.quit],
        [gtk.TRUE, 5])
    buttonBox.show()

    # box1: text entry field for entering guesses to the left of the buttonBox.
    box1 = self.Pack(gtk.HBox,
        [self.guessField, None, buttonBox],
        packArgs=[gtk.TRUE, gtk.TRUE, 5])
    box1.show()

    # clueBox: correctText packed above clues.
    clueBox = self.Pack(gtk.VBox,
                        [self.correctText, self.clueFrame],
                        packArgs=[gtk.TRUE, gtk.TRUE])

    # textBox: correctText to the left of guessedText.
    textBox = self.Pack(gtk.HBox,
        [clueBox, None, guessFrame],
        packArgs=[gtk.TRUE, gtk.TRUE, 20])
    textBox.show()

    # Default has clues hidden.
    self.clueFrame.hide()

    # Pack the textBox, gallowsFrame and box1 into a box with a 15 pixel
    # border around it.
    box2 = gtk.VBox()
    box2.set_border_width(15)
    box2.pack_start(textBox, gtk.FALSE, gtk.FALSE, 5)
    box2.pack_start(self.gallowsFrame, gtk.TRUE, gtk.TRUE, 5)
    box2.pack_start(box1, gtk.FALSE, gtk.FALSE)
    box2.show()

    # Create a menu bar.
    self.makeMenu()

    # Final box.  Pack the menubar into the top, so it spans the entire width
    # of the window.  Pack box2 underneath it.
    box3 = gtk.VBox()
    box3.pack_start(self.menuBar, gtk.FALSE, gtk.TRUE, 0)
    box3.pack_start(box2, gtk.TRUE, gtk.TRUE, 0)
    box3.show()

    # Add the outer box to the window.
    self.window.add(box3)

    # Last thing, show the window.
    self.window.show()

  def makeMenu(self):
    """ Creates a menu bar and saves it to self.menuBar. """
    accelGroup = gtk.AccelGroup()
    styles = self.getStyles()

    menuItems = (
        ("/_File", None, None, 0, "<Branch>"),
        ("/File/New", None, None, 0, "<Branch>"),
        ("/File/New/_Game from File", "<control>N", self.NewGame, 0, None),
        ("/File/New/_Internet Game", "<control>I", self.NewGame, 1, None),
        ("/File/_Open", "<control>O", self.openFile, 0, None),
        ("/File/sep", None, None, 0, "<Separator>"),
        ("/File/_Quit", "<control>Q", self.Quit, 0, None),
        ("/_Options", None, None, 0, "<Branch>"),
        ("/Options/Enable clues", None, self.toggleClues, 0, "<CheckItem>"),
        ("/Options/Game Style", None, None, 0, "<Branch>"),
        ("/_Help", None, None, 0, "<Branch>"),
        ("/Help/_About Hangman", None, None, 0, None))

    itemFactory = gtk.ItemFactory(gtk.MenuBar, "<main>", accelGroup)
    # Create menu items from menuItems.
    itemFactory.create_items(menuItems)

    self.window.add_accel_group(accelGroup)
    self.menuBar = itemFactory.get_widget("<main>")

    # Now add all the of the style menu items to the menu.
    gameStyle = itemFactory.get_widget("<main>/Options/Game Style")

    for style in styles:
      gameStyle.append(style)
      style.show()

    # Show the menu bar.
    self.menuBar.show()

  def getStyles(self):
    """ Get a list of all the available style directories and create a tuple for the
        menu items from the list, using directory names as the style name.
        """
    # Get the first style so that we'll have a group to assign all of the other
    # RadioMenuItems to.
    styles = [gtk.RadioMenuItem(label=self.gallows.styleList[0])]

    # Create the other RadioMenuItems in the group of the first one.
    for i in range(len(self.gallows.styleList)-1):
      styles.append(gtk.RadioMenuItem(styles[0], self.gallows.styleList[i+1]))

    # Connect, the style's index in the list is passed to setStyle in order to determine
    # which style needs to be set, since the list of menu widgets is created from the
    # list of styles in the Gallows class.
    for style in styles:
      style.connect_object("toggled", self.setStyle, style, styles.index(style))

    self.currentStyle = styles[0]

    return styles

  def setStyle(self, widget, data):
    """ Set the style from the menu check item selected, only if the newly selected
        style isn't the same as the current style.
	"""
    if widget is not self.currentStyle:
      self.currentStyle = widget
      self.gallows.setStyle(data)
      self.gallowsFrame.set(0.5, ratio=self.gallows.imageAspect, obey_child=gtk.FALSE)

  def GuessCheck(self, widget, guessField):
    """ Grab the guess entered by the user and then call update the window
        with the new information.  If the game is over make the text entry
        field uneditable.
        """
    entry = guessField.get_text()
    guessField.select_region(0,1)
    # Check the guess.
    self.controller.correct,self.controller.guesses = self.controller.Guess(entry)
    # Update the window.
    self.Update(self.controller.gameStat())
    # If the game is over, prevent entry of new guesses.
    if self.controller.correct.count('_') == 0 or self.controller.numMissed == 6:
      guessField.set_editable(gtk.FALSE)

  def Quit(self, widget, data=None):
    """ Quit. """
    # Create a dialog to check if the user really wants to quit.
    dialog = gtk.MessageDialog(self.window,
                        gtk.DIALOG_DESTROY_WITH_PARENT|
                        gtk.DIALOG_MODAL,
                        gtk.MESSAGE_QUESTION,
                        gtk.BUTTONS_YES_NO,
                        "Are you sure you want to quit?")
    dialog.set_title("Quit")
    dialog.set_resizable(gtk.FALSE)

    dialog.show()
    response = dialog.run()
    if response == gtk.RESPONSE_YES:
      self.destroy()
    else:
      dialog.destroy()

  def toggleClues(self, widget, data=None):
    """ Toggle clues on and off. """
    if data.get_active():
      self.clueFrame.show()
    else:
      self.clueFrame.hide()

  def destroy(self, widget=None, event=None):
    """ Quit gtk.main. """
    gtk.main_quit()
    return gtk.FALSE

  def Pack(self, boxClass, widgets, boxArgs=[], packArgs=[]):
    """ Packs all the widgets in the last argument into a box of boxClass
        and returns the box.  packArgs are used as the arguments to the pack
        function. widgets is a list of widgets to be packed: everything before
        a 'None' item is packed with pack_start(), everything after is packed
        with pack_end() in reverse order, so that the order the widgets are
        passed is the order they will appear in the box.
        """
    # Create a new box.
    box = boxClass(*boxArgs)

    # Get the index of the None argument.
    if widgets.count(None) == 0:
      split = len(widgets)
    else:
      split = widgets.index(None)

    # Pack the widgets with packArgs using pack_start.
    for child in widgets[:split]:
      box.pack_start(child, *packArgs)
      child.show()

    # Reverse widgets for pack_end.
    endWidgets = widgets[split+1:]
    endWidgets.reverse()

    # Pack the widgets with packArgs using pack_end.
    for child in endWidgets:
      box.pack_end(child, *packArgs)
      child.show()

    return box

  def Update(self, updateString):
    """ Update the window with the new information. """
    self.correctText.set_text(updateString[0])
    self.guessedText.set_text(updateString[1])
    self.gallows.update(self.controller.numMissed)

  def NewGame(self, data=None, widget=None):
    """ Begin a new game. """
    if data == 1:
      self.controller.NetGame()
    else:
      # If there is nothing loaded bring up the file selection widget.
      if self.controller.words == {}:
        self.openFile()
        return
      self.controller.NewGame()

    # Update window.
    self.clue.set_text(self.controller.words[self.controller.answer])
    self.Update(self.controller.gameStat())
    self.gallows.update(0)

    # Clear text entry field, make editable again, and give it focus.
    self.guessField.set_text("")
    self.guessField.set_editable(gtk.TRUE)
    self.guessField.grab_focus()

  def openFile(self, widget=None, data=None):
    """ Load the contents of a file into the game. """
    # File selector.
    fileWindow = gtk.FileSelection("Hangman")
    fileWindow.set_transient_for(self.window)
    fileWindow.set_filename("games/")

    # Set up buttons.
    fileWindow.ok_button.connect("clicked", self.read, fileWindow)
    fileWindow.ok_button.connect("clicked", self.NewGame)
    fileWindow.ok_button.connect("clicked", lambda w: fileWindow.destroy())
    fileWindow.cancel_button.connect("clicked", lambda w: fileWindow.destroy())

    # Show.
    fileWindow.show()

  def read(self, data, widget):
    """ Callback for file selection from openFile().  Get the file name
        and send it to be opened and read into the controller.
        """
    self.controller.openFile(widget.get_filename())

  def error(self, parent=None, data=None):
    """ General error message generator.  data is the text to be printed in
        the dialog box.
        """
    # Create the dialog box.
    errorBox = gtk.MessageDialog(parent, gtk.DIALOG_MODAL,
                                  gtk.MESSAGE_WARNING,
                                  gtk.BUTTONS_OK,
                                  data)

    errorBox.set_resizable(gtk.FALSE)
    errorBox.show()
    if errorBox.run() == gtk.RESPONSE_OK:
      errorBox.destroy()

class Gallows(gtk.DrawingArea):
  """ gtk.DrawingArea subclass for drawing the gallows and hanged man. """
  def __init__(self):
    gtk.DrawingArea.__init__(self)
    # Event handlers.
    self.connect_after("configure_event", self.gallowsConfigure)
    self.connect("expose_event", self.redraw)

    # Set up the style.
    self.styleFile = "graphics/styles/Burton"
    self.styleList = os.listdir("graphics/styles")

    # Remove any hidden directories from styleList, specifically: .svn/ .
    for style in self.styleList:
      if style[:1] == '.':
        self.styleList.remove(style)
    self.styleList.sort()

    # Load the first .png.
    self.image = gtk.gdk.pixbuf_new_from_file("graphics/styles/Burton/gallows.png")
    # Calculate image aspect.
    self.imageAspect = float(self.image.get_width()) / self.image.get_height()

  def gallowsConfigure(self, widget, event):
    """ When the drawing area is realized set up a graphics context for it. """
    # Get a color map and set the foreground to white in the graphics context.
    colormap = self.get_colormap()
    fgColor = colormap.alloc_color('white')
    self.bg_gc = self.window.new_gc(foreground=fgColor)
    self.fg_gc = self.window.new_gc()

    # Get the width and height of the drawing area.
    None, None, self.width, self.height, None = self.window.get_geometry()

  def setStyle(self, data, widget=None):
    self.styleFile = "graphics/styles/" + self.styleList[data]
    self.image = gtk.gdk.pixbuf_new_from_file(self.styleFile + "/gallows.png")
    self.imageAspect = float(self.image.get_width()) / self.image.get_height()
    self.redraw()

  def redraw(self, widget=None, data=None):
    """ Redraw the screen because of an expose_event. """
    self.window.draw_rectangle(self.fg_gc, gtk.FALSE, 0, 0, self.width, self.height)
    x, y, width, height = self.drawingArea()
    image = self.image.scale_simple(width, height, gtk.gdk.INTERP_BILINEAR)
    image.render_to_drawable(self.window, self.fg_gc, 0, 0,
                                  x, y, width, height, gtk.gdk.RGB_DITHER_NORMAL,
                                  0, 0)
  def update(self, numMissed):
    """ Update the image to be drawn to the screen according to the number of incorrect
        guesses.
        """
    imageList = ["/gallows.png", "/gallows1.png", "/gallows2.png", "/gallows3.png",
                 "/gallows4.png", "/gallows5.png", "/gallows6.png"]
    self.image = gtk.gdk.pixbuf_new_from_file(self.styleFile + imageList[numMissed])
    self.redraw()

  def drawingArea(self):
    """ Calculate a square area within the gtk.DrawingArea in which to draw
        the gallows, data must be a list or tuple of x, y, height and width.
        Return a tuple contianing the x, y coordinates of the top left corner
        and the width and height.  Allows for rescalable images.
        """
    # Width is limiting factor.
    if self.width < self.height or self.width == self.height:
      height = self.width / self.imageAspect
      y = self.height / 2 - height/2
      return (0, y, self.width, height)
    # Height is limiting factor.
    elif self.height < self.width:
      width = self.height * self.imageAspect
      x = self.width / 2 - width/2
      return (x, 0, width, self.height)

    return area
