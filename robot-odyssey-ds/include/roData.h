/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Reverse engineered constants and memory offsets for Robot Odyssey.
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

#ifndef _RODATA_H_
#define _RODATA_H_

#include <stdint.h>
#include "sbt86.h"


/*
 * Colors
 */
enum ROColor {
    RO_COLOR_WIRE_HOT   = 5,
    RO_COLOR_WIRE_COLD  = 7,
};


/*
 * Room IDs
 */
enum RORoomId {
    RO_ROOM_SCANNER     = 0x0B,
    RO_ROOM_NONE        = 0x3F,
};


/*
 * Object IDs
 */
enum ROObjectId {
    RO_OBJ_PLAYER                     = 0x00,
    RO_OBJ_NODE_SPARKY_GRABBER_IN     = 0x1C,
    RO_OBJ_NODE_CHECKERS_GRABBER_IN   = 0x1D,
    RO_OBJ_NODE_SCANNER_GRABBER_IN    = 0x1E,
    RO_OBJ_TOOLBOX                    = 0xE1,
    RO_OBJ_CHIP1                      = 0xE8,
    RO_OBJ_CHIP2                      = 0xE9,
    RO_OBJ_CHIP3                      = 0xEA,
    RO_OBJ_CHIP4                      = 0xEB,
    RO_OBJ_CHIP5                      = 0xEC,
    RO_OBJ_CHIP6                      = 0xED,
    RO_OBJ_CHIP7                      = 0xEE,
    RO_OBJ_CHIP8                      = 0xEF,
    RO_OBJ_ROBOT_SPARKY_L             = 0xF0,
    RO_OBJ_ROBOT_SPARKY_R             = 0xF1,
    RO_OBJ_ROBOT_CHECKERS_L           = 0xF2,
    RO_OBJ_ROBOT_CHECKERS_R           = 0xF3,
    RO_OBJ_ROBOT_SCANNER_L            = 0xF4,
    RO_OBJ_ROBOT_SCANNER_R            = 0xF5,
    RO_OBJ_ANTENNA                    = 0xFC,
    RO_OBJ_CURSOR                     = 0xFE,
    RO_OBJ_NONE                       = 0xFF,
};


/*
 * Memory offsets
 */
enum ROMemOffset {
    RO_MEM_WORLD_DATA = 0x2AC,
};


/*
 * Sprites are a 16x8 bitmap
 */
typedef uint8_t ROSprite[16];

/*
 * Rooms are a 30-byte bitmap, in which each byte is a 2x4 grid of tiles.
 */
typedef uint8_t RORoomTiles[30];


/*
 * Robot Odyssey's world file data. This is in memory at the address
 * RO_MEM_WORLD_DATA, and it is the first structure in the saved game
 * and level data files.
 */
class ROWorld {
 public:
    struct {
        uint8_t nextInRoom[0x100];
        uint8_t spriteId[0x100];
        uint8_t color[0x100];
        uint8_t room[0x100];
        uint8_t x[0x100];
        uint8_t y[0x100];
        struct {
            uint8_t object[0x100];
            struct {
                uint8_t x[0x100];
                uint8_t y[0x100];
            } offset;
        } movedBy;
        uint8_t unk[0x100];
    } objects;

    ROSprite sprites[64];

    struct {
        uint8_t objectListHead[0x40];
        uint8_t bgColor[0x40];
        uint8_t fgColor[0x40];
        struct {
            uint8_t up[0x40];
            uint8_t down[0x40];
            uint8_t right[0x40];
            uint8_t left[0x40];
        } links;
        uint8_t unk[0x40];
        RORoomTiles tiles[0x40];
    } rooms;

    uint8_t unknown[0x100];

    struct {
        uint8_t room[0x80];
        uint8_t x[0x80];
        uint8_t y[0x80];
        uint8_t style[0x80];
        uint8_t font[0x80];
        uint8_t color[0x80];
        uint8_t ptrLow[0x80];
        uint8_t ptrHigh[0x80];

        uint8_t stringHeap[0x1880];
    } text;

    /*
     * Functions for accessing world data
     */

    static ROWorld *fromProcess(SBTProcess *proc);

    RORoomId getObjectRoom(ROObjectId obj);
    void getObjectXY(ROObjectId obj, int &x, int &y);

    void setObjectRoom(ROObjectId obj, RORoomId room);
    void setObjectXY(ROObjectId obj, int x, int y);

    void setRobotRoom(ROObjectId obj, RORoomId room);
    void setRobotXY(ROObjectId obj, int x, int y);

 private:
    void removeObjectFromRoom(ROObjectId obj, RORoomId room);
    void addObjectToRoom(ROObjectId obj, RORoomId room);

} __attribute__ ((packed));


#endif // _RODATA_H_
