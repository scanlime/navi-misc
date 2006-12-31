#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# tests.py -- Unit tests for the recipes app
#
# Copyright (C) 2006 David Trowbridge
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

import unittest
from bonvivant.apps.recipes.models import Ingredient

class IngredientConstructionTests(unittest.TestCase):
    """
    Comprehensive tests for Ingredient.construct()

    This is a feeble attempt at some reasonable coverage of the parsing
    facilities used to create an Ingredient object from a user-provided
    string.
    """

    def testSuccess00(self):
        ingredient = Ingredient.construct('2 tbsp olive oil')

        self.assertEquals(ingredient.amount,   2.0)
        self.assertEquals(ingredient.unit,     'tbsp')
        self.assertEquals(ingredient.item,     'olive oil')
        self.assertEquals(ingredient.optional, False)

    def testSuccess01(self):
        ingredient = Ingredient.construct('3 large eggs')

        self.assertEquals(ingredient.amount,   3.0)
        self.assertEquals(ingredient.unit,     '')
        self.assertEquals(ingredient.item,     'large eggs')
        self.assertEquals(ingredient.optional, False)

    def testSuccess02(self):
        ingredient = Ingredient.construct('6 c. chicken stock')

        self.assertEquals(ingredient.amount,   6.0)
        self.assertEquals(ingredient.unit,     'c.')
        self.assertEquals(ingredient.item,     'chicken stock')
        self.assertEquals(ingredient.optional, False)

    def testSuccess03(self):
        ingredient = Ingredient.construct('salt and pepper to taste')

        self.assertEquals(ingredient.amount,   0.0)
        self.assertEquals(ingredient.unit,     '')
        self.assertEquals(ingredient.item,     'salt and pepper to taste')
        self.assertEquals(ingredient.optional, False)

    def testSuccess04(self):
        ingredient = Ingredient.construct(u'2 ⅛ mg. lime juice (optional)')

        self.assertEquals(ingredient.amount,   2.125)
        self.assertEquals(ingredient.unit,     'mg.')
        self.assertEquals(ingredient.item,     'lime juice')
        self.assertEquals(ingredient.optional, True)

    def testSuccess05(self):
        ingredient = Ingredient.construct('2 1/2 pounds potatoes (peeled)')

        self.assertEquals(ingredient.amount,   2.5)
        self.assertEquals(ingredient.unit,     'pounds')
        self.assertEquals(ingredient.item,     'potatoes (peeled)')
        self.assertEquals(ingredient.optional, False)

    def testSuccess06(self):
        ingredient = Ingredient.construct('one and a half large green chillies, peeled, seeded and cored')

        self.assertEquals(ingredient.amount,   1.5)
        self.assertEquals(ingredient.unit,     '')
        self.assertEquals(ingredient.item,     'large green chillies, peeled, seeded and cored')
        self.assertEquals(ingredient.optional, False)

    def testSuccess07(self):
        ingredient = Ingredient.construct(u'¼ fl. oz. ice water')

        self.assertEquals(ingredient.amount,   0.25)
        self.assertEquals(ingredient.unit,     'fl. oz.')
        self.assertEquals(ingredient.item,     'ice water')
        self.assertEquals(ingredient.optional, False)

    def testSuccess08(self):
        ingredient = Ingredient.construct('1 egg')

        self.assertEquals(ingredient.amount,   1.0)
        self.assertEquals(ingredient.unit,     '')
        self.assertEquals(ingredient.item,     'egg')
        self.assertEquals(ingredient.optional, False)

    def testSuccess09(self):
        ingredient = Ingredient.construct('pinch of salt (Optional)')

        self.assertEquals(ingredient.amount,   0.0)
        self.assertEquals(ingredient.unit,     'pinch of')
        self.assertEquals(ingredient.item,     'salt')
        self.assertEquals(ingredient.optional, True)

    def testSuccess10(self):
        ingredient = Ingredient.construct('grated parmesan cheese')

        self.assertEquals(ingredient.amount,   0.0)
        self.assertEquals(ingredient.unit,     '')
        self.assertEquals(ingredient.item,     'grated parmesan cheese')
        self.assertEquals(ingredient.optional, False)

    def testFailure00(self):
        ingredient = Ingredient.construct('')

        self.assertEquals(ingredient, None)

    def testFailure01(self):
        self.assertRaises(ValueError, Ingredient.construct, 'hello')

    def testFailure02(self):
        self.assertRaises(ValueError, Ingredient.construct, '2')

    def testFailure03(self):
        self.assertRaises(ValueError, Ingredient.construct, 'fishies')
