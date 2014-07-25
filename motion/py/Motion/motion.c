/*
 * motion.c - python class which wraps the AMC data
 *
 * Copyright (C) 2005-2007 David Trowbridge
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
#include <structmember.h>
#include <Numeric/arrayobject.h>
#include <glib.h>
#include "motion.h"

static PyObject *AMC_new      (PyTypeObject *type, PyObject *args, PyObject *kw);
static int       AMC_init     (AMC *motion, PyObject *args, PyObject *kw);
static void      AMC_dealloc  (AMC *motion);
static PyObject *AMC_repr     (AMC *motion);
static PyObject *AMC_fromFile (AMC *self, PyObject *args);
static PyObject *AMC_save     (AMC *self, PyObject *args);
static int       AMC_clear    (AMC *self);

static PyMethodDef MotionC_methods[] = {
	{NULL, NULL, 0, NULL},
};

static PyMethodDef AMC_methods[] = {
	{"from_file", (PyCFunction) AMC_fromFile, METH_STATIC | METH_VARARGS, "Create from an AMC file"},
	{"save",      (PyCFunction) AMC_save,     METH_VARARGS,               "Save as an AMC file"},
	{NULL,        NULL,                       0,                          NULL},
};

static PyMemberDef AMC_members[] = {
	{"bones",    T_OBJECT, offsetof (AMC, bones),    0, "bones array"},
	{"comments", T_OBJECT, offsetof (AMC, comments), 0, "comments strings"},
	{"format",   T_OBJECT, offsetof (AMC, format),   0, "format strings"},
	{NULL,       0,        0,                        0, NULL},
};

PyTypeObject AMC_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                          // ob_size
	"AMC",                      // tp_name
	sizeof (AMC),               // tp_basicsize
	0,                          // tp_itemsize
	(destructor) AMC_dealloc,   // tp_dealloc
	0,                          // tp_print
	0,                          // tp_getattr
	0,                          // tp_setattr
	0,                          // tp_compare
	(reprfunc) AMC_repr,        // tp_repr
	0,                          // tp_as_number
	0,                          // tp_as_sequence
	0,                          // tp_as_mapping
	0,                          // tp_as_hash
	0,                          // tp_call
	0,                          // tp_str
	0,                          // tp_getattro
	0,                          // tp_setattro
	0,                          // tp_as_buffer
	0,                          // tp_flags
	0,                          // tp_doc
	0,                          // tp_traverse
	(inquiry) AMC_clear,        // tp_clear
	0,                          // tp_richcompare
	0,                          // tp_weaklistoffset
	0,                          // tp_iter
	0,                          // tp_iternext
	AMC_methods,                // tp_methods
	AMC_members,                // tp_members
	0,                          // tp_getset
	0,                          // tp_base
	0,                          // tp_dict
	0,                          // tp_descr_get
	0,                          // tp_descr_set
	0,                          // tp_dictoffset
	(initproc) AMC_init,        // tp_init
	0,                          // tp_alloc
	AMC_new,                    // tp_new
};

PyObject *
CreateAMC ()
{
	AMC *self;

	self = (AMC *) PyObject_NEW (AMC, &AMC_Type);
	if (self != NULL)
		AMC_init (self, NULL, NULL);

	return (PyObject *) self;
}

static PyObject *
AMC_new (PyTypeObject *type, PyObject *args, PyObject *kw)
{
	return CreateAMC ();
}

static int
AMC_init (AMC *motion, PyObject *args, PyObject *kw)
{
	motion->bones    = PyDict_New ();
	motion->comments = PyList_New (0);
	motion->format   = PyList_New (0);

	return 0;
}

static void
AMC_dealloc (AMC *motion)
{
	AMC_clear (motion);
	PyObject_DEL (motion);
}

static PyObject *
AMC_repr (AMC *motion)
{
	if (motion->name == NULL)
		return PyString_FromFormat ("[AMC]");
	return PyString_FromFormat ("[AMC \"%s\"]", motion->name);
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
	GSList *i, *j;
	float *f;
	PyArrayObject *pao;

	// get filename
	if (!PyArg_ParseTuple (args, "s;expected 'string'", &filename))
		return NULL;

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
			PyObject *string = PyString_FromString (line);
			PyList_Append (motion->comments, string);
			Py_DECREF (string);
		} else if (line[0] == ':') {
			// format specifier
			PyObject *string = PyString_FromString (line);
			PyList_Append (motion->format, string);
			Py_DECREF (string);
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
					g_strfreev (tokens);
				}
			}
		}
		g_free (line);
	}

	total_frames = current_frame;

	// allocate Numeric arrays
	for (i = dofs; i; i = g_slist_next (i)) {
		int dims[2];
		guint dof = GPOINTER_TO_UINT (i->data);
		PyObject *array;

		dims[0] = total_frames;
		dims[1] = dof;

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
				current_frame = g_ascii_strtoull (line, NULL, 10) - 1;
				i = data;
				pao = i->data;
			} else {
				gchar **tokens = g_strsplit (line, " ", 0);
				guint token, dof = 0;

				g_strstrip (tokens[0]);
				for (token = 1; tokens[token]; token++) {
					g_strstrip (tokens[token]);
					if (strlen (tokens[token])) {
						f = (float *) (pao->data + (current_frame * pao->strides[0]) + (dof * pao->strides[1]));
						*f = (float) g_ascii_strtod (tokens[token], NULL);
					}
					dof++;
				}

				g_strfreev (tokens);
				i = g_slist_next (i);
				if (i != NULL) {
					pao = i->data;
				}
			}
		}

		g_free (line);
	}

	for (i = data, j = bones; i; i = g_slist_next (i), j = g_slist_next (j)) {
		PyObject *key = PyString_FromString (j->data);
		PyDict_SetItem (motion->bones, key, i->data);
	}

	g_slist_foreach (bones, (GFunc) g_free, NULL);
	g_slist_free (bones);
	g_slist_free (dofs);
	g_slist_free (data);

	g_io_channel_shutdown (file, FALSE, NULL);
	g_io_channel_unref (file);

	return (PyObject *) motion;
}

static PyObject *
AMC_save (AMC *self, PyObject *args)
{
	char *filename;
	GIOChannel *file;
	int size, frames, i, j, k;
	PyObject *keys;

	// get filename
	if (!PyArg_ParseTuple (args, "s;expected 'string'", &filename))
		return NULL;

	file = g_io_channel_new_file (filename, "w", NULL);
	if (file == NULL) {
		// FIXME - we should be using the GError, not errno
		return PyErr_SetFromErrnoWithFilename (PyExc_IOError, filename);
	}

	// write out comment
	size = PyList_Size (self->comments);
	for (i = 0; i < size; i++) {
		PyObject *line;
		char *cline;

		line = PyList_GetItem (self->comments, i);
		cline = PyString_AsString (line);

		g_io_channel_write_chars (file, cline, strlen (cline), NULL, NULL);
		g_io_channel_write_chars (file, "\n", 1, NULL, NULL);
	}

	// write out format
	size = PyList_Size (self->format);
	if (size == 0)
		g_io_channel_write_chars (file, ":FULLY-SPECIFIED\n:DEGREES\n", strlen (":FULLY-SPECIFIED\n:DEGREES\n"), NULL, NULL);
	for (i = 0; i < size; i++) {
		PyObject *line;
		char *cline;

		line = PyList_GetItem (self->format, i);
		cline = PyString_AsString (line);

		g_io_channel_write_chars (file, cline, strlen (cline), NULL, NULL);
		g_io_channel_write_chars (file, "\n", 1, NULL, NULL);
	}

	// get keys
	keys = PyDict_Keys (self->bones);
	size = PyList_Size (keys);
	if (size == 0)
		// FIXME - throw error
		return Py_False;

	// find # of frames
	{
		PyObject *bone = PyDict_GetItem (self->bones, PyList_GetItem (keys, 0));
		PyArrayObject *array = (PyArrayObject *) bone;
		frames = array->dimensions[0];
	}

	for (j = 0; j < frames; j++) {
		GIOStatus status;
		char *frame;

		// Write out the frame number
		frame = g_strdup_printf ("%d\n", j + 1);
		g_io_channel_write_chars (file, frame, strlen (frame), NULL, NULL);
		g_free (frame);

		for (i = 0; i < size; i++) {
			PyObject *key;
			char *bone_name;
			PyArrayObject *bone;

			// Write the bone name
			key = PyList_GetItem (keys, i);
			bone = (PyArrayObject *) PyDict_GetItem (self->bones, key);
			bone_name = PyString_AsString (key);
			g_io_channel_write_chars (file, bone_name, strlen (bone_name), NULL, NULL);

			// Write out the bone data
			for (k = 0; k < bone->dimensions[1]; k++) {
				char *data = g_strdup_printf (" %f", *((float*) (bone->data + (j * bone->strides[0]) + (k * bone->strides[1]))));
				g_io_channel_write_chars (file, data, strlen (data), NULL, NULL);
				g_free (data);
			}
			 g_io_channel_write_chars (file, "\n", 1, NULL, NULL);
		}
	}

	g_io_channel_shutdown (file, TRUE, NULL);
	g_io_channel_unref (file);

	Py_RETURN_TRUE;
}

static int
AMC_clear (AMC *self)
{
	Py_CLEAR (self->bones);
	Py_CLEAR (self->comments);
	Py_CLEAR (self->format);
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
