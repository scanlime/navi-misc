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
SBT_DECL_PROCESS(GameEXE);
SBT_DECL_PROCESS(TutorialEXE);
SBT_DECL_PROCESS(RendererEXE);

int
main(int argc, char **argv)
{
    defaultExceptionHandler();
    consoleDemoInit();

    static GameEXE game;
    static SBTHardwareMain hwMain;
    hwMain.reset();
    game.hardware = &hwMain;
    game.exec("");

    static RendererEXE render;
    static SBTHardwareSub hwSub;
    hwSub.reset();
    render.hardware = &hwSub;
    render.exec();

    while (1) {

        /*
         * Run one normal frame.
         */
        while (game.run() != SBTHALT_FRAME_DRAWN);

        /*
         * Run one sub-screen frame.
         */
        int haltCode;

        //const RORoomId subRoom = RO_ROOM_ESC_TEXT;

            ROWorld *world = ROWorld::fromProcess(&render);
            ROCircuit *circuit = ROCircuit::fromProcess(&render);
            RORobot *robots = RORobot::fromProcess(&render);

                *world = *ROWorld::fromProcess(&game);
                *circuit = *ROCircuit::fromProcess(&game);

                memcpy(robots, RORobot::fromProcess(&game),
                       sizeof(RORobot) * RORobot::NUM_ROBOTS);

                /*
                memset(world->text.room, RO_ROOM_NONE, sizeof world->text.room);
                world->rooms.bgColor[subRoom] = 0;
                world->rooms.fgColor[subRoom] = 0;

                world->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_SCANNER_L, 40, 60);

                world->setRobotRoom(RO_OBJ_ROBOT_CHECKERS_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_CHECKERS_L, 70, 60);

                world->setRobotRoom(RO_OBJ_ROBOT_SPARKY_L, subRoom);
                world->setRobotXY(RO_OBJ_ROBOT_SPARKY_L, 100, 60);
                */

        do {
            /*
             * Move the robots into an unused room, and draw them without
             * any playfield or text.
             */

            haltCode = render.run();

            switch (haltCode) {

                /*
                 * Override the room ID.
                 */
            case SBTHALT_LOAD_ROOM_ID:
                //render.reg.al = subRoom;
                break;
            }
        } while (haltCode != SBTHALT_FRAME_DRAWN);

    }

    return 0;
}
