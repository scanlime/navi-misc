#!/usr/bin/env python

# This file should be available from
# http://www.pobox.com/~asl2/software/Pinefs
# and is licensed under the X Consortium license:
# Copyright (c) 2003, Aaron S. Lav, asl2@pobox.com
# All rights reserved. 

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, provided that the above
# copyright notice(s) and this permission notice appear in all copies of
# the Software and that both the above copyright notice(s) and this
# permission notice appear in supporting documentation. 

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
# OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
# INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
# FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

# Except as contained in this notice, the name of a copyright holder
# shall not be used in advertising or otherwise to promote the sale, use
# or other dealings in this Software without prior written authorization
# of the copyright holder. 


"""fsbase implements some utility functions and classes for filesystems."""

import array
import os
import time
import random

import rfc1094




class NFSError (Exception):
    """Raise this with one of the errors defined in RFC1094 to cause
    that error return (XXX make sure all possible callers catch this
    error)."""
    def __init__ (self, err):
        self.err = err

class Ctr:
    """Utility class for generating file handles and fileids."""
    def __init__ (self, randomize_start = 0):
        if randomize_start == 0:
            self.cnt = 0
        else:
            self.cnt = 0x10000 * random.randrange (0, 0x10000, 1)
            # XXX since this is used for fileids, and we don't
            # currently wrap, we can be restricted to 0xFFFF fileids
            # before we lose.  Rethink using Ctr for both, since there's
            # a lot more numbering space available in file handle than
            # we're using.
    def next (self):
        self.cnt += 1
        return self.cnt
    fmt = "%%0%dx" % (rfc1094.FHSIZE,)
    def next_fh (self):
        fh = self.fmt % (self.cnt,)
        self.cnt += 1
        return fh


def mk_time (sec, usec):
    """Utility for creating rfc1094 timevals"""
    tv = rfc1094.timeval ()
    tv.seconds = sec
    tv.useconds = usec
    return tv

def mk_now ():
    """Converts current time into timeval format."""
    now = time.time ()
    return mk_time (int (now), 1000000 * (now - int(now)))


# No symbolic names defined in rfc1094 for these
MODE_DIR = 0040000
MODE_CSPECIAL =       0020000 
MODE_BSPECIAL =  0060000
MODE_REG =    0100000
MODE_SYMLINK =      0120000
MODE_NAMEDSOCKET =   0140000 # "type" field should be NFNON.
MODE_SUID = 0004000
MODE_SGID =  0002000
MODE_STICKY =   0001000


typ_to_mode = {
    rfc1094.NFNON : MODE_NAMEDSOCKET, # XXX or should other modes be made possible for NFNON?
    rfc1094.NFREG : MODE_REG,
    rfc1094.NFDIR : MODE_DIR,
    rfc1094.NFBLK : MODE_BSPECIAL,
    rfc1094.NFCHR : MODE_CSPECIAL,
    rfc1094.NFLNK : MODE_SYMLINK}

unsigned_minus_one = 4294967295L
# -1, unsigned, flag to setattr to leave unchanged.

class FileObj(rfc1094.fattr.get_val_class ()):
    """File and attributes base class.  Some useful defaults and
    procedures."""
    try:
        uid = os.getuid ()
        gid = os.getgid ()
    except AttributeError:
        print "Can't get uid/gid" # happens on Win 98
        uid = 0
        gid = 0
    blocksize = 1024
    rdev = 1
    fsid = 1
    atime = mk_now ()
    mtime = mk_now ()
    ctime = mk_now ()
    mode = 0775
    nlink = 1
    def __init__ (self):
        self.set_size ()
        self.mode = self.mode | typ_to_mode [self.type]
        # rfc1094, 2.3.5, specification of type in both typ and mode
        # "really a bug in the protocol".
    def set_size (self):
        if hasattr (self, 'data'):
            self.size = len (self.data)
            # len == 0: 0 blocks
            # len = 1 - 4096 : 1 block
            # len = 4097 - 8192 # 2 blocks
            self.blocks = ((self.size  - 1)/ self.blocksize) + 1
    def update (self, sattr):
        for k in sattr.__slots__:
            try:
                v = getattr (sattr, k)
            except KeyError:
                continue
            if k == 'atime' or k == 'mtime' or k == 'ctime':
                if (v.seconds == unsigned_minus_one and
                    v.useconds == unsigned_minus_one):
                    continue
            else:
                if v == unsigned_minus_one:
                    continue
                if k == 'size':
                    if  v == 0:
                        self.truncate ()
                        self.set_size ()
                    else:
                        pass # XXX effect of other size changes undefined?
                    continue
            setattr (self, k, v)




