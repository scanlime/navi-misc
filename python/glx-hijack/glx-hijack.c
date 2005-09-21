#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <Python.h>

void glXSwapBuffers(void *display, void *drawable) {
  static void (*orig)(void*, void*) = NULL;
  static PyObject *module, *func, *result;

  if (!orig) {
    Py_Initialize();
    module = PyImport_ImportModule(getenv("PY_MODULE"));
    if (!module) {
      PyErr_Print();
      exit(1);
    }
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
  
