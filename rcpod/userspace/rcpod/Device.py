""" rcpod.Device

Low-level interface to the rcpod firmware and hardware
"""
#
# Remote Controlled PIC of Doom
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

__all__ = ['Device']

from fcntl import ioctl
import struct, glob


# A table of special function registers, converted from the P16C765.INC header file
registerTable = {
    "INDF":	0x0000,
    "TMR0":	0x0001,
    "PCL":	0x0002,
    "STATUS":	0x0003,
    "FSR":	0x0004,
    "PORTA":	0x0005,
    "PORTB":	0x0006,
    "PORTC":	0x0007,
    "PORTD":	0x0008,
    "PORTE":	0x0009,
    "PCLATH":	0x000A,
    "INTCON":	0x000B,
    "PIR1":	0x000C,
    "PIR2":	0x000D,
    "TMR1L":	0x000E,
    "TMR1H":	0x000F,
    "T1CON":	0x0010,
    "TMR2":	0x0011,
    "T2CON":	0x0012,
    "CCPR1L":	0x0015,
    "CCPR1H":	0x0016,
    "CCP1CON":	0x0017,
    "RCSTA":	0x0018,
    "TXREG":	0x0019,
    "RCREG":	0x001A,
    "CCPR2L":	0x001B,
    "CCPR2H":	0x001C,
    "CCP2CON":	0x001D,
    "ADRES":	0x001E,
    "ADCON0":	0x001F,

    "OPTION_REG": 0x0081,
    "TRISA":	0x0085,
    "TRISB":	0x0086,
    "TRISC":	0x0087,
    "TRISD":	0x0088,
    "TRISE":	0x0089,
    "PIE1":	0x008C,
    "PIE2":	0x008D,
    "PCON":	0x008E,
    "PR2":	0x0092,
    "TXSTA":	0x0098,
    "SPBRG":	0x0099,
    "ADCON1":	0x009F,
    "UIR":	0x0190,
    "UIE":	0x0191,
    "UEIR":	0x0192,
    "UEIE":	0x0193,
    "USTAT":	0x0194,
    "UCTRL":	0x0195,
    "UADDR":	0x0196,
    "USWSTAT":	0x0197,
    "UEP0":	0x0198,
    "UEP1":	0x0199,
    "UEP2":	0x019A,

    "BD0OST":	0x01A0,
    "BD0OBC":	0x01A1,
    "BD0OAL":	0x01A2,
    "BD0IST":	0x01A4,
    "BD0IBC":	0x01A5,
    "BD0IAL":	0x01A6,

    "BD1OST":	0x01A8,
    "BD1OBC":	0x01A9,
    "BD1OAL":	0x01AA,
    "BD1IST":	0x01AC,
    "BD1IBC":	0x01AD,
    "BD1IAL":	0x01AE,

    "BD2OST":	0x01B0,
    "BD2OBC":	0x01B1,
    "BD2OAL":	0x01B2,
    "BD2IST":	0x01B4,
    "BD2IBC":	0x01B5,
    "BD2IAL":	0x01B6,
    }


class Device:
    """Container for the PIC hardware reachable through the rcpod interface.
       Device is a pattern to search for the device node with.
       This object can be used in multiple ways...

       Memory addresses can be accessed directly using indexing:
           t = pic[0x85]
           pic[7] = 0x05

       Special function registers can be accessed as object attributes:
           print pic.TRISA
           pic.TMR0 = 12

       Commands in the rcpod firmware can be called:
           print pic.peek(0x0B)
           pic.poke(0x11, 123)
       """
    def __init__(self, devPattern="/dev/usb/rcpod*", reset=True):
        devs = glob.glob(devPattern)
        if not devs:
            raise IOError, "No rcpod device found"
        dev = open(devs[0], "w")

        # Set our attributes like this so as not to call __setattr__
        self.__dict__.update({
            'registerTable': dict(registerTable),
            'dev': dev,
            })

        if reset:
            self.reset()

    def peek(self, address):
        """Return the byte at the given 9-bit address"""
        return ioctl(self.dev, 0x3702, address)

    def poke(self, address, data):
        """Store the given address/data pair"""
        ioctl(self.dev, 0x3701, struct.pack("HB", address, data))

    def __setitem__(self, address, data):
        self.poke(address, data)

    def __getitem__(self, address):
        return self.peek(address)

    def __setattr__(self, register, data):
        """Map our object attributes to register names"""
        self.poke(self.registerTable[register], data)

    def __getattr__(self, register):
        """Map our object attributes to register names"""
        return self.peek(self.registerTable[register])

    def reset(self):
        """Give power-on values to all registers not used internally by rcpod
           that have defined values at powerup.
           """
        self.TRISA = 0xFF
        self.TRISB = 0xFF
        self.TRISC = 0xFF
        self.TRISD = 0xFF
        self.TRISE = 0xFF
        self.PORTA = 0x00
        self.PORTB = 0x00
        self.PORTC = 0x00
        self.PORTD = 0x00
        self.PORTE = 0x00
        self.T1CON = 0x00
        self.T2CON = 0x00
        self.CCP1CON = 0x00
        self.CCP2CON = 0x00
        self.RCSTA = 0x00
        self.TXSTA = 0x02
        self.ADCON0 = 0x00
        self.ADCON1 = 0x00

    def analogAll(self):
        """Scan all analog to digital converters, return their values"""
        return struct.unpack("BBBBBBBB", ioctl(self.dev, 0x3703, struct.pack("xxxxxxxx")))

### The End ###
