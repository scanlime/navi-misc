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

from __future__ import nested_scopes
import new
import xdrlib
import rpc

trace_struct = 0
trace_union = 0
trace_rpc = 0

fixed = 0 
var   = 1 

class LengthMismatchException (Exception): pass
class BadUnionSwitchException (Exception): pass

class arr:
    """Pack and unpack a fixed-length or variable-length array,
    both corresponding to a Python list"""
    def __init__ (self, base_type, var_fixed, length = None):
        self.base_type = base_type
        self.var_fixed = var_fixed
        self.length = length
        assert (not (var_fixed == fixed and length == None))
    def check_pack_len (self, v):
        if self.var_fixed <> fixed and self.length <> None:
            # if it's a fixed type, xdrlib checks
            val_len = len (v)
            if val_len > self.length:
                raise LengthMismatchException (self.length, val_len)
    def pack (self, p, val):
        def pack_one (v):
            self.base_type.pack (p, v)
        self.check_pack_len (v)
        if self.var_fixed == fixed:
            p.pack_farray (len (val), val, pack_one)
        else:
            p.pack_array (val, pack_one)
    def unpack (self, up):
        def unpack_one ():
            return self.base_type.unpack (up)
        if self.var_fixed == fixed:
            return up.unpack_farray (self.length, unpack_one)
        else:
            return up.unpack_opaque (unpack_one)

class opaque_or_string (arr):
    """Pack and unpack an opaque or string type, both corresponding
    to a Python string"""
    def __init__ (self, var_fixed, length = None):
        self.var_fixed = var_fixed
        self.length = length
        assert (not (var_fixed == fixed and length == None))
    def pack (self, p, val):
        self.check_pack_len (val)
        if self.var_fixed == fixed:
            p.pack_fopaque (len (val), val)
        else:
            p.pack_opaque (val)
    def unpack (self, up):
        if self.var_fixed == fixed:
            return up.unpack_fopaque (self.length)
        else:
            return up.unpack_opaque ()

# so happens that the underlying encodings are the same for opaque and string
opaque = opaque_or_string
string = opaque_or_string


# for making structs or unions.

class struct_or_union_base(object):
    """Base class for struct or union data.  The packing code for
    struct or union types doesn't require values to be derived
    from this class, but it does provide some minor conveniences."""
    def __init__ (self, **kw):
        for k, v in kw.items ():
            # can't self.__dict__.update (kw) b/c __slots__ is defined
            setattr (self, k, v)
    def __str__ (self):
        rv = self.__class__.__name__ + ": " + str (self.__dict__)
        if hasattr (self, '__slots__'):
            def strify (obj):
                """Prevent recursive stringification, or printing
                too much of a string (as in NFS read/write data).
                Also, use repr () on strings, to avoid printing control
                characters (again, useful for NFS read/write data)"""
                if isinstance (obj, struct_or_union_base):
                    return object.__str__ (obj)
                if isinstance (obj, str):
                    return repr (obj) [0:80] # 80 arbitrarily chosen
                return str (obj)
            vals = ["%s:%s" % (k, strify(getattr (self, k, None)))
                    for k in self.__slots__]
            rv +=   ": {" + ", ".join (vals) + "}"
        return rv

def struct_union_class_factory (class_name, class_dict):
    """Create a class type with name class_name, descended from
    struct_or_union_base and object, to be a factory for instantiating
    struct/union values."""
    return new.classobj (class_name, (struct_or_union_base,),
                         class_dict)


class struct_union_impl:
    """Base class for packing/unpacking structs/unions"""
    def mk_val_class (self, member_names, nested_typ_list):
        class_dict = {'__slots__': member_names}
        for typ in nested_typ_list:
            if isinstance (typ, struct_union_impl):
                class_dict [typ.name] = typ
        self.val_base_class = struct_union_class_factory (self.name,
                                                          class_dict)
    def get_val_class (self):
        return self.val_base_class
    def __call__ (self, **kw):
        return self.val_base_class (**kw)
    def get_nested_def (self, nested_name):
        for typ in self.nested_defs_list:
            if isinstance (typ, struct_union_impl):
                if nested_name == typ.name:
                    return typ
        return None

class union (struct_union_impl):
    """Pack and unpack a union as a two-membered instance, with
    members switch_name and '_data'.  Note that _data is not a valid
    switch_name, and I use it for that reason, rather than because the
    data member is private."""
    def __init__ (self, union_name, switch_decl, switch_name, union_dict):
        self.name = union_name
        self.switch_decl = switch_decl
        self.switch_name = switch_name
        self.union_dict  = union_dict
        self.def_typ = self.union_dict.get (None, None)
        member_names = ['_data', self.switch_name]
        self.mk_val_class (member_names, self.union_dict.values ())
    def _sw_val_to_typ (self, sw_val):
        """Get the type descriptor for the arm of the union specified by
        sw_val."""
        typ = self.union_dict.get (sw_val, self.def_typ)
        if typ == None: # no default clause
            raise BadUnionSwitchException (sw_val)
        return typ
    def pack (self, p, val):
        sw_val = getattr (val, self.switch_name)
        if trace_union:
            print "union: packing switch", self.switch_name, sw_val
        self.switch_decl.pack (p, sw_val)
        typ = self._sw_val_to_typ (sw_val)
        if typ == r_void: # avoid accessint _data
            return
        if trace_union:
            print "union: typ", "data:", val._data
        typ.pack (p, val._data)
    def unpack (self, up):
        tmp = self ()
        sw_val = self.switch_decl.unpack (up)
        setattr (tmp, self.switch_name, sw_val)
        tmp._data = self._sw_val_to_typ (sw_val).unpack (up)
        return tmp

class struct(struct_union_impl):
    """Pack and unpack an instance with member names as given
    by the structure definition."""
    def __init__ (self, struct_name, elt_list):
        self.elt_list = elt_list
        self.name = struct_name
        member_names = [elt [0] for elt in self.elt_list]
        self.mk_val_class (member_names, [elt[1] for elt in self.elt_list])
    def pack (self, p, val):
        for (nm, typ) in self.elt_list:
            member_val = getattr (val, nm)
            if trace_struct:
                print "packing", nm, typ, str (member_val)
            typ.pack (p, member_val)
    def unpack (self, up):
        tmp = self ()
        for (nm, typ) in self.elt_list:
            member_val = typ.unpack (up)
            setattr (tmp, nm, member_val)
        return tmp

class linked_list(struct_union_impl):
    """Pack and unpack an XDR linked list as a Python list."""
    def __init__ (self, struct_name, link_index, elt_list):
        self.name = struct_name
        elt_list[link_index] = (None, None)
        self.elt_list = elt_list
        
        self.link_index = link_index
        member_names = [elt[0] for elt in self.elt_list if elt[0] <> None]
        self.mk_val_class (member_names, [elt[1] for elt in self.elt_list
                                          if elt [1] <> None])

    def pack (self, p, val_list, ind = 0):
        val = val_list [ind] # note: wrong to pass 0-len list!
        for (nm, typ) in self.elt_list:
            if nm == None:
                next_ind = ind + 1
                if next_ind == len (val_list):
                    if trace_struct: print "packing end"
                    p.pack_bool (0)
                else:
                    if trace_struct: print "recursing", next_ind
                    p.pack_bool (1)
                    self.pack (p, val_list, next_ind)
            else:
                member_val = getattr (val, nm)
                if trace_struct:
                    print "packing", nm, typ, member_val
                typ.pack (p, member_val)
    def unpack (self, up, l = None):
        if l == None:
            l = []
            begin = 1
        else:
            begin = 0
        tmp = self ()
        for (nm, typ) in self.elt_list:
            if nm == None:
                b = up.unpack_bool ()
                if b:
                    self.unpack (up, l)
            else:
                member_val = typ.unpack (up)
                setattr (tmp, nm, member_val)
        l.append (tmp)
        if begin == 1:
            l.reverse ()
            return l

# we use None as the out-of-band indicator.  This 

class opt_data:
    
    """Pack and unpack an optional value, as either None or the value
    itself.  This choice means that we can't encode declarations which
    resolve to void * in both ways (both void absent, and void
    present).  It looks like the rfc1832 grammar disallows such
    declarations, and they aren't all that useful anyway (since they
    could be replaced w/ a bool with no change in wire format or
    semantic content), so disallowing them seems worth the simplicity
    gain for users of opt_data.  (OTOH, if we had the ML-style
    "option" type ...)"""

    def __init__ (self, typ_lambda): # lambda to enable recursive def'ns
        self.typ_lambda = typ_lambda
    def pack (self, p, val):
        if val == None:
            p.pack_bool (0)
        else:
            p.pack_bool (1)
            self.typ_lambda ().pack (p, val)
    def unpack (self, up):
        tmp = up.unpack_bool ()
        if tmp:
            return self.typ_lambda ().unpack (up)
        else: return None


class linked_list_help:
    """utility function to convert back and forth
    between Python-style lists and structs corresponding to linked
    lists.  Usually rpcgen should auto-generate calls to linked_list
    instead, but this might be useful if it gets confused, or as
    sample code to hack if you need mutually recursive linked lists or
    trees."""
    
    def __init__ (self, link_member):
        self.link_member = link_member
    def to_ll (self, l):
        if l == []:
            return None
        first = l[0]
        last  = l[0]
        for elt in l[1:]:
            setattr (last, self.link_member, elt)
            last = elt
        setattr (last, self.link_member, None)
        return first
    def from_ll (self, linked):
        l = []
        while 1:
            if linked == None: break
            v = getattr (linked, self.link_member)
            setattr (linked, self.link_member, None)
            # setattr avoids surprising space leaks if we keep just one elt.
            l.append (linked)
            linked = v
        return l
    

class base_type:
    def __init__ (self, p, up):
        self.p_proc = p
        self.up_proc = up
    def pack (self, p, val):
        self.p_proc (p, val)
    def unpack (self, up):
        return self.up_proc (up) 

# r_ prefix to avoid shadowing Python names
r_uint = base_type (xdrlib.Packer.pack_uint, xdrlib.Unpacker.unpack_uint)
r_int = base_type (xdrlib.Packer.pack_int, xdrlib.Unpacker.unpack_int)
r_bool = base_type (xdrlib.Packer.pack_bool, xdrlib.Unpacker.unpack_bool)
r_void = base_type (lambda p,v: None, lambda up: None)
r_hyper = base_type (xdrlib.Packer.pack_hyper, xdrlib.Unpacker.unpack_hyper)
r_uhyper = base_type (xdrlib.Packer.pack_uhyper, xdrlib.Unpacker.unpack_uhyper)
r_float  = base_type (xdrlib.Packer.pack_float, xdrlib.Unpacker.unpack_float)
r_double = base_type (xdrlib.Packer.pack_double, xdrlib.Unpacker.unpack_double)
# XXX should add quadruple, but no direct Python support for it.

class Proc:
    """Manage a RPC procedure definition."""
    def __init__ (self, name, ret_type, arg_types):
        self.name = name
        self.ret_type = ret_type
        self.arg_types = arg_types
    def check_for_implementation (self, server):
        if (not hasattr (server, self.name) and
            not self.name in server.deliberately_unimplemented):
            print "Warning: %s not implemented in %s" % (self.name,
                                                         server.__class__)
    def __str__ (self):
        return "Proc: %s %s %s" % (self.name, str(self.ret_type),
                                   str (self.arg_types))

class Server:
    """Base class for rpcgen-created server classes.  Unpack arguments,
    dispatch to appropriate procedure, and pack return value.  Check,
    at instantiation time, whether there are any procedures defined in the
    IDL which are both unimplemented and whose names are missing from the
    deliberately_unimplemented member.
    
    Stashes transaction_id in self.xid for duration of call.  Perhaps
    should provide optional caching facility for results of non-idempotent
    calls.

    As a convenience, allows creation of transport server w/
    create_transport_server.  In what every way the server is created,
    you must call register."""
    deliberately_unimplemented = []
    def __init__ (self):
        for p in self.procs.values ():
            p.check_for_implementation (self)
    def create_transport_server (self, port, typ = rpc.UDPServer, lock=None):
        """Create a transport server (to pass to register (), below).
        The same transport server can be used with multiple rpchelp.Server
        servers (with different program and version numbers).  This
        is now just a convenience function, and not actually that
        additionally convenient."""
        host = ''
        return typ (host, port, lock = lock)
    def register (self, transport_server):
        try:
            transport_server.register (self.prog, self.vers, self)
        except RuntimeError, val:
            print "Runtime error registering", val

    def handle_proc (self, i, transport):
        try:
            proc = self.procs [i]
        except KeyError:
            # Actually, this means the client sent us a procedure call
            # not documented in the IDL.
            raise NotImplementedError ()
        if trace_rpc:
            print "Proc: %s" % (proc.name,),
        argl = [arg_type.unpack (transport.unpacker)
                for arg_type in proc.arg_types]
        transport.turn_around ()
        try:
            fn = getattr (self, proc.name)
        except AttributeError:
            raise NotImplementedError ()
        rv = fn (*argl)
        proc.ret_type.pack (transport.packer, rv)
        if trace_rpc:
            print "(%s) -> %s" % (",".join (map (str, (argl))), str (rv))
            
