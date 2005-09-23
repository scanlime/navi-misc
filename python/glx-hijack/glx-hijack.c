#include <Python.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // RTLD_NEXT and other goodies
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static PyObject *py_module, *py_frame, *py_viewport, *py_gl_enabled;
static void (*glXSwapBuffers_p)(void*, void*) = NULL;
static void (*glViewport_p)(int, int, int, int) = NULL;
static void (*glEnable_p)(int) = NULL;
static void (*glDisable_p)(int) = NULL;

static void handle_py_error() {
  PyErr_Print();
  exit(1);
}

/* Load the Python module ASAP, so we're in as stable
 * a state as possible when we do so. This also catches
 * common Python problems before we bother letting the
 * original app initialize.
 *
 * Unfortunately we still have to do symbol resolution
 * lazily, since libGL might not be loaded at this point.
 */
static void __attribute__ ((constructor)) init() {
  Py_Initialize ();

  py_module = PyImport_ImportModule(getenv("PY_MODULE"));
  if (!py_module) handle_py_error();

  py_frame = PyObject_GetAttrString(py_module, "frame");
  if (!py_frame) handle_py_error();

  py_viewport = PyObject_GetAttrString(py_module, "viewport");
  if (!py_viewport) handle_py_error();

  /* We keep a dictionary that tracks OpenGL capabilities.
   * glEnable() sets a capability to True, glDisable() sets it to
   * False. The Python code can use this to clean up the OpenGL
   * state before rendering, then safely put back anything it changed.
   */
  py_gl_enabled = PyDict_New();
  if (PyModule_AddObject(py_module, "gl_enabled", py_gl_enabled) == -1)
    handle_py_error();
}

void glXSwapBuffers(void *display, void *drawable) {
  PyObject *result = PyObject_CallFunction(py_frame, NULL);
  if (!result) handle_py_error();
  Py_DECREF(result);

  if (!glXSwapBuffers_p)
    glXSwapBuffers_p = dlsym(RTLD_NEXT, "glXSwapBuffers");
  glXSwapBuffers_p(display, drawable);
}

void glEnable(int cap) {
  PyObject *py_cap;

  if (!glEnable_p)
    glEnable_p = dlsym(RTLD_NEXT, "glEnable");
  glEnable_p(cap);

  py_cap = PyInt_FromLong(cap);
  PyDict_SetItem(py_gl_enabled, py_cap, Py_True);
  Py_DECREF(py_cap);
}

void glDisable(int cap) {
  PyObject *py_cap;

  if (!glDisable_p)
    glDisable_p = dlsym(RTLD_NEXT, "glDisable");
  glDisable_p(cap);

  py_cap = PyInt_FromLong(cap);
  PyDict_SetItem(py_gl_enabled, py_cap, Py_False);
  Py_DECREF(py_cap);
}

void glViewport(int x, int y, int width, int height) {
  PyObject *result;

  if (!glViewport_p)
    glViewport_p = dlsym(RTLD_NEXT, "glViewport");
  glViewport_p(x, y, width, height);

  result = PyObject_CallFunction(py_viewport, "iiii", x, y, width, height);
  if (!result) handle_py_error();
  Py_DECREF(result);
}
