#!/usr/bin/env python
#
# Fun with /dev/uinput, included with Linux 2.6
# --Micah
#

import os, struct, fcntl, time, math

###### Constants and structs

UI_DEV_CREATE  = 0x5501
UI_DEV_DESTROY = 0x5502
UI_SET_EVBIT   = 0x40045564
UI_SET_KEYBIT  = 0x40045565
UI_SET_RELBIT  = 0x40045566

EV_SYN = 0x00
EV_KEY = 0x01
EV_REL = 0x02

REL_X = 0x00
REL_Y = 0x01

BUS_USB = 0x03

BTN_MOUSE = 0x110

SYN_REPORT = 0

uinput_user_dev = "80sHHHHi128s128s128s128s"
input_event = "LLHHl"

###### Test proggie

input = os.open("/dev/uinput", os.O_RDWR)

# Write device info
os.write(input, struct.pack(uinput_user_dev,
                            "Electric Spring",   # Device name
                            BUS_USB,             # Bus type
                            1,                   # Vendor
                            1,                   # Product
                            1,                   # Version
                            0,                   # ff_effects_max
                            "",                  # absmax
                            "",                  # absmin
                            "",                  # absfuzz
                            "",                  # absflat
                            ))

# Set the event bits
fcntl.ioctl(input, UI_SET_EVBIT, EV_REL)
fcntl.ioctl(input, UI_SET_RELBIT, REL_X)
fcntl.ioctl(input, UI_SET_RELBIT, REL_Y)
fcntl.ioctl(input, UI_SET_EVBIT, EV_KEY)
fcntl.ioctl(input, UI_SET_KEYBIT, BTN_MOUSE)

# Create the device
fcntl.ioctl(input, UI_DEV_CREATE)

# Send some events
theta = 0
while True:
    x_rel = int(math.sin(theta) * 10)
    y_rel = int(math.cos(theta) * 10)
    theta += 0.15
    os.write(input, struct.pack(input_event, time.time(), 0, EV_REL, REL_X, x_rel))
    os.write(input, struct.pack(input_event, time.time(), 0, EV_REL, REL_Y, y_rel))
    os.write(input, struct.pack(input_event, time.time(), 0, EV_SYN, SYN_REPORT, 0))
    time.sleep(0.008)
