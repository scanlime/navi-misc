""" Hangman.py

Hangman class, functions for controlling a game of hangman.
"""

#
# W. Evan Sheehan
# evan@navi.picogui.org
#

import sys, os, gtk, string
from Menu import menu
from random import seed, choice

class Hangman:
	""" Functions and data for running a game of Hangman. """
	def __init__(self):
		seed()
		self.words = []
		self.answer = None
		self.guesses = []
		self.numMissed = 0
		self.correct = []

	def Guess(self, guess):
		""" Checks the guess against the correct answer.  If it's correct: guess
				gets added to self.correct in all of the appropriate spots in the word.
				If it's incorrect it gets appended to self.guesses.  correct and guesses
				are returned in a list, correct first. 
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
		# Most of this is probably temporary.  It will hopefully be replaced with
		# something more useful.
		# Clear the guesses and correct lists.
		self.guesses = []
		self.correct = []
		# Set a new word.
		self.answer = choice(self.words)
		# Fill the correct list with '_'.
		for i in range(len(self.answer)):
			self.correct.append('_')
		# Temporary	
		print self.answer

	def gameStat(self):
		""" Check the status of the current game. """
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
	""" A class for creating and controlling a GUI for Hangman. __init__ creates
			all the necessary widgets and connects them to the appropriate Hangman 
			functions. 
			"""
	def __init__(self):
		# Game controller.
		self.controller = Hangman()

		# Set up the window.
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.set_title("Hangman")
		self.window.set_border_width(10)
		# Minimum window size.
		self.window.set_size_request(600, 400)

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
		self.newGame = gtk.Button("New Game")
		self.newGame.connect("clicked", self.NewGame)

		# Open new file button.
		self.openButton = gtk.Button("Open File")
		self.openButton.connect("clicked", self.openFile)

		# Quit button.
		self.quit = gtk.Button("Quit")
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

		# box2: Pack the gallows above textBox, which is above the buttons and
		# text field for entering guesses.
		box2 = gtk.VBox(spacing=20)
		box2.pack_start(textBox, gtk.FALSE, gtk.FALSE, 5)
		box2.pack_start(gallowsFrame, gtk.TRUE, gtk.TRUE, 5)
		box2.pack_start(box1, gtk.FALSE, gtk.FALSE)
		self.window.add(box2)
		box2.show()

		# Last thing, show the window.
		self.window.show()

	def GuessCheck(self, widget, guessField):
		""" Grab the guess entered by the user."""
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
		dialog = gtk.Dialog("Quit", 
				flags=gtk.DIALOG_DESTROY_WITH_PARENT|gtk.DIALOG_NO_SEPARATOR)
		
		# Yes button, they really want to quit.
		yes = gtk.Button("Yes")
		yes.connect("clicked", self.destroy)
		yes.show()

		# No button, they don't actually want to quit.
		no = gtk.Button("No")
		no.connect("clicked", lambda w: dialog.destroy())
		no.show()

		# Pose the question.
		label = gtk.Label("Are you sure you want to quit?")
		label.show()

		# Pack the dialog box and show, give focus to the 'no' button.
		dialog.vbox.pack_start(label)
		dialog.action_area.pack_start(yes, gtk.TRUE, gtk.TRUE)
		dialog.action_area.pack_start(no, gtk.TRUE, gtk.TRUE)

		no.grab_focus()
		dialog.show()

	def destroy(self, widget, event=None):
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

		# Set up buttons.
		fileWindow.ok_button.connect("clicked", self.read, fileWindow)
		fileWindow.ok_button.connect("clicked", self.NewGame)
		fileWindow.ok_button.connect("clicked", lambda w: fileWindow.destroy())
		fileWindow.cancel_button.connect("clicked", lambda w: fileWindow.destroy())

		# Show.
		fileWindow.show()

	def read(self, widget, event=None):
		filename = event.get_filename()
		if filename.find(".hmn") == -1:
			self.error(data="Please choose a .hmn file.")
			return gtk.TRUE
		else:
			self.controller.words = open(filename, 'r').readlines()
			self.controller.words = [word.strip() for word in self.controller.words]

	def error(self, widget=None, data=None):
		""" General error message generator.  data is the text to be printed in
				the dialog box.
				"""
		# Create the dialog box.
		errorBox = gtk.Dialog("Error", flags=gtk.DIALOG_NO_SEPARATOR)

		# Create the label.
		message = gtk.Label(data)
		message.show()

		# Ok button.
		okButton = gtk.Button("Ok")
		okButton.connect("clicked", lambda w: errorBox.destroy())
		okButton.show()

		# Pack everything and show.
		errorBox.vbox.pack_start(message, gtk.TRUE, gtk.TRUE)
		errorBox.vbox.pack_end(okButton)
		errorBox.show()

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
		self.gc = self.window.new_gc(foreground=fgColor)

	def redraw(self, widget, event):
		""" Redraw the screen because of an expose_event. """
		x , y, width, height = event.area
		widget.window.draw_rectangle(self.gc, gtk.TRUE, x, y, width, height)
