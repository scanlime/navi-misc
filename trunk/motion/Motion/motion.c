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

static void      AMC_dealloc  (AMC *motion);
static PyObject *AMC_getAttr  (AMC *motion, char *name);
static void      AMC_setAttr  (AMC *motion, char *name, PyObject *v);
static PyObject *AMC_repr     (AMC *motion);
static PyObject *AMC_fromFile (AMC *self, PyObject *args);
static PyObject *AMC_save     (AMC *self, PyObject *args);

static PyMethodDef MotionC_methods[] = {
	{NULL, NULL, 0, NULL},
};

static PyMethodDef AMC_methods[] = {
	{"from_file", (PyCFunction) AMC_fromFile, METH_STATIC | METH_VARARGS, "Create from an AMC file"},
	{"save",      (PyCFunction) AMC_save,     METH_VARARGS,               "Save as an AMC file"},
	{NULL,        NULL,                          0,                          NULL},
};

PyTypeObject AMC_Type = {
	PyObject_HEAD_INIT(NULL)
	0,				// ob_size
	"AMC",				// tp_name
	sizeof (AMC),			// tp_basicsize
	0,				// tp_itemsize
	(destructor) AMC_dealloc,	// tp_dealloc
	0,				// tp_print
	(getattrfunc) AMC_getAttr,	// tp_getattr
	(setattrfunc) AMC_setAttr,	// tp_setattr
	0,				// tp_compare
	(reprfunc) AMC_repr,		// tp_repr
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
	AMC_methods,			// tp_methods
	0,				// tp_members

};

PyObject *CreateAMC ()
{
	AMC *m = (AMC *) PyObject_NEW (AMC, &AMC_Type);
	m->bones    = PyDict_New ();
	m->comments = PyList_New (0);
	m->format   = PyList_New (0);
	return (PyObject *) m;
}

static void
AMC_dealloc (AMC *motion)
{
	PyObject_DEL (motion->bones);
	PyObject_DEL (motion->comments);
	PyObject_DEL (motion->format);
	PyObject_DEL (motion);
}

static PyObject *
AMC_getAttr (AMC *motion, char *name)
{
	PyObject *attr = Py_None;

	if (strcmp (name, "name") == 0) {
		// None is an appropriate response here, so don't bother with the checks below
		if (motion->name == NULL)
			return Py_None;
		return PyString_FromString (motion->name);
	} else if (strcmp (name, "comments") == 0) {
		attr = motion->comments;
	} else if (strcmp (name, "format") == 0) {
		attr = motion->format;
	} else if (strcmp (name, "__members__") == 0) {
		attr = Py_BuildValue ("[s,s,s]", "name", "comments", "format");
	}

	if (attr == Py_None)
		return Py_FindMethod (AMC_methods, (PyObject *) motion, name);

	return attr;
}

static void
AMC_setAttr (AMC *motion, char *name, PyObject *v)
{
}

static PyObject *
AMC_repr (AMC *motion)
{
	return PyString_FromFormat ("[AMC \"s\", d frames]");
}

static PyObject *
AMC_fromFile (AMC *self, PyObject *args)
{
	char *filename;
	GIOChannel *file;
	AMC *motion;
	gchar *line;
	int terminator;
	guint64 current_frame, total_frames;
	GSList *bones = NULL;
	GSList *dofs = NULL;
	GSList *data = NULL;
	GSList *i;

	// get filename
	if (!PyArg_ParseTuple (args, "s", &filename)) {
		PyErr_SetObject (PyExc_TypeError, PyString_FromString ("expected 'string'"));
		return NULL;
	}

	file = g_io_channel_new_file (filename, "r", NULL);
	if (file == NULL) {
		// FIXME - we should be using the GError, not errno
		return PyErr_SetFromErrnoWithFilename (PyExc_IOError, filename);
	}

	motion = (AMC *) CreateAMC ();

	// first pass - go through, grab comments & format, parse first frame
	// to find #dof for each bone, total number of frames
	while (g_io_channel_read_line (file, &line, NULL, &terminator, NULL) == G_IO_STATUS_NORMAL) {
		// get rid of line terminator
		line[terminator] = '\0';

		// parse data
		if (line[0] == '#') {
			// comment
			PyList_Append (motion->comments, PyString_FromString (line));
		} else if (line[0] == ':') {
			// format specifier
			PyList_Append (motion->format, PyString_FromString (line));
		} else {
			gchar *ch;
			gboolean newframe = TRUE;

			g_strstrip (line);

			// empty line?
			if (strlen (line) == 0)
				continue;

			// determine if this line just contains an int - if so, it's
			// the beginning of a new frame
			for (ch = line; *ch; ch++) {
				if (!g_ascii_isdigit (*ch)) {
					newframe = FALSE;
					break;
				}
			}

			if (newframe) {
				current_frame = g_ascii_strtoull (line, NULL, 10);
			} else {
				if (current_frame == 1) {
					gchar **tokens = g_strsplit (line, " ", 0);
					guint token;
					guint dof = 0;

					g_strstrip (tokens[0]);
					for (token = 1; tokens[token]; token++) {
						g_strstrip (tokens[token]);
						if (strlen (tokens[token]))
							dof++;
					}
					// create two lists, one with the names of the bones (so we
					// know what order things go in), the other with the number
					// of dofs for each bone
					bones = g_slist_append (bones, g_strdup (tokens[0]));
					dofs = g_slist_append (dofs, GUINT_TO_POINTER (dof));
					g_print ("found bone '%s' with %d dof\n", tokens[0], dof);
					g_strfreev (tokens);
				}
			}
		}
		g_free (line);
	}

	total_frames = current_frame;
	g_print ("%lld total frames found\n", total_frames);

	// allocate Numeric arrays
	for (i = dofs; i; i = g_slist_next (i)) {
		int dims[2];
		guint dof = GPOINTER_TO_UINT (i->data);
		PyObject *array;

		dims[0] = dof;
		dims[1] = total_frames;

		array = PyArray_FromDims (2, dims, PyArray_FLOAT);
		data = g_slist_append (data, array);
	}

	g_io_channel_seek_position (file, 0, G_SEEK_SET, NULL);

	// second pass - read in all the data, now that we have space allocated for it
	while (g_io_channel_read_line (file, &line, NULL, &terminator, NULL) == G_IO_STATUS_NORMAL) {
		// get rid of line terminator
		line[terminator] = '\0';

		// parse data
		if (line[0] == '#' || line[0] == ':') {
			// we read these in during the first pass. ignore
			continue;
		} else {
			gchar *ch;
			gboolean newframe = TRUE;

			g_strstrip (line);

			// empty line ?
			if (strlen (line) == 0)
				continue;

			// determine if this line just contains an int - if so, it's
			// the beginning of a new frame
			for (ch = line; *ch; ch++) {
				if (!g_ascii_isdigit (*ch)) {
					newframe = FALSE;
					break;
				}
			}


			if (newframe) {
				current_frame = g_ascii_strtoull (line, NULL, 10);
			} else {
				gchar **tokens = g_strsplit (line, " ", 0);
				guint token;

				g_strstrip (tokens[0]);
				for (token = 1; tokens[token]; token++) {
					g_strstrip (tokens[token]);
					if (strlen (tokens[token])) {
					}
				}

				g_strfreev (tokens);
			}
		}

		g_free (line);
	}

	g_slist_foreach (bones, (GFunc) g_free, NULL);
	g_slist_free (bones);
	g_slist_free (dofs);

	g_io_channel_close (file);

	return (PyObject *) motion;
}

static PyObject *
AMC_save (AMC *self, PyObject *args)
{
	return NULL;
}

PyMODINIT_FUNC
initmotion_c (void)
{
	PyObject *module;

	if (PyType_Ready (&AMC_Type) < 0)
		return;

	module = Py_InitModule ("motion_c", MotionC_methods);

	// initialize Numeric
	import_array ();

	if (module == NULL)
		return;

	Py_INCREF (&AMC_Type);
	PyModule_AddObject (module, "AMC", (PyObject *) &AMC_Type);
}
