/*
 * Python FAM, 2002 Robert Sander <robert.sander@epigenomics.com>
 *
 * released under LGPL
 *
 * USE AT YOUR OWN RISK
 *
 * $Id: _fam.h,v 1.6 2002/09/21 15:50:42 gurubert Exp $
 *
 */

#include <Python.h>
#include <fam.h>

static char _fam_connection_type__doc__[] = "FAMConnection";
static PyTypeObject _fam_connection_type;

static char _fam_request_type__doc__[] = "FAMRequest";
static PyTypeObject _fam_request_type;

static char _fam_event_type__doc__[] = "FAMEvent";
static PyTypeObject _fam_event_type;

static char _fam_open__doc__[] = "opens a new connection to the fam daemon, returning a FAMConnection object";
static PyObject* _fam_open(PyObject* self, PyObject* args);

static char _fam_close__doc__[] = "closes the connection to the fam daemon";
static PyObject* _fam_close(PyObject* self, PyObject* args);

static char _fam_monitor_directory__doc__[] = "monitors a directory, returning a FAMRequest object\nfirst argument is the full directory name, second an arbitrary object for later use";
static PyObject* _fam_monitor_directory(PyObject* self, PyObject* args);

static char _fam_monitor_file__doc__[] = "monitors a file, returning a FAMRequest object\nfirst argument is the full filename, second an arbitrary object for later use";
static PyObject* _fam_monitor_file(PyObject* self, PyObject* args);

static char _fam_suspend_monitor__doc__[] = "suspends the request";
static PyObject* _fam_suspend_monitor(PyObject* self, PyObject* args);

static char _fam_resume_monitor__doc__[] = "resumes the requests";
static PyObject* _fam_resume_monitor(PyObject* self, PyObject* args);

static char _fam_cancel_monitor__doc__[] = "cancels the request";
static PyObject* _fam_cancel_monitor(PyObject* self, PyObject* args);

static char _fam_next_event__doc__[] = "returns the next FAMEvent object from the queue";
static PyObject* _fam_next_event(PyObject* self, PyObject* args);

static char _fam_pending__doc__[] = "returns 1 if an event is pending, None if not";
static PyObject* _fam_pending(PyObject* self, PyObject* args);

static char _fam_code2str__doc__[] = "converts event code to meaningful string";
static PyObject* _fam_code2str(PyObject* self, PyObject* args);

static char _fam_fileno__doc__[] = "returns the file descriptor of the connection";
static PyObject* _fam_fileno(PyObject* self, PyObject* args);

static char _fam_request_id__doc__[] = "returns the request id";
static PyObject* _fam_request_id(PyObject* self, PyObject* args);
