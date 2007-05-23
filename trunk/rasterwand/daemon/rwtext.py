#!/usr/bin/env python

import os, sys, socket

font = {
    'a': '\x18$$<',
    'b': '>$$\x18',
    'c': '\x18$$',
    'd': '\x18$$>',
    'e': '\x184,\x08',
    'f': '\x08<\x0a',
    'g': '\x18\xa4\xa4|',
    'h': '>\x04\x048',
    'i': ':',
    'j': '\x80z',
    'k': '>\x10\x18$',
    'l': '>',
    'm': '<\x04<\x048',
    'n': '<\x04\x048',
    'o': '\x18$$\x18',
    'p': '\xfc$$\x18',
    'q': '\x18$$\xfc',
    'r': '<\x08\x04',
    's': '(,4\x14',
    't': '\x04\x1e$',
    'u': '\x1c\x20\x20<',
    'v': '\x1c\x20\x10\x0c',
    'w': '\x0c0\x0c0\x0c',
    'x': '$\x18$',
    'y': '\x1c\xa0\xa0|',
    'z': '$4,$',
    'A': '<\x12\x12<',
    'B': '>**\x14',
    'C': '\x1c\x22\x22',
    'D': '>\x22\x22\x1c',
    'E': '>**',
    'F': '>\x0a\x0a',
    'G': '\x1c\x22*:',
    'H': '>\x08\x08>',
    'I': '\x22>\x22',
    'J': '\x10\x20\x22\x1e',
    'K': '>\x08\x14\x22',
    'L': '>\x20\x20',
    'M': '>\x04\x08\x04>',
    'N': '>\x04\x08>',
    'O': '\x1c\x22\x22\x1c',
    'P': '>\x12\x12\x0c',
    'Q': '\x1c\x22\x22\x5c',
    'R': '>\x12\x12,',
    'S': '$**\x12',
    'T': '\x02>\x02',
    'U': '\x1e\x20\x20\x1e',
    'V': '\x1e\x20\x18\x06',
    'W': '\x1e\x20\x1c\x20\x1e',
    'X': '6\x08\x086',
    'Y': '\x06((\x1e',
    'Z': '2*&',
    '!': '.',
    '"': '\x06\x00\x06',
    '#': '\x14>\x14>\x14',
    '$': '(,v\x14',
    '%': '\x020\x08\x06\x20',
    '&': '\x14**\x10(',
    '\'': '\x06',
    '(': '\x1c\x22',
    ')': '\x22\x1c',
    '*': '\x0a\x04\x0a',
    '+': '\x08\x1c\x08',
    ',': '@\x20',
    '-': '\x08\x08\x08',
    '.': '\x20',
    '/': '\x20\x10\x08\x04\x02',
    ':': '\x14',
    ';': '4',
    '<': '\x08\x14\x22',
    '=': '\x14\x14\x14',
    '>': '\x22\x14\x08',
    '?': '\x02*\x0a\x04',
    '@': '\x1c\x22:*\x1c',
    '[': '>\x22',
    '\\': '\x02\x04\x08\x10\x20',
    ']': '\x22>',
    '^': '\x04\x02\x04',
    '_': '\x20\x20\x20\x20',
    '`': '\x02\x04',
    '{': '\x086\x22',
    '|': '>',
    '}': '\x226\x08',
    '~': '\x04\x02\x04\x02',
    '0': '\x1c\x22\x22\x1c',
    '1': '\x02>',
    '2': '2**$',
    '3': '\x22**\x14',
    '4': '\x18\x14>\x10',
    '5': '.**\x12',
    '6': '\x1c**\x10',
    '7': '\x022\x0a\x06',
    '8': '\x14**\x14',
    '9': '\x04**\x1c',
}

def send_frame(frame):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.sendto(frame, (os.getenv("RWD"), 12345))
    
def scroll_frame(frame):
    for shift in range(8, -1, -1):
        for reps in range(3):
            send_frame(''.join([chr((ord(c) >> shift) & 0xFF) for c in frame]))

def send_text(text, min_width=50):
    frame = '\0'.join([font.get(c, '') for c in text])

    if min_width and min_width > len(frame):
        frame = frame.center(min_width, '\0')

    scroll_frame(frame)

if __name__ == "__main__":
    while True:
        line = sys.stdin.readline()
        if line:
            send_text(line.strip())
        else:
            break
