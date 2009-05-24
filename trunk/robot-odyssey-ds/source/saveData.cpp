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

#include <sstream>
#include <algorithm>

#include "saveData.h"


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


void SaveType::listFiles(SaveFileList &l, bool showNewFile) {
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
        if (stat(dent->d_name, &st)) {
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

    if (showNewFile) {
        l.push_front(newFile(l));
    }
}

SaveFile SaveType::newFile(SaveFileList &otherFiles) {
    /*
     * Return a new save file for this type, with a name that's unique
     * among all other save files.  The file won't actually be created
     * until its write() method is called.
     */

    int slotNumber = 1;

    while (1) {
        std::ostringstream s;

        s << "Slot " << slotNumber;
        SaveFile file(this, s.str());

        SaveFileList::iterator it = std::find(otherFiles.begin(),
                                              otherFiles.end(),
                                              file);

        if (it == otherFiles.end()) {
            return file;
        }

        slotNumber++;
    }
}

bool SaveType::operator ==(const SaveType &other) const {
    return !strcmp(extension, other.extension);
}


//********************************************************** SaveFile


void SaveFile::getTimestamp(char *buf, size_t bufSize) {
    strftime(buf, bufSize, "%Y-%m-%d %H:%M", gmtime(&timestamp));
}

bool SaveFile::write(void *buffer, size_t size) {
    std::string fullName = name + st->extension;

    if (!st->sd->enterSaveDir(true)) {
        return false;
    }

    FILE *f = fopen(fullName.c_str(), "wb");
    if (!f) {
        return false;
    }

    size_t result = fwrite(buffer, 1, size, f);
    fclose(f);
    size = std::max(0, (int)result);

    return result == getSize();
}

bool SaveFile::read(void *buffer, size_t size) {
    std::string fullName = name + st->extension;

    if (!st->sd->enterSaveDir(false)) {
        return false;
    }

    FILE *f = fopen(fullName.c_str(), "rb");
    if (!f) {
        return false;
    }

    size_t result = fread(buffer, 1, size, f);
    fclose(f);

    return result == size;
}

bool SaveFile::operator ==(const SaveFile &other) const {
    return *st == *other.st && name == other.name;
}

bool SaveFile::loadGame(SBTProcess *game, HwCommon *hw) {
    if (isNew()) {
        game->exec();
    } else {
        if (!read(&hw->fs)) {
            return false;
        }
        game->exec("99");
    }
    return true;
}
