""" units

Simple utilities for unit conversions

"""
#
# Wireless therm system
# Copyright (C) 2004 Micah Dowty
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

def degCtoF(degC):
    """Convert degrees centigrade to fahrenheit"""
    return degC * 9.0 / 5.0 + 32


class UnitCollection:
    """An abstract group of units for some quntity"""
    # A list of (singular name, plural name, multiplier) tuples.
    # Must be sorted by multiplier in descending order.
    units = []

    # If the converted value would be less than this
    # threshold, a smaller unit will be chosen.
    threshold = 0.8

    # Smallest decimal place to represent
    precision = 0.01

    def format(self, value):
        """Pick an appropriate unit and format the given value"""
        for singular, plural, multiplier in self.units:
            converted = value / multiplier
            if converted > self.threshold:
                break

        # Round the converted value to our set precision
        converted += self.precision / 2
        converted -= converted % self.precision

        # Is it close enough to 1 to use our singular form?
        if abs(converted - 1) < self.precision:
            unit = singular
        else:
            unit = plural

        # Chop off the trailing .0 if there is one
        s = str(converted)
        if s.endswith('.0'):
            s = s[:-2]
        return s + ' ' + unit


class TimeUnits(UnitCollection):
    """Time units, standard unit is 1 second"""
    units = [
        ('year',        'years',        365 * 24 * 60 * 60),
        ('month',       'months',       30 * 24 * 60 * 60),
        ('week',        'weeks',        7 * 24 * 60 * 60),
        ('day',         'days',         24 * 60 * 60),
        ('hour',        'hours',        60 * 60),
        ('minute',      'minutes',      60),
        ('second',      'seconds',      1),
        ('millisecond', 'milliseconds', 0.001),
        ('microsecond', 'microseconds', 0.000001),
        ]

formatTime = TimeUnits().format

### The End ###
