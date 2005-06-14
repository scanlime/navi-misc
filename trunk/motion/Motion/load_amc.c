/*
 * load_amc.c - C extension which loads an AMC (acclaim motion-capture)
 * 	file and returns it as a dictionary of Numeric arrays (per-bone).
 * 	This exists mostly because doing it with pyparsing is painfully
 * 	slow.
 *
 * Copyright (C) 2005 David Trowbridge
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

#include <Python.h>
#include <Numeric/arrayobject.h>

static PyObject *load (PyObject *self, PyObject *args);

static PyMethodDef
LoadAmcMethods[] = {
	{"load", load, METH_VARARGS, "Load an AMC File"},
	{NULL,   NULL, 0,            NULL},
};

PyMODINIT_FUNC
init_load_amc (void)
{
	(void) Py_InitModule ("load_amc", LoadAmcMethods);
}

static PyObject *
load (PyObject *self, PyObject *args)
{
	char *filename;

	if (!PyArg_ParseTuple (args, "s", &filename)) {
		// FIXME - throw error
		return NULL;
	}
	PyObject *dict = PyDict_New ();
}
