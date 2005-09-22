#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <Python.h>

static PyObject *module = NULL;

void init() {
  Py_Initialize ();
  module = PyImport_ImportModule(getenv("PY_MODULE"));
  if (!module) {
    PyErr_Print();
    exit(1);
  }
}

void glXSwapBuffers(void *display, void *drawable) {
  static void (*orig)(void*, void*) = NULL;
  static PyObject *func, *result;

  if (!orig) {
    if (!module)
      init ();
    func = PyObject_GetAttrString(module, "frame");
    if (!func) {
      PyErr_Print();
      exit(1);
    }

    orig = dlsym(RTLD_NEXT, "glXSwapBuffers");
  }

  result = PyObject_CallFunction(func, NULL);
  if (!result) {
    PyErr_Print();
    exit(1);
  }
  Py_DECREF(result);

  orig(display, drawable);
}

void glViewport(int x, int y, int width, int height) {
  static void (*orig)(int, int, int, int) = NULL;
  static PyObject *func, *result;

  if (!orig) {
    if (!module)
      init ();
    func = PyObject_GetAttrString(module, "viewport");
    if (!func) {
      PyErr_Print();
      exit(1);
    }
    orig = dlsym(RTLD_NEXT, "glViewport");
  }

  orig(x, y, width, height);

  result = PyObject_CallFunction(func, "iiii", x, y, width, height);
  if (!result) {
    PyErr_Print();
    exit(1);
  }
  Py_DECREF(result);
}
