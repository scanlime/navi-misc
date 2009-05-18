/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Model for save data. (Games, chips)
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include "saveData.h"


/*
 * There seems to be a compilation inconsistency in some versions of
 * libfat, where the library and the installed headers disagree on the
 * size of the dev_t inside struct stat. This causes most of the
 * structure to be misaligned quite badly.
 *
 * This is a stat() wrapper that works around the problem.
 *
 * We figure out which version of struct stat is being used by looking
 * at st_blksize. If it isn't 0x200, something's wrong.
 */

static int statFix(const char *path, struct stat *sbuf) {
    union {
        struct stat installed;
        struct {
            uint16_t      st_dev;
            uint16_t      pad1;
            uint32_t      st_ino;
            uint32_t      st_mode;
            uint16_t      st_nlink;
            uint16_t      st_uid;
            uint16_t      st_gid;
            uint16_t      st_rdev;
            uint32_t      st_size;
            uint32_t      st_atime;
            uint32_t      pad2;
            uint32_t      st_mtime;
            uint32_t      pad3;
            uint32_t      st_ctime;
            uint32_t      pad4;
            int32_t       st_blksize;
            uint32_t      st_blocks;
            uint32_t      padEnd[5];
        } local;
    } u;

    const int32_t expectedBlkSize = 0x200;
    int retval = stat(path, &u.installed);

    if (u.installed.st_blksize == expectedBlkSize) {
        /*
         * Installed version of 'struct stat' matches libfat.
         */
        memcpy(sbuf, &u.installed, sizeof u.installed);

    } else if (u.local.st_blksize == expectedBlkSize) {
        /*
         * Installed version doesn't match, but our local one does.
         */
        sbuf->st_dev = u.local.st_dev;
        sbuf->st_ino = u.local.st_ino;
        sbuf->st_mode = u.local.st_mode;
        sbuf->st_nlink= u.local.st_nlink;
        sbuf->st_uid = u.local.st_uid;
        sbuf->st_gid = u.local.st_gid;
        sbuf->st_rdev = u.local.st_rdev;
        sbuf->st_size = u.local.st_size;
        sbuf->st_atime = u.local.st_atime;
        sbuf->st_mtime = u.local.st_mtime;
        sbuf->st_ctime = u.local.st_ctime;
        sbuf->st_blksize = u.local.st_blksize;
        sbuf->st_blocks = u.local.st_blocks;

    } else {
        sassert(false,
                "'struct stat' doesn't look right.\n"
                "Compile problem in libfat?");
    }

    return retval;
}


//********************************************************** SaveData


bool SaveData::init() {
    initialized = fatInitDefault();
    return initialized;
}

const char *SaveData::getInitErrorMessage() {
    return "Can't find a storage device!"
           " If you continue, you"
           " will be unable to save your progress."
           "\n\v"
           "(Make sure you have applied the correct"
           " DLDI patch for your homebrew device!)";
}

bool SaveData::enterSaveDir(bool canCreate) {
    /*
     * Change to our save file directory.  If 'canCreate' is true,
     * creates the directory if it doesn't already exist.
     *
     * Returns true on success, false on error.
     */

    const char *parentDir = "/data/";
    const char *dataDir = "/data/RobotOdyssey/";

    if (chdir(dataDir) == 0) {
        return true;
    }

    if (canCreate) {
        /*
         * These will fail harmlessly if the directory or its parent exists.
         */
        mkdir(parentDir, 0777);
        mkdir(dataDir, 0777);

        if (chdir(dataDir) == 0) {
            return true;
        }
    }

    return false;
}


//********************************************************** SaveType


void SaveType::listFiles(SaveFileList &l) {
    /*
     * Append all save files to 'l'.
     */

    if (!sd->initialized) {
        return;
    }
    if (!sd->enterSaveDir()) {
        return;
    }

    DIR *dir = opendir(".");
    if (!dir) {
        return;
    }

    struct dirent *dent;
    struct stat st;
    int extensionLen = strlen(extension);

    while ((dent = readdir(dir))) {
        if (statFix(dent->d_name, &st)) {
            continue;
        }

        /*
         * Skip directories
         */
        if (st.st_mode & S_IFDIR) {
            continue;
        }

        /*
         * Look for a matching extension. (Case insensitive)
         */
        const char *filename = dent->d_name;
        int filenameLen = strlen(filename);
        int baseLen = filenameLen - extensionLen;
        if (baseLen > 0 &&
            strncasecmp(filename + baseLen,extension, extensionLen) == 0) {

            l.push_back(SaveFile(this, std::string(filename, baseLen),
                                 st.st_size, st.st_mtime));
        }
    }

    closedir(dir);
}

SaveFile SaveType::newFile() {
    /*
     * Return a new save file for this type.  The file won't actually
     * be created until its write() method is called.
     */
    return SaveFile(this, "foo");
}


//********************************************************** SaveFile


bool SaveFile::write(uint8_t *buffer, int size) {
    return false;
}

bool SaveFile::read(uint8_t *buffer) {
    return false;
}
