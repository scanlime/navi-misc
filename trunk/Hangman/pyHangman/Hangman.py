""" Hangman.py

Hangman class, functions for controlling a game of hangman.
"""

#
# W. Evan Sheehan
# evan@navi.picogui.org
#

import sys, os
from Menu import menu
from random import seed, randint

class Hangman:
	def __init__(self):
		seed()
		self.answer = 0
		self.words = []
		self.guesses = []
		self.correct = []


	def FilePlay(self, fileName):
		""" Load information from a file.  If file is not specified in 
		argument list get one from the user.
		"""
		self.words = open(fileName,'r').readlines()
		self.words = [word.strip() for word in self.words]
		self.answer = randint(0, len(self.words)-1)
		
		print self.words[self.answer]
		self.Play()


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
		
		for i in range(len(self.words[self.answer])):
			self.correct.append('_')
		
		while len(self.guesses) < 8:
			print "Enter a guess: "
			temp = sys.stdin.readline().strip()

			# Test for a correct answer.
			if self.words[self.answer].find(temp) != -1:
				# If the answer is correct insert it into the correct places in the
				# 'correct' list.
				self.correct = [self.TestEntry(i, temp) for i in range(len(self.correct))]
			# If not correct append to guesses.
			else:
				self.guesses.append(temp)

			print self.correct

	def TestEntry(self, index, entry):
		""" Test 'entry' against all of the characters in words. If it matches, return
		it, if not and that letter has already been guessed correctly return the letter,
		otherwise return a '_'.
		"""
		if self.correct[index] != '_':	
			return self.correct[index]
		elif self.words[self.answer][index] != entry:	
			return '_'
		else:
			return entry


	def GetFileName(self):
		""" Prompt for a file name and return it. """
		print "Enter a file name:"
		fileName = sys.stdin.readline().strip()
		
		if fileName == '?':
			fileName = menu("Files:", os.listdir("games"))

		fileName = "games/" + fileName
		return fileName
