#!/usr/bin/env python

# Create a mapping from valid addresses to the resulting start address on the bus.
# We start by mapping each valid address to 0x0000...
addrmap = {}
for code in open("valid.addresses").readlines():
    addrmap[int(code)] = 0

# Set the proper bits read from the individual map files
for pin in xrange(5, 15):
    for code in open("a%d.map" % pin).readlines():
        addrmap[int(code)] = addrmap[int(code)] | (1 << pin)

# Reverse the mapping, so we get a map from addresses to the codes that generate them
codemap = {}
for code, addr in addrmap.iteritems():
    codemap.setdefault(addr,[]).append(code)

# Print the results- address first, then list of codes, sorted by address
addrs = codemap.keys()
addrs.sort()
for addr in addrs:
    print "0x%04X : %s" % (addr, " ".join(["0x%04X" % code for code in codemap[addr]]))
