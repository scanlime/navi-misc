/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*- */ static char* _doc =
"                                                                            \n"
" Loopy - the Linker-assisted OpenGL Overlay for Python                      \n"
" Copyright (C) 2005 Micah Dowty <micah@navi.cx>                             \n"
"                                                                            \n"
" Loopy is a way to embed Python code into most unmodified OpenGL programs,  \n"
" take control periodically, and draw your own graphics on top of the        \n"
" original output of the target program. Loopy does not provide OpenGL       \n"
" bindings of its own, but it is compatible with other Python modules that   \n"
" do, such as PyOpenGL or Soya 3D.                                           \n"
"                                                                            \n"
" Loopy has two main jobs: it must run arbitrary Python code in the address  \n"
" space of an unmodified application, and it must track enough of the OpenGL \n"
" state to prevent the overlay program from interfering with the target      \n"
" program or vice versa. It uses dynamic linker tricks to accomplish both    \n"
" of these goals. For this reason, it is only compatible with systems that   \n"
" run glibc2.                                                                \n"
"                                                                            \n"
" Loopy may be used as a normal Python module: 'import loopy'. This is       \n"
" handy for inspecting the built-in documentation with help() or pydoc, and  \n"
" it can be convenient for application development. To really get any work   \n"
" done, however, Loopy needs to be loaded inside the target application.     \n"
" From there Loopy starts a normal Python interpreter, registers the 'loopy' \n"
" module as a builtin, then runs your code. This is all set up using two     \n"
" environment variables. For example:                                        \n"
"                                                                            \n"
"     LD_PRELOAD=./loopy.so LOOPY_MAIN=hello.py glxgears                     \n"
"                                                                            \n"
" LD_PRELOAD instructs the dynamic linker to load Loopy itself, then Loopy   \n"
" runs your 'hello' Python program. Once 'hello' has finished executing, the \n"
" 'glxgears' target program starts running.                                  \n"
"                                                                            \n"
" It may seem like hello.py and glxgears would never be running at the same  \n"
" time. This is true- however hello.py can import the 'loopy' module and use \n"
" it to set up objects which later on affect the execution of glxgears and   \n"
" even re-enter Python code.                                                 \n"
"                                                                            \n"
" See the included examples and the rest of the built-in documentation for   \n"
" more information.                                                          \n"
"                                                                            \n"
"  This library is free software; you can redistribute it and/or             \n"
"  modify it under the terms of the GNU Lesser General Public                \n"
"  License as published by the Free Software Foundation; either              \n"
"  version 2.1 of the License, or (at your option) any later version.        \n"
"                                                                            \n"
"  This library is distributed in the hope that it will be useful,           \n"
"  but WITHOUT ANY WARRANTY; without even the implied warranty of            \n"
"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         \n"
"  Lesser General Public License for more details.                           \n"
"                                                                            \n"
"  You should have received a copy of the GNU Lesser General Public          \n"
"  License along with this library; if not, write to the Free Software       \n"
"  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA \n"
"                                                                            \n";

#include <Python.h>
#include <structmember.h>  /* PyMemberDef and friends */

#ifndef _GNU_SOURCE        /* Python.h might be setting this already   */
#define _GNU_SOURCE        /* RTLD_NEXT and other goodies              */
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

/* Python hook functions, NULL if disabled */
static PyObject *pyhook_frame = NULL;
static PyObject *pyhook_viewport = NULL;

/* All OpenGL state tracking gets disabled when we enter Python
 * code, so we can be sure we're only tracking the target app
 * and not ourselves.
 */
static int enable_state_tracker = 1;


/************************************************************************/
/********************************************* OpenGL State Tracker *****/
/************************************************************************/

#define GLSTATE_ENABLE        (1<<0)
#define GLSTATE_DISABLE       (1<<1)
#define GLSTATE_BIND_TEXTURE  (1<<2)
#define GLSTATE_COLOR         (1<<3)

#define GLSTATE_CAPABILITIES  (GLSTATE_ENABLE | GLSTATE_DISABLE)
#define GLSTATE_ALL           (GLSTATE_CAPABILITIES | \
                               GLSTATE_BIND_TEXTURE | \
                               GLSTATE_COLOR)

typedef struct {
    PyObject_HEAD
    unsigned int trackingFlags;
    unsigned int restoreFlags;
    PyObject *capabilities;
    PyObject *textureBindings;
} GLState;

static PyMemberDef glstate_members[] = {
    { "trackingFlags", T_UINT, offsetof(GLState, trackingFlags), 0,
      "A set of flags indicating which aspects of the OpenGL\n"
      "state get tracked while this GLState is current.\n"
    },
    { "restoreFlags", T_UINT, offsetof(GLState, restoreFlags), 0,
      "A set of flags indicating which aspects of the OpenGL\n"
      "state are restored when this GLState becomes current.\n"
    },
    { "capabilities", T_OBJECT, offsetof(GLState, capabilities), READONLY,
      "A dictionary mapping OpenGL capability numbers to either\n"
      "True (glEnable) or False (glDisable). Missing capabilities\n"
      "haven't been touched by either glEnable or glDisable.\n"
    },
    { "textureBindings", T_OBJECT, offsetof(GLState, textureBindings), READONLY,
      "A dictionary mapping targets (like GL_TEXTURE_2D) to\n"
      "texture IDs. Missing targets haven't been bound to a texture\n"
      "ID. This will track traditional 1D/2D/3D targets, plus new\n"
      "targets defined by OpenGL extensions.\n"
    },
    {0}
};

static int glstate_init(GLState *self, PyObject *args, PyObject *kw) {
    if (!PyArg_ParseTuple(args, ""))
        return -1;

    self->trackingFlags = GLSTATE_ALL;
    self->restoreFlags = GLSTATE_ALL;

    self->capabilities = PyDict_New();
    self->textureBindings = PyDict_New();
    return 0;
}

static void glstate_dealloc(GLState *self) {
    Py_XDECREF(self->capabilities);
    Py_XDECREF(self->textureBindings);

    self->ob_type->tp_free((PyObject *)self);
}

static PyTypeObject glstate_type = {
    PyObject_HEAD_INIT(0)
    .tp_name = "loopy.GLState",
    .tp_basicsize = sizeof(GLState),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = glstate_members,
    .tp_init = (initproc) glstate_init,
    .tp_dealloc = (destructor) glstate_dealloc,
    .tp_doc = "GLState()\n"
              "\n"
              "GLState instances represent a subset of the state\n"
              "stored in the current OpenGL context. When a GLState\n"
              "object is made current by the Loopy backend, it will\n"
              "be automatically updated to reflect changes to the real\n"
              "OpenGL state.\n"
              "\n"
              "The primary purpose of a GLState object is to provide a\n"
              "way to perform 'context switches' between the OpenGL\n"
              "environments expected by different applications. This\n"
              "is done by Loopy automatically behind the scenes.\n"
              "\n"
              "The Python interface to GLState can be used to control\n"
              "which parts of the OpenGL state get saved and restored,\n"
              "and to access or modify this saved state.\n"
};

static void glstate_add_const(const char *name, int value) {
    PyObject *py_value = PyInt_FromLong(value);
    PyDict_SetItemString(glstate_type.tp_dict, name, py_value);
    Py_DECREF(py_value);
}

static void glstate_type_init(PyObject *module) {
    Py_INCREF(&glstate_type);

    glstate_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&glstate_type) < 0)
        return;

    glstate_add_const("ENABLE",        GLSTATE_ENABLE);
    glstate_add_const("DISABLE",       GLSTATE_DISABLE);
    glstate_add_const("BIND_TEXTURE",  GLSTATE_BIND_TEXTURE);
    glstate_add_const("COLOR",         GLSTATE_COLOR);
    glstate_add_const("CAPABILITIES",  GLSTATE_CAPABILITIES);
    glstate_add_const("ALL",           GLSTATE_ALL);

    PyModule_AddObject(module, "GLState", (PyObject *) &glstate_type);
}

/************************************************************************/
/**************************************************** Python Module *****/
/************************************************************************/

static PyMethodDef LoopyMethods[] = {
    {NULL, NULL, 0, NULL}
};

/*
 * Initialize the 'loopy' extension module. This is normally called
 * internally for the Python interpreter we embed in the target app,
 * but this library can also be used as a normal extension module.
 */
PyMODINIT_FUNC initloopy(void)
{
    PyObject *m = Py_InitModule3("loopy", LoopyMethods, _doc);
    glstate_type_init(m);
}

/************************************************************************/
/********************************************** Dynamic Linker Mojo *****/
/************************************************************************/

/* Pointers to dynamically-resolved original functions corresponding
 * to the functions we override in this library.
 */
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

    if (Py_IsInitialized())
        return;

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

    /* LOOPY_MAIN specifies the name of a file to run code from.
     * It should nearly always be specified, but technically it's
     * optional.
     */
    main_filename = getenv("LOOPY_MAIN");
    if (main_filename) {
        FILE *main_file = fopen(main_filename, "r");
        if (!main_file) {
            perror(main_filename);
            exit(1);
        }

        /* Run the user program until it finishes */
        if (PyRun_SimpleFile(main_file, main_filename) < 0)
            handle_py_error();

        fclose(main_file);
    }
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

    /*
    if (enable_state_tracker) {
        py_cap = PyInt_FromLong(cap);
        PyDict_SetItem(glstate_enabled, py_cap, Py_True);
        Py_DECREF(py_cap);
    }
    */
}

void glDisable(int cap) {
    PyObject *py_cap;

    RESOLVE(glDisable);
    glDisable_p(cap);

    /*
    if (enable_state_tracker) {
        py_cap = PyInt_FromLong(cap);
        PyDict_SetItem(glstate_enabled, py_cap, Py_False);
        Py_DECREF(py_cap);
    }
    */
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

    /*
    if (enable_state_tracker) {
        py_target = PyInt_FromLong(target);
        py_texture = PyInt_FromLong(texture);
        PyDict_SetItem(glstate_textures, py_target, py_texture);
        Py_DECREF(py_target);
        Py_DECREF(py_texture);
    }
    */
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
