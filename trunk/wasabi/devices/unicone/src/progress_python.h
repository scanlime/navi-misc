/*
 * progress_python.h - A progress reporter object that wraps an analogous
 *                     set of Python objects implementing a progress reporter
 *                     and progress operations.
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

#ifndef _H_PROGRESS_PYTHON
#define _H_PROGRESS_PYTHON

#include <Python.h>
#include "progress.h"

/* The supplied python object is expected to have 'start', 'message',
 * and 'error' methods, matching the signature of the corresponding
 * C functions in progress_reporter. The 'start' method should
 * return another python object with 'update' and 'finish' methods.
 */
struct progress_reporter* progress_reporter_python_new(PyObject *pyo);

#endif /* _H_PROGRESS_PYTHON */

/* The End */
