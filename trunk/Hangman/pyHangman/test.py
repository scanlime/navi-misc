#!/usr/bin/env python
""" test.py

Yay for tiny test controllers.
"""

from Menu import menu
import Hangman

control = Hangman.Hangman()
r = menu("Test", {
    "Read":control.DisplayFile,
		"Play":control.FilePlay})
r(control.GetFileName())

