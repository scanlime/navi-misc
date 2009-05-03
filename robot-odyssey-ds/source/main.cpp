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
#include "hwSub.h"
#include "hwMain.h"
#include "hwSprites.h"
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

    static HwMain hwMain;
    static TutorialEXE game;
    hwMain.reset();
    game.hardware = &hwMain;
    game.exec("25");

    static HwSprites hwSub;
    static RendererEXE render;
    hwSub.reset();
    render.hardware = &hwSub;
    render.exec();

    ROData gameData(&game);
    ROData renderData(&render);

    /* XXX: Setup for Scanner to grab an object */
    while (game.run() != SBTHALT_FRAME_DRAWN);
    gameData.world->setObjectRoom(RO_OBJ_WORLD_0,
                             (RORoomId) gameData.world->rooms.links.right[
                                 gameData.world->getObjectRoom(RO_OBJ_PLAYER)]);
    gameData.world->setObjectXY(RO_OBJ_WORLD_0, 20, 160);

    while (1) {
        // XXX: Energize Scanner's grabber.
        gameData.world->objects.color[RO_OBJ_NODE_SCANNER_GRABBER_IN] =
            RO_COLOR_WIRE_HOT;

        /*
         * Run one normal frame.
         */
        while (game.run() != SBTHALT_FRAME_DRAWN);

        /*
         * Run one sub-screen frame.
         */
        int haltCode = SBTHALT_FRAME_DRAWN;

        const RORoomId subRoom = RO_ROOM_ESC_TEXT;

        renderData.copyFrom(&gameData);

        renderData.world->rooms.bgColor[subRoom] = 0;
        renderData.world->rooms.fgColor[subRoom] = 0;

        renderData.world->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
        renderData.world->setRobotXY(RO_OBJ_ROBOT_SCANNER_L, 40, 60);

        renderData.world->setRobotRoom(RO_OBJ_ROBOT_CHECKERS_L, subRoom);
        renderData.world->setRobotXY(RO_OBJ_ROBOT_CHECKERS_L, 70, 60);

        renderData.world->setRobotRoom(RO_OBJ_ROBOT_SPARKY_L, subRoom);
        renderData.world->setRobotXY(RO_OBJ_ROBOT_SPARKY_L, 100, 60);

        do {
            haltCode = render.run();

            if (haltCode == SBTHALT_LOAD_ROOM_ID) {
                render.reg.al = subRoom;
            }

        } while (haltCode != SBTHALT_FRAME_DRAWN);

    }

    return 0;
}
