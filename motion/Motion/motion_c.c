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
#include "motion.h"

static PyObject *load (PyObject *self, PyObject *args);

static PyMethodDef
MotionCMethods[] = {
	{"load", load, METH_VARARGS, "Load an AMC File"},
	{NULL,   NULL, 0,            NULL},
};

PyMODINIT_FUNC
initmotion_c (void)
{
	(void) Py_InitModule ("motion_c", MotionCMethods);
}

static PyObject *
load (PyObject *self, PyObject *args)
{
	char *filename;
	FILE *file;
	PyObject *motion;

	if (!PyArg_ParseTuple (args, "s", &filename)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected 'string'"));
		return NULL;
	}

	file = fopen (filename, "r");
	if (file == NULL) {
		return PyErr_SetFromErrnoWithFilename (PyExc_IOError, filename);
	}

	motion = CreateMotion ();

	fclose (file);

	return motion;
}
