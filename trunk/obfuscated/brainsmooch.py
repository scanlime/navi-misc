#!/usr/bin/env python
#
# brainsmooch.. it's like brainfuck, but with emoticons :)
#
# Why the weird name? Well, the :-* operator is quite common,
# as you can see from this snippet of sample code:
#     :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-*
#     :-* :-* :-* :) :p :-} :( :-{ :) :-{ :p :-} :) :-{ :p :-} :( :( :-{ :)
#     :-* :) :-* :( :( :p :-} :) :) :-{ :( :( :-* :) :) :p :-} :) :) :) :-{
#     :p :-} :( :( :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :( :-{ :) :) :) :-*
#     :( :( :-{ :) :-* :) :-{ :p :-} :( :( :p :-} :) :-{ :( :-* :) :p :-} :)
#     :-{ :( :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :) :) :-* :(
#
# The twisted creation of Micah Dowty <micah@picogui.org> :p
#
# This file is both a python program and a brainsmooch program :-}
# When executed in python, it's both a brainsmooch interpreter and a
# transliterator for converting brainfuck to brainsmooch. When executed in
# brainsmooch, it prints the "99 Bottles of Beer on the Wall" song :( :(
#
# The brainsmooch program is transliterated from Ben Olmstead's original
# brainfuck version :p
# You can hopefully find that at:
#   http://esoteric.sange.fi/brainfuck/bf-source/prog/99botles.bf
#
# My apologies to anyone who tries to use this :(
#

languageDefinition = {
    # BS     BF   Definition
    ':)':  ('>', 'pointer_increment'),
    ':-{': ('[', 'start_loop'),
    ':(':  ('<', 'pointer_decrement'),
    ':p':  ('-', 'memory_decrement'),
    ':-}': (']', 'end_loop'),
    ':-*': ('+', 'memory_increment'),   
    '^_^': ('.', 'output'),
    '^o^': (',', 'input'),
    }

import sys, re
try:
    import psyco
    psyco.full()
except:
    pass

def readTokens(stream):
    """Read a brainsmooch file into a token list"""
    tokens = []
    for token in re.split(r'\s+', stream.read()):
        if languageDefinition.has_key(token):
            tokens.append(token)
    return tokens

def readBrainfuck(stream):
    """Read a brainfuck program, converting into brainsmooch tokens"""
    tokens = []
    for char in stream.read():
        for token in languageDefinition:
            if languageDefinition[token][0] == char:
                tokens.append(token)
                break
    return tokens

def writeTokens(tokens, stream, pageWidth=70):
    """Write a token list into a brainsmooch file"""
    line = ''
    for token in tokens:
        if len(line) + len(token) + 1 > pageWidth:
            stream.write(line + '\n')
            line = ''
        if line:
            line += ' ' + token
        else:
            line = token
    stream.write(line + '\n')

class Interpreter:
    def __init__(self, program, memsize=64*1024):
        self.program = program
        self.memory = [0] * memsize
        self.pointer = 0
        self.programCounter = 0

    def run(self):
        while self.programCounter < len(self.program):
            token = self.program[self.programCounter]
            getattr(self, languageDefinition[token][1])()

    def pointer_increment(self):
        self.pointer += 1
        self.programCounter += 1

    def pointer_decrement(self):
        self.pointer -= 1
        self.programCounter += 1

    def memory_increment(self):
        self.memory[self.pointer] = (self.memory[self.pointer] + 1) & 255
        self.programCounter += 1

    def memory_decrement(self):
        self.memory[self.pointer] = (self.memory[self.pointer] - 1) & 255
        self.programCounter += 1

    def output(self):
        sys.stdout.write(chr(self.memory[self.pointer]))
        self.programCounter += 1

    def input(self):
        self.memory[self.pointer] = ord(sys.stdin.read(1))
        self.programCounter += 1

    def start_loop(self):
        if self.memory[self.pointer]:
            self.programCounter += 1
        else:
            # Jump past the matching end_loop
            nesting = 0
            while True:
                token = self.program[self.programCounter]
                command = languageDefinition[token][1]
                if command == 'start_loop':
                    nesting += 1
                if command == 'end_loop':
                    nesting -= 1
                self.programCounter += 1
                if not nesting:
                    break
            
    def end_loop(self):
        # Jump to the matching start_loop
        nesting = 0
        while True:
            token = self.program[self.programCounter]
            command = languageDefinition[token][1]
            if command == 'start_loop':
                nesting += 1
            if command == 'end_loop':
                nesting -= 1
            if not nesting:
                break
            self.programCounter -= 1
        
if __name__ == '__main__':
    # Cheesy command line interface
    if len(sys.argv) > 1 and sys.argv[1] == 'bf_to_bs':
        writeTokens(readBrainfuck(sys.stdin),sys.stdout)
    elif len(sys.argv) > 1 and sys.argv[1] == 'dump_tokens':
        writeTokens(readTokens(sys.stdin),sys.stdout)
    else:
        stream = sys.stdin
        if len(sys.argv) > 1:
            stream = open(sys.argv[1])
        Interpreter(readTokens(stream)).run()
        
# :p :-} :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :) :-{ :( :p :) :p :-} :) :) :-* :) :-{ :( :-{ :p :-} :( :( :-* :) :)
# :) :p :-} :) :-{ :p :-} :-* :( :( :-{ :) :-* :) :p :( :( :p :-} :( :(
# :( :-{ :) :) :-* :) :-* :( :( :( :p :-} :) :) :) :-{ :( :( :( :-* :)
# :) :) :p :-} :) :-{ :( :-* :) :p :-} :( :( :p :-{ :) :-{ :p :-} :( :-{
# :p :-} :-} :) :) :-* :( :-{ :) :-{ :p :-} :( :p :-} :( :-* :-* :-* :-*
# :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :( :-* :-* :-* :-* :-*
# :-* :) :) :p :-} :) :) :) :-{ :) :-* :) :-* :( :( :p :-} :) :) :-{ :(
# :( :-* :) :) :p :-} :( :-{ :( :( :( :( :( ^_^ :) :) :) :) :) :p :-}
# :( :( :( :( :( :( ^_^ :) :) :-{ :p :-} :) :-{ :p :-} :-* :-* :-* :-*
# :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :) :-* :-*
# :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :-* :-*
# ^_^ :) :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :) :p :-} :( ^_^ :) :( :-* :-* :-* :-* :-* ^_^ ^_^ :p :p :p :p :p
# :p :p :p ^_^ :p :p :p :p :p :p :p ^_^ :) :) :-{ :) :) :-* :) :-* :(
# :( :( :p :-} :) :) :) :-{ :( :( :( :-* :) :) :) :p :-} :( :-{ :( :( :(
# :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :) :)
# :) :) :p :-} :( :( :( :( :-{ :p :-} :) :-* :-* :-* :-* :-{ :( :-* :-*
# :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :) :-* :-* :-* :-* :-* :-*
# :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p
# :p ^_^ :p :p :p :p :p :p :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-*
# :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p :-} :( ^_^ :) :-* :-* :-*
# :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p
# :-} :( ^_^ :-* :-* :-* ^_^ ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :-* :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p
# :p :p :p :p :p :p :p :p :) :p :-} :( :p :p ^_^ :) :-* :-* :-* :-* :-*
# :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-}
# :( :p :p ^_^ :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p
# :p :p :p :p :p :p :p :p :) :p :-} :( :-* :-* ^_^ :) :-* :-* :-* :-*
# :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p
# :-} :( :-* :-* :-* :-* ^_^ :p :p :p :p :p :p :p :p :p :p :p :p ^_^
# :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p
# :p :p :p :p :) :p :-} :( :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-*
# :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p
# ^_^ :) :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :p :) :p :-} :(
# ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^ ^_^ :) :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p
# :-} :( :p :p :p :p :p ^_^ :p :p :p ^_^ :) :) :) :-{ :) :-* :) :-* :(
# :( :p :-} :) :) :-{ :( :( :-* :) :) :p :-} :( :-{ :( :( :( :( :( ^_^
# :) :) :) :) :) :p :-} :( :( :( :( :( :( ^_^ :) :) :) :-* :-* :-* :-*
# :-{ :( :-* :-* :-* :-* :-* :-* :) :p :-} :( :p :p ^_^ :) :-* :-* :-*
# :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :-* :-* ^_^
# :) :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :)
# :p :-} :( ^_^ :) :( :-* :-* :-* :-* :-* ^_^ ^_^ :p :p :p :p :p :p
# :p :p ^_^ :p :p :p :p :p :p :p ^_^ :) :) :-{ :) :) :-* :) :-* :( :(
# :( :p :-} :) :) :) :-{ :( :( :( :-* :) :) :) :p :-} :( :-{ :( :( :( :(
# :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :) :) :)
# :) :p :-} :( :( :( :( :-{ :p :-} :) :-* :-* :-* :-* :-{ :( :-* :-* :-*
# :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :) :-* :-* :-* :-* :-* :-* :-*
# :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p :p
# ^_^ :p :p :p :p :p :p :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-*
# :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p :-} :( ^_^ :) :-* :-* :-*
# :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p
# :-} :( ^_^ :-* :-* :-* ^_^ ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :-* :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-{ :(
# :p :p :p :p :p :p :p :p :p :p :) :p :-} :( :p ^_^ :p :p :p ^_^ :)
# :-* :-* :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :) :p :-} :( :-* :-* :-* :-* ^_^ :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :-* :-* :-* :-* ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^
# :p :p :p :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p
# :p :p :p :p :p :p :p :) :p :-} :( :-* ^_^ :) :-* :-* :-* :-* :-* :-*
# :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p
# ^_^ :p ^_^ :p :p :p :p :p :p :p :p :p ^_^ :) :-* :-* :-* :-* :-*
# :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p :-} :( :-* ^_^ :)
# :-* :-* :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :) :p :-} :( :p :p ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-* ^_^ :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :p :p :p :p :p :p :p :p
# :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p
# :p :p :p :) :p :-} :( :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-{ :( :-*
# :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :-*
# :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :p :p :p :p :p :p
# :p :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p
# :p :p :p :p :p :) :p :-} :( :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-*
# :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :(
# ^_^ :) :-* :-* :-* :-{ :( :p :p :p :p :p :) :p :-} :( ^_^ :) :-* :-*
# :-* :-{ :( :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ ^_^ :) :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p :) :p
# :-} :( :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-{ :( :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :-* :-* :-* ^_^ :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-*
# :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :p :) :p :-} :( :-* :-* :-*
# :-* ^_^ :) :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :) :-* :-* :-* :-{ :( :-* :-*
# :-* :-* :-* :-* :) :p :-} :( :p ^_^ :p :p :p ^_^ :-* :-* :-* :-* :-*
# :-* ^_^ :p :p :p :p :p :p :p ^_^ :p :p :p :p :p :p :p :p :p :p ^_^
# :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p
# :p :p :) :p :-} :( :-* ^_^ :p :p :p ^_^ :-{ :p :-} :( :( :( :p :)
# :-{ :p :-} :) :-{ :p :-} :( :( :-{ :) :-* :) :-* :( :( :p :-} :) :)
# :-{ :( :( :-* :) :) :p :-} :) :) :) :-{ :p :-} :( :( :( :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :( :-{ :) :) :) :-* :( :( :-{ :) :-* :) :-{ :p
# :-} :( :( :p :-} :) :-{ :( :-* :) :p :-} :) :-{ :( :( :-* :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :) :) :) :-* :( :p :-} :( :( :p :( :p :-} :-*
# :-* :-* :-* :-* :-* :-* :-* :-* :) :-{ :( :p :) :p :-} :) :) :-* :)
# :-{ :( :-{ :p :-} :( :( :-* :) :) :) :p :-} :) :-{ :p :-} :-* :( :(
# :-{ :) :-* :) :p :( :( :p :-} :( :( :( :-{ :) :) :-* :) :-* :( :( :(
# :p :-} :) :) :) :-{ :( :( :( :-* :) :) :) :p :-} :( :) :) :-{ :( :-*
# :) :p :-} :( :( :p :-{ :) :-{ :p :-} :( :-{ :p :-} :-} :) :) :-* :(
# :-{ :) :-{ :p :-} :( :p :-} :( :-* :-* :-* :-* :-* :-* :-* :-* :-{ :(
# :-* :-* :-* :-* :-* :-* :( :-* :-* :-* :-* :-* :-* :) :) :p :-} :) :)
# :) :-{ :) :-* :) :-* :( :( :p :-} :) :) :-{ :( :( :-* :) :) :p :-} :(
# :-{ :( :( :( :( :( ^_^ :) :) :) :) :) :p :-} :( :( :( :( :( :( ^_^
# :) :) :-{ :p :-} :) :-{ :p :-} :-* :-* :-* :-* :-{ :( :-* :-* :-* :-*
# :-* :-* :-* :-* :) :p :-} :( ^_^ :) :-* :-* :-* :-* :-{ :( :-* :-*
# :-* :-* :-* :-* :-* :-* :) :p :-} :( :-* :-* ^_^ :) :-* :-* :-* :-*
# :-* :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :) :(
# :-* :-* :-* :-* :-* ^_^ ^_^ :p :p :p :p :p :p :p :p ^_^ :p :p :p :p
# :p :p :p ^_^ :) :) :-{ :) :) :-* :) :-* :( :( :( :p :-} :) :) :) :-{
# :( :( :( :-* :) :) :) :p :-} :( :-{ :( :( :( :( :-* :-* :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :-* :-* ^_^ :) :) :) :) :p :-} :( :( :(
# :( :-{ :p :-} :) :-* :-* :-* :-* :-{ :( :-* :-* :-* :-* :-* :-* :-*
# :-* :) :p :-} :( ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-* :-{ :(
# :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p :p ^_^ :p :p :p
# :p :p :p :p :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p
# :p :p :p :p :p :p :p :) :p :-} :( ^_^ :) :-* :-* :-* :-* :-* :-* :-{
# :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( ^_^ :-*
# :-* :-* ^_^ ^_^ :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :-*
# ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p
# :p :p :) :p :-} :( :p :p ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-*
# :-{ :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p :p ^_^ :p
# ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p
# :p :p :) :p :-} :( :-* :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{
# :( :-* :-* :-* :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :-* :-* :-*
# :-* ^_^ :p :p :p :p :p :p :p :p :p :p :p :p ^_^ :p :p :p ^_^ :) :-*
# :-* :-* :-* :-* :-* :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p :-}
# :( :-* ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-* :-{ :( :-* :-* :-* :-*
# :-* :-* :-* :-* :-* :-* :-* :) :p :-} :( :p ^_^ :) :-* :-* :-{ :( :p
# :p :p :p :p :p :p :p :p :p :p :) :p :-} :( ^_^ :-* :-* :-* :-* :-*
# :-* :-* :-* :-* :-* :-* ^_^ ^_^ :) :-* :-* :-* :-* :-* :-* :-* :-*
# :-* :-{ :( :p :p :p :p :p :p :p :p :p :p :) :p :-} :( :p :p :p :p :p
# ^_^ :p :p :p ^_^ :-* :-* :-* ^_^ :p :p :p ^_^ :-{ :p :-} :( :( :(

# The End, thank goodness :-}
