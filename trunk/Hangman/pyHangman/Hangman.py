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
	def __init__(self):
		seed()
		self.answer = None
		self.guesses = []
		self.correct = []
		self.words = open("games/test.txt", 'r').readlines()
		self.words = [word.strip() for word in self.words]

	def Guess(self, guess):
		""" Checks the guess against the correct answer.  If it's correct: guess
		gets added to self.correct in all of the appropriate spots in the word.
		If it's incorrect it gets appended to self.guesses.  correct and guesses are
		returned in a list, correct first. """
		# Test for a correct answer.
		if self.answer.find(guess) != -1:
			# If the answer is correct insert it into the correct places in the
			# 'correct' list.
			self.correct = [self.TestEntry(i, guess) for i in range(len(self.correct))]
		# If not correct append to guesses.
		else:
			if len(self.guesses) % 8 == 0 and len(self.guesses) != 0:
				self.guesses.append('\n')
			self.guesses.append(guess)

		# Return the updated lists.
		return (self.correct, self.guesses)

	def NewGame(self):
		""" Create a new game. """
		# Most of this is probably temporary.  It will hopefully be replaced with
		# something more useful.
		self.guesses = []
		self.correct = []
		self.answer = choice(self.words)
		for i in range(len(self.answer)):
			self.correct.append('_')
		
		print self.answer

	def TestEntry(self, index, entry):
		""" Test 'entry' against all of the characters in words. If it matches, return
		it, if not and that letter has already been guessed correctly return the letter,
		otherwise return a '_'.
		"""
		if self.correct[index] != '_':	
			return self.correct[index]
		elif self.answer[index] != entry:	
			return '_'
		else:
			return entry

class HangmanGUI:
	""" A class for creating and controlling a GUI for Hangman. __init__ creates
	all the necessary widgets and connects them to the appropriate Hangman functions. """
	def __init__(self):
		# Game controller.
		self.controller = Hangman()
		self.controller.NewGame()

		# Set up the window.
		self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
		self.window.set_title("Hangman")
		self.window.set_border_width(10)

		self.window.connect("delete_event", self.Quit)

		# Text entry field for entering guesses.
		self.guessField = gtk.Entry(1)
		self.guessField.connect("activate", self.GuessCheck, self.guessField)

		# Label to display the correctly guessed letters.
		self.correctText = gtk.Label(" ".join(self.controller.correct))

		# Label to display the incorrect guesses.
		guessFrame = gtk.Frame("Missed Guesses")
		self.guessedText = gtk.Label(None)
		self.guessedText.set_line_wrap(gtk.TRUE)
		self.guessedText.set_justify(gtk.JUSTIFY_FILL)
		guessFrame.add(self.guessedText)
		self.guessedText.show()

		# New game button.
		self.newGame = gtk.Button("New Game")
		self.newGame.connect("clicked", self.NewGame)

		# Quit button.
		self.quit = gtk.Button("Quit")
		self.quit.connect("clicked", self.Quit)

		# Boxes for window layout.
		
		# buttonBox: contains the New Game button above the Quit button.
		buttonBox = self.Pack(gtk.VBox, [None, self.quit, self.newGame], [gtk.TRUE, gtk.FALSE, 5])
		buttonBox.show()
		
		# box1: text entry field for entering guesses to the left of the buttonBox.
		box1 = self.Pack(gtk.HBox, [self.guessField, None, buttonBox], [gtk.FALSE, gtk.FALSE, 5])
		box1.show()

		# textBox: correctText to the left of guessedText.
		textBox = self.Pack(gtk.HBox, [self.correctText, None, guessFrame], [gtk.TRUE, gtk.TRUE, 20])
		textBox.show()
		# box2: label displaying correct guesses packed above box1.
		box2 = self.Pack(gtk.VBox, [textBox, box1], [gtk.TRUE, gtk.FALSE, 5])
		self.window.add(box2)
		box2.show()

		# Last thing, show the window.
		self.window.show()

	def GuessCheck(self, widget, guessField):
		""" Grab the guess entered by the user."""
		entry = guessField.get_text()
		guessField.select_region(0,1)
		self.controller.correct, self.controller.guesses = self.controller.Guess(entry)
		self.Update()

	def Quit(self, widget, data=None):
		gtk.main_quit()
		return gtk.FALSE

	def Pack(self, boxClass, widgets, packArgs=[]):
		""" Packs all the widgets in the last argument into a box of boxClass
		and returns the box.  packArgs are used as the arguments to the pack 
		function. widgets is a list of widgets to be packed: everything before
		a 'None' item is packed with pack_start(), everything after is packed with
		pack_end() in reverse order."""
		
		# Create a new box.
		box = boxClass()
		# Get the index of the None argument.
		if widgets.count(None) == 0:
			split = len(widgets)
		else:
			split = widgets.index(None)

		# Pack the widgets with packArgs using pack_start.
		for child in widgets[:split]:
			box.pack_start(child, *packArgs)
			child.show()
		
		endWidgets = widgets[split+1:]
		endWidgets.reverse()
		# Pack the widgets with packArgs using pack_end.
		for child in endWidgets:
			box.pack_end(child, *packArgs)
			child.show()

		return box

	def Update(self):
		self.correctText.set_text(" ".join(self.controller.correct))
		self.guessedText.set_text(" ".join(self.controller.guesses))

	def NewGame(self, widget, data=None):
		self.controller.NewGame()
		self.Update()
