""" Hangman

A python package containing classes for running a game of hangman.

The Hangman class contains information about the game state: a list
of possible words to use in the game; a single word selected from the
list as answer; lists for the incorrect and correct guesses and a counter
keeping track of the number of misses.

The HangmanGUI class acts as a view and controller for the game.
"""

#
# W. Evan Sheehan
# evan@navi.picogui.org
#

import sys, os, gtk, string
from Menu import menu
from random import seed, choice

__all__ = ["Hangman", "HangmanGUI"]

class Hangman:
	""" Functions and data for running a game of Hangman. """
	def __init__(self):
		seed()
		# List of words to select an answer from.
		self.words = []
		# Answer for the current game.
		self.answer = None
		# List of incorrectly guessed characters.
		self.guesses = []
		# Number of incorrect guesses.
		self.numMissed = 0
		# List of correctly guessed characters.
		self.correct = []

	def Guess(self, guess):
		""" Checks the guess against the correct answer.  If it's correct guess
				gets added to self.correct in all of the appropriate spots in the word.
				If it's incorrect it gets appended to self.guesses.  correct and guesses
				are returned in a tuple, correct first. 
				"""
		# If the answer is correct insert it into the correct places in the
		# 'correct' list.
		if self.answer.find(guess) != -1:
			self.correct = [self.TestEntry(i, guess) for i in range(len(self.correct))]
		
		# If not correct and the letter hasn't been guessed yet, append to guesses.
		# Increment Hangman.numMissed.
		else:
			self.numMissed += 1
			if self.guesses.count(guess) == 0:
				self.guesses.append(guess)
	
		return (self.correct, self.guesses)

	def NewGame(self):
		""" Create a new game. """
		# Clear the guesses and correct lists.
		self.guesses = []
		self.correct = []
		# Set a new word.
		self.answer = choice(self.words)
		
		# Fill the correct list with '_'.
		for i in range(len(self.answer)):
			self.correct.append('_')
		# For debugging ease.  Should be removed.
		print self.answer

	def gameStat(self):
		""" Check the status of the current game. Return strings created from the
				correct and guesses lists in a tuple for the purpose of printing.
				In the event that the game has been won or lost an appropriate string
				is appeneded to the correct string before returning.
				"""
		correctStr = " ".join(self.correct)
		guessedStr = " ".join(self.guesses)

		if self.correct.count('_') == 0:
			correctStr += "\nYou Win!!"
		elif self.numMissed == 7:
			correctStr += "\nYou Lose!"

		return (correctStr, guessedStr)

	def TestEntry(self, index, entry):
		""" Provides a way to set the values in Hangman.correct with a list
				comprehension when the user guesses correct.  TestEntry() returns
				the appropriate character for that index of Hangman.correct.  If the
				letter there has been guessed correctly before, it is returned.  If 
				guess does not go there a '_' is returned.  If the guess belongs in
				the list at index return the guess.
				For Example:
						
					self.correct = [self.TestEntry(i, guess) 
													for i in range(len(self.correct))]
				"""
		# If the item at correct[index] has been guessed, return the already correct
		# letter.
		if self.correct[index] != '_':	
			return self.correct[index]
		# If the guess is wrong, return a '_'
		elif self.answer[index] != entry:	
			return '_'
		# If the guess is right, return it.
		else:
			return entry

class HangmanGUI:
	""" Acts as a view and controller for the Hangman class.  __init__ creates
			all the necessary widgets for the main window.  By creating an 
			instance of HangmanGUI and starting gtk.main the game is all set up.
			"""
	def __init__(self):
		# Game controller.
		self.controller = Hangman()

		# Set up the window.
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.set_title("Hangman")
		# Default window size.
		self.window.set_default_size(640, 480)

		self.window.connect("delete_event", self.destroy)

		# Text entry field for entering guesses.
		self.guessField = gtk.Entry(1)
		self.guessField.connect("activate", self.GuessCheck, self.guessField)

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

		# New game button.
		self.newGame = gtk.Button(stock=gtk.STOCK_NEW)
		self.newGame.connect("clicked", self.NewGame)

		# Open new file button.
		self.openButton = gtk.Button(stock=gtk.STOCK_OPEN)
		self.openButton.connect("clicked", self.openFile)

		# Quit button.
		self.quit = gtk.Button(stock=gtk.STOCK_QUIT)
		self.quit.connect("clicked", self.Quit)

		# Gallows.
		gallowsFrame = gtk.Frame()
		gallowsFrame.set_shadow_type(gtk.SHADOW_IN)
		self.gallows = Gallows()
		gallowsFrame.add(self.gallows)
		self.gallows.show()
		gallowsFrame.show()

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

		# textBox: correctText to the left of guessedText.
		textBox = self.Pack(gtk.HBox, 
				[self.correctText, None, guessFrame], 
				packArgs=[gtk.TRUE, gtk.TRUE, 20])
		textBox.show()

		# Pack the textBox, gallowsFrame and box1 into a box with a 15 pixel
		# border around it.
		box2 = gtk.VBox()
		box2.set_border_width(15)
		box2.pack_start(textBox, gtk.FALSE, gtk.FALSE, 5)
		box2.pack_start(gallowsFrame, gtk.TRUE, gtk.TRUE, 5)
		box2.pack_start(box1, gtk.FALSE, gtk.FALSE)
		box2.show()

		# Create a menu bar.
		self.makeMenu()
		self.menuBar.show()

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

		menuItems = (
				("/_File", None, None, 0, "<Branch>"),
				("/File/_New Game", "<control>N", self.NewGame, 0, None),
				("/File/_Open", "<control>O", self.openFile, 0, None),
				("/File/sep", None, None, 0, "<Separator>"),
				("/File/_Quit", "<control>Q", self.Quit, 0, None),
				("/_Options", None, None, 0, "<Branch>"),
				("/Options/Preferences", None, None, 0, None),
				("/_Help", None, None, 0, "<Branch>"),
				("/Help/_About Hangman", None, None, 0, None))

		itemFactory = gtk.ItemFactory(gtk.MenuBar, "<main>", accelGroup)
		itemFactory.create_items(menuItems)
		self.window.add_accel_group(accelGroup)
		self.menuBar = itemFactory.get_widget("<main>")

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
		if self.controller.correct.count('_') == 0 or self.controller.numMissed == 7:
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

	def NewGame(self, widget=None, data=None):
		""" Begin a new game. """
		# If there is nothing loaded bring up the file selection widget.
		if self.controller.words == []:
			self.openFile()
			return

		self.controller.NewGame()
		# Update window.
		self.Update(self.controller.gameStat())
		# Clear text entry field, make editable again, and give it focus.
		self.guessField.set_text("")
		self.guessField.set_editable(gtk.TRUE)
		self.guessField.grab_focus()

	def openFile(self, widget=None, data=None):
		""" Load the contents of a file into the game. """
		# File selector.
		fileWindow = gtk.FileSelection("Hangman")
		fileWindow.set_transient_for(self.window)

		# Set up buttons.
		fileWindow.ok_button.connect("clicked", self.read, fileWindow)
		fileWindow.ok_button.connect("clicked", self.NewGame)
		fileWindow.ok_button.connect("clicked", lambda w: fileWindow.destroy())
		fileWindow.cancel_button.connect("clicked", lambda w: fileWindow.destroy())

		# Show.
		fileWindow.show()

	def read(self, widget, data=None):
		""" Callback for file selection from openFile().  Get the file name
				and test it to make sure it has the right extension.
				"""
		filename = data.get_filename()
		if filename.find(".txt") == -1:
			self.error(data, "Please choose a .txt file.")
			return gtk.TRUE
		else:
			self.controller.words = open(filename, 'r').readlines()
			self.controller.words = [word.strip() for word in self.controller.words]

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
		# Minimum size of drawing window.
		self.set_size_request(100, 100)
		# Event handlers.
		self.connect_after("configure_event", self.gallowsRealize)
		self.connect("expose_event", self.redraw)

	def gallowsRealize(self, widget, event):
		""" When the drawing area is realized set up a graphics context for it. """
		# Get a color map and set the foreground to white in the graphics context.
		colormap = self.get_colormap()
		fgColor = colormap.alloc_color('white')
		self.bg_gc = self.window.new_gc(foreground=fgColor)
		self.fg_gc = self.window.new_gc()

	def redraw(self, widget, event):
		""" Redraw the screen because of an expose_event. """
		x , y, width, height = event.area
		widget.window.draw_rectangle(self.bg_gc, gtk.TRUE, x, y, width, height)

		x, y, width, height = self.drawingArea(event)
		widget.window.draw_rectangle(self.fg_gc, gtk.FALSE, x, y, width, height)

	def drawingArea(self, data):
		""" Calculate a square area within the gtk.DrawingArea in which to draw
				the gallows, data must be a list or tuple of x, y, height and width.
				Return a tuple contianing the x, y coordinates of the top left corner 
				and the width and height.  Allows for rescalable images.
				"""
		x, y, width, height = data.area

		# Width is limiting factor.
		if width < height:
			self.area = (x, height/2 - width/2, width, width)
		# Height is limiting factor.
		elif height < width:
			self.area = (width/2 - height/2, y, height, height)
		# Square drawing area.
		else:
			self.area = (x, y, width, height)

		return self.area
