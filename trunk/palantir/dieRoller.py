''' dieRoller.py

The DieRoller object can be used by UIs that need an action object for loading character
sheets.  The roll method in DieRoller will calculte the die rolls and then call the UI's
SendRoll function with the data of the roll.  This object can be used in any UI as long as
it implements a SendRoll method.
'''

from random import randint

class DieRoller:
  ''' This is the class that handles the dice rolling for the client.  When a character
      sheet is loaded into the window it will get an instance of this class to use for
      button presses.
      '''
  def __init__(self, ui):
    ''' The roller needs to have a reference to the UI so that it can display the results
        of the dice rolls.
	'''
    self.ui = ui
    self.system = 'D&D'

  # All action objects for the character sheet are required to have this function.
  def roll(self, times, sides, mods=0, diff=0):
    ''' Roll the dice. '''
    ### This needs to be cleaned up a lot.  And support for critical misses should be
    ### added.
    total = 0
    totalTimes = 0
    rolls = []

    # Roll the dice and store each individual roll as well as a running total
    for time in times:
      totalTimes += time

    # Use the dice system from D&D.
    if self.system == 'D&D':
      for roll in range(totalTimes):
        rolls.append(randint(1, sides))
        total += rolls[len(rolls) - 1]

      # Add the modifiers to the total.
      for mod in mods:
        total += mod

    # Use the dice system from the White Wolf games.
    else:
      for roll in range(totalTimes):
	# Get the die roll.
	score = randint(1, sides)
	# Critical failure.
	if score == 1:
	  total -= 1
	else:
	  # Add the modifiers.
	  for mod in mods:
	    score += mod
	  # If it's a success increase the total by one.
	  if score >= diff:
	    total += 1
	# Append the roll to the list of rolls.
        rolls.append(score)

    # Send the results to the channel.
    self.ui.SendRoll(totalTimes, sides, rolls, total)
