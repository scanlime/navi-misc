import pyrcpod, time
dev = pyrcpod.devices[0].open()

rowList = [dev.ra1, dev.ra2, dev.ra3, dev.ra4,
           dev.ra5, dev.re0, dev.re1, dev.re2,
           dev.rc0]

dev.poke('trisd', 0x00)
dev.poke('trisb', 0x00)

for row in rowList:
    row.deassert()


def clearRow(r):
    rowList[r].input().assert_()

def setRow(r):
    rowList[r].output().assert_()

def setColumns(c):
    dev.poke('portB', c >> 8)
    dev.poke('portd', c & 0xFF)

try:
    while 1:
        for row in xrange(9):
            setColumns(0xFFFF)
            setRow(row)
            time.sleep(0.01)
            clearRow(row)
finally:
    dev.reset()
