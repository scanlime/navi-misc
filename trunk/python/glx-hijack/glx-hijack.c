#include <Python.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // RTLD_NEXT and other goodies
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static PyObject *py_module, *py_frame, *py_viewport;
static void (*glXSwapBuffers_p)(void*, void*) = NULL;
static void (*glViewport_p)(int, int, int, int) = NULL;

static void handle_py_error() {
  PyErr_Print();
  exit(1);
}

static void __attribute__ ((constructor)) init() {
  Py_Initialize ();

  py_module = PyImport_ImportModule(getenv("PY_MODULE"));
  if (!py_module) handle_py_error();

  py_frame = PyObject_GetAttrString(py_module, "frame");
  if (!py_frame) handle_py_error();

  py_viewport = PyObject_GetAttrString(py_module, "viewport");
  if (!py_viewport) handle_py_error();

  glXSwapBuffers_p = dlsym(RTLD_NEXT, "glXSwapBuffers");
  glViewport_p = dlsym(RTLD_NEXT, "glViewport");
}

void glXSwapBuffers(void *display, void *drawable) {
  PyObject *result = PyObject_CallFunction(py_frame, NULL);
  if (!result) handle_py_error();
  Py_DECREF(result);

  glXSwapBuffers_p(display, drawable);
}

void glViewport(int x, int y, int width, int height) {
  PyObject *result;

  glViewport_p(x, y, width, height);

  result = PyObject_CallFunction(py_viewport, "iiii", x, y, width, height);
  if (!result) handle_py_error();
  Py_DECREF(result);
}
