/*
 * motion.c - python class which wraps the AMC data
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
#include <glib.h>
#include "motion.h"

static void      Motion_dealloc  (Motion *motion);
static PyObject *Motion_getAttr  (Motion *motion, char *name);
static void      Motion_setAttr  (Motion *motion, char *name, PyObject *v);
static PyObject *Motion_repr     (Motion *motion);
static PyObject *Motion_fromFile (Motion *self, PyObject *args);
static PyObject *Motion_save     (Motion *self, PyObject *args);

static PyMethodDef MotionC_methods[] = {
	{NULL, NULL, 0, NULL},
};

static PyMethodDef Motion_methods[] = {
	{"from_file", (PyCFunction) Motion_fromFile, METH_STATIC | METH_VARARGS, "Create from an AMC file"},
	{"save",      (PyCFunction) Motion_save,     METH_VARARGS,               "Save as an AMC file"},
	{NULL,        NULL,                          0,                          NULL},
};

PyTypeObject Motion_Type = {
	PyObject_HEAD_INIT(NULL)
	0,				// ob_size
	"AMC Motion",			// tp_name
	sizeof (Motion),		// tp_basicsize
	0,				// tp_itemsize
	(destructor) Motion_dealloc,	// tp_dealloc
	0,				// tp_print
	(getattrfunc) Motion_getAttr,	// tp_getattr
	(setattrfunc) Motion_setAttr,	// tp_setattr
	0,				// tp_compare
	(reprfunc) Motion_repr,		// tp_repr
	0,				// tp_as_number
	0,				// tp_as_sequence
	0,				// tp_as_mapping
	0,				// tp_as_hash
	0,				// tp_call
	0,				// tp_str
	0,				// tp_getattro
	0,				// tp_setattro
	0,				// tp_as_buffer
	0,				// tp_flags
	0,				// tp_doc
	0,				// tp_traverse
	0,				// tp_clear
	0,				// tp_richcompare
	0,				// tp_weaklistoffset
	0,				// tp_iter
	0,				// tp_iternext
	Motion_methods,			// tp_methods
	0,				// tp_members

};

PyObject *CreateMotion ()
{
	Motion *m = (Motion *) PyObject_NEW (Motion, &Motion_Type);
	m->bones = PyDict_New ();
	m->comments = PyList_New (0);
	return (PyObject *) m;
}

static void
Motion_dealloc (Motion *motion)
{
	PyObject_DEL (motion->bones);
	PyObject_DEL (motion->comments);
	PyObject_DEL (motion);
}

static PyObject *
Motion_getAttr (Motion *motion, char *name)
{
	PyObject *attr = Py_None;

	if (strcmp (name, "name") == 0) {
		// None is an appropriate response here, so don't bother with the checks below
		if (motion->name == NULL)
			return Py_None;
		return PyString_FromString (motion->name);
	} else if (strcmp (name, "comments") == 0) {
		attr = motion->comments;
	} else if (strcmp (name, "__members__") == 0) {
		attr = Py_BuildValue ("[s,s]", "name", "comments");
	}

	if (attr == Py_None)
		return Py_FindMethod (Motion_methods, (PyObject *) motion, name);

	return attr;
}

static void
Motion_setAttr (Motion *motion, char *name, PyObject *v)
{
}

static PyObject *
Motion_repr (Motion *motion)
{
	return PyString_FromFormat ("[Motion \"s\", d frames]");
}

static PyObject *
Motion_fromFile (Motion *self, PyObject *args)
{
	char *filename;
	GIOChannel *file;
	Motion *motion;
	gchar *line;

	if (!PyArg_ParseTuple (args, "s", &filename)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected 'string'"));
		return NULL;
	}

	file = g_io_channel_new_file (filename, "r", NULL);
	if (file == NULL) {
		// FIXME - we should be using the GError, not errno
		return PyErr_SetFromErrnoWithFilename (PyExc_IOError, filename);
	}

	motion = (Motion *) CreateMotion ();

	while (g_io_channel_read_line (file, &line, NULL, NULL, NULL) == G_IO_STATUS_NORMAL) {
		if (line[0] == '#') {
			// comment
			PyList_Append (motion->comments, PyString_FromString (line));
		} else if (line[0] == ':') {
			// format specifier
		}
		g_free (line);
	}

	g_io_channel_close (file);

	return (PyObject *) motion;
}

static PyObject *
Motion_save (Motion *self, PyObject *args)
{
	return NULL;
}

PyMODINIT_FUNC
initmotion_c (void)
{
	PyObject *module;

	if (PyType_Ready (&Motion_Type) < 0)
		return;

	module = Py_InitModule ("motion_c", MotionC_methods);

	if (module == NULL)
		return;

	Py_INCREF (&Motion_Type);
	PyModule_AddObject (module, "Motion", (PyObject *) &Motion_Type);
}
