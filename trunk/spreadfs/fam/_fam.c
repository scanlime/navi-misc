/*
 * Python FAM, 2002 Robert Sander <robert.sander@epigenomics.com>
 *
 * released under LGPL
 *
 * USE AT YOUR OWN RISK
 *
 * $Id: _fam.c,v 1.11 2003/01/29 21:39:08 gurubert Exp $
 *
 */

#include <Python.h>
#include <fam.h>
#include "_fam.h"

// #define _DEBUG 1

#define add_int(d, name)                                                \
	{								\
		PyObject *i = PyInt_FromLong(FAM##name);		\
		PyDict_SetItemString( d, #name, i );			\
		Py_DECREF(i);						\
	}

char* code2str(int code) {
  char* rv;

  switch (code) {
  case FAMExists:
    rv = "exists";
    break;
  case FAMEndExist:
    rv = "endExist";
    break;
  case FAMChanged:
    rv = "changed";
    break;
  case FAMDeleted:
    rv = "deleted";
    break;
  case FAMStartExecuting:
    rv = "started executing";
    break;
  case FAMStopExecuting:
    rv = "stopped executing";
    break;
  case FAMCreated:
    rv = "created";
    break;
  case FAMMoved:
    rv = "moved";
    break;
  case FAMAcknowledge:
    rv = "acknowleged";
    break;
  default:
    rv = NULL;
  }
  return rv;
}

/*
 * FAMConnection object definitions
 *
 * The Connection object is returned by the open() method of the module.
 * It implements FAMClose(), FAMMonitorDirectory(), FAMMonitorFile(),
 * FAMNextEvent() and FAMPending().
 *
 */

typedef struct {
  PyObject_HEAD
  FAMConnection *fc;
} _fam_connection_object;

static PyMethodDef _fam_connection_methods[] = {
  { "close", _fam_close, 1, _fam_close__doc__},
  { "monitorDirectory", _fam_monitor_directory, 1, _fam_monitor_directory__doc__},
  { "monitorFile", _fam_monitor_file, 1, _fam_monitor_file__doc__},
  { "nextEvent", _fam_next_event, 1, _fam_next_event__doc__},
  { "pending", _fam_pending, 1, _fam_pending__doc__},
  { "fileno", _fam_fileno, 1, _fam_fileno__doc__},
  { NULL, NULL, 0, NULL}
};

static PyObject* _fam_connection_getattr(PyObject* self, char* name) {
#ifdef _DEBUG
  fprintf(stderr, "_fam: getting attribute %s from connection object.\n", name);
#endif
  return Py_FindMethod(_fam_connection_methods, self, name);
}

void _fam_connection_del(PyObject* self) {
#ifdef _DEBUG
  fprintf(stderr, "_fam: deleting connection object.\n");
#endif
  _fam_close(self, NULL);
  PyMem_DEL(self);
}

static PyTypeObject _fam_connection_type = {
#if defined(WIN32) || defined(__CYGWIN__)
  PyObject_HEAD_INIT(NULL)
#else
  PyObject_HEAD_INIT(&PyType_Type)
#endif
  0,                               /* ob_size */
  "FAMConnection",                 /* tp_name */
  sizeof(_fam_connection_object),  /* tp_basicsize */
  0,                               /* tp_itemsize */
  _fam_connection_del,             /* tp_dealloc */
  0,                               /* tp_print */
  _fam_connection_getattr,         /* tp_getattr */
  0,                               /* tp_setattr */
  0,                               /* tp_compare */
  0,                               /* tp_repr */
  0,                               /* tp_as_number */
  0,                               /* tp_as_sequence */
  0,                               /* tp_as_mapping */
  0,                               /* tp_hash */
  0,                               /* call */
  0,                               /* str */
  0,                               /* tp_getattro */
  0,                               /* tp_setattro */
  0,                               /* tp_as_buffer */
  0,                               /* tp_flags */
  _fam_connection_type__doc__,     /* doc string */
  0,                               /* tp_traverse */
  0                                /* tp_clear */
};

/*
 * FAMRequest object definitions
 *
 * A Request object is returned by one of the two monitor methods.
 * It implements FAMSuspendMonitor(), FAMResumeMonitor() and
 * FAMCancelMonitor()
 *
 */

typedef struct {
  PyObject_HEAD
  _fam_connection_object *fc_obj;
  FAMRequest *fr;
} _fam_request_object;

static PyMethodDef _fam_request_methods[] = {
  { "suspendMonitor", _fam_suspend_monitor, 1, _fam_suspend_monitor__doc__},
  { "resumeMonitor", _fam_resume_monitor, 1, _fam_resume_monitor__doc__},
  { "cancelMonitor", _fam_cancel_monitor, 1, _fam_cancel_monitor__doc__},
  { "requestID", _fam_request_id, 1, _fam_request_id__doc__},
  { NULL, NULL, 0, NULL}
};

static PyObject* _fam_request_getattr(PyObject* self, char* name) {
#ifdef _DEBUG
  fprintf(stderr, "_fam: getting attribute %s from request object.\n", name);
#endif
  return Py_FindMethod(_fam_request_methods, self, name);
}

void _fam_request_del(PyObject* self) {
#ifdef _DEBUG
  fprintf(stderr, "_fam: deleting request object.\n");
#endif
  _fam_cancel_monitor(self, NULL);
  PyMem_DEL(self);
}

static PyTypeObject _fam_request_type = {
#if defined(WIN32) || defined(__CYGWIN__)
  PyObject_HEAD_INIT(NULL)
#else
  PyObject_HEAD_INIT(&PyType_Type)
#endif
  0,                               /* ob_size */
  "FAMRequest",                    /* tp_name */
  sizeof(_fam_request_object),     /* tp_basicsize */
  0,                               /* tp_itemsize */
  _fam_request_del,                /* tp_dealloc */
  0,                               /* tp_print */
  _fam_request_getattr,            /* tp_getattr */
  0,                               /* tp_setattr */
  0,                               /* tp_compare */
  0,                               /* tp_repr */
  0,                               /* tp_as_number */
  0,                               /* tp_as_sequence */
  0,                               /* tp_as_mapping */
  0,                               /* tp_hash */
  0,                               /* call */
  0,                               /* str */
  0,                               /* tp_getattro */
  0,                               /* tp_setattro */
  0,                               /* tp_as_buffer */
  0,                               /* tp_flags */
  _fam_request_type__doc__,        /* doc string */
  0,                               /* tp_traverse */
  0                                /* tp_clear */
};

/*
 * FAMEvent object definitions
 *
 * An Event object is returned by the nextEvent method.
 * It implements the code2str() method and has
 * the attributes connection, requestID, hostname,
 * filename, userData and code.
 *
 */

typedef struct {
  PyObject_HEAD
  PyObject *attr;
  _fam_connection_object *fc_obj;
  enum FAMCodes code;
} _fam_event_object;

static PyMethodDef _fam_event_methods[] = {
  { "code2str", _fam_code2str, 1, _fam_code2str__doc__},
  { NULL, NULL, 0, NULL}
};

static PyObject* _fam_event_getattr(PyObject* self, char* name) {
  _fam_event_object *fe_obj;
  PyObject *v;

  fe_obj = (_fam_event_object *)self;

  if (fe_obj->attr) {

    v = PyDict_GetItemString(fe_obj->attr, name);
    if (v) {
      Py_INCREF(v);
      return v;
    }
  }
  return Py_FindMethod(_fam_event_methods, self, name);
}

static int _fam_event_setattr(PyObject* self, char* name, PyObject* v) {
  _fam_event_object *fe_obj = (_fam_event_object *)self;

  if (! fe_obj->attr) {
    fe_obj->attr = PyDict_New();
    if (! fe_obj->attr)
      return -1;
  }
  if (! v) {
    int rv = PyDict_DelItemString(fe_obj->attr, name);
    if (rv < 0)
      PyErr_SetString(PyExc_AttributeError, "delete non-existing FAMEvent attribute");
    return rv;
  } else
    return PyDict_SetItemString(fe_obj->attr, name, v);
}

void _fam_event_del(PyObject* self) {
  _fam_event_object *fe_obj = (_fam_event_object *)self;
#ifdef _DEBUG
  fprintf(stderr, "_fam: deleting event object.\n");
#endif
  if (fe_obj) {
    if (fe_obj->fc_obj) {
      Py_DECREF(fe_obj->fc_obj);
      fe_obj->fc_obj = NULL;
    }
    Py_XDECREF(fe_obj->attr);
    PyMem_DEL(fe_obj);
  }
}

static PyTypeObject _fam_event_type = {
#if defined(WIN32) || defined(__CYGWIN__)
  PyObject_HEAD_INIT(NULL)
#else
  PyObject_HEAD_INIT(&PyType_Type)
#endif
  0,                               /* ob_size */
  "FAMEvent",                      /* tp_name */
  sizeof(_fam_event_object),       /* tp_basicsize */
  0,                               /* tp_itemsize */
  _fam_event_del,                  /* tp_dealloc */
  0,                               /* tp_print */
  _fam_event_getattr,              /* tp_getattr */
  _fam_event_setattr,              /* tp_setattr */
  0,                               /* tp_compare */
  0,                               /* tp_repr */
  0,                               /* tp_as_number */
  0,                               /* tp_as_sequence */
  0,                               /* tp_as_mapping */
  0,                               /* tp_hash */
  0,                               /* call */
  0,                               /* str */
  0,                               /* tp_getattro */
  0,                               /* tp_setattro */
  0,                               /* tp_as_buffer */
  0,                               /* tp_flags */
  _fam_event_type__doc__,          /* doc string */
  0,                               /* tp_traverse */
  0                                /* tp_clear */
};

/*
 * The method definitions.
 *
 */

static PyObject* _fam_open(PyObject* self, PyObject* args) {

  int rc;
  
  _fam_connection_object *fc_obj = PyObject_NEW(_fam_connection_object, &_fam_connection_type);
  if (!fc_obj)
    return NULL;

  fc_obj->fc = (FAMConnection *)malloc(sizeof(FAMConnection));

  if (!fc_obj->fc) {
    PyErr_SetString(PyExc_MemoryError, "_fam: unable to malloc for connection");
    return NULL;
  }

  rc = FAMOpen(fc_obj->fc);

  if (!rc) {
    return (PyObject*)fc_obj;
  } else {
    PyErr_SetString(PyExc_IOError, "_fam: unable to open connection");
    return NULL;
  }
};

static PyObject* _fam_close(PyObject* self, PyObject* args) {
  int rc;
  _fam_connection_object *fc_obj = (_fam_connection_object *)self;

  if (fc_obj->fc) {
    rc = FAMClose(fc_obj->fc);

    free(fc_obj->fc);
    fc_obj->fc = NULL;

    if (!rc) {
#ifdef _DEBUG
      fprintf(stderr, "_fam: closed connection.\n");
#endif
    } else {

      PyErr_SetString(PyExc_IOError, "_fam: unable to close connection");
      return NULL;
    }
  } else {
#ifdef _DEBUG
    fprintf(stderr, "_fam: no connection to close.\n");
#endif
  }
  Py_INCREF(Py_None);
  return Py_None;
};

static PyObject* _fam_monitor_directory(PyObject* self, PyObject* args) {
  int rc;
  char *filename;
  PyObject *userData;
  _fam_connection_object *fc_obj;
  _fam_request_object *fr_obj;

  fc_obj = (_fam_connection_object *)self;

  if (fc_obj->fc) {

    if (!PyArg_ParseTuple(args, "sO", &filename, &userData))
      return NULL;

    fr_obj = PyObject_NEW(_fam_request_object, &_fam_request_type);
    if (!fr_obj)
      return NULL;

    fr_obj->fc_obj = fc_obj;
    Py_INCREF(fc_obj);
    fr_obj->fr = (FAMRequest *)malloc(sizeof(FAMRequest));

    if (!fr_obj->fr) {
      PyErr_SetString(PyExc_MemoryError, "_fam: unable to malloc for request");
      return NULL;
    }

    rc = FAMMonitorDirectory(fc_obj->fc,
			     filename,
			     fr_obj->fr,
			     (void *)userData);

    if (!rc) {
      Py_INCREF(userData);

      return (PyObject*)fr_obj;
    } else {
      PyErr_SetString(PyExc_IOError, "_fam: unable to monitor directory");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_RuntimeError, "_fam: no connection to monitor");
    return NULL;
  }
};

static PyObject* _fam_monitor_file(PyObject* self, PyObject* args) {
  int rc;
  char *filename;
  PyObject *userData;
  
  _fam_connection_object *fc_obj;
  _fam_request_object *fr_obj;

  fc_obj = (_fam_connection_object *)self;

  if (fc_obj->fc) {

    if (!PyArg_ParseTuple(args, "sO", &filename, &userData))
      return NULL;

    fr_obj = PyObject_NEW(_fam_request_object, &_fam_request_type);
    if (!fr_obj)
      return NULL;

    fr_obj->fc_obj = fc_obj;
    Py_INCREF(fc_obj);
    fr_obj->fr = (FAMRequest *)malloc(sizeof(FAMRequest));

    if (!fr_obj->fr) {
      PyErr_SetString(PyExc_MemoryError, "_fam: unable to malloc for request");
      return NULL;
    }

    rc = FAMMonitorFile(fc_obj->fc, filename, fr_obj->fr, (void *)userData);

    if (!rc) {
      Py_INCREF(userData);

      return (PyObject*)fr_obj;
    } else {
      PyErr_SetString(PyExc_IOError, "_fam: unable to monitor file");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_RuntimeError, "_fam: no connection to monitor");
    return NULL;
  }
};

static PyObject* _fam_suspend_monitor(PyObject* self, PyObject* args) {
  int rc;
  _fam_request_object *fr_obj;

  fr_obj = (_fam_request_object *)self;

  if (fr_obj->fc_obj && fr_obj->fc_obj->fc && fr_obj->fr) {
    rc = FAMSuspendMonitor(fr_obj->fc_obj->fc, fr_obj->fr);

    if (rc) {
      PyErr_SetString(PyExc_RuntimeError, "_fam: unable to suspend monitor");
      return NULL;
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
};

static PyObject* _fam_resume_monitor(PyObject* self, PyObject* args) {
  int rc;
  _fam_request_object *fr_obj;

  fr_obj = (_fam_request_object *)self;

  if (fr_obj->fc_obj && fr_obj->fc_obj->fc && fr_obj->fr) {
    rc = FAMResumeMonitor(fr_obj->fc_obj->fc, fr_obj->fr);

    if (rc) {
      PyErr_SetString(PyExc_RuntimeError, "_fam: unable to resume monitor");
      return NULL;
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
};

static PyObject* _fam_cancel_monitor(PyObject* self, PyObject* args) {
  int rc;
  _fam_request_object *fr_obj;

  fr_obj = (_fam_request_object *)self;

  if (fr_obj->fc_obj && fr_obj->fc_obj->fc && fr_obj->fr) {
    rc = FAMCancelMonitor(fr_obj->fc_obj->fc, fr_obj->fr);

    free(fr_obj->fr);
    fr_obj->fr = NULL;
    Py_DECREF(fr_obj->fc_obj);
    fr_obj->fc_obj = NULL;
  } else {
#ifdef _DEBUG
    fprintf(stderr, "_fam: no monitor to cancel.\n");
#endif
  }
  Py_INCREF(Py_None);
  return Py_None;
};

static PyObject* _fam_next_event(PyObject* self, PyObject* args) {
  int rc;
  _fam_connection_object *fc_obj;
  _fam_event_object *fe_obj;
  FAMEvent fe;
  PyObject *reqid, *hostname, *filename, *userData, *code;

  fe.fc = NULL;
  fe.hostname = NULL;
  fe.userdata = NULL;

  fc_obj = (_fam_connection_object *)self;

  if (fc_obj->fc) {

    fe_obj = PyObject_NEW(_fam_event_object, &_fam_event_type);
    if (!fe_obj)
      return NULL;

#ifdef _DEBUG
    fprintf(stderr, "_fam: got fe_obj.\n");
#endif

    fe_obj->attr = NULL;

    fe_obj->fc_obj = fc_obj;
    Py_INCREF(fc_obj);

    rc = FAMNextEvent(fc_obj->fc, &fe);

    if (rc == 1) {
      if (PyObject_SetAttrString((PyObject*)fe_obj,
				 "connection",
				 (PyObject*)fc_obj) < 0)
	return NULL;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got next event from queue.\n");
#endif

      reqid = PyInt_FromLong((long)fe.fr.reqnum);
      if (PyObject_SetAttrString((PyObject*)fe_obj, "requestID", reqid) < 0)
	return NULL;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got request ID.\n");
#endif

      filename = PyString_FromString(fe.filename);
      if (PyObject_SetAttrString((PyObject*)fe_obj, "filename", filename) < 0)
	return NULL;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got filename %s.\n", fe.filename);
#endif

      userData = (PyObject*)fe.userdata;
      if (PyObject_SetAttrString((PyObject*)fe_obj, "userData", userData) < 0)
	return NULL;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got userdata.\n");
#endif

      code = PyInt_FromLong(fe.code);
      if (PyObject_SetAttrString((PyObject*)fe_obj, "code", code) < 0)
	return NULL;

      fe_obj->code = fe.code;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got code %d: '%s'.\n", fe.code, code2str(fe.code));
#endif

      if (! fe.hostname) {
#ifdef _DEBUG
	fprintf(stderr, "_fam: null pointer to hostname.\n");
#endif
	fe.hostname = (char*)malloc(255*sizeof(char));
	bzero(fe.hostname,255);
#ifdef _DEBUG
      } else {
	fprintf(stderr, "_fam: valid pointer to hostname %s.\n", fe.hostname);
#endif
      }
      
#ifdef _DEBUG
      fprintf(stderr, "_fam: casting hostname to PyString.\n");
#endif

      hostname = PyString_FromString(fe.hostname);

#ifdef _DEBUG
      fprintf(stderr, "_fam: casted hostname to PyString.\n");
#endif

      if (PyObject_SetAttrString((PyObject*)fe_obj, "hostname", hostname) < 0)
	return NULL;

#ifdef _DEBUG
      fprintf(stderr, "_fam: got hostname.\n");
#endif

      return (PyObject*)fe_obj;
    } else {
      PyErr_SetString(PyExc_IOError, "_fam: unable to get next event");
      return NULL;
    }
  } else {
    PyErr_SetString(PyExc_RuntimeError, "_fam: no connection for next event");
    return NULL;
  }
};

static PyObject* _fam_pending(PyObject* self, PyObject* args) {
  int rc;
  _fam_connection_object *fc_obj;

  fc_obj = (_fam_connection_object *)self;

  if (fc_obj->fc) {
    rc = FAMPending(fc_obj->fc);

    if (rc) {
      PyObject *pyval;

      if (pyval = PyInt_FromLong(1)) {
	return pyval;
      }
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
};

static PyObject* _fam_code2str(PyObject* self, PyObject* args) {
  _fam_event_object *fe_obj;
  PyObject *rv;
  char *str;

  fe_obj = (_fam_event_object *)self;

  str = code2str(fe_obj->code);

  if (str) {
    rv = PyString_FromString(str);
  } else {
    PyErr_SetString(PyExc_RuntimeError, "_fam: unknown event code");
    rv = NULL;
  }
  return rv;
}

static PyObject* _fam_fileno(PyObject* self, PyObject* args) {
  _fam_connection_object *fc_obj;
  PyObject *rv;

  fc_obj = (_fam_connection_object *)self;

  rv = PyInt_FromLong(FAMCONNECTION_GETFD(fc_obj->fc));

  if (!rv) {
    PyErr_SetString(PyExc_RuntimeError,
		    "_fam: unable to get file descriptor from connection");
  }
  return rv;
}

static PyObject* _fam_request_id(PyObject* self, PyObject* args) {
  _fam_request_object *fr_obj;
  PyObject *rv;

  fr_obj = (_fam_request_object *)self;

  rv = PyInt_FromLong(FAMREQUEST_GETREQNUM(fr_obj->fr));

  if (!rv) {
    PyErr_SetString(PyExc_RuntimeError, "_fam: unable to get ID from request");
  }
  return rv;
}

void _fam_cleanup() {
#ifdef _DEBUG
  fprintf(stderr, "cleaning up _fam.\n");
#endif
}

static PyMethodDef moduleMethods[] = {
  { "open", _fam_open, 1, _fam_open__doc__},
  { NULL, NULL, 0, NULL}
};

static char _fam__doc__[] = "File Alteration Monitor library routines";

void init_fam() {
  PyObject *m, *d;

  m = Py_InitModule3("_fam",
		     moduleMethods,
		     _fam__doc__);

  d = PyModule_GetDict(m);

  add_int(d, Changed);
  add_int(d, Deleted);
  add_int(d, StartExecuting);
  add_int(d, StopExecuting);
  add_int(d, Created);
  add_int(d, Moved);
  add_int(d, Acknowledge);
  add_int(d, Exists);
  add_int(d, EndExist);

#if defined(WIN32) || defined(__CYGWIN__)
  _fam_connection_type.ob_type = &PyType_Type;
  _fam_request_type.ob_type = &PyType_Type;
#endif

  if (Py_AtExit(_fam_cleanup)) {
    fprintf(stderr, "_fam: warining: cleanup procedure not registered.\n");
  }
};
