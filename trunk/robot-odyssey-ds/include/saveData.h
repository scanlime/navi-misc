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

#ifndef _SAVEDATA_H_
#define _SAVEDATA_H_

#include <string>
#include <list>

class SaveFile;
class SaveType;
typedef std::list<SaveFile> SaveFileList;


/*
 * Top-level class for save data. Responsible for libfat
 * initialization, and the actual save data directory.
 */
class SaveData
{
public:
    SaveData() : initialized(false) {}

    bool init();
    const char *getInitErrorMessage();

    bool isInitialized() {
        return initialized;
    }

private:
    friend class SaveType;
    friend class SaveFile;

    bool initialized;
    bool enterSaveDir(bool canCreate = false);
};


/*
 * Operations for one type of save file, identified by its extension.
 */
class SaveType
{
public:
    SaveType(SaveData *_sd, const char *_extension)
        : sd(_sd), extension(_extension) {}

    void listFiles(SaveFileList &l, bool showNewFile=false);

    bool operator ==(const SaveType &other) const;

    const char *getExtension() {
        return extension;
    }

private:
    friend class SaveFile;

    SaveData *sd;
    const char *extension;

    SaveFile newFile(SaveFileList &otherFiles);
};


/*
 * A single save file.
 */
class SaveFile
{
public:
    const char *getName() {
        return name.c_str();
    }

    size_t getSize() {
        return size;
    }

    time_t getTimestamp() {
        return timestamp;
    }

    bool isNew() {
        return newFile;
    }

    bool write(void *buffer, size_t size);
    bool read(void *buffer, size_t size);

    bool operator ==(const SaveFile &other) const;

private:
    friend class SaveType;

    SaveFile(SaveType *_st, std::string _name, int _size, time_t _timestamp)
        : st(_st), name(_name), size(_size), timestamp(_timestamp), newFile(false) {}

    SaveFile(SaveType *_st, std::string _name)
        : st(_st), name(_name), size(0), timestamp(time(NULL)), newFile(true) {}

    SaveType *st;
    std::string name;
    size_t size;
    time_t timestamp;
    bool newFile;
};


#endif // _SAVEDATA_H_
