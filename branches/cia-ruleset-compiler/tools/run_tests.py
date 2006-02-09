#!/usr/bin/env python
#
# Runs doctests for all applicable CIA modules
#

import sys; sys.path[0] += '/..'
import doctest

import LibCIA.Web.RegexTransform
doctest.testmod(LibCIA.Web.RegexTransform)

import LibCIA.IRC.Formatting
doctest.testmod(LibCIA.IRC.Formatting)

import LibCIA.Database
doctest.testmod(LibCIA.Database)

import LibCIA.TimeUtil
doctest.testmod(LibCIA.TimeUtil)

import LibCIA.Ruleset
doctest.testmod(LibCIA.Ruleset)

import LibCIA.ColorText
doctest.testmod(LibCIA.ColorText)

import LibCIA.Message
doctest.testmod(LibCIA.Message)

### The End ###
