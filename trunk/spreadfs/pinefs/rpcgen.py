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

"""Parser for ONC RPC IDL.  The grammar is taken from RFC1832, sections
5, and RFC1831, section 11.2.

The output Python code (which requires rpchelp and rpc from the Pinefs
distribution) contains a separate class (with rpchelp.Server as a base
class) for every version defined in every program statement.  To
implement a service, for each version of each program, derive a class
from the class named <prog>_<version>, with method names corresponding
to the procedure names in the IDL you want to implement.  (At
instantiation, any procedure names defined in the IDL but neither
implemented nor listed in the deliberately_unimplemented member will
cause a warning to be printed.)  Also, define a member function
check_host_ok, which is passed (host name, credentials, verifier) on each
call, and should return a true value if the call should be accepted,
and false otherwise.

To use instances of the server class, create a transport server (with
the create_transport_server(port) function), and then, for every server
instance you want associated with that port, call its
register(transport_server) function, which will register with the
local portmapper.  (This architecture allows multiple versions of
multiple programs all to listen on the same port, or for a single version
to listen on, e.g, both a TCP and UDP port.)

Member functions will be passed Python values, and should return
a Python value.  The correspondence between IDL datatypes and
Python datatypes is:
- base types uint, int, float, double are the same
- void is None
- an array (either fixed or var-length) is a Python sequence
- an opaque or a string is a Python string
- a structure is a Python instance, with IDL member names corresponding
  to Python attribute names
- a union is a two-attribute instance, with one attribute named the
  name of the discriminant declaration, and the other named '_data'
  (with a value appropriate to the value of the discriminant).
- an optional value (*) is either None, or the value
- a linked list is special-cased, and turned into a Python list
  of structures without the link member.
- const and enum declarations are top-level constant variables.

IDL identifiers which are Python reserved words (or Python reserved
words with 1 or more underscores suffixed) are escaped by appending
an underscore.

Top-level struct and union declarations generate Python declarations
of the corresponding name, and calling the object bound to the name
will generate an instance suitable for populating.  (The class defines
__slots__ to be the member names, and has, as attributes, any nested
struct or union definitions.  The packing/unpacking function don't
require the use of this class, and, for the unnamed struct/union
declarations created by declaring struct or union types as either
return values or argument types in a procedure definition, you'll need
to create your own classes, either by using
rpchelp.struct_union_class_factory, or some other way.)

Enum declarations nested inside struct or union declarations, or
procedure definitions, generate top-level definitions.  (I think this
treatment of nested enum definitions is wrong, according to RFC1832
section 5.4, but I'm not sure.)

Rpcgen doesn't support:
- 'unsigned' as a synonym for 'unsigned int'
- case fall-through in unions
Neither seems to be defined in the grammar, but I should support them,
and look around for an updated IDL specification.
"""

from __future__ import nested_scopes
import string
import time


class LexError (Exception):
    pass

class ParseError(Exception):
    pass

tokens = ('LPAREN', 'RPAREN','LBRACK', 'RBRACK', 'LANGLE', 'RANGLE',
          'STAR', 'COMMA', 'COLON', 'VOID', 'UNSIGNED', 'TYPE',
          'ENUM', 'LCBRACK', 'RCBRACK', 'EQ', 'STRUCT', 'UNION',
          'SWITCH', 'CASE', 'DEFAULT', 'CONST', 'SEMICOLON', 'IDENT',
          'CONSTVAL', 'OPAQUE', 'STRING', 'TYPEDEF', 'PROGRAM', 'VERSION')

t_LPAREN = r'\('
t_RPAREN = r'\)'
t_LBRACK = r'\['
t_RBRACK = r'\]'
t_LANGLE = r'<'
t_RANGLE = r'>'
t_STAR   = r'\*'
t_COMMA  = r','
t_COLON  = r':'
t_LCBRACK = r'{'
t_RCBRACK = r'}'
t_EQ = r'='
t_SEMICOLON = r';'

reserved_tuple = (
# RFC 1832, XDR
    'void', 
    'unsigned',
    'opaque',
    'string',
    'enum',
    'struct',
    'union',
    'switch',
    'case',
    'default',
    'const',
    'typedef',
# RFC 1831, RPC
    'program',
    'version')

types = ('int', 'hyper', 'float', 'double', 'quadruple', 'bool')

py_reserved_words = ['def', 'print', 'del', 'pass', 'break', 'continue',
                     'return', 'yield', 'raise', 'import', 'from',
                     'global', 'exec', 'assert', 'if', 'while', 'else',
                     'for', 'in', 'try', 'finally', 'except', 'and',
                     'not', 'or', 'is', 'lambda', 'class']

reserved_dict = {}
for r in reserved_tuple:
    reserved_dict[r] = r.upper ()
for t in types:
    reserved_dict[t] = 'TYPE'

def t_CONSTVAL(t):
    r"(0x[0-9A-Fa-f]+)|(-?\d+)"
    return t

def t_COMMENT (t):
    r"/\*(.|\n)*?\*/"
    t.lineno += t.value.count('\n')
    return None

def t_NEWLINE(t):
    r'\n+'
    t.lineno += t.value.count("\n")

t_ignore = " \t"

def needs_escaping (val):
    """Check whether this identifier is a valid IDL but not Python identifier,
    or whether it would otherwise be an escaped IDL identifier.  (Escaping
    is done by appending a '_', if the identifier, once stripped of all
    trailing underscores, is a Python keyword."""
    while val [-1] == '_':
        val = val [:-1]
    return val in py_reserved_words

def t_IDENT (t):
    r"[a-zA-Z]([a-zA-Z]|_|[0-9])*"
    t.type = reserved_dict.get (t.value, 'IDENT')
    if needs_escaping (t.value):
        t.value += '_'
    return t

def t_error (t):
    raise LexError (t)

import lex

lexer = lex.lex ()

class Ctx:
    def __init__ (self):
        self.indent = 0
        self.deferred_list = [] # used for deferring nested Enum definitions
    def defer (self, val):
        self.deferred_list.append (val)
    def finish (self):
        return "\n".join (self.deferred_list)
        
class Node:
    def __init__ (self, val=None, children=None, **kw):
        self.val = val
        self.children = children
        self.__dict__.update (kw)
        
    def set_ident (self, ident):
        """Sets name (currently only used to get struct/union tags into
        the right place)"""
        pass
    
    def to_str (self, ctx):
        return '# Unimplemented ' + self.__class__.__name__ + " " + str (self.__dict__)

class NodeList(Node):
    sep = '\n'
    def __init__ (self, node, node_list = None):
        if node_list == None:
            Node.__init__ (self,  children = [node])
        else:
            Node.__init__ (self,  children = [node] +
                           node_list.children)
    def to_str (self, ctx):
        l = [c.to_str (ctx) for c in self.children]
        return self.sep.join (l)
            
class NodeListComma (NodeList):
    sep = ', '
            
class Specification(NodeList): pass

class SimpleType (Node):
    def __init__ (self, typ, ident): # ident can be None
        typ.set_ident (ident)
        Node.__init__ (self,  typ=typ, ident = ident)
    def to_str (self, ctx):
        return self.typ.to_str (ctx)


class ArrType (Node):
    fixed = 0
    var = 1
    def __init__ (self, typ, ident, var_fixed, maxind = None):
        Node.__init__ (self,  typ=typ, ident=ident,
                       var_fixed = var_fixed, maxind = maxind)
    def to_str (self, ctx):
        var_fixed = ['rpchelp.fixed', 'rpchelp.var'] [self.var_fixed]
        if self.typ in ['string', 'opaque']:
            return 'rpchelp.%s (%s, %s)' % (self.typ, var_fixed, self.maxind)
        return 'rpchelp.arr (%s, %s, %s)' % (
            self.typ.to_str (ctx), var_fixed, self.maxind)


class OptData (Node):
    def __init__ (self, type_spec, ident):
        Node.__init__ (self,  type_spec=type_spec,
                       ident = ident)
    def to_str (self, ctx):
        return 'rpchelp.opt_data (lambda : %s)' % (self.type_spec.to_str (ctx))

class TypeSpec (Node):
    unsignable = {'int' : 'uint', 'hyper' : 'uhyper'}
    def __init__ (self, val, unsigned, base, compound = 0):
        if unsigned:
            v = self.unsignable.get (val, None)
            if v == None:
                raise ParseError (val + ' cannot be combined w/ unsigned ')
            val = v
        Node.__init__ (self,  val=val, base = base, compound = compound)
    def set_ident (self, ident):
        if self.compound:
            self.val.set_ident (ident)
    def to_str (self, ctx):
        if self.base:
            return 'rpchelp.r_' + self.val
        else:
            if self.compound:
                return self.val.to_str (ctx)
            return self.val

class Enum (Node):
    def __init__ (self, body):
        Node.__init__ (self,  body=body)
    def to_str (self, ctx):
        return self.body.to_str (ctx)


class EnumList(NodeList):
    sep = ''
    def to_str (self, ctx):
        NodeList.to_str (self, ctx) # call for side effect of deferring defs.
        return 'rpchelp.r_int'

class EnumClause (Node):
    def __init__ (self, ident, val):
        Node.__init__ (self,  ident = ident, val=val)
    def to_str (self, ctx):
        ctx.defer ('%s = %s' % (self.ident, self.val))
        return ''
                      
class Struct (Node):
    def __init__ (self, body):
        body.name = '_unnamed' 
        Node.__init__ (self,  body=body)
    def set_ident (self, ident):
        self.body.name = ident
    def to_str (self, ctx):
        return self.body.to_str (ctx)

class StructList(NodeList):
    def to_str (self, ctx):
        l = []
        l_count = 0
        l_index = None
        for (decl,i) in zip(self.children, range (0, len (self.children))):
            elt = "('%s',%s)" % (decl.ident, decl.to_str (ctx))
            l.append (elt)
            if (self.name <> '_unnamed' and isinstance (decl, OptData)
                and decl.type_spec.val == self.name):
                l_count += 1
                l_index = i
        members_str = ",".join (l)
        if l_count == 1:
            return "rpchelp.linked_list ('%s', %d, [%s])" % (self.name,i,
                                                             members_str)
        else: # either a flat struct or a tree or something more complex
            return "rpchelp.struct ('%s', [%s])" % (self.name, members_str)
        return 

class Union (Node):
    def __init__ (self, body):
        body.name = 'unnamed'
        Node.__init__ (self,  body=body)
    def set_ident (self, ident):
        self.body.name = ident

    def to_str (self, ctx):
        return self.body.to_str (ctx)


class UnionList(NodeListComma): pass

class UnionBody(Node):
    def __init__ (self, sw_decl, body):
        Node.__init__ (self,  sw_decl = sw_decl,
                       body=body)
    def to_str (self, ctx):
        return "rpchelp.union ('%s', %s, '%s', {%s})" % (
            self.name, 
            self.sw_decl.typ.to_str (ctx),
            self.sw_decl.ident,
            self.body.to_str (ctx))

class UnionElt (Node):
    def __init__ (self, val, decl):
        Node.__init__ (self,  val=val, decl=decl)
    def to_str (self, ctx):
        typestr = self.decl.to_str (ctx)
        return "%s : %s" % (self.val, typestr)
    # We ignore self.decl.ident.  Not needed for current union
    # implementation, because we can assign the data to '_data',
    # because the type goes with the object bound to '_data',
    # not the declaration.

class UnionDefElt (UnionElt):
    def __init__ (self, decl):
        Node.__init__ (self,  val = 'None', decl=decl)

class VersionList (NodeList): pass

class ProcedureList (NodeListComma): pass

class TypeSpecList (NodeList):
    sep = ','

class Const (Node):
    def __init__ (self, ident, val):
        Node.__init__ (self,  ident=ident, val=val)
    def to_str (self, ctx):
        return '%s = %s' % (self.ident, self.val)


class TypeDef (Node):
    def __init__ (self, decl):
        Node.__init__ (self,  decl=decl)
    def to_str (self, ctx):
        if self.decl.ident == None:
            return '# "typedef void;" encountered'
        # a legit construction according to my reading of the grammar, but not
        # semantically useful.
        typestr = self.decl.to_str (ctx)
        return '%s = %s' % (self.decl.ident, typestr)

class TypeDefCompound (Node):
    def __init__ (self, ident, typ, body):
        body.name = ident
        Node.__init__ (self,  ident=ident, typ=typ,
                       body = body)
    def to_str (self, ctx):
        return ('%s = %s' % (self.ident, self.body.to_str (ctx)))

class Program (Node):
    def __init__ (self, ident, versions, program_id):
        Node.__init__ (self, ident=ident,
                       versions = versions, program_id = program_id)
    def str_one_vers (self, ctx, vers):
        class_decl = 'class %s_%s(rpchelp.Server):' % (self.ident,
                                                       vers.version_id)
        prog = 'prog = %s' % (self.program_id,)
        vers_str = 'vers = %s'  % (vers.version_id,)
        proc_list = [p.to_str (ctx) for p in vers.proc_defs.children]
        return "\n\t".join ([class_decl, prog, vers_str, 'procs = {' + 
                            ",\n\t".join (proc_list)]) + '}\n'
    def to_str (self, ctx):
        return "\n".join (
            [self.str_one_vers (ctx, vers) for vers in self.versions.children])

        
class Version (Node):
    def __init__ (self, ident, proc_defs, version_id):
        Node.__init__ (self,  ident=ident,
                       proc_defs = proc_defs, version_id = version_id)

class Procedure (Node):
    def __init__ (self, ident, ret_type, parm_list, proc_id):
        Node.__init__ (self, ident=ident,
                       ret_type = ret_type, parm_list = parm_list,
                       proc_id = proc_id)
    def to_str (self, ctx):
        return "%s : rpchelp.Proc ('%s', %s, [%s])" % (
            self.proc_id, self.ident, self.ret_type.to_str (ctx),
            self.parm_list.to_str (ctx))

def p_specification_1 (t):
    """specification : definition specification
    | program_def specification"""
    t[0] = Specification (t[1], t[2])

def p_specification_2 (t):
    """specification : definition
    | program_def"""
    t[0] = Specification (t[1])

def p_decl_1(t):
    """declaration : type_specifier IDENT"""
    t[0] = SimpleType (t[1], t[2])

def p_decl_2(t):
    """declaration : type_specifier IDENT LBRACK value RBRACK"""
    t[0] = ArrType (t[1], t[2], ArrType.fixed, t[4])

def p_decl_3(t):
    """declaration : type_specifier IDENT LANGLE value RANGLE"""
    t[0] = ArrType (t[1], t[2], ArrType.var, t[4])

def p_decl_4(t):
    """declaration : type_specifier IDENT LANGLE RANGLE"""
    t[0] = ArrType (t[1], t[2], ArrType.var)

def p_decl_5(t):
    """declaration : OPAQUE IDENT LBRACK value RBRACK""" # fixed opaque
    t[0] = ArrType ('opaque', t[2], ArrType.fixed, t[4])
    
def p_decl_6(t):
    """declaration : OPAQUE IDENT LANGLE value RANGLE
    | STRING IDENT LANGLE value RANGLE""" # var-len opaque/string
    t[0] = ArrType (t[1], t[2], ArrType.var, t[4])

def p_decl_7(t):
    """declaration : OPAQUE IDENT LANGLE RANGLE
    | STRING IDENT LANGLE RANGLE""" # var-len opaque/string
    t[0] = ArrType (t[1], t[2], ArrType.var)

def p_decl_8(t): # optional data
    """declaration : type_specifier STAR IDENT"""
    t[0] = OptData (t[1],t[3])

def p_decl_9 (t):
    """declaration : void"""
    t[0] = SimpleType (t[1], None)

def p_void (t):
    """void : VOID"""
    t[0] = TypeSpec (t[1], unsigned = 0, base = 1)
    
def p_value (t):
    """value : CONSTVAL
    | IDENT"""
    t[0] = t[1]

# XXX some idl uses "unsigned" by itself (= unsigned int).  
# Should revise grammar to allow 1 or more TYPE, sort out semantics
# later
def p_type_spec_1 (t):
    """type_specifier : UNSIGNED TYPE"""
    t[0] = TypeSpec (t[2], unsigned = 1, base = 1)

def p_type_spec_2 (t):
    """type_specifier : TYPE"""
    t[0] = TypeSpec (t[1], unsigned = 0, base = 1)

def p_type_spec_3 (t):
    """type_specifier :  enum_spec 
    | struct_spec 
    | union_spec"""
    t[0] = TypeSpec (t[1], unsigned = 0, base = 0, compound = 1)
    
def p_type_spec_4 (t):
    """type_specifier : IDENT"""
    t[0] = TypeSpec (t[1], unsigned = 0, base = 0)


def p_enum (t):
    """enum_spec : ENUM enum_body"""
    t[0] = Enum (t[2])

def p_enum_body(t):
    """enum_body : LCBRACK enum_body_aux RCBRACK"""
    t[0] = t[2]

def p_enum_body_aux_1 (t):
    """enum_body_aux : IDENT EQ value COMMA enum_body_aux"""
    t[0] = EnumList (EnumClause (t[1], t[3]), t[5])
    
def p_enum_body_aux_2 (t):
    """enum_body_aux : IDENT EQ value"""
    t[0] = EnumList (EnumClause (t[1], t[3]))

def p_struct_spec (t):
    """struct_spec : STRUCT struct_body"""
    t[0] = Struct (t[2])

def p_struct_body(t):
    """struct_body : LCBRACK struct_body_aux RCBRACK"""
    t[0] = t[2]

def p_struct_body_aux_1 (t):
    """struct_body_aux : declaration SEMICOLON struct_body_aux"""
    t[0] = StructList (t[1], t[3])

def p_struct_body_aux_2 (t):
    """struct_body_aux : declaration SEMICOLON"""
    t[0] = StructList (t[1])
    
def p_union (t):
    "union_spec : UNION union_body"
    t[0] = Union (t[2])

def p_union_body(t):
    """union_body : SWITCH LPAREN declaration RPAREN LCBRACK union_body_aux RCBRACK"""
    t[0] = UnionBody (t[3], t[6])

def p_union_body_aux_1 (t):
    "union_body_aux : union_case"
    t[0] = UnionList (t[1])

def p_union_body_aux_2 (t):
    "union_body_aux : union_case union_body_aux"
    t[0] = UnionList (t[1], t[2])

def p_union_case_1 (t):
    "union_case : CASE value COLON declaration SEMICOLON"
    t[0] = UnionElt (t[2], t[4])
    

def p_union_case_2 (t):
    "union_case : DEFAULT COLON declaration SEMICOLON"
    t[0] = UnionDefElt (t[3])

def p_constant_def(t):
    "constant_def : CONST IDENT EQ CONSTVAL SEMICOLON"
    t[0] = Const (t[2], t[4])

def p_type_def_1(t):
    "type_def : TYPEDEF declaration SEMICOLON"
    t[0] = TypeDef (t[2])

def p_type_def_2(t):
    """type_def : ENUM IDENT enum_body SEMICOLON  
    | STRUCT IDENT struct_body SEMICOLON 
    | UNION IDENT union_body SEMICOLON"""
    t[0] = TypeDefCompound (t[2], t[1], t[3])

def p_definition (t):
    """definition : type_def 
    | constant_def"""
    t[0] = t[1]

def p_program_def (t):
    "program_def : PROGRAM IDENT LCBRACK version_defs RCBRACK EQ CONSTVAL SEMICOLON"
    t[0] = Program (t[2], t[4], t[7])

def p_version_defs_1 (t):
    "version_defs : version_def version_defs"
    t[0] = VersionList (t[1], t[2])

def p_version_defs_2 (t):
    "version_defs : version_def"
    t[0] = VersionList (t[1])

def p_version_def (t):
    "version_def : VERSION IDENT LCBRACK procedure_defs RCBRACK EQ CONSTVAL SEMICOLON"
    t[0] = Version (t[2], t[4], t[7])

def p_procedure_defs_1 (t):
    "procedure_defs : procedure_def procedure_defs"
    t[0] = ProcedureList (t[1], t[2])
    
def p_procedure_defs_2 (t):
    "procedure_defs : procedure_def"
    t[0] = ProcedureList (t[1])

def p_procedure_def (t):
    "procedure_def : arg_type_specifier IDENT LPAREN type_spec_list RPAREN EQ CONSTVAL SEMICOLON"
    t[0] = Procedure (t[2], t[1], t[4], t[7])

def p_type_spec_list_1 (t):
    "type_spec_list : arg_type_specifier COMMA type_spec_list"
    t[0] = TypeSpecList (t[1], t[3])
    
def p_type_spec_list_2 (t):
    "type_spec_list : arg_type_specifier"
    t[0] = TypeSpecList (t[1])

def p_arg_type_specifier_1 (t):
    """arg_type_specifier : type_specifier
    | void"""
    # void isn't mentioned as a possibility in the RFC183[12] grammar,
    # but it happens in IDL files.  I should add semantics that
    # if void appears in an arglist, it must be the only elt. XXX
    t[0] = t[1]

def p_error(t):
    raise ParseError (t)

import yacc
yacc.yacc ()
                 
def testlex (s, fn):
    lexer.input (s)
    while 1:
        token = lexer.token ()
        if not token:
            break
        print token

def print_ast (ast, level = 0):
    print " " * 4 * level
    if isinstance (ast, Node):
        print ast.__class__.__name__, 
        if ast.val <> None:
            print_ast (ast.val)
        if ast.children <> None:
            for c in ast.children:
                print_ast (c, level + 1)
    else: print ast
    
def testyacc (s, fn):
    ast = yacc.parse (s)
    print_ast (ast)

import sys

def test (s, fn):
    ast = yacc.parse (s)
    ctx = Ctx ()
    print "### Auto-generated at %s from %s" % (
        time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime()),
        fn)
    print "import rpchelp"
    tmp = ast.to_str (ctx)
    print ctx.finish ()
    print tmp

if __name__ == '__main__':
    testfn = test
#    testfn = testyacc
#    testfn = testlex

    for fn in sys.argv[1:]:
        f = open (fn)
        s = f.read ()
        testfn (s, fn)
