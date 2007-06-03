#!/usr/bin/env python
""" i2c-header

usage: i2c-header.py image.desc image.bin

This is a utility that facilitates building EEPROM images for the
TUSB3410 bootloader using a tree of Python objects.

The EEPROM image is built using a tree of serializable objects. Each
of these objects is converted to binary form using str(). These
objects include USB descriptors, firmware, and the various headers
needed to store each.

The image description is a python file that assigns a BootDevice
instance to the global variable 'device'. This script then serializes
that device and writes it to the given output file.

For a full idea of what's possible in the description file, read the
doc strings and source.

Here is a simple example for the TUSB3410 that sets the vendor and
product ID, and the two default string descriptors:

device = BootDevice()[

    DeviceDescriptorBootBlock()[
        DeviceDescriptor(idVendor  = 0x1234,
                         idProduct = 0x5326,
                         ),
    ],

    # We need to include a config descriptor, but all
    # settings are left at their defaults.
    ConfigurationDescriptorBootBlock() [
        ConfigurationDescriptor() [
            InterfaceDescriptor() [
                EndpointDescriptor(),
            ],
        ],
    ],

    # These two strings are referred to by default values in the device descriptor.
    StringDescriptorBootBlock() [
        LanguageTable(),
        # String 1, the manufacturer
        StringDescriptor("Zombimatic Inc."),
        # String 2, the product name
        StringDescriptor("Widget Express Pro"),
    ],

]

Note that different devices support different types of boot blocks.
This is an example for setting the vendor/product ID on a TUSB3210:

device = BootDevice("TUSB3210")[
    BasicInfoBootBlock(idVendor  = 0x1234,
                       idProduct = 0x5678,
                       ),
]

"""
#
# TUSB3410 Utilities
# Copyright (C) 2004-2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import struct, sys


class Serializable:
    """This is the base class for objects that can be serialized to
       a binary format usable in the EEPROM image.
       """
    def writeTo(self, stream):
        stream.write(str(self))

    def writeToFile(self, name):
        self.writeTo(open(name, "wb"))


class Container(Serializable):
    """A serializable object that contains zero or more other objects,
       specified using Nevow-like [] syntax.
       """
    contents = []

    def __getitem__(self, contents):
        self.contents = list(contents)
        return self

    def attachHeader(self, joined):
        return joined

    def __str__(self):
        return self.attachHeader("".join(map(str, self.contents)))


class SerializableValue:
    """A Serializable object that wraps a singla python value"""
    def __init__(self, value):
        self.value = value

class Int16(SerializableValue):
    def __str__(self):
        return struct.pack("<H", self.value)

class Int8(SerializableValue):
    def __str__(self):
        return chr(self.value)

class UTF16String(SerializableValue):
    def __str__(self):
        return self.value.encode("UTF-16LE")


class BootDevice(Container):
    """This is the top-level container for boot device information.
       Its header includes the device signature, and each object contained
       within this is a 'descriptor block' processed by the bootloader.
       """
    def __init__(self, product="TUSB3410"):
        assert len(product) == 8
        assert product[0:4] == "TUSB"
        self.signature = int(product[4:8], 16)

        # The TUSB3210 is really a TUSB2136 in disguise
        if self.signature == 0x3210:
            self.signature = 0x2136

    def attachHeader(self, joined):
        return ''.join(map(str, [
            Int16(self.signature),
            joined,
            Int8(0),
            ]))


class BootBlock(Container):
    """One 'descriptor block' in the boot device. They will just be referred
       to as 'boot blocks' here, to reduce confusion with USB descriptors.
       Each block is equipped with a header containing the type, length, and checksum.
       """
    blockType = None

    def checksum(self, data):
        """Calculate the TUSB3410's cheesy little 8-bit checksum"""
        sum = 0
        for byte in data:
            sum = (sum + ord(byte)) & 0xFF
        return sum

    def attachHeader(self, joined):
        return ''.join(map(str, [
            Int8(self.blockType),
            Int16(len(joined)),
            Int8(self.checksum(joined)),
            joined,
            ]))


class BasicInfoBootBlock(BootBlock):
    """This boot block contains only basic information about
       the USB device- mostly its vendor and product ID. This
       is the only way to modify the bootloader descriptors
       on the devices that use it.
       Supported by the TUSB3210, TUSB2136, and TUSB5152.
       """
    blockType = 1

    def __init__(self,
                 idVendor       = 0x0451,
                 idProduct      = 0x2136,
                 idHubProduct   = 0x2136,
                 selfPowerBit   = 0x00,
                 powerGoodTime  = 0,
                 hubCurrent     = 50):
        self.bootInfo = Container()[
            Int8(selfPowerBit),
            Int16(idVendor),
            Int16(idHubProduct),
            Int16(idProduct),
            Int8(powerGoodTime),
            Int8(hubCurrent),
            ]

    def attachHeader(self, joined):
        return BootBlock.attachHeader(
            self, str(self.bootInfo) + joined)


class ApplicationCodeBootBlock(BootBlock):
    """This is a boot block holding an application firmware
       image. It performs the same function as the later
       AutoexecFirmwareBootBlock, but it prepends an application
       code revision number to the firmware image.
       Supported by the TUSB3210, TUSB2136, and TUSB5152.
       """
    blockType = 2

    def __init__(self, revision=1):
        self.revision = revision

    def attachHeader(self, joined):
        return BootBlock.attachHeader(
            self, str(Int16(self.revision)) + joined)


class DeviceDescriptorBootBlock(BootBlock):
    """This boot block contains only the USB device descriptor.
       Supported by the TUSB3410.
       """
    blockType = 3

class ConfigurationDescriptorBootBlock(BootBlock):
    """This boot block contains all configuration descriptors,
       and associated endpoint and interface descriptors.
       Supported by the TUSB3410.
       """
    blockType = 4

class StringDescriptorBootBlock(BootBlock):
    """This boot block contains all string descriptors,
       including the language table.
       Supported by the TUSB3410.
       """
    blockType = 5

class AutoexecFirmwareBootBlock(BootBlock):
    """This boot block contains firmware for the bootloader to run.
       Since this firmware takes over before any USB initialization,
       it is generally the only boot block present.

       The content of this block is generally a BinaryFile object,
       however other objects may be implemented for loading Intel HEX
       or other firmware formats.

       Supported by the TUSB3410.
       """
    blockType = 7

    def attachHeader(self, joined):
        return BootBlock.attachHeader(self, joined) + '\0'

class BinaryFile(SerializableValue):
    """Opens the given file and returns all data in it. This
       is convenient for use with AutoexecFirmwareBootBlock.
       """
    def __str__(self):
        return open(self.value, "rb").read()


class PaddedTo(Container):
    """In some circumstances it may be useful to pad the
       firmware image or the entire bootloader image to
       some preset size. This pads its contents to exactly
       'bytes' long, using copies of the 'fill' character.
       """
    def __init__(self, bytes, fill='\0'):
        assert len(fill) == 1
        self.bytes = bytes
        self.fill = fill

    def attachHeader(self, joined):
        l = len(joined)
        if l > self.bytes:
            raise ValueError("Contents already %d bytes, can't pad to %d" %
                             (l, self.bytes))
        return joined + self.fill * (self.bytes - l)


class ChildCount(Serializable):
    """A placeholder class used in the descriptorContent lists, that inserts
       a count of all children of a particular type.
       """
    def __init__(self, baseClass):
        self.baseClass = baseClass

    def __str__(self):
        count = 0
        for child in self.children:
            if isinstance(child, self.baseClass):
                count += 1
        return str(Int8(count))


class TotalLength(Int16):
    """A placeholder class used in the descriptorContent lists, that inserts
       the total length of a descriptor and all children.
       """
    def __init__(self):
        self.value = 0


class USBDescriptor(Container):
    """A generic USB descriptor, with length and type header. A descriptor's
       'content' is its list of child descriptors. Each individual descriptor
       sets the 'descriptorContent' attribute.
       """
    descriptorType = None
    def attachHeader(self, content):
        # Give ChildCount objects a reference to our list of children
        for obj in self.descriptorContent:
            if isinstance(obj, ChildCount):
                obj.children = self.contents

        # Join everything once to calculate total length
        descriptorContent = ''.join(map(str, self.descriptorContent))
        totalLength = len(descriptorContent) + len(content) + 2

        # Fill in TotalLength placeholders
        for obj in self.descriptorContent:
            if isinstance(obj, TotalLength):
                obj.value = totalLength

        # Finally assemble everything
        descriptorContent = ''.join(map(str, self.descriptorContent))
        return ''.join(map(str, [
            Int8(len(descriptorContent) + 2),
            Int8(self.descriptorType),
            descriptorContent,
            content,
            ]))


class StringDescriptor(USBDescriptor):
    descriptorType = 3
    def __init__(self, string):
         self.descriptorContent = [UTF16String(string)]


class LanguageTable(StringDescriptor):
    """A special string descriptor that includes a table of supported
       language IDs. By default, this includes English, but it may be
       overridden.
       """
    def __init__(self, languages=[0x0409]):
        self.descriptorContent = map(Int16, languages)


class InterfaceDescriptor(USBDescriptor):
    """A USB interface descriptor. Children of this descriptor should
       be the endpoint descriptors for this interface.
       """
    descriptorType = 4
    def __init__(self,
                 bInterfaceNumber   = 0,
                 bAlternateSetting  = 0,
                 bInterfaceClass    = 0xFF,
                 bInterfaceSubclass = 0,
                 bInterfaceProtocol = 0,
                 iInterface         = 0,
                 ):
        self.descriptorContent = [
            Int8(bInterfaceNumber),
            Int8(bAlternateSetting),
            ChildCount(EndpointDescriptor),
            Int8(bInterfaceClass),
            Int8(bInterfaceSubclass),
            Int8(bInterfaceProtocol),
            Int8(iInterface),
            ]


class EndpointDescriptor(USBDescriptor):
    descriptorType = 5
    def __init__(self,
                 bEndpointAddress   = 0x01,
                 bmAttributes       = 0x02,
                 wMaxPacketSize     = 64,
                 bInterval          = 0,
                 ):
        self.descriptorContent = [
            Int8(bEndpointAddress),
            Int8(bmAttributes),
            Int16(wMaxPacketSize),
            Int8(bInterval),
            ]


class DeviceDescriptor(USBDescriptor):
    """A USB device descriptor. Note that configuration descriptors
       will normally not be children of the device descriptor, they
       are stored in separate bootloader blocks.
       """
    descriptorType = 1
    def __init__(self,
                 bcdUSB             = 0x0110,
                 bDeviceClass       = 0xFF,
                 bDeviceSubClass    = 0x00,
                 bDeviceProtocol    = 0x00,
                 bMaxPacketSize0    = 8,
                 idVendor           = 0x0451,
                 idProduct          = 0x3410,
                 bcdDevice          = 0x0100,
                 iManufacturer      = 1,
                 iProduct           = 2,
                 iSerialNumber      = 0,
                 bNumConfigurations = 1,
                 ):
        self.descriptorContent = [
            Int16(bcdUSB),
            Int8(bDeviceClass),
            Int8(bDeviceSubClass),
            Int8(bDeviceProtocol),
            Int8(bMaxPacketSize0),
            Int16(idVendor),
            Int16(idProduct),
            Int16(bcdDevice),
            Int8(iManufacturer),
            Int8(iProduct),
            Int8(iSerialNumber),
            Int8(bNumConfigurations),
            ]


class ConfigurationDescriptor(USBDescriptor):
    """The USB configuration descriptor. In addition to its own
       information, this descriptor acts as a container for the
       interface and endpoint descriptors.
       """
    descriptorType = 2
    def __init__(self,
                 bConfigurationValue = 1,
                 iConfiguration      = 0,
                 bmAttributes        = 0x80,
                 bMaxPower           = 50,
                 ):
        self.descriptorContent = [
            TotalLength(),
            ChildCount(InterfaceDescriptor),
            Int8(bConfigurationValue),
            Int8(iConfiguration),
            Int8(bmAttributes),
            Int8(bMaxPower),
            ]


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "usage: i2c_header.py image_description.py image.bin"
        sys.exit(1)
    exec open(sys.argv[1])
    open(sys.argv[2], "wb").write(str(device))

### The End ###
