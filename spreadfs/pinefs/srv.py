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

"""
Mount and NFS servers.

Note: our implementation of READDIR/directory cookies is somewhat
broken, since I can't figure out how to implement directory cookies
which are stable across deletions/additions to the directory in, e.g.,
pyfs.  Maybe upgrading to NFS v3 with the cookie verifier field would
help (pyfs could certainly increment a directory version number
whenever it noticed the directory changed), but
http://playground.sun.com/pub/nfsv4/nfsv4-wg-archive/2001/0046.html
claims (in 2001) Unix clients don't like the NFS3ERR_BAD_COOKIE
return.  (The whole thread is worth reading.)"""

import threading
import code
import sys
import getopt


import rfc1094
import rpchelp
import rpc

import memfs
import pyfs
import fsbase

try:
    import tarfs
except ImportError:
    tarfs = None

class HostAccessControl:
    def check_host_ok (self, host, cred, verf):
        # if you want to actually use cred or verf, you'll need
        # to add packing/unpacking code.
        is_ok = host == '127.0.0.1'
        # I haven't tried the attack, but it seems relatively
        # easy for some random other machine to inject malicious
        # Python bytecode into pyfs, so we only allow the local machine. 
        if not is_ok:
            print "unauthorized host", host
        return is_ok

class MntSrv (rfc1094.MOUNTPROG_1, HostAccessControl):
    """See RFC1094."""
    deliberately_unimplemented = ['MOUNTPROC_EXPORT', 'MOUNTPROC_UMNTALL',
                     'MOUNTPROC_UMNT', 'MOUNTPROC_DUMP']
    def __init__ (self, fs):
        self.fs = fs
        rfc1094.MOUNTPROG_1.__init__ (self)
    def MOUNTPROC_NULL (self):
        return None
    
    def MOUNTPROC_MNT (self, dirpath):
        ret = rfc1094.fhstatus ()
        fh = self.fs.mount (dirpath)
        if fh == None:
            ret.status = rfc1094.NFSERR_NOENT
        else:
            ret.status = rfc1094.NFS_OK
            ret._data = fh
        return ret

class NfsSrv (rfc1094.NFS_PROGRAM_2, HostAccessControl):
    """See RFC1094."""
    deliberately_unimplemented = ['NFSPROC_WRITECACHE', 'NFSPROC_ROOT']
    # WRITECACHE and ROOT aren't used at all.
    def __init__ (self, fs):
        self.fs = fs
        rfc1094.NFS_PROGRAM_2.__init__ (self)
    def NFSPROC_NULL (self):
        return None
    def NFSPROC_GETATTR (self, fh):
        as = rfc1094.attrstat ()
        fil = self.fs.get_fil (fh)
        if fil == None:
            as.status = rfc1094.NFSERR_STALE
        else:
            as.status = rfc1094.NFS_OK
            as._data = fil
        return as
    
    def NFSPROC_SETATTR (self, sattrargs):
        rv = rfc1094.attrstat ()
        try:
            fil= self.fs.get_fil (sattrargs.file)
            if fil <> None:
                fil.update (sattrargs.attributes)
                rv.status = rfc1094.NFS_OK
                rv._data = fil
            else:
                rv.status = rfc1094.NFSERR_STALE
        except fsbase.NFSError, err:
            rv.status = err.err
        return rv

    def NFSPROC_READLINK (self, fhandle):
        readlinkres = rfc1094.readlinkres ()
        fil = self.fs.get_fil (fhandle)
        if fil == None:
            readlinkres.status = rfc1094.NFSERR_STALE
        else:
            readlinkres.status = rfc1094.NFS_OK
            readlinkres._data = fil.read (0, fil.size)
        return readlinkres

    def NFSPROC_LOOKUP (self, diropargs):
        dir_fil = self.fs.get_fil (diropargs.dir)
        diropres = rfc1094.diropres ()
        if dir_fil <> None:
            if dir_fil.type <> rfc1094.NFDIR:
                diropres.status = rfc1094.NFSERR_NOTDIR
                return diropres
            fh = dir_fil.get_dir ().get (diropargs.name, None)
            if fh <> None:
                attr = self.fs.get_fil (fh)
                diropres.status = rfc1094.NFS_OK
                diropres._data = diropres.diropok (
                    file = fh, attributes = attr)
                return diropres
            diropres.status = rfc1094.NFSERR_NOENT
        else:
            diropres.status = rfc1094.NFSERR_STALE
        return diropres

    def NFSPROC_READ (self, readargs):
        readres = rfc1094.readres ()
        fil = self.fs.get_fil (readargs.file)
        if fil == None:
            readres.status = rfc1094.NFSERR_STALE
        else:
            data = fil.read (readargs.offset, readargs.count)
            readres.status = rfc1094.NFS_OK
            readres._data = rfc1094.attrdat(attributes = fil,
                                             data = data)
        return readres

    def NFSPROC_WRITE (self, wa):
        rv = rfc1094.attrstat ()
        try:
            fil = self.fs.get_fil (wa.file)
            if fil == None:
                raise fsbase.NFSError (rfc1094.NFSERR_STALE)
            fil.write (wa.offset, wa.data)
            rv.status = rfc1094.NFS_OK
            rv._data = fil
        except fsbase.NFSError, err:
            rv.status = err.err
        return rv

    def create_aux (self, ca, **fil_parms):
        """Utility function called by CREATE, MKDIR, and SYMLINK."""
        rv = rfc1094.diropres ()
        try:
            new_fh, new_fil = self.fs.create_fil (
                ca.where.dir, ca.where.name, **fil_parms)
            new_fil.update (ca.attributes)
            rv.status = rfc1094.NFS_OK
            rv._data = rv.diropok (file = new_fh, attributes = new_fil)
        except fsbase.NFSError, v:
            rv.status = v.err
        return rv

    def NFSPROC_CREATE (self, ca):
        return self.create_aux (ca, type = rfc1094.NFREG, data = '')

    def NFSPROC_REMOVE (self, doa):
        try:
            self.fs.remove (doa.dir, doa.name)
            return rfc1094.NFS_OK
        except fsbase.NFSError, v:
            return v.err

    def NFSPROC_RENAME (self, renameargs):
        try:
            self.fs.rename (renameargs.from_.dir,
                            renameargs.from_.name,
                            renameargs.to.dir,
                            renameargs.to.name)
            return rfc1094.NFS_OK
        except fsbase.NFSError, v:
            return v.err
    
    def NFSPROC_LINK (self, linkargs):
        try:
            to_dir = self.fs.get_fil (linkargs.to.dir)
            to_dir.mk_link (linkargs.to.name, linkargs.from_)
            return rfc1094.NFS_OK
        except fsbase.NFSError, v:
            return v.err

    def NFSPROC_SYMLINK (self, symlinkargs):
        createargs = rfc1094.createargs ()
        createargs.where = symlinkargs.from_
        createargs.attributes = symlinkargs.attributes

        rv = self.create_aux (createargs, type=rfc1094.NFLNK,
                                data = symlinkargs.to)
        return rv.status
    def NFSPROC_MKDIR (self, ca):
        return self.create_aux (ca, type = rfc1094.NFDIR, dirlist = [])
    NFSPROC_RMDIR = NFSPROC_REMOVE
        
    def NFSPROC_READDIR (self, rda):
        rda.count = min (rda.count, 40) # XXX 40 is to prevent overflow of packet size.  Probably could be larger most of the time, might need to be smaller.
        # would be useful to have packet size feedback from xdr layer
        # to here.
        def ind_to_cookie (i):
            assert i < 9999 # if this fails, switch to binary cookie
            s = "%.04d" % (i + 1,) # + 1 b/c pointer to next entry
            assert len (s) == 4
            return s
        def cookie_to_ind (cookie):
            if cookie == '\x00\x00\x00\x00': return 0
            return int (cookie)
        res = rfc1094.readdirres ()
        try:
            d_fil = self.fs.get_fil (rda.dir)
            if d_fil == None:
                res.status = rfc1094.NFSERR_STALE
                return res
            if d_fil.type <> rfc1094.NFDIR:
                res.status = rfrc1094.NFSERR_NOTDIR
                return res
            d = d_fil.get_dir ()
            entries = []
            start = cookie_to_ind (rda.cookie)
            d_items = d.items ()
            end = min (start + rda.count, len (d))
            for i in range (start,end):
                fil = self.fs.get_fil (d_items [i][1])
                if fil == None:
                    print "fil is None in readdir"
                    continue
                ent = rfc1094.entry ()
                ent.fileid = fil.fileid
                ent.name = d_items [i][0]
                ent.cookie = ind_to_cookie (i) 
                entries.append (ent)
            res = rfc1094.readdirres ()
            res.status = rfc1094.NFS_OK
            res._data  = res.readdirok ()
        
            if len (entries) == 0:
                res._data.entries = None
            else:
                res._data.entries = entries
            
            res._data.eof = end == len (d)
        except fsbase.NFSError, v:
            res.status = v.err
        return res
    def NFSPROC_STATFS (self, fh):
        stat = rfc1094.statfsres ()
        stat.status = rfc1094.NFS_OK
        stat._data = stat.info (
            tsize = 4096, bsize = 4096, blocks = 100, bfree = 50, bavail = 50)
        return stat

if __name__ == '__main__':

    server_class = rpc.UDPServer
    optlist, args = getopt.getopt (sys.argv[1:], 'f:t')
    fs_dict = {'py': (pyfs, 1),
               'mem': (memfs, 0),
               'tar': (tarfs, 0)}
    fs_type = 'py'
    for (opt, val) in optlist:
        if opt == '-f':
            fs_type = val
        elif opt == '-t':
            server_class = rpc.TCPServer       
    fs_mod, interact_loop = fs_dict.get (fs_type, (None, None))
    if fs_mod == None:
        print "Bad fs type", fs_type
        sys.exit (0)

    fs = fs_mod.FileSystem ()
    lock = threading.Lock ()
    ms = MntSrv (fs)
    
    mnt_rpc = ms.create_transport_server (5555, server_class, lock=lock)
    ms.register (mnt_rpc)
    mntthread = threading.Thread (target=mnt_rpc.loop)
    mntthread.start ()
    ns = NfsSrv(fs)
    nfs_rpc = ns.create_transport_server (2049, server_class, lock=lock)
    ns.register (nfs_rpc)
    try:
        if interact_loop:
            nfsthread = threading.Thread(target = nfs_rpc.loop)
            nfsthread.start ()
            code.interact ("NFS-server enabled Python interpreter loop")
        else:
            nfs_rpc.loop ()
    finally:
        mnt_rpc.stop ()
        nfs_rpc.stop ()
        mnt_rpc.unregister ()
        nfs_rpc.unregister ()

    
