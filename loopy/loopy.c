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

/* This is a hack to prevent glx.h and glxext.h from defining
 * glXGetProcAddress as a function pointer, which would prevent
 * us from using that same symbol name for a regular function below.
 */
#define GLX_ARB_get_proc_address 1
typedef void * __GLXextFuncPtr;

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <SDL/SDL.h>

/* GLState is a Python object providing a user-visible
 * interface to saved OpenGL state. We always have at
 * most one 'current' GLState that the OpenGL tracker
 * runs on.
 */
typedef struct {
    PyObject_HEAD
    unsigned int trackingFlags;
    unsigned int restoreFlags;
    unsigned int clearMask;

    PyObject *capabilities;
    PyObject *textureBindings;
    PyObject *textures;
    PyObject *color;
    PyObject *blendFunc;
    PyObject *clearColor;
    int depthFunc;
    int matrixMode;
    PyObject *matrices;
    PyObject *pushedMatrices;
} GLState;

/* Unlike GLState, GLXState is not visible to Python.
 * There's not (yet) anything in it that's interesting
 * outside of C. We track the GLX state in order to
 * provide extensions like RenderTarget (implemented
 * using pbuffers)
 */
typedef struct {
    Bool is_valid;
    Display *dpy;
    GLXDrawable drawable;
    GLXContext ctx;
} GLXState;

/*
 * Constants for GLState trackingFlags and restoreFlags.
 * These are defined for C and for Python. This table
 * is loaded into a dictionary during initialization.
 */
static const struct {
    const char *name;
    int value;
} glstate_enums[] = {

#   define GLSTATE_CAPABILITIES       (1  << 0 )
#   define GLSTATE_MATRICES           (1  << 1 )
#   define GLSTATE_TEXTURES           (1  << 2 )
#   define GLSTATE_COLOR              (1  << 3 )
#   define GLSTATE_BLEND_FUNC         (1  << 4 )
#   define GLSTATE_CLEAR_COLOR        (1  << 5 )
#   define GLSTATE_DEPTH_FUNC         (1  << 6 )
#   define GLSTATE_ALL                ((1 << 7 )-1)

    {"CAPABILITIES",     GLSTATE_CAPABILITIES},
    {"MATRICES",         GLSTATE_MATRICES},
    {"TEXTURES",         GLSTATE_TEXTURES},
    {"COLOR",            GLSTATE_COLOR},
    {"BLEND_FUNC",       GLSTATE_BLEND_FUNC},
    {"CLEAR_COLOR",      GLSTATE_CLEAR_COLOR},
    {"DEPTH_FUNC",       GLSTATE_DEPTH_FUNC},
    {"ALL",              GLSTATE_ALL},
    {0}
};

/* targetGLState tracks the state for our target application. It
 * is not created until just before control is transferred back
 * to the target app.
 */
static GLState *target_glstate;

/* currentGLState is the state associated with currently executing
 * code. While we're running the target app this should be another
 * reference to targetGLState. While we're running an overlay, it's
 * a reference to that overlay's GLState. Before the target app
 * begins or while Loopy is running internal OpenGL code, this must
 * be NULL.
 */
static GLState *current_glstate;

/* There's only one GLX state. We never modify the GLX context,
 * but we need this information to interact with GLX extensions.
 */
static GLXState current_glxstate;

#define RUNNING_IN_TARGET    (current_glstate == target_glstate)
#define IS_TRACKING(flag)    (current_glstate && \
                             (current_glstate->trackingFlags & (flag)))

PyMODINIT_FUNC initloopy(void);
static GLState* glstate_new(void);

/* Procedures we pull in dynamically using RESOLVE(). Most of
 * these are functions we override locally, so we have to look
 * up the original unmodified version ourselves.
 */
static void* (*dlsym_p)(void*, __const char*);
static void  (*glViewport_p)(GLint, GLint, GLsizei, GLsizei);
static void  (*glClear_p)(GLbitfield);
static void  (*glEnable_p)(GLenum);
static void  (*glDisable_p)(GLenum);
static void  (*glMatrixMode_p)(GLenum);
static void  (*glBindTexture_p)(GLenum, GLuint);
static void  (*glGenTextures_p)(GLsizei, GLuint*);
static void  (*glDeleteTextures_p)(GLsizei, const GLuint*);
static GLboolean (*glIsTexture_p)(GLuint);
static void  (*glTexParameterf_p)(GLenum, GLenum, GLfloat);
static void  (*glTexParameteri_p)(GLenum, GLenum, GLint);
static void  (*glGetDoublev_p)(GLenum, GLdouble*);
static void  (*glLoadMatrixd_p)(const GLdouble*);
static void  (*glPushMatrix_p)(void);
static void  (*glPopMatrix_p)(void);
static void  (*glLoadIdentity_p)(void);
static GLenum(*glGetError_p)(void);
static void  (*glColor4dv_p)(GLdouble*);
static void  (*glBlendFunc_p)(GLenum, GLenum);
static void  (*glClearColor_p)(GLclampf, GLclampf, GLclampf, GLclampf);
static void  (*glDepthFunc_p)(GLenum);

static Bool  (*glXMakeCurrent_p)(Display*, GLXDrawable, GLXContext);
static void* (*glXGetProcAddress_p)(const GLubyte *);
static void* (*glXGetProcAddressARB_p)(const GLubyte *);
static void  (*glXSwapBuffers_p)(Display*, GLXDrawable);

static int   (*SDL_Init_p)(Uint32);
static int   (*SDL_InitSubSystem_p)(Uint32);
static void  (*SDL_QuitSubSystem_p)(Uint32);
static void  (*SDL_Quit_p)(void);
static void  (*SDL_PumpEvents_p)(void);
static void  (*SDL_WM_SetCaption_p)(const char *, const char *);
static void  (*SDL_WM_GetCaption_p)(char **, char **);
static void  (*SDL_WM_SetIcon_p)(SDL_Surface*, Uint8*);
static void* (*SDL_GL_GetProcAddress_p)(const char*);
static int   (*SDL_GL_SetAttribute_p)(SDL_GLattr, int);
static int   (*SDL_GL_GetAttribute_p)(SDL_GLattr, int*);
static void  (*SDL_GL_SwapBuffers_p)(void);
static Uint32 (*SDL_WasInit_p)(Uint32);
static SDL_Surface* (*SDL_GetVideoSurface_p)(void);
static const SDL_VideoInfo* (*SDL_GetVideoInfo_p)(void);
static SDL_Surface* (*SDL_SetVideoMode_p)(int, int, int, Uint32);

/* Fake SDL data structures for the overlay to use.
 * We support just a smidgen of SDL so that existing
 * 3D rendering frameworks based on SDL or pygame
 * can be used without modification.
 */
static SDL_PixelFormat overlay_sdl_format = {};
static SDL_VideoInfo overlay_sdl_info = {
    .vfmt = &overlay_sdl_format,
};
static SDL_Surface overlay_sdl_surface = {
    .flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL | SDL_RESIZABLE,
    .format = &overlay_sdl_format,
    /* width/height filled in later */
};

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
        if (!dlsym_p) \
            handle_link_error("dlsym"); \
    } while (0)

/* A macro to resolve one of the above original symbols at runtime.
 * This can fail, leaving the symbol NULL.
 */
#define RESOLVE_SOFT(sym) \
    do { \
        if (!sym ## _p) { \
            RESOLVE_DLSYM(); \
            sym ## _p = dlsym_p(RTLD_NEXT, #sym); \
        } \
    } while (0)

/* Resolve a symbol at runtime, cause a fatal error if this fails */
#define RESOLVE(sym) \
    do { \
        RESOLVE_SOFT(sym); \
        if (!sym ## _p) \
            handle_link_error(#sym); \
    } while (0)

/*
 * Handle python errors we can't pass back to the user module.
 * This includes errors importing the user module, and errors
 * in running hook functions.
 */
static void handle_py_error(void) {
    PyErr_Print();
    exit(1);
}

/*
 * Handle fatal runtime symbol resolution errors
 */
static void handle_link_error(const char *name) {
    fprintf(stderr,
            "Loopy runtime link error: "
            "Can't resolve symbol '%s'\n", name);
    exit(1);
}

/*
 * This initialization function is called by the dynamic
 * linker- if we aren't already running inside python,
 * this sets up the interpreter and our extension module.
 */
static void __attribute__ ((constructor)) init(void) {
    char *var;

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

    /* LOOPY_EXEC is a less common environment variable that can
     * be used as an alternative to LOOPY_MAIN- it gives a Python
     * string to run immediately.
     */
    var = getenv("LOOPY_EXEC");
    if (var) {
        if (PyRun_SimpleString(var) < 0)
            handle_py_error();
    }

    /* LOOPY_MAIN specifies the name of a file to run code from.
     * It should nearly always be specified, but technically it's
     * optional.
     */
    var = getenv("LOOPY_MAIN");
    if (var) {
        FILE *f = fopen(var, "r");
        static char *py_argv;

        if (!f) {
            perror(var);
            exit(1);
        }

        /* Set argv[0] to LOOPY_MAIN */
        py_argv = var;
        PySys_SetArgv(1, &py_argv);

        /* Run the user program until it finishes */
        if (PyRun_SimpleFile(f, var) < 0)
            handle_py_error();

        fclose(f);
    }

    /* We're about to start running the target app- allocate
     * a GLState and start tracking it.
     */
    current_glstate = target_glstate = glstate_new();
    Py_INCREF(current_glstate);
}

/* Any time the target application uses dlsym(), glXGetProcAddress(),
 * or glXGetProcAddressARB() we want to be able to override the resulting
 * symbol with one of ours dynamically. Returns a non-NULL pointer to
 * override the address of the specified symbol.
 */
static void *check_dynamic_symbol(const char *name) {
    /* Don't even try to look up symbols other than
     * those with prefixes we can identify.
     *
     * When we search our own symbol namespace below,
     * we're searching a lot of libraries other than this
     * one and OpenGL: this can really screw up an
     * app's plugin loader, for example.
     */
    if (strncmp(name, "gl",   2) &&
        strncmp(name, "SDL_", 4  ))
        return NULL;

    RESOLVE_DLSYM();
    return dlsym_p(RTLD_DEFAULT, name);
}

/************************************************************************/
/*************************************************** GLState Object *****/
/************************************************************************/

static PyMemberDef glstate_members[] = {
    { "trackingFlags", T_UINT, offsetof(GLState, trackingFlags), 0,
      "A set of flags indicating which aspects of the OpenGL\n"
      "state get tracked while this GLState is current.\n"
    },
    { "restoreFlags", T_UINT, offsetof(GLState, restoreFlags), 0,
      "A set of flags indicating which aspects of the OpenGL\n"
      "state are restored when this GLState becomes current.\n"
    },
    { "clearMask", T_UINT, offsetof(GLState, clearMask), 0,
      "Any time a glClear() is run in this GLState, this mask is\n"
      "AND'ed with its flags. This is an easy noninvasive way to\n"
      "prevent a particular context from clearing the color buffer\n"
      "or the depth buffer, for example.\n"
    },
    { "capabilities", T_OBJECT, offsetof(GLState, capabilities), READONLY,
      "A dictionary mapping OpenGL capability numbers to either\n"
      "True (glEnable) or False (glDisable). Missing capabilities\n"
      "haven't been touched by either glEnable or glDisable.\n"
    },
    { "textureBindings", T_OBJECT, offsetof(GLState, textureBindings), READONLY,
      "A dictionary mapping targets (like GL_TEXTURE_2D) to\n"
      "virtual texture IDs. Missing targets haven't been bound yet.\n"
      "This will track traditional 1D/2D/3D targets, plus new targets\n"
      "defined by OpenGL extensions.\n"
    },
    { "textures", T_OBJECT, offsetof(GLState, textures), READONLY,
      "A dictionary mapping all virtual texture IDs used by this GLState\n"
      "to their corresponding real texture IDs. All texture bindings\n"
      "performed while this state is active get mapped through this\n"
      "dictionary.\n"
    },
    { "color", T_OBJECT, offsetof(GLState, color), 0,
      "The current OpenGL color, as a 4-tuple of floats.\n"
      "You may modify this, but values other than 4-tuples\n"
      "of floats will have undefined results.\n"
    },
    { "blendFunc", T_OBJECT, offsetof(GLState, blendFunc), 0,
      "The current OpenGL blend function, as a 2-tuple of\n"
      "integers. You may modify this, but values other than\n"
      "2-tuples of ints will have undefined results.\n"
    },
    { "clearColor", T_OBJECT, offsetof(GLState, color), 0,
      "The current glClearColor, as a 4-tuple of floats.\n"
      "You may modify this, but values other than 4-tuples\n"
      "of floats will have undefined results.\n"
    },
    { "depthFunc", T_INT, offsetof(GLState, depthFunc), 0,
      "The current glDepthFunc value.\n"
    },
    { "matrixMode", T_INT, offsetof(GLState, matrixMode), 0,
      "The current OpenGL matrix mode.\n"
    },
    { "matrices", T_OBJECT, offsetof(GLState, matrices), READONLY,
      "The keys in this dictionary are all OpenGL matrix modes\n"
      "that have been used while this state was current. Values\n"
      "are saved OpenGL matrices. Note that the matrix tracker\n"
      "currently only saves one matrix of each type. This means\n"
      "that an Overlay should never try to pop matrices it pushed\n"
      "during setup.\n"
    },
    { "pushedMatrices", T_OBJECT, offsetof(GLState, pushedMatrices), READONLY,
      "This is a list of OpenGL matrix modes that have been pushed\n"
      "by Loopy. If an Overlay wants to prevent Loopy from popping\n"
      "a particular matrix, for example, it can modify this list.\n"
      "Note that this will refer to a different list object every\n"
      "time a new GLState is made current, and if no matrices are\n"
      "being tracked it will be None.\n"
    },
    {0}
};

static int glstate_init(GLState *self, PyObject *args, PyObject *kw) {
    if (!PyArg_ParseTuple(args, ""))
        return -1;

    self->trackingFlags = GLSTATE_ALL;
    self->restoreFlags = GLSTATE_ALL;
    self->clearMask = ~0;
    self->matrixMode = 0;
    self->depthFunc = GL_LESS;

    self->capabilities = PyDict_New();
    self->textureBindings = PyDict_New();
    self->textures = PyDict_New();
    self->color = Py_BuildValue("(ffff)", 1.0f, 1.0f, 1.0f, 1.0f);
    self->blendFunc = Py_BuildValue("(ii)", GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    self->clearColor = Py_BuildValue("(ffff)", 0.0f, 0.0f, 0.0f, 1.0f);
    self->matrices = PyDict_New();
    self->pushedMatrices = NULL;
    return 0;
}

static void glstate_dealloc(GLState *self) {
    Py_XDECREF(self->capabilities);
    Py_XDECREF(self->textureBindings);
    Py_XDECREF(self->textures);
    Py_XDECREF(self->color);
    Py_XDECREF(self->blendFunc);
    Py_XDECREF(self->clearColor);
    Py_XDECREF(self->matrices);
    Py_XDECREF(self->pushedMatrices);
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

static GLState* glstate_new(void) {
    GLState *self = PyObject_New(GLState, &glstate_type);
    PyObject *args = PyTuple_New(0);
    glstate_init(self, args, NULL);
    Py_DECREF(args);
    return self;
}

static void glstate_type_init(PyObject *module) {
    int i;
    Py_INCREF(&glstate_type);

    glstate_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&glstate_type) < 0)
        return;

    /* Add glstate enumerations */
    for (i=0; glstate_enums[i].name; i++) {
        PyObject *py_value = PyInt_FromLong(glstate_enums[i].value);
        PyDict_SetItemString(glstate_type.tp_dict, glstate_enums[i].name, py_value);
        Py_DECREF(py_value);
    }

    PyModule_AddObject(module, "GLState", (PyObject *) &glstate_type);
}

/************************************************************************/
/********************************************** RenderTarget Object *****/
/************************************************************************/

typedef struct {
    PyObject_HEAD

    unsigned int texture;
    unsigned int format;
    int w, h;
    int current;
    PyObject *py_texture;  /* Mirrors 'texture', but can also be None */

} RenderTarget;

static PyMemberDef rendertarget_members[] = {
    { "texture", T_OBJECT, offsetof(RenderTarget, py_texture), READONLY,
      "The OpenGL texture ID that this RenderTarget is bound to,\n"
      "or None if this RenderTarget is not bound.\n"
    },
    { "format", T_UINT, offsetof(RenderTarget, format), READONLY,
      "The texture format this RenderTarget, either GL_RGB or GL_RGBA.\n"
    },
    { "w", T_INT, offsetof(RenderTarget, w), READONLY,
      "The width of this RenderTarget, in pixels.\n"
    },
    { "h", T_INT, offsetof(RenderTarget, h), READONLY,
      "The width of this RenderTarget, in pixels.\n"
    },
    { "current", T_INT, offsetof(RenderTarget, current), READONLY,
      "True if this is the current draw target.\n"
    },
    {0}
};

static int rendertarget_init(GLState *self, PyObject *args, PyObject *kw) {
    int width, height;
    if (!PyArg_ParseTuple(args, "ii", &width, &height))
        return -1;

    return 0;
}

static PyObject* rendertarget_bind(GLState *self, PyObject *args) {
    int texture;
    if (!PyArg_ParseTuple(args, "i", &texture))
        return NULL;

    Py_RETURN_NONE;
}

static PyObject* rendertarget_lock(GLState *self) {

    Py_RETURN_NONE;
}

static PyObject* rendertarget_unlock(GLState *self) {

    Py_RETURN_NONE;
}

static void rendertarget_dealloc(GLState *self) {

    self->ob_type->tp_free((PyObject *)self);
}

static PyMethodDef rendertarget_methods[] = {
    { "bind", (PyCFunction) rendertarget_bind, METH_VARARGS,
      "bind(textureID) -> None\n"
      "\n"
      "Set the texture that contains the color buffer of the render target\n"
      "after it is unlocked. The texture contents are invalid between lock()\n"
      "and unlock(). This may be called with None to delete any previous\n"
      "texture binding.\n"
    },
    { "lock", (PyCFunction) rendertarget_lock, METH_NOARGS,
      "lock() -> None\n"
      "\n"
      "Make this the current OpenGL drawing target.\n"
    },
    { "unlock", (PyCFunction) rendertarget_unlock, METH_NOARGS,
      "unlock() -> None\n"
      "\n"
      "Restore the OpenGL drawing target, and update the texture currently\n"
      "bound to this target, if any.\n"
    },
    {0}
};

static PyTypeObject rendertarget_type = {
    PyObject_HEAD_INIT(0)
    .tp_name = "loopy.RenderTarget",
    .tp_basicsize = sizeof(RenderTarget),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_members = rendertarget_members,
    .tp_methods = rendertarget_methods,
    .tp_init = (initproc) rendertarget_init,
    .tp_dealloc = (destructor) rendertarget_dealloc,
    .tp_doc = "RenderTarget(width, height)\n"
              "\n"
              "RenderTarget is an interface for rendering to texture,\n"
              "implemented using the SGIX_pbuffer OpenGL extension. The\n"
              "design mimics the RenderTarget API provided by current\n"
              "development versions of SDL. It is implemented here in\n"
              "Loopy for two reasons: Python bindings for pbuffers are\n"
              "not generally available, and any code using pbuffers needs\n"
              "access to GLX state that Loopy tracks internally.\n"
              "\n"
              "After creating a RenderTarget, it may be made current by\n"
              "locking it. While locked, any OpenGL rendering operations\n"
              "operate on the RenderTarget rather than the screen. When\n"
              "the RenderTarget is unlocked, it can optionally update a\n"
              "bound texture with its contents. This effectively provides\n"
              "a render-to-texture without the speed hit and inconvenience\n"
              "of rendering to the backbuffer and performing a readback.\n"
};

static void rendertarget_type_init(PyObject *module) {
    int i;
    Py_INCREF(&rendertarget_type);

    rendertarget_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&rendertarget_type) < 0)
        return;

    PyModule_AddObject(module, "RenderTarget", (PyObject *) &rendertarget_type);
}

/************************************************************************/
/****************************************** OpenGL State Management *****/
/************************************************************************/

/*
 * A macro for internal OpenGL calls, including dynamic symbol
 * resolution and error checking.
 */
#define GL(name, args) \
    do { \
        int _gl_error; \
        RESOLVE(name); \
        RESOLVE(glGetError); \
        name ## _p args; \
        _gl_error = glGetError_p(); \
        if (_gl_error) \
            fprintf(stderr, "Loopy internal OpenGL error 0x%08x in '%s' at %s:%d\n", \
                    _gl_error, #name, __FILE__, __LINE__); \
    } while (0)

/* Create a new Python tuple from a GLdouble vector of fixed length */
static PyObject* pytuple_from_glvector(GLdouble *v, int len) {
    PyObject *tuple = PyTuple_New(len);
    int i;
    for (i=0; i<len; i++)
        PyTuple_SetItem(tuple, i, PyFloat_FromDouble(v[i]));
    return tuple;
}

/* Populate a GLdouble vector of fixed length from a Python tuple.
 * Missing or invalid entries in the tuple leave 0 in the vector.
 */
static void pytuple_to_glvector(PyObject *tuple, GLdouble *v, int len) {
    int tupleLen = PyTuple_Check(tuple) ? PyTuple_GET_SIZE(tuple) : 0;
    int i;
    for (i=0; i<len; i++) {
        if (i<tupleLen) {
            PyObject *item = PyTuple_GET_ITEM(tuple, i);
            if (PyFloat_Check(item)) {
                v[i] = PyFloat_AS_DOUBLE(item);
                continue;
            }
        }
        v[i] = 0;
    }
}

/* Perform the glEnables and glDisables necessary to transition
 * between two OpenGL state dictionaries. This proceeds in two steps:
 *
 * 1. The 'prev' dictionary is scanned. For each enabled capability
 *    that is either missing from 'next' or disabled in 'next', run
 *    a glDisable.
 *
 * 2. The 'next' dictionary is scanned. For each enabled capability
 *    that is either missing from 'prev' or disabled in 'prev, run
 *    a glEnable.
 */
static void glstate_switch_capabilities(PyObject *prev, PyObject *next) {
    PyObject *key, *v1, *v2;
    int pos;

    RESOLVE(glDisable);
    RESOLVE(glEnable);

    pos = 0;
    while (PyDict_Next(prev, &pos, &key, &v1)) {
        if (!PyInt_Check(key))
            continue;
        if (PyObject_Not(v1))
            continue;
        v2 = PyDict_GetItem(next, key);
        if (v2 && PyObject_IsTrue(v2))
            continue;

        GL(glDisable, (PyInt_AS_LONG(key)));
    }

    pos = 0;
    while (PyDict_Next(next, &pos, &key, &v1)) {
        if (!PyInt_Check(key))
            continue;
        if (PyObject_Not(v1))
            continue;
        v2 = PyDict_GetItem(prev, key);
        if (v2 && PyObject_IsTrue(v2))
            continue;

        GL(glEnable, (PyInt_AS_LONG(key)));
    }
}

/*
 * Given a virtual texture ID for this GLState, return the corresponding
 * real texture ID. If this virtual ID hasn't been seen before, we assume
 * the application is using textures without allocating them, and we
 * allocate a new texture on their behalf.
 */
static int glstate_remap_texture(GLState *state, int virtual) {
    PyObject *py_virtual = PyInt_FromLong(virtual);
    PyObject *py_real;
    GLuint real;

    py_real = PyDict_GetItem(state->textures, py_virtual);

    if (py_real && PyInt_Check(py_real)) {
        /* If this texture already exists in the mapping, return it */
        real = PyInt_AS_LONG(py_real);
    }
    else {
        /* If not, this app is using an ID we haven't seen. Allocate
         * a corresponding real ID and store a new mapping.
         */
        GL(glGenTextures, (1, &real));
        py_real = PyInt_FromLong(real);
        PyDict_SetItem(state->textures, py_virtual, py_real);
        Py_DECREF(py_real);
    }

    Py_DECREF(py_virtual);
    return real;
}

/* Track a texture allocation performed by an application. This
 * adds an identity mapping to the current GLState's texture map.
 */
static void glstate_track_texture_alloc(int texture) {
    PyObject *py_texture = PyInt_FromLong(texture);
    PyDict_SetItem(current_glstate->textures, py_texture, py_texture);
    Py_DECREF(py_texture);
}

/* Track a texture free performed by an application. We're given
 * the virtual address, and delete both the texture itself and
 * our mapping to it.
 */
static void glstate_track_texture_free(int virtual) {
    PyObject *py_virtual = PyInt_FromLong(virtual);
    PyObject *py_real = PyDict_GetItem(current_glstate->textures, py_virtual);

    if (py_real && PyInt_Check(py_real)) {
        GLuint real = PyInt_AS_LONG(py_real);
        GL(glDeleteTextures, (1, &real));

        PyDict_DelItem(current_glstate->textures, py_virtual);
    }

    Py_DECREF(py_virtual);
}

static void glstate_restore_texture_bindings(GLState *state) {
    PyObject *key, *value;
    int pos;

    RESOLVE(glBindTexture);

    pos = 0;
    while (PyDict_Next(state->textureBindings, &pos, &key, &value)) {
        if (!PyInt_Check(key))
            continue;
        if (!PyInt_Check(value))
            continue;

        /* Note that, to make our applications' lives easier,
         * the bindings are represented in virtual texture IDs.
         */
        GL(glBindTexture, (PyInt_AS_LONG(key),
                           glstate_remap_texture(state, PyInt_AS_LONG(value))));
    }
}

static void glstate_save_matrices(PyObject *prev) {
    PyObject *key, *value;
    GLdouble matrix[16];
    int pos, n;

    RESOLVE(glGetDoublev);

    pos = 0;
    while (PyDict_Next(prev, &pos, &key, NULL)) {
        if (!PyInt_Check(key))
            continue;

        /* Save matrices into the previous state.
         * This is ugly, since we have to translate from matrix
         * name to the glGet name. Bad OpenGL!
         */
        switch (PyInt_AS_LONG(key)) {
        case GL_MODELVIEW:  n = GL_MODELVIEW_MATRIX;  break;
        case GL_PROJECTION: n = GL_PROJECTION_MATRIX; break;
        case GL_TEXTURE:    n = GL_TEXTURE_MATRIX;    break;
        default:
            continue;
        }

        GL(glGetDoublev, (n, matrix));
        value = pytuple_from_glvector(matrix, 16);
        PyDict_SetItem(prev, key, value);
        Py_DECREF(value);
    }
}

/* Restore saved matrices from the next state. Like the
 * capability tracker, this has to know which matrices
 * need to be cleaned up from the old state. This runs
 * in two steps:
 *
 * 1. The 'prev' dictionary is scanned. Any matrices
 *    missing from 'next' are reset to the identity.
 *
 * 2. The 'next' dictionary is scanned, and all matrices
 *    from it are loaded.
 */
static void glstate_switch_matrices(PyObject *prev, PyObject *next) {
    PyObject *key, *value;
    GLdouble matrix[16];
    int pos;

    RESOLVE(glMatrixMode);
    RESOLVE(glLoadIdentity);
    RESOLVE(glLoadMatrixd);

    pos = 0;
    while (PyDict_Next(prev, &pos, &key, NULL)) {
        if (!PyInt_Check(key))
            continue;
        if (PyDict_GetItem(next, key))
            continue;

        /* Reset matrices used in the old state but not the new */
        GL(glMatrixMode, (PyInt_AS_LONG(key)));
        GL(glLoadIdentity, ());
    }

    pos = 0;
    while (PyDict_Next(next, &pos, &key, &value)) {
        if (!PyInt_Check(key))
            continue;

        /* Load matrices from the next state */
        GL(glMatrixMode, (PyInt_AS_LONG(key)));
        pytuple_to_glvector(value, matrix, 16);
        GL(glLoadMatrixd, (matrix));
    }
}

/* Push every matrix we're tracking in the current glstate,
 * returning a new reference to a list of those matrices.
 */
static PyObject* glstate_push_matrices(PyObject *matrices) {
    PyObject *key, *list = PyList_New(0);
    int pos = 0;

    while (PyDict_Next(matrices, &pos, &key, NULL)) {
        if (!PyInt_Check(key))
            continue;

        PyList_Append(list, key);
        GL(glMatrixMode, (PyInt_AS_LONG(key)));
        GL(glPushMatrix, ());
    }

    return list;
}

/* Pop matrices, given the list returned by glstate_push_matrices(). */
static void glstate_pop_matrices(PyObject *list) {
    int i, len = PyList_GET_SIZE(list);

    for (i=0; i<len; i++) {
        PyObject *key = PyList_GET_ITEM(list, i);
        if (!PyInt_Check(key))
            continue;

        GL(glMatrixMode, (PyInt_AS_LONG(key)));
        GL(glPopMatrix, ());
    }
}

/* Perform an OpenGL state transition. The current_glstate as well as
 * the next state must be non-NULL. Normally current_glstate should
 * only be NULL while a switch is actually in progress.
 */
static void glstate_switch(GLState *next) {
    /* Steal current_glstate's reference, and signal that we
     * have a switch in progress.
     */
    GLState *previous = current_glstate;
    if (next == previous) return;
    current_glstate = NULL;

    if (next->restoreFlags & GLSTATE_CAPABILITIES)
        glstate_switch_capabilities(previous->capabilities,
                                    next->capabilities);

    if (next->restoreFlags & GLSTATE_TEXTURES)
        glstate_restore_texture_bindings(next);

    if (previous->trackingFlags & GLSTATE_COLOR) {
        GLdouble color[4];
        GL(glGetDoublev, (GL_CURRENT_COLOR, color));
        Py_XDECREF(previous->color);
        previous->color = pytuple_from_glvector(color, 4);
    }

    if (previous->trackingFlags & GLSTATE_MATRICES)
        glstate_save_matrices(previous->matrices);

    if (previous->pushedMatrices) {
        glstate_pop_matrices(previous->pushedMatrices);
        Py_DECREF(previous->pushedMatrices);
        previous->pushedMatrices = NULL;
    }

    if (next->restoreFlags & GLSTATE_MATRICES) {
        Py_XDECREF(next->pushedMatrices);
        next->pushedMatrices = glstate_push_matrices(next->matrices);

        glstate_switch_matrices(previous->matrices, next->matrices);
        if (next->matrixMode)
            GL(glMatrixMode, (next->matrixMode));
    }

    if (next->restoreFlags & GLSTATE_COLOR) {
        GLdouble color[4];
        pytuple_to_glvector(next->color, color, 4);
        GL(glColor4dv, (color));
    }

    if (next->restoreFlags & GLSTATE_CLEAR_COLOR) {
        GLdouble color[4];
        pytuple_to_glvector(next->clearColor, color, 4);
        GL(glClearColor, (color[0], color[1], color[2], color[3]));
    }

    if (next->restoreFlags & GLSTATE_BLEND_FUNC) {
        if (PyTuple_Check(next->blendFunc) &&
            PyTuple_GET_SIZE(next->blendFunc) == 2) {
            PyObject *src = PyTuple_GET_ITEM(next->blendFunc, 0);
            PyObject *dest = PyTuple_GET_ITEM(next->blendFunc, 1);
            if (PyInt_Check(src) && PyInt_Check(dest)) {
                GL(glBlendFunc, (PyInt_AS_LONG(src), PyInt_AS_LONG(dest)));
            }
        }
    }

    if (next->restoreFlags & GLSTATE_DEPTH_FUNC)
        GL(glDepthFunc, (next->depthFunc));

    /* Complete the switch, drop the reference we stole from
     * current_glstate, then grab a new reference for our new
     * state.
     */
    Py_INCREF(next);
    Py_DECREF(previous);
    current_glstate = next;
}

static void glstate_track_capability(int capability, int status) {
    PyObject *py_cap = PyInt_FromLong(capability);
    PyDict_SetItem(current_glstate->capabilities,
                   py_cap, status ? Py_True : Py_False);
    Py_DECREF(py_cap);
}

static void glstate_track_matrix_mode(int mode) {
    /* Make sure that any modes the app has touched get represented
     * by at least a None in the matrix dictionary.
     */
    PyObject *py_mode = PyInt_FromLong(mode);
    if (!PyDict_GetItem(current_glstate->matrices, py_mode))
        PyDict_SetItem(current_glstate->matrices, py_mode, Py_None);
    Py_DECREF(py_mode);

    current_glstate->matrixMode = mode;
}

static void glstate_track_texture_binding(int target, int texture) {
    /* Track texture bindings, using virtual texture IDs */
    PyObject *py_target = PyInt_FromLong(target);
    PyObject *py_texture = PyInt_FromLong(texture);
    PyDict_SetItem(current_glstate->textureBindings, py_target, py_texture);
    Py_DECREF(py_target);
    Py_DECREF(py_texture);
}

static void glstate_init_target(int target) {
    /* Start tracking a texture target, if we aren't already.
     * This is necessary to get up implicit texture bindings
     * on targets an application starts using without explicitly
     * binding a texture.
     */
    PyObject *py_target = PyInt_FromLong(target);

    if (!PyDict_GetItem(current_glstate->textureBindings, py_target)) {
        RESOLVE(glBindTexture);
        glstate_track_texture_binding(target, 0);
        glBindTexture_p(target, glstate_remap_texture(current_glstate, 0));
    }

    Py_DECREF(py_target);
}

/************************************************************************/
/*************************************************** Overlay Object *****/
/************************************************************************/

static PyObject *overlay_list;

struct ivector2 {
    int x, y;
};

typedef struct {
    PyObject_HEAD
    int enabled;
    GLState *glState;
    PyObject *py_resolution;

    /* Hidden from Python */
    int initialized;
    struct ivector2 c_resolution;
} Overlay;

static PyObject* return_none(PyObject* self) {
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef overlay_methods[] = {
    { "render", (PyCFunction) return_none, METH_NOARGS,
      "render() -> None\n"
      "\n"
      "Subclasses override this function to render their OpenGL scene.\n"
      "For all enabled Overlay instances in Loopy.overlays, this is called\n"
      "just before the target application is allowed to swap buffers.\n"
      "The default implementation does nothing.\n"
    },
    { "setup", (PyCFunction) return_none, METH_NOARGS,
      "setup() -> None\n"
      "\n"
      "This is called only once, after the OpenGL context is valid and\n"
      "we know the output resolution, but before render() can be invoked.\n"
      "Subclasses override this to perform their 3D engine initialization.\n"
      "The default implementation does nothing.\n"
    },
    { "resized", (PyCFunction) return_none, METH_NOARGS,
      "resized() -> None\n"
      "\n"
      "This is called any time the output resolution changes after the\n"
      "initial call to setup(). The default implementation does nothing.\n"
    },
    {0}
};

static PyMemberDef overlay_members[] = {
    { "enabled", T_INT, offsetof(Overlay, enabled), 0,
      "This Overlay can only be run by Loopy when 'enabled' is nonzero.\n"
    },
    { "glState", T_OBJECT, offsetof(Overlay, glState), READONLY,
      "The GLState instance tracking OpenGL state changes that occur\n"
      "while this Overlay is running. It is automatically made current\n"
      "when hook functions begin running code from this Overlay.\n"
    },
    { "resolution", T_OBJECT, offsetof(Overlay, py_resolution), READONLY,
      "This is a (width, height) tuple containing the current output\n"
      "resolution, if it's known. If not, this is None. This is guaranteed\n"
      "to be valid before a call to setup() or resize().\n"
    },
    {0}
};

static int overlay_init(Overlay *self, PyObject *args, PyObject *kw) {
    if (!PyArg_ParseTuple(args, ""))
        return -1;

    self->glState = glstate_new();
    self->enabled = 1;

    /* By default, don't let overlays clear the screen. This can be
     * overridden for overlays that actually want to clear the color
     * buffer, but it's a good default that makes writing overlays
     * with existing 3D engines a little easier.
     */
    self->glState->clearMask &= ~GL_COLOR_BUFFER_BIT;

    return 0;
}

static void overlay_dealloc(Overlay *self) {
    Py_XDECREF(self->glState);
    self->ob_type->tp_free((PyObject *)self);
}

static PyTypeObject overlay_type = {
    PyObject_HEAD_INIT(0)
    .tp_name = "loopy.Overlay",
    .tp_basicsize = sizeof(Overlay),
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_members = overlay_members,
    .tp_methods = overlay_methods,
    .tp_init = (initproc) overlay_init,
    .tp_dealloc = (destructor) overlay_dealloc,
    .tp_doc = "Overlay()\n"
              "\n"
              "An Overlay is a partition created within the target\n"
              "application's OpenGL context, performing extra rendering\n"
              "after the target app renders each frame. Zero or more\n"
              "overlays can be stacked onto the target app, and each\n"
              "can be individually enabled or disabled.\n"
              "\n"
              "Subclass Overlay and override render() to start writing\n"
              "your own Loopy overlays. To install the overlay, append\n"
              "it to the Loopy.overlays list.\n"
};

static void overlay_type_init(PyObject *module) {
    Py_INCREF(&overlay_type);

    overlay_type.tp_new = PyType_GenericNew;
    if (PyType_Ready(&overlay_type) < 0)
        return;

    PyModule_AddObject(module, "Overlay", (PyObject *) &overlay_type);

    overlay_list = PyList_New(0);
    PyModule_AddObject(module, "overlays", overlay_list);
}

/*
 * Loop over all enabled overlays, with attention to errors
 * that might be caused by users. On failure, returns -1
 * with a Python exception. The callback may return -1 to
 * propagate a Python exception.
 */
static int foreach_overlay(int (*callback)(Overlay*, void*), void *user_data) {
    int i, result, n_overlays;

    n_overlays = PyList_Size(overlay_list);
    if (n_overlays < 0)
        return -1;

    for (i=0; i<n_overlays; i++) {
        /* Note that this could fail if the list changed size while
         * we're traversing it. It's safe to append overlays to the
         * list from within this loop, but reordering or deleting
         * entries will cause unexpected results.
         */
        Overlay *item = (Overlay*) PyList_GetItem(overlay_list, i);
        if (!item)
            return -1;

        if (!PyObject_IsInstance((PyObject*) item, (PyObject*) &overlay_type)) {
            PyErr_SetString(PyExc_TypeError,
                            "Only Overlay instances are allowed in Loopy.overlays");
            return -1;
        }

        if (!item->enabled)
            continue;

        /* Our item is a borrowed reference- hold onto it while
         * we're executing user code to avoid errors caused by
         * modifying the list.
         */
        Py_INCREF(item);
        result = callback(item, user_data);
        Py_DECREF(item);

        if (result < 0)
            return -1;
    }
    return 0;
}

/* Call an arbitrary method in the Overlay, with no parameters,
 * performing the proper OpenGL state management before and after.
 */
static int overlay_call_method(Overlay *self, char *method) {
    PyObject *result;
    int retval = 0;

    glstate_switch(self->glState);
    result = PyObject_CallMethod((PyObject*) self, method, NULL);
    if (result)
        Py_DECREF(result);
    else
        retval = -1;

    /* Throw away any leftover errors from the Overlay */
    RESOLVE(glGetError);
    glGetError_p();

    return retval;
}

/* If this overlay has been initialized, switch to
 * its GLState and render() it.
 */
static int overlay_render(Overlay *self, void* user_data) {
    if (self->initialized)
        return overlay_call_method(self, "render");
    return 0;
}

/* Propagate a new screen resolution to this overlay.
 * If the overlay hasn't yet been initialized, it calls setup().
 * Otherwise we call resize() if and only if the resolution
 * has changed. user_data is an ivector2.
 */
static int overlay_resize(Overlay *self, void* user_data) {
    struct ivector2 *current_res = (struct ivector2*) user_data;
    char *method;

    if (self->initialized) {
        if (current_res->x == self->c_resolution.x &&
            current_res->y == self->c_resolution.y)
            return 0;
        method = "resized";
    }
    else {
        method = "setup";
    }

    Py_XDECREF(self->py_resolution);
    self->c_resolution = *current_res;
    self->py_resolution = Py_BuildValue("(ii)", current_res->x, current_res->y);

    if (overlay_call_method(self, method) < 0)
        return -1;

    self->initialized = 1;
    return 0;
}

/************************************************************************/
/**************************************************** Python Module *****/
/************************************************************************/

static PyObject *get_target_glstate(PyObject *self) {
    if (target_glstate) {
        Py_INCREF(target_glstate);
        return (PyObject*) target_glstate;
    }
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}

static PyMethodDef module_methods[] = {
    { "getTargetGLState", (PyCFunction) get_target_glstate, METH_NOARGS,
      "getTargetGLState() -> GLState\n"
      "\n"
      "Return the GLState used for tracking the target application.\n"
      "If Loopy is running without a target application, or the target\n"
      "hasn't started executing yet, this returns None.\n"
    },
    {0}
};

/*
 * Initialize the 'loopy' extension module. This is normally called
 * internally for the Python interpreter we embed in the target app,
 * but this library can also be used as a normal extension module.
 */
PyMODINIT_FUNC initloopy(void)
{
    PyObject *m = Py_InitModule3("loopy", module_methods, _doc);

    glstate_type_init(m);
    overlay_type_init(m);
    rendertarget_type_init(m);
}

/************************************************************************/
/*********************************************** Override Functions *****/
/************************************************************************/

void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
    /* Clear any stale errors from the target app */
    RESOLVE(glGetError);
    glGetError_p();

    /* Give each overlay a chance to run, then swap back */
    if (foreach_overlay(overlay_render, NULL) < 0)
        handle_py_error();
    glstate_switch(target_glstate);

    RESOLVE(glXSwapBuffers);
    glXSwapBuffers_p(dpy, drawable);
}

Bool glXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx) {
    Bool result;
    RESOLVE(glXMakeCurrent);
    result = glXMakeCurrent_p(dpy, drawable, ctx);
    if (result) {
        current_glxstate.dpy = dpy;
        current_glxstate.drawable = drawable;
        current_glxstate.ctx = ctx;
        current_glxstate.is_valid = True;
    }
    return result;
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    RESOLVE(glViewport);
    glViewport_p(x, y, width, height);

    if (RUNNING_IN_TARGET) {
        /* This isn't fool-proof, but we detect the current
         * window resolution by looking for glViewport() calls
         * with x,y == 0,0. If this turns out to be a problem,
         * the only alternative might be hooking Xlib. Yech.
         */
        if (x==0 && y==0) {
            struct ivector2 res = {width, height};

            /* Resize our fake SDL surface */
            overlay_sdl_surface.w = res.x;
            overlay_sdl_surface.h = res.y;

            /* Notify all the overlays */
            if (foreach_overlay(overlay_resize, &res) < 0)
                handle_py_error();
            glstate_switch(target_glstate);
        }
    }
}

void glClear(GLbitfield flags) {
    if (current_glstate)
        flags &= current_glstate->clearMask;
    if (flags) {
        RESOLVE(glClear);
        glClear_p(flags);
    }
}

void glEnable(GLenum cap) {
    RESOLVE(glEnable);
    glEnable_p(cap);
    if (IS_TRACKING(GLSTATE_CAPABILITIES))
        glstate_track_capability(cap, 1);
}

void glDisable(GLenum cap) {
    RESOLVE(glDisable);
    glDisable_p(cap);
    if (IS_TRACKING(GLSTATE_CAPABILITIES))
        glstate_track_capability(cap, 0);
}

void glMatrixMode(GLenum mode) {
    RESOLVE(glMatrixMode);
    glMatrixMode_p(mode);
    if (IS_TRACKING(GLSTATE_MATRICES))
        glstate_track_matrix_mode(mode);
}

void glBlendFunc(GLenum src, GLenum dest) {
    RESOLVE(glBlendFunc);
    glBlendFunc_p(src, dest);
    if (IS_TRACKING(GLSTATE_BLEND_FUNC)) {
        Py_XDECREF(current_glstate->blendFunc);
        current_glstate->blendFunc = Py_BuildValue("(ii)", src, dest);
    }
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) {
    RESOLVE(glClearColor);
    glClearColor_p(red, green, blue, alpha);
    if (IS_TRACKING(GLSTATE_CLEAR_COLOR)) {
        GLdouble color[] = {red, green, blue, alpha};
        Py_XDECREF(current_glstate->clearColor);
        current_glstate->clearColor = pytuple_from_glvector(color, 4);
    }
}

void glDepthFunc(GLenum func) {
    RESOLVE(glDepthFunc);
    glDepthFunc_p(func);
    if (IS_TRACKING(GLSTATE_DEPTH_FUNC))
        current_glstate->depthFunc = func;
}

void glBindTexture(GLenum target, GLuint texture) {
    if (IS_TRACKING(GLSTATE_TEXTURES)) {
        glstate_track_texture_binding(target, texture);
        texture = glstate_remap_texture(current_glstate, texture);
    }
    RESOLVE(glBindTexture);
    glBindTexture_p(target, texture);
}

void glGenTextures(GLsizei n, GLuint *textures) {
    RESOLVE(glGenTextures);
    glGenTextures_p(n, textures);
    if (IS_TRACKING(GLSTATE_TEXTURES)) {
        int i;
        for (i=0; i<n; i++)
            glstate_track_texture_alloc(textures[i]);
    }
}

void glDeleteTextures(GLsizei n, const GLuint *textures) {
    if (IS_TRACKING(GLSTATE_TEXTURES)) {
        int i;
        for (i=0; i<n; i++)
            glstate_track_texture_free(textures[i]);
    }
    else {
        RESOLVE(glDeleteTextures);
        glDeleteTextures_p(n, textures);
    }
}

GLboolean glIsTexture(GLuint texture) {
    if (IS_TRACKING(GLSTATE_TEXTURES)) {
        /* This could be dangerous if the app uses glIsTexture()
         * indiscriminately on texture IDs that don't exist,
         * since we'll end up creating a virtual texture mapping
         * each time we're called.
         */
        texture = glstate_remap_texture(current_glstate, texture);
    }
    RESOLVE(glIsTexture);
    return glIsTexture_p(texture);
}

void glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
    if (IS_TRACKING(GLSTATE_TEXTURES))
        glstate_init_target(target);
    RESOLVE(glTexParameterf);
    glTexParameterf_p(target, pname, param);
}

void glTexParameteri(GLenum target, GLenum pname, GLint param) {
    if (IS_TRACKING(GLSTATE_TEXTURES))
        glstate_init_target(target);
    RESOLVE(glTexParameteri);
    glTexParameteri_p(target, pname, param);
}

void glBindTextureEXT(GLenum target, GLuint texture) {
    glBindTexture(target, texture);
}

void glGenTexturesEXT(GLsizei n, GLuint *textures) {
    glGenTextures(n, textures);
}

void glDeleteTexturesEXT(GLsizei n, GLuint *textures) {
    glDeleteTextures(n, textures);
}

GLboolean glIsTextureEXT(GLuint texture) {
    return glIsTexture(texture);
}

void *dlsym (void *__restrict handle, __const char *__restrict name) {
    void *override = check_dynamic_symbol(name);
    if (override)
        return override;
    RESOLVE_DLSYM();
    return dlsym_p(handle, name);
}

void *glXGetProcAddress(const GLubyte *name) {
    void *override = check_dynamic_symbol(name);
    if (override)
        return override;
    RESOLVE(glXGetProcAddress);
    return glXGetProcAddress_p(name);
}

void *glXGetProcAddressARB(const GLubyte *name) {
    void *override = check_dynamic_symbol(name);
    if (override)
        return override;
    RESOLVE(glXGetProcAddressARB);
    return glXGetProcAddressARB_p(name);
}

void *SDL_GL_GetProcAddress(const char* name) {
    void *override = check_dynamic_symbol(name);
    if (override)
        return override;
    RESOLVE(SDL_GL_GetProcAddress);
    return SDL_GL_GetProcAddress_p(name);
}

int SDL_GL_SetAttribute(SDL_GLattr attr, int value) {
    /* Don't let the overlay touch GL attributes */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_GL_SetAttribute);
        return SDL_GL_SetAttribute_p(attr, value);
    }
    return 0;
}

int SDL_GL_GetAttribute(SDL_GLattr attr, int *value) {
    /* Don't let the overlay touch GL attributes */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_GL_GetAttribute);
        return SDL_GL_GetAttribute_p(attr, value);
    }
    return 0;
}

void SDL_GL_SwapBuffers(void) {
    /* This will eventually get trapped at glXSwapBuffers,
     * but the overlay isn't going to have a properly
     * initialized video driver so this is unsafe to call.
     */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_GL_SwapBuffers);
        SDL_GL_SwapBuffers_p();
    }
}

int SDL_Init(Uint32 flags) {
    /* Don't let the overlay touch SDL initialization */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_Init);
        return SDL_Init_p(flags);
    }
    return 0;
}

int SDL_InitSubSystem(Uint32 flags) {
    /* Don't let the overlay touch SDL initialization */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_InitSubSystem);
        return SDL_InitSubSystem_p(flags);
    }
    return 0;
}

Uint32 SDL_WasInit(Uint32 flags) {
    /* Let the overlay think it ran a successful initialization,
     * even if the target application isn't using SDL at all.
     */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_WasInit);
        return SDL_WasInit_p(flags);
    }
    if (flags) {
        /* Return what the user wants to see */
        return flags;
    }
    else {
        /* They're asking about all subsystems */
        return ~0;
    }
}


void SDL_QuitSubSystem(Uint32 flags) {
    /* Don't let the overlay touch SDL initialization */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_QuitSubSystem);
        SDL_QuitSubSystem_p(flags);
    }
}

void SDL_Quit(void) {
    /* Don't let the overlay touch SDL initialization */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_Quit);
        SDL_Quit_p();
    }
}

void SDL_PumpEvents(void) {
    /* Very bad idea to let the overlay handle events..
     * Note that we dont' really support running any kind
     * of event loop in the overlay, but Pygame is naughty
     * and calls this during initialization.
     */
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_PumpEvents);
        SDL_PumpEvents_p();
    }
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_WM_SetCaption);
        SDL_WM_SetCaption_p(title, icon);
    }
}

void SDL_WM_GetCaption(char **title, char **icon) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_WM_GetCaption);
        SDL_WM_GetCaption_p(title, icon);
    }
    else {
        if (title) *title = NULL;
        if (icon) *icon = NULL;
    }
}

void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_WM_SetIcon);
        SDL_WM_SetIcon_p(icon, mask);
    }
}

SDL_Surface *SDL_GetVideoSurface(void) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_GetVideoSurface);
        return SDL_GetVideoSurface_p();
    }
    /* Don't let the overlay see the real SDL surface,
     * just give it one we've filled in with some very
     * basic info like output resolution.
     */
    return &overlay_sdl_surface;
}

const SDL_VideoInfo *SDL_GetVideoInfo(void) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_GetVideoInfo);
        return SDL_GetVideoInfo_p();
    }
    return &overlay_sdl_info;
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
    if (RUNNING_IN_TARGET) {
        RESOLVE(SDL_SetVideoMode);
        return SDL_SetVideoMode_p(width, height, bpp, flags);
    }
    /* For the overlay, ignore the requested resolution
     * and still return our fake surface.
     */
    return &overlay_sdl_surface;
}

/* The End */
