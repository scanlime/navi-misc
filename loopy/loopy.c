/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * loopy - the Linker-assisted OpenGL Overlay for Python
 * Copyright (C) 2005 Micah Dowty <micah@navi.cx>
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

/* Our 'loopy' extension module */
static PyObject *loopy_module;

/* Python hook functions, NULL if disabled */
static PyObject *pyhook_frame = NULL;
static PyObject *pyhook_viewport = NULL;

/* Python dictionaries tracking OpenGL state */
static PyObject *glstate_enabled;
static PyObject *glstate_textures;

/* All OpenGL state tracking gets disabled when we enter Python
 * code, so we can be sure we're only tracking the target app
 * and not ourselves.
 */
static int enable_state_tracker = 1;

static void *check_dynamic_symbol(const char *name);
PyMODINIT_FUNC initloopy(void);

/* Pointers to the original functions, for those we override here */
static void* (*dlsym_p)(void*, __const char*) = NULL;
static void* (*glXGetProcAddress_p)(char *) = NULL;
static void* (*glXGetProcAddressARB_p)(char *) = NULL;
static void (*glXSwapBuffers_p)(void*, void*) = NULL;
static void (*glViewport_p)(int, int, int, int) = NULL;
static void (*glEnable_p)(int) = NULL;
static void (*glDisable_p)(int) = NULL;
static void (*glBindTexture_p)(int, int) = NULL;

/************************************************************************/
/********************************************** Dynamic Linker Mojo *****/
/************************************************************************/

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

/* 
 * Handle python errors we can't pass back to the user module.
 * This includes errors importing the user module, and errors
 * in running hook functions.
 */
static void handle_py_error() {
    PyErr_Print();
    exit(1);
}

/*
 * This initialization function is called by the dynamic
 * linker- if we aren't already running inside python,
 * this sets up the interpreter and our extension module.
 */
static void __attribute__ ((constructor)) init() {
    char *main_filename;
    FILE *main_file;

    if (Py_IsInitialized())
	return;

    main_filename = getenv("LOOPY_MAIN");
    if (!main_filename) {
	fprintf(stderr,
		"You must set the LOOPY_MAIN environment variable\n"
		"to the name of a Python file to run within the target\n"
		"process.\n");
	exit(1);
    }
  
    main_file = fopen(main_filename, "r");
    if (!main_file) {
	perror(main_filename);
	exit(1);
    }

    /* In Python 2.4 or later, we can use Py_InitializeEx() to
     * skip installing signal handlers- this lets us ctrl-C
     * the target app cleanly. If we don't have Python 2.4,
     * no big deal.
     */
#if (PY_VERSION_HEX >= 0x02040000)
    Py_InitializeEx(0);
#else
    Py_Initialize();
#endif

    /* Create the extension module */
    initloopy();

    /* Run the user module. It should exit after setting up hook functions
     * we'll call later as the target application runs.
     */
    if (PyRun_SimpleFile(main_file, main_filename) < 0)
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

/************************************************************************/
/******************************************* Python Built-in Module *****/
/************************************************************************/

typedef struct {
    PyObject_HEAD
} GLState;

static PyTypeObject GLStateType = {
    PyObject_HEAD_INIT(0)
    .tp_name = "loopy.GLState",
};

static PyMethodDef LoopyMethods[] = {
    {NULL, NULL, 0, NULL}
};

/*
 * Initialize the 'loopy' extension module. This is normally called
 * internally for the Python interpreter we embed in the target app,
 * but this library can also be used as a normal extension module for
 * testing purposes.
 */
PyMODINIT_FUNC initloopy(void)
{
    loopy_module = Py_InitModule("loopy", LoopyMethods);

    /* We keep a dictionary that tracks OpenGL capabilities.
     * glEnable() sets a capability to True, glDisable() sets it to
     * False. The Python code can use this to clean up the OpenGL
     * state before rendering, then safely put back anything it changed.
     */
    glstate_enabled = PyDict_New();
    PyModule_AddObject(loopy_module, "_glstate_enabled", glstate_enabled);

    /* This is an analogous dictionary for texture bindings */
    glstate_textures = PyDict_New();
    PyModule_AddObject(loopy_module, "_glstate_textures", glstate_textures);
}

/************************************************************************/
/*********************************************** Override Functions *****/
/************************************************************************/

void glXSwapBuffers(void *display, void *drawable) {
    PyObject *result;

    if (enable_state_tracker && pyhook_frame) {
	enable_state_tracker = 0;
	result = PyObject_CallFunction(pyhook_frame, NULL);
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
	PyDict_SetItem(glstate_enabled, py_cap, Py_True);
	Py_DECREF(py_cap);
    }
}

void glDisable(int cap) {
    PyObject *py_cap;

    RESOLVE(glDisable);
    glDisable_p(cap);

    if (enable_state_tracker) {
	py_cap = PyInt_FromLong(cap);
	PyDict_SetItem(glstate_enabled, py_cap, Py_False);
	Py_DECREF(py_cap);
    }
}

void glViewport(int x, int y, int width, int height) {
    PyObject *result;

    RESOLVE(glViewport);
    glViewport_p(x, y, width, height);

    if (enable_state_tracker && pyhook_viewport) {
	enable_state_tracker = 0;
	result = PyObject_CallFunction(pyhook_viewport, "iiii",
				       x, y, width, height);
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
	PyDict_SetItem(glstate_textures, py_target, py_texture);
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
    /* This is part of a hack to initialize Soya 3D without SDL and without
     * patching it. We ensure that soya's set_video() fails just after it
     * stores a new resolution.
     */
    return NULL;
}

/* The End */
