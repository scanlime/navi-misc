""" Hangman.py

Hangman class, functions for controlling a game of hangman.
"""

#
# W. Evan Sheehan
# evan@navi.picogui.org
#

import sys, pickle
from random import seed, randint

class Hangman:
	def __init__(self):
		seed()
		self.answer = 0
		self.words = []
		self.guesses = []

	def FilePlay(self, fileName):
		""" Load information from a file.  If file is not specified in 
		argument list get one from the user.
		"""
		self.words = open(fileName,'r').readlines()
		self.words = [word.strip() for word in self.words]
		self.answer = randint(0, len(self.words)-1)
		
		print self.words[self.answer]

	def NetPlay(self):
		""" Load information from the net. """
		return None

	def DisplayFile(self, fileName = None):
		""" Display the contents in the file. """
		self.words = open(fileName,'r').readlines()
		self.words = [word.strip() for word in self.words]
		
		for word in self.words:
			print word

	def Play(self):
		""" Once game data is all set this runs the game flow through the course of
		1 round.
		"""
	
	def GetFileName(self):
		""" Prompt for a file name and return it. """
		print "Enter a file name:"
		fileName = sys.stdin.readline().strip()

		return fileName
