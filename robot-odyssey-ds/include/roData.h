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
    RO_ROOM_ESC_TEXT    = 0x00,   // "To go back to the menu, press ESC."
    RO_ROOM_SPARKY      = 0x09,   // Inside Sparky
    RO_ROOM_CHECKERS    = 0x0A,   // Inside Checkers
    RO_ROOM_SCANNER     = 0x0B,   // Inside Scanner
    RO_ROOM_NONE        = 0x3F,
};


/*
 * Object IDs
 */
enum ROObjectId {
    RO_OBJ_PLAYER                     = 0x00,

    RO_OBJ_ROBOT_MC_L                 = 0x01,  // Master Computer only in GAME.EXE
    RO_OBJ_ROBOT_MC_R                 = 0x02,

    RO_OBJ_NODE_SPARKY_GRABBER_IN     = 0x1C,
    RO_OBJ_NODE_CHECKERS_GRABBER_IN   = 0x1D,
    RO_OBJ_NODE_SCANNER_GRABBER_IN    = 0x1E,

    RO_OBJ_MC_THRUSTER_1              = 0x39,  // Master Computer only in GAME.EXE
    RO_OBJ_MC_THRUSTER_2              = 0x3A,
    RO_OBJ_MC_THRUSTER_3              = 0x3B,
    RO_OBJ_MC_THRUSTER_4              = 0x3C,
    RO_OBJ_MC_BUMPER_1                = 0x3D,
    RO_OBJ_MC_BUMPER_2                = 0x3E,
    RO_OBJ_MC_BUMPER_3                = 0x3F,
    RO_OBJ_MC_BUMPER_4                = 0x40,

    RO_OBJ_SCANNER_THRUSTER_1         = 0x41,
    RO_OBJ_SCANNER_THRUSTER_2         = 0x42,
    RO_OBJ_SCANNER_THRUSTER_3         = 0x43,
    RO_OBJ_SCANNER_THRUSTER_4         = 0x44,
    RO_OBJ_SCANNER_BUMPER_1           = 0x45,
    RO_OBJ_SCANNER_BUMPER_2           = 0x46,
    RO_OBJ_SCANNER_BUMPER_3           = 0x47,
    RO_OBJ_SCANNER_BUMPER_4           = 0x48,

    RO_OBJ_CHECKERS_THRUSTER_1        = 0x49,
    RO_OBJ_CHECKERS_THRUSTER_2        = 0x4A,
    RO_OBJ_CHECKERS_THRUSTER_3        = 0x4B,
    RO_OBJ_CHECKERS_THRUSTER_4        = 0x4C,
    RO_OBJ_CHECKERS_BUMPER_1          = 0x4D,
    RO_OBJ_CHECKERS_BUMPER_2          = 0x4E,
    RO_OBJ_CHECKERS_BUMPER_3          = 0x4F,
    RO_OBJ_CHECKERS_BUMPER_4          = 0x50,

    RO_OBJ_SPARKY_THRUSTER_1          = 0x51,
    RO_OBJ_SPARKY_THRUSTER_2          = 0x52,
    RO_OBJ_SPARKY_THRUSTER_3          = 0x53,
    RO_OBJ_SPARKY_THRUSTER_4          = 0x54,
    RO_OBJ_SPARKY_BUMPER_1            = 0x55,
    RO_OBJ_SPARKY_BUMPER_2            = 0x56,
    RO_OBJ_SPARKY_BUMPER_3            = 0x57,
    RO_OBJ_SPARKY_BUMPER_4            = 0x58,

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
 * Sprites are a 16x8 bitmap
 */
typedef uint8_t ROSprite[16];

/*
 * Rooms are a 30-byte bitmap, in which each byte is a 2x4 grid of tiles.
 */
typedef uint8_t RORoomTiles[30];


/*
 * Robot Odyssey's world file data. This is in memory at the address
 * SBTADDR_WORLD_DATA, and it is the first structure in the saved game
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


/*
 * Robot Odyssey's circuit data. This is in memory at the address
 * SBTADDR_CIRCUIT_DATA, and it is also saved on disk as the .CIR file
 * or as part of a saved game file.
 */
class ROCircuit {
 public:
    /* XXX: Todo */

    uint8_t unk1[0x100];

    struct {
        uint8_t x1[0x100];
        uint8_t x2[0x100];
        uint8_t y1[0x100];
        uint8_t y2[0x100];
    } wires;

    uint8_t unk2[0x100];

    uint8_t unk_byte_1;
    uint8_t unk_byte_2;
    uint8_t unk_byte_3;
    uint8_t unk_byte_4;
    uint8_t unk_byte_5;
    uint8_t unk_byte_6;
    uint8_t unk_byte_7;
    uint8_t unk_byte_8;
    uint8_t unk_byte_9;
    uint8_t unk_byte_10;
    uint8_t unk_byte_11;
    uint8_t unk_byte_12;
    uint8_t unk_byte_13;
    uint8_t unk_byte_14;
    uint8_t unk_byte_15;
    uint8_t remoteControlFlag;
    uint8_t unk_byte_17;
    uint8_t unk_byte_18;
    uint8_t unk_byte_19;
    uint8_t toolboxIsClosed;

    /*
     * Functions for accessing circuit data
     */

    static ROCircuit *fromProcess(SBTProcess *proc);

} __attribute__ ((packed));


/*
 * Robot Odyssey's per-robot internal data. This contains thruster
 * state, bumper state, thruster switch state, etc. It includes state
 * that can't be recovered from just a world file.
 */
class RORobot {
 public:
    uint8_t objLeft;           // Left half of robot
    uint8_t objLeft2;          // (duplicate)
    uint8_t objRight;          // Right half of robot
    uint8_t objRight2;         // (duplicate)

    uint8_t objThrusters[4];
    uint8_t objBumpers[4];

    uint8_t thrusterState[4];  // Animation frame #, or 0 if off
    uint8_t bumperState[4];    // Color ID
    uint8_t grabberState[4];

    uint8_t grabEnableCount;
    uint8_t thrusterSwitch;

    /*
     * Functions for accessing robot data
     */

    static const unsigned NUM_ROBOTS = 4;
    static RORobot *fromProcess(SBTProcess *proc);

} __attribute__ ((packed));


#endif // _RODATA_H_
