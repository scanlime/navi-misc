#!/usr/bin/env python

"""In-memory filesystem, pre-populated with a couple of files.  Not
complete (e.g. you can remove directories which aren't empty), but
works well enough to use emacs and gcc."""

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

import rfc1094
import fsbase
import array



class FileObj(fsbase.FileObj):
    fileid_ctr = fsbase.Ctr ()
    def __init__ (self, **kw):
        self.fileid = self.fileid_ctr.next ()
        if not kw.has_key ('data'):
            kw ['data'] = ''
        if kw['type'] == rfc1094.NFDIR and not kw.has_key ('dir'):
            kw['dir'] = {}
        self.data = array.array ('b')
        self.data.fromstring (kw['data'])
        del kw['data']
        for k,v in kw.items ():
            setattr (self, k, v)
        fsbase.FileObj.__init__ (self)
        
    def read (self, offset, count):
        return (self.data [offset: offset + count]).tostring ()
    
    def write (self, offset, newdata):
        n = array.array ('b')
        n.fromstring (newdata)
        if offset > len (self.data):
            extend_len = offset - len (self.data)
            fill = array.array ('b')
            fill.fromlist ([0] * extend_len)
            self.data.extend (fill)
        self.data [offset:offset + len (newdata)] = n
        self.set_size ()
        
    def get_dir (self):
        return self.dir
    def truncate (self):
        self.data = array.array ('b')
    def mk_link (self, name, from_fh):
        self.dir [name] = from_fh


class FileSystem:
    def __init__ (self, fh_ctr = fsbase.Ctr ()):
        self._fh_ctr = fh_ctr
        self._fils = {}
        self._root, _ = self.create_fil (None, '', type = rfc1094.NFDIR,
                                         size = 4)
        self.create_fil (self._root, 'foo', type = rfc1094.NFREG,
                         data = 40 * 'A')
        self.create_fil (self._root, 'bar', type = rfc1094.NFREG,
                         data = 20 * 'B')
        dir_fh, _ = self.create_fil (self._root, 'dir', type = rfc1094.NFDIR,
                                     size = 4)
        self.create_fil (self._root, 'baz', type = rfc1094.NFREG,
                         data = 20 * 'C')
    def mount (self, dirpath):
        if dirpath == '/':
            return self._root
        return None
    def _register (self, fil):
        fh = self._fh_ctr.next_fh ()
        self._fils [fh] = fil
        return fh
    def get_fil (self, fh):
        return self._fils.get (fh, None)
    def _add_fil (self, dir_fh, name, new_fh):
        dir_fil = self.get_fil (dir_fh)
        assert (dir_fil <> None)
        dir = dir_fil.get_dir ()
        if dir.has_key (name):
            raise fsbase.NFSError (rfc1094.NFSERR_EXIST)
        dir_fil.get_dir ()[name] =  new_fh
    def create_fil (self, dir_fh, name, **kw):
        fil = FileObj (**kw)
        fh = self._register (fil)
        if dir_fh <> None: # if dir_fh == None, we're creating root directory
            self._add_fil (dir_fh, name, fh)
        return fh, fil
    def rename (self, old_dir, old_name, new_dir, new_name):
        from_dir_fil = self.get_fil (old_dir)
        to_dir_fil = self.get_fil (new_dir)
        move_fil = from_dir_fil.get_dir() [old_name]
        to_dir_fil.get_dir () [new_name] = move_fil
        del from_dir_fil.get_dir() [old_name]

    def remove (self, dir_fh, name):
        dir_fil = self.get_fil (dir_fh)
        if dir_fil == None: # XXX should raise error?
            return
        fh = dir_fil.get_dir ().get (name, None)
        if fh == None:
            raise fsbase.NFSError (rfc1094.NFSERROR_NOENT)
        fil = self.get_fil (fh)
        
        if fil.type == rfc1094.NFDIR:
            if fil.dir <> {}:
                raise fsbase.NFSError (rfc1094.NFSERR_NOTEMPTY)
        del self._fils [fh]
        del dir_fil.get_dir() [name]


                


