/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Main loop and initialization for Robot Odyssey DS.
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
#include <stdio.h>
#include <string.h>

#include "sbt86.h"
#include "sbtHardwareSub.h"
#include "sbtHardwareMain.h"
#include "roData.h"

SBT_DECL_PROCESS(LabEXE);
SBT_DECL_PROCESS(TutorialEXE);

int
main(int argc, char **argv)
{
    defaultExceptionHandler();
    consoleDemoInit();

    static TutorialEXE tutorial;
    static SBTHardwareMain hwMain;
    hwMain.reset();
    tutorial.hardware = &hwMain;
    tutorial.exec("25");

    static TutorialEXE tutSub;
    static SBTHardwareSub hwSub;
    hwSub.reset();
    tutSub.hardware = &hwSub;
    tutSub.exec("25");

    /* Let both binaries initialize themselves */
    while (tutorial.run() != SBTHALT_FRAME_DRAWN);
    while (tutSub.run() != SBTHALT_FRAME_DRAWN);

    while (1) {
        while (tutorial.run() != SBTHALT_FRAME_DRAWN);

        /*
         * We don't want to copy the whole data segment, just copy the
         * world data. This is faster, and it doesn't clobber the
         * robot animation state.
         *
         * Note that we also must copy circuit data, or the sub
         * process can hang when it tries to simulate a circuit which
         * doesn't match the world data. This is also where we get
         * things like robot thruster state, and remote control state.
         */
        ROWorld *wld = ROWorld::fromProcess(&tutSub);
        *wld = *ROWorld::fromProcess(&tutorial);

        *ROCircuit::fromProcess(&tutSub) = *ROCircuit::fromProcess(&tutorial);

        /*
         * Move the robots into an unused room, and draw them without
         * any playfield or text.
         */
        const RORoomId subRoom = RO_ROOM_ESC_TEXT;

        while (tutSub.run() != SBTHALT_LOAD_ROOM_ID);
        tutSub.reg.al = subRoom;

        memset(wld->text.room, RO_ROOM_NONE, sizeof wld->text.room);
        wld->rooms.bgColor[subRoom] = 0;
        wld->rooms.fgColor[subRoom] = 0;

        wld->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
        wld->setRobotXY(RO_OBJ_ROBOT_SCANNER_L, 20, 100);

        wld->setRobotRoom(RO_OBJ_ROBOT_CHECKERS_L, subRoom);
        wld->setRobotXY(RO_OBJ_ROBOT_CHECKERS_L, 50, 100);

        wld->setRobotRoom(RO_OBJ_ROBOT_SPARKY_L, subRoom);
        wld->setRobotXY(RO_OBJ_ROBOT_SPARKY_L, 80, 100);

        while (tutSub.run() != SBTHALT_FRAME_DRAWN);
    }

    return 0;
}
