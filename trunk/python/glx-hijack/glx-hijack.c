#include <Python.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // RTLD_NEXT and other goodies
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

static PyObject *py_module, *py_frame, *py_viewport;
static PyObject *py_gl_enabled, *py_gl_textures;

/* All OpenGL state tracking gets disabled when we enter Python
 * code, so we can be sure we're only tracking the target app
 * and not ourselves.
 */
static int enable_state_tracker = 1;

/* Pointers to the original functions, for those we override here */
static void* (*dlsym_p)(void*, __const char*) = NULL;
static void* (*glXGetProcAddress_p)(char *) = NULL;
static void* (*glXGetProcAddressARB_p)(char *) = NULL;
static void (*glXSwapBuffers_p)(void*, void*) = NULL;
static void (*glViewport_p)(int, int, int, int) = NULL;
static void (*glEnable_p)(int) = NULL;
static void (*glDisable_p)(int) = NULL;
static void (*glBindTexture_p)(int, int) = NULL;

/* A macro to resolve one of the above original symbols at runtime */
#define RESOLVE(sym) \
  do { \
    if (!sym ## _p) \
      sym ## _p = dlsym_p(RTLD_NEXT, #sym); \
  } while (0);

/* Die gracelessly (but with a traceback) if any Python exception is caught */
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
  /* Absolutely the first thing we do must be retrieving
   * a pointer to the real dlsym(). This presents a chicken-and-egg
   * problem- we get around that by looking up dlsym() using
   * dlvsym() instead.
   */
  dlsym_p = dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.0");

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

  /* This is an analogous dictionary for texture bindings */
  py_gl_textures = PyDict_New();
  if (PyModule_AddObject(py_module, "gl_textures", py_gl_textures) == -1)
    handle_py_error();
}

/* Any time the target application uses dlsym(), glXGetProcAddress(),
 * or glXGetProcAddressARB() we want to be able to override the resulting
 * symbol with one of ours dynamically. Returns a non-NULL pointer to
 * override the address of the specified symbol.
 */
static void *check_dynamic_symbol(const char *name) {
  /* Don't even try to look up non-OpenGL symbols.
   * When we search our own symbol namespace below,
   * we're searching a lot of libraries other than this
   * one and OpenGL: this can really screw up an
   * app's plugin loader, for example.
   */
  if (name[0] != 'g' || name[1] != 'l')
    return NULL;

  return dlsym_p(RTLD_DEFAULT, name);
}

void glXSwapBuffers(void *display, void *drawable) {
  PyObject *result;

  if (enable_state_tracker) {
    enable_state_tracker = 0;
    result = PyObject_CallFunction(py_frame, NULL);
    enable_state_tracker = 1;
    if (!result) handle_py_error();
    Py_DECREF(result);
  }

  RESOLVE(glXSwapBuffers);
  glXSwapBuffers_p(display, drawable);
}

void glEnable(int cap) {
  PyObject *py_cap;

  RESOLVE(glEnable);
  glEnable_p(cap);

  if (enable_state_tracker) {
    py_cap = PyInt_FromLong(cap);
    PyDict_SetItem(py_gl_enabled, py_cap, Py_True);
    Py_DECREF(py_cap);
  }
}

void glDisable(int cap) {
  PyObject *py_cap;

  RESOLVE(glDisable);
  glDisable_p(cap);

  if (enable_state_tracker) {
    py_cap = PyInt_FromLong(cap);
    PyDict_SetItem(py_gl_enabled, py_cap, Py_False);
    Py_DECREF(py_cap);
  }
}

void glViewport(int x, int y, int width, int height) {
  PyObject *result;

  RESOLVE(glViewport);
  glViewport_p(x, y, width, height);

  if (enable_state_tracker) {
    enable_state_tracker = 0;
    result = PyObject_CallFunction(py_viewport, "iiii", x, y, width, height);
    enable_state_tracker = 1;
    if (!result) handle_py_error();
    Py_DECREF(result);
  }
}

void glBindTexture(int target, int texture) {
  PyObject *py_target, *py_texture;

  RESOLVE(glBindTexture);
  glBindTexture_p(target, texture);

  if (enable_state_tracker) {
    py_target = PyInt_FromLong(target);
    py_texture = PyInt_FromLong(texture);
    PyDict_SetItem(py_gl_textures, py_target, py_texture);
    Py_DECREF(py_target);
    Py_DECREF(py_texture);
  }
}

void glBindTextureEXT(int target, int texture) {
  glBindTexture(target, texture);
}

void *dlsym (void *__restrict handle, __const char *__restrict name) {
  void *override = check_dynamic_symbol(name);
  if (override)
    return override;
  return dlsym_p(handle, name);
}

void *glXGetProcAddress(char *name) {
  void *override = check_dynamic_symbol(name);
  if (override)
    return override;
  RESOLVE(glXGetProcAddress);
  return glXGetProcAddress_p(name);
}

void *glXGetProcAddressARB(char *name) {
  void *override = check_dynamic_symbol(name);
  if (override)
    return override;
  RESOLVE(glXGetProcAddressARB);
  return glXGetProcAddressARB_p(name);
}
