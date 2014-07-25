""" Hangman

A python package containing classes for running a game of hangman.

The Hangman class contains information about the game state: a list
of possible words to use in the game; a single word selected from the
list as answer; lists for the incorrect and correct guesses and a counter
keeping track of the number of misses.
"""

#
# W. Evan Sheehan <evan@navi.picogui.org>
#

import sys, string, wordoftheday
from urllib2 import urlopen
from random import seed, choice

__version__ = "0.2"
#__all__ = ["Hangman"]

class Hangman:
  """ Functions and data for running a game of Hangman. """
  def __init__(self):
    seed()
    # List of words to select an answer from.
    self.words = {}
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
    wordList = self.words.keys()
    self.answer = choice(wordList)

    # Fill the correct list with '_'.
    for i in range(len(self.answer)):
      self.correct.append('_')

    # Reset the number of misses
    self.numMissed = 0
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
    elif self.numMissed == 6:
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
    elif self.answer[index] != entry:
      return '_'
    # If the guess is right, return it.
    else:
      return entry

  def openFile(self, filename):
    """ Open file filename and read it into a list.  Separate the list into
        a dictionary wher the key is the word and the value is a clue.  Store
        the dictionary in self.words.
        """
    # Get the strings from the file.
    wordList = open(filename).readlines()
    wordList = [word.strip() for word in wordList]

    # Make the dictionary.
    self.words = dict([self.getClues(wordList, i) for i in range(len(wordList))])

  def getClues(self, wordList, i):
    """ Allows the use of a list comprehension to create key value pairs
        of the strings from a file.  If no clue is provided after a word then
        the value returned with the word is "".
        """
    # If there is no clue along with the word.
    if wordList[i].count(' ') == 0:
      return (wordList[i],"")
    # Otherwise split the word from the clue.
    else:
      index = wordList[i].find(' ')
      return (wordList[i][:index].strip(),wordList[i][index:].strip())

  def NetGame(self):
    """ Start a new game using the word of the day from dictionary.com.
        """
    self.words = wordoftheday.get_word()
    self.NewGame()
