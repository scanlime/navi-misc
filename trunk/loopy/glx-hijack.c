/*
 * loopy - the Linker-assisted OpenGL Overlay for Python
 * Copyright (C) 2005 Micah Dowty <micah@navi.cx>
 *
 * This compiles to a shared object which can be loaded into
 * arbitrary OpenGL applications using LD_PRELOAD. It then
 * embeds a Python interpreter into the target process' address
 * space. The PYTHONPATH and LOOPY_MODULE environment variables
 * are used to locate a Python module containing the new code
 * to inject.
 *
 * This Python module will be able to import a "loopy" module
 * containing an interface for our OpenGL hooks and state tracking.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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

/* A macro to resolve dlsym() itself at runtime. To avoid the
 * chicken-and-egg problem, this reads dlsym() as a versioned symbol
 * rather than a regular one.
 */
#define RESOLVE_DLSYM() \
  do { \
    if (!dlsym_p) \
      dlsym_p = dlvsym(RTLD_NEXT, "dlsym", "GLIBC_2.0"); \
  } while (0);

/* A macro to resolve one of the above original symbols at runtime */
#define RESOLVE(sym) \
  do { \
    if (!sym ## _p) { \
      RESOLVE_DLSYM(); \
      sym ## _p = dlsym_p(RTLD_NEXT, #sym); \
    } \
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

  RESOLVE_DLSYM();
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
  RESOLVE_DLSYM();
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

void *SDL_GetVideoInfo() {
  /* This is part of a hack to initialize soya without SDL and without patching it.
   * We ensure that soya's set_video() fails just after it stores a new resolution.
   */
  return NULL;
}
