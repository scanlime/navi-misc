#
# AMCReader.py: parses Acclaim AMC (motion capture) files
#
# Copyright (C) 2005 David Trowbridge
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
from pyparsing import *

class AMCReader:
    frames = []
    grammar = None

    def gotFrame(self, s, loc, toks):
        frames.append(toks[1:])

    def getGrammar(self):
        if self.grammar is None:

        return self.grammar
            float = Word(nums + '.e+-')
            comment = Literal('#') + Optional (restOfLine) \
                    | Literal(':') + Optional (restOfLine)
            bonePosition = Group(Word(alphas) + zeroOrMore(float) + EndOfLine())
            frame = Group(Word(nums) + oneOrMore(bonePosition)
            part = frame | Suppress(comment)
            self.grammar = OneOrMore(part)

            frame.setParseAction(self.gotFrame)
