# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.
import _librsvg
def _swig_setattr(self,class_type,name,value):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    self.__dict__[name] = value

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


rsvg_error_quark = _librsvg.rsvg_error_quark

rsvg_set_default_dpi = _librsvg.rsvg_set_default_dpi

rsvg_handle_new = _librsvg.rsvg_handle_new

rsvg_handle_set_dpi = _librsvg.rsvg_handle_set_dpi

rsvg_handle_set_size_callback = _librsvg.rsvg_handle_set_size_callback

rsvg_handle_write = _librsvg.rsvg_handle_write

rsvg_handle_close = _librsvg.rsvg_handle_close

rsvg_handle_get_pixbuf = _librsvg.rsvg_handle_get_pixbuf

rsvg_handle_free = _librsvg.rsvg_handle_free

cvar = _librsvg.cvar

