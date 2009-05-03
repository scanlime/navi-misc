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
    tutorial.exec("21");

    static TutorialEXE tutSub;
    static SBTHardwareSub hwSub;
    hwSub.reset();
    tutSub.hardware = &hwSub;
    tutSub.exec("25");

    /*
     * Run each binary until it reaches the main loop.
     * If we start patching too early, initialization
     * won't work correctly.
     */
    while (tutorial.run() != SBTHALT_FRAME_DRAWN);
    while (tutSub.run() != SBTHALT_FRAME_DRAWN);

    while (1) {
        int haltCode;

        /*
         * Run one normal frame.
         */
        while (tutorial.run() != SBTHALT_FRAME_DRAWN);

        /*
         * Run one sub-screen frame.
         */
        do {
            /*
             * Move the robots into an unused room, and draw them without
             * any playfield or text.
             */

            const RORoomId subRoom = RO_ROOM_ESC_TEXT;

            ROWorld *world = ROWorld::fromProcess(&tutSub);
            ROCircuit *circuit = ROCircuit::fromProcess(&tutSub);
            RORobot *robots = RORobot::fromProcess(&tutSub);

            haltCode = tutSub.run();

            switch (haltCode) {

                /*
                 * Override the room ID.
                 */
            case SBTHALT_LOAD_ROOM_ID:
                tutSub.reg.al = subRoom;

                world->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_SCANNER_L, 40, 60);

                world->setRobotRoom(RO_OBJ_ROBOT_CHECKERS_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_CHECKERS_L, 70, 60);

                world->setRobotRoom(RO_OBJ_ROBOT_SPARKY_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_SPARKY_L, 100, 60);

                break;

            case SBTHALT_KEYBOARD_POLL:
                /*
                 * The keyboard poll comes pretty early in the main loop.
                 * Use this opportunity to refresh the world state.
                 */

                *world = *ROWorld::fromProcess(&tutorial);
                *circuit = *ROCircuit::fromProcess(&tutorial);

                memcpy(robots, RORobot::fromProcess(&tutorial),
                       sizeof(RORobot) * RORobot::NUM_ROBOTS);

                memset(world->text.room, RO_ROOM_NONE, sizeof world->text.room);
                world->rooms.bgColor[subRoom] = 0;
                world->rooms.fgColor[subRoom] = 0;

                break;
            }
        } while (haltCode != SBTHALT_FRAME_DRAWN);

    }

    return 0;
}
