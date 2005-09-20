#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <Python.h>

void glXSwapBuffers(void *display, void *drawable) {
  static void (*orig)(void*, void*) = NULL;
  static void *module;
  static void *func;

  if (!orig) {
    Py_Initialize();
    module = PyImport_ImportModule(getenv("PY_MODULE"));
    printf("module: %p\n", module);
    func = PyObject_GetAttrString(module, "frame");
    printf("func: %p\n", func);
    orig = dlsym(RTLD_NEXT, "glXSwapBuffers");
  }

  Py_DECREF(PyObject_CallFunction(func, NULL));
  orig(display, drawable);
}
  
