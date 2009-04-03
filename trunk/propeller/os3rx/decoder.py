#!/usr/bin/env python

import sys

logfile = open(sys.argv[1], 'r')
imgfile = open("img.pnm", "wb")

prevBits = None
count = None

def intbin(i):
    if i == 0:
        return ''
    elif i & 1:
        return '1' + intbin(i >> 1)
    else:
        return '0' + intbin(i >> 1)

def reverse(s):
    if s:
        return reverse(s[1:]) + s[0]
    else:
        return ''

for line in logfile:
    prefix, bits = line.split('[')
    bits = bits[:92]
    if len(bits) != 92:
        continue

    if bits != prevBits:
        if count is None or line.find("0"*30) >= 0:
            count = 0
        else:
            count += 1
        prevBits = bits

# 1235344918,Packet, 79bits [01011111 011101[00] 1110(01)[00] 000111000001100111000100000010011100000 1001010100100111] 
# 1235344929,Packet, 79bits [01011111 011101[00] 1110(01)[00] 111111001111100111001100111111111110011 1001000101111000] 
# 1235344943,Packet, 79bits [01011111 011101[01] 1110(10)[01] 111100110011000000100100001110111110001 0110100100000000] 
# 1235344954,Packet, 80bits [01011111 011101[00] 1110(11)[00] 110011001111000011101100111110001110001 0111111000100110 1] 
# 1235344963,Packet, 92bits [01010100 100110[00] 0010(00)[00] 01011010000110101010010010000001001011100000000000001110001011101111] 


        unkBits = bits[:23]
        intervalBits = bits[23:35]
        voltsBits = bits[35:52]
        inchesBits = bits[52:76]
        checkBits = bits[76:92]
        
        inches = "%06x" % int(reverse(inchesBits), 2)
        inches = inches[:3] + '.' + inches[3:]

        interval = int(reverse(intervalBits), 2)
        volts = int(reverse(voltsBits), 2)
        check = int(reverse(checkBits), 2)

        print "[%4d] %s interval=%d volts=%04x %s\" chk=%04x" % (count, unkBits, interval, volts, inches, check)
