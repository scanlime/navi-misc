/*
 * libunicone.i - Swig interface between python and our C code for configuring
 *                the device and low-level communication with it.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

%module libunicone

%{
#include <unicone_device.h>
#include <genesis_device.h>
#include <progress_python.h>
#include <progress_console.h>
%}

%include typemaps.i

%typemap(in) (const unsigned char *data, int length) {
   $1 = PyString_AsString($input);
   $2 = PyString_Size($input);
}

%include unicone_device.h
%include genesis_device.h
%include progress.h
%include progress_python.h
%include progress_console.h

/* The End */
