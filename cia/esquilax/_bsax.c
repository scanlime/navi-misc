/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * bsax - Helper module for fast queries against binary SAX streams 
 * Copyright (C) 2007 Micah Dowty <micah@navi.cx>
 *
 * The BSAX format is a simple binary SAX event encoding with
 * memoized strings. BSAX files have no header. Each message must
 * be appended to the file atomically. A message's ID consists
 * of a file ID and an offset within that file.
 *
 * Each message consists of:
 *
 *   - A small fixed-size binary header:
 *
 *       1. One NUL byte, indicating the beginning of the message. This
 *          is used as a flag character, since messages may not contain
 *          an internal NUL.
 *
 *       2. The length of this message, including all headers, as a
 *          32-bit big endian integer.
 *
 *       3. The message version, as a single byte. This identifies
 *          the dictionary used to encode the message, and it can indicate
 *          the presence of extra fields in the message header.
 *
 *   - UTF-8 encoded data, interpreted as a string of events
 *
 *       Every event begins with a single unicode
 *       character that has an opcode packed into the low
 *       2 bits, and a paramter occupying all upper bits.
 *
 *       Note that opcodes shouldn't be treated as valid Unicode
 *       characters. Opcodes with parameters above 0x3600 risk
 *       being interpreted as surrogate pairs. Opcodes should
 *       be treated as variable-length integers that happen to
 *       be encoded in a method compatible with UTF-8.
 *
 *       Opcodes:
 *
 *         0. The parameter specifies the number of endElement events to emit
 *
 *         1. The parameter is a string length L. The following L bytes define
 *            a new string, which is also added to the memo.
 *
 *         2. Emit a memoized string. The parameter is the memo ID of that string.
 *            Memo IDs start at zero, but the memo is generally pre-populated with
 *            a database-wide dictionary.
 *
 *         3. Emit whitespace. The parameter is a number of blank spaces to insert.
 *
 *       Strings, either included directly or copied from the memo, may appear in
 *       multiple forms:
 *
 *         - In the default context, a string can be interpreted as either character
 *           data or as an element. Character data strings are prefixed with 0x01,
 *           while element data strings are prefixed with 0x02 plus their attribute
 *           count. After an element with attributes, the stream alternates between
 *           attribute name and attribute value contexts.
 *
 *         - In attribute name/value contexts, no string prefix is used.
 *
 *       This method of encoding actually makes an element's arity part of its name,
 *       as far as our compression is concerned.
 *
 * License:
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

#include <glib.h>
#include <fcntl.h>
#include <unistd.h>
#include <Python.h>

/*
 * We can get pread() from unistd.h by defining _XOPEN_SOURCE to 500,
 * but that conflicts with the values defined by Python.h.
 */
ssize_t pread(int fd, void *buf, size_t count, off_t offset);

//#define DEBUG

#ifdef DEBUG
#define DBG(args...) fprintf(stderr, args)
#else
#define DBG(args...)
#endif

/************************************************************************/
/************************************************** Open File Cache *****/
/************************************************************************/

typedef unsigned int file_id_t;

typedef struct {
    file_id_t id;
    int fd;
} opened_file_t;

struct {
    GHashTable *hash;
    GList *mru;
    opened_file_t *recent;

    PyObject *get_filename_by_id;
    int max_open_files;
} filecache;

/*
 * Open a new file, creating a new opened_file_t from a file ID.
 * Requires a valid filecache.get_filename_by_id function. Returns
 * NULL on exception.
 */
static opened_file_t *
opened_file_new(file_id_t id)
{
    PyObject *filename_str;
    const char *filename;
    opened_file_t *newfile;
    int fd;

    if (!filecache.get_filename_by_id) {
        PyErr_SetString(PyExc_ValueError, "No get_filename_by_id handler has been defined");
        return NULL;
    }
        
    filename_str = PyObject_CallFunction(filecache.get_filename_by_id, "I", id);
    if (!filename_str) {
        return NULL;
    }
       
    filename = PyString_AsString(filename_str);
    if (!filename_str) {
        return NULL;
    }

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    newfile = malloc(sizeof *newfile);
    if (!newfile) {
        close(fd);
        PyErr_NoMemory();
        return NULL;
    }

    newfile->id = id;
    newfile->fd = fd;

    return newfile;
}

/*
 * Close and free an opened_file_t object
 */
static void
opened_file_free(opened_file_t *self)
{
    close(self->fd);
    free(self);
}

/*
 * Initialize the filecache if necessary, and set its parameters.
 * Callable from Python:
 *    filecache_init(max_open_files, get_filename_by_id)
 */
static PyObject*
py_filecache_init(PyObject *self, PyObject *args)
{
    Py_XDECREF(filecache.get_filename_by_id);
    filecache.get_filename_by_id = NULL;

    if (!PyArg_ParseTuple(args, "iO", &filecache.max_open_files, &filecache.get_filename_by_id))
        return NULL;

    if (!filecache.hash) {
        filecache.hash = g_hash_table_new(NULL, NULL);
    }

    Py_RETURN_NONE;
}

/*
 * Look up a file from the cache, opening it if necessary.
 * Returns NULL on exception.
 */
static opened_file_t *
filecache_lookup(file_id_t id)
{
    GList *file_link;
    opened_file_t *file;

    DBG("filecache: lookup %d\n", id);

    if (filecache.recent && filecache.recent->id == id) {
        /*
         * Fast path for the most recently used file.  Also note that
         * it's already at the MRU list head.
         */
        DBG("filecache: hit l0-cache\n");
        return filecache.recent;
    }

    file_link = g_hash_table_lookup(filecache.hash, (gpointer) id);
    if (file_link) {
        /*
         * Bump to the front of the MRU list.
         */
        filecache.mru = g_list_concat(file_link, g_list_remove_link(filecache.mru, file_link));
        file = file_link->data;
        filecache.recent = file;
        DBG("filecache: hit hash table\n");
        return file;
    }

    /*
     * This file isn't in the cache. First, make room if necessary...
     */
    while (g_hash_table_size(filecache.hash) >= filecache.max_open_files) {
        GList *lru_link = g_list_last(filecache.mru);
        opened_file_t *lru = lru_link->data;

        DBG("filecache: evicting %d\n", lru->id);

        filecache.mru = g_list_remove_link(filecache.mru, lru_link);
        g_list_free(lru_link);
        g_hash_table_remove(filecache.hash, (gpointer) lru->id);
        opened_file_free(lru);
    }

    /*
     * Try to open the file
     */
    file = opened_file_new(id);
    if (file) {
        filecache.mru = g_list_prepend(filecache.mru, file);
        g_hash_table_insert(filecache.hash, (gpointer) id, filecache.mru);
        filecache.recent = file;
    }

    DBG("filecache: miss %d\n", id);

    return file;
}


/************************************************************************/
/****************************************************** Block Cache *****/
/************************************************************************/

typedef struct {
    file_id_t file;
    off_t offset;
} block_id_t;

typedef struct {
    block_id_t id;
    unsigned char *buffer;
    ssize_t size;
} block_buffer_t;

struct {
    GHashTable *hash;
    GList *mru;
    block_buffer_t *recent;

    int block_size;
    int max_blocks;
} blockcache;

/*
 * Hashing function for block_id_t
 */
static guint
block_id_hash(gconstpointer v)
{
    const block_id_t *id = v;
    return g_direct_hash((gpointer) id->file) ^ g_direct_hash((gpointer) id->offset);
}

/*
 * Comparison function for block_id_t
 */
static gint
block_id_equal(gconstpointer v1, gconstpointer v2)
{
    const block_id_t *id1 = v1;
    const block_id_t *id2 = v2;
    return (id1->file == id2->file) && (id1->offset == id2->offset);
}

/*
 * Initialize the blockcache if necessary, and set its parameters.
 * Callable from Python exactly once:
 *    blockcache_init(block_size, max_blocks)
 */
static PyObject*
py_blockcache_init(PyObject *self, PyObject *args)
{
    if (blockcache.hash) {
        PyErr_SetString(PyExc_ValueError, "Block cache already initialized");
        return NULL;
    }

    if (!PyArg_ParseTuple(args, "ii", &blockcache.block_size, &blockcache.max_blocks))
        return NULL;

    if (!blockcache.hash) {
        blockcache.hash = g_hash_table_new(block_id_hash, block_id_equal);
    }

    Py_RETURN_NONE;
}

/*
 * Look up a block from the cache, reading it and evicting old blocks if necessary.
 * The returned pointer is only guaranteed valid until the next blockcache_lookup.
 * Returns NULL on exception.
 */
static block_buffer_t *
blockcache_lookup(file_id_t file_id, off_t offset)
{
    GList *block_link;
    block_id_t static_id = { file_id, offset };
    block_buffer_t *block;
    opened_file_t *file;

    DBG("blockcache: lookup (%d, %d)\n", file_id, (int) offset);

    if (blockcache.recent && block_id_equal(&blockcache.recent->id, &static_id)) {
        /*
         * Fast path for the most recently used block, already at the MRU head.
         */
        DBG("blockcache: hit l0-cache\n");
        return blockcache.recent;
    }

    block_link = g_hash_table_lookup(blockcache.hash, &static_id);
    if (block_link) {
        /*
         * Bump to the front of the MRU list.
         */
        blockcache.mru = g_list_concat(block_link, g_list_remove_link(blockcache.mru, block_link));
        block = block_link->data;
        blockcache.recent = block;
        DBG("blockcache: hit hash table\n");
        return block;
    }

    /*
     * This block isn't in the cache. First, make room if necessary...
     */
    while (g_hash_table_size(blockcache.hash) >= blockcache.max_blocks) {
        GList *lru_link = g_list_last(blockcache.mru);
        block_buffer_t *lru = lru_link->data;

        DBG("blockcache: evicting (%d, %d)\n", lru->id.file, (int) lru->id.offset);

        blockcache.mru = g_list_remove_link(blockcache.mru, lru_link);
        g_list_free(lru_link);

        g_hash_table_remove(blockcache.hash, &lru->id);
        free(lru->buffer);
        free(lru);
    }

    /*
     * Try to open the file and read this block
     */
    file = filecache_lookup(file_id);
    if (!file) {
        return NULL;
    }

    block = malloc(sizeof *block);
    if (!block) {
        PyErr_NoMemory();
        return NULL;
    }
    block->id = static_id;

    block->buffer = malloc(blockcache.block_size);
    if (!block->buffer) {
        free(block);
        PyErr_NoMemory();
        return NULL;
    }

    block->size = pread(file->fd, block->buffer, blockcache.block_size, offset);
    if (block->size < 0) {
        free(block->buffer);
        free(block);
        PyErr_SetFromErrno(PyExc_IOError);
        return NULL;
    }

    /* Store this block in the cache */
    blockcache.mru = g_list_prepend(blockcache.mru, block);
    g_hash_table_insert(blockcache.hash, &block->id, blockcache.mru);
    blockcache.recent = block;

    DBG("blockcache: miss (%d, %d)\n", file_id, (int) offset);

    return block;
}

/*
 * Python-callable version of blockcache_lookup()
 */
static PyObject*
py_blockcache_lookup(PyObject *self, PyObject *args)
{
    int file_id;
    int offset;
    block_buffer_t *block;

    if (!PyArg_ParseTuple(args, "ii", &file_id, &offset))
        return NULL;

    block = blockcache_lookup(file_id, offset);
    if (!block)
        return NULL;

    return PyString_FromStringAndSize((void*) block->buffer, block->size);
}


/************************************************************************/
/************************************************** Python Module *******/
/************************************************************************/

static PyMethodDef module_methods[] = {
    { "filecache_init",    (PyCFunction) py_filecache_init,     METH_VARARGS },
    { "blockcache_init",   (PyCFunction) py_blockcache_init,    METH_VARARGS },
    { "blockcache_lookup", (PyCFunction) py_blockcache_lookup,  METH_VARARGS },
    {0}
};

PyMODINIT_FUNC
init_bsax(void)
{
    Py_InitModule("_bsax", module_methods);
}
