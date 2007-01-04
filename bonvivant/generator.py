#!/usr/bin/env python
#
# recipe-generator.py -- Simple script which generates a bunch of random
#                        recipes to populate the database with.
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

import apps.recipes.models as recipes
from lib.gourmet.defaults import lang as defaults
from django.contrib.auth.models import User

import random
import sys

def ingredient():
    amount = float(random.randint(1, 20))
    unit = random.choice(defaults.UNITS)[0]
    item = random.choice(defaults.INGREDIENT_DATA)[0]
    optional = random.randint(0, 10) == 0
    ing = recipes.Ingredient(amount = amount, unit = unit, item = item, optional = optional)
    return ing

def recipe():
    n_ingredients = random.randint(3, 15)

    cuisine   = random.choice([x + ' ' for x in defaults.fields['cuisine']] + ['']*5)
    qualifier = random.choice(['baked ', 'sour ', 'cheese and ', 'pickled '] + ['']*5)
    suffix    = random.choice([' soup'] + ['']*3)

    title = cuisine + qualifier + random.choice(defaults.INGREDIENT_DATA)[0] + suffix

    author = random.choice(User.objects.all())

    recipe = recipes.Recipe(title = title, author = author, instructions = 'Blank for now')
    recipe.save()

    for i in range(n_ingredients):
        ing = ingredient()
        ing.recipe = recipe
        ing.save()

def generate(num = 100):
    for i in range(num):
        recipe()
