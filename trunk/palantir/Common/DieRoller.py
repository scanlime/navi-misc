''' dieRoller.py

The DieRoller object can be used by UIs that need an action object for loading character
sheets.  The roll method in DieRoller will calculte the die rolls and then call the UI's
SendRoll function with the data of the roll.  This object can be used in any UI as long as
it implements a SendRoll method.
'''

# Copyright (C) 2004 W. Evan Sheehan <evan@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
    totalTimes = 0
    rolls = []

    # Roll the dice and store each individual roll as well as a running total
    for time in times:
      totalTimes += time
    for roll in range(totalTimes):
      rolls.append(randint(1, sides))

    # Use the dice system from D&D.
    if self.system == 'D&D':
      total = self.D_and_D(rolls, mods)
    # Use White Wolf dice system.
    elif self.system == 'WW':
      total = self.White_Wolf(rolls, mods, diff)

    # Send the results to the channel.
    self.ui.SendRoll(totalTimes, sides, rolls, total)

  def D_and_D(self, rolls, mods):
    ''' Take rolls (a list of dice rolls) and calculate the total score, plus all the
        modifiers and return it.
	rolls is a list, mods is an integer.
	'''
    total = 0
    # Add the rolls to the total.
    for roll in rolls:
      total += roll
    # Add the modifiers to the total.
    for mod in mods:
      total += mod
    # Return the total.
    return total

  def White_Wolf(self, rolls, mods, diff):
    ''' Take rolls and calculate the total number of rolls that scored equal to or greater
        than diff after adding mods to them.
	rolls is a list of integers, mods and diff are integers.
	'''
    total = 0
    for i in range(len(rolls)):
      # If the roll isn't a critical failure...
      if rolls[i] != 1:
	# Add the modifiers.
        for mod in mods:
	  rolls[i] += mod
	# If the roll beats the difficulty increase the total successes.
	if rolls[i] >= diff:
	  total += 1
      # On a critical failure subtract one from the successes.
      else:
	total -= 1
    # Return the total successes.
    return total
