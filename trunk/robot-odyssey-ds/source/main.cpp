/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Main loop and initialization for Robot Odyssey DS.
 *
 * XXX: This whole file is a huge hack right now, just a test rig for
 *      prototyping other modules.
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
#include "sbt86.h"
#include "hwMain.h"
#include "uiSubScreen.h"

SBT_DECL_PROCESS(LabEXE);
SBT_DECL_PROCESS(GameEXE);
SBT_DECL_PROCESS(TutorialEXE);
SBT_DECL_PROCESS(RendererEXE);


int main() {
    defaultExceptionHandler();

    static TutorialEXE game;
    static HwMain hwMain;

    hwMain.reset();
    game.hardware = &hwMain;
    game.exec("25");

    ROData gameData(&game);
    UISubScreen subScreen(&gameData, &hwMain);

#if 0
    static HwSpriteScraper hwSub;
    static RendererEXE render;
    hwSub.reset();
    render.hardware = &hwSub;
    render.exec();

    ROData renderData(&render);

    SpriteScraperRect *r1 = hwSub.allocRect(&oamSub);
    MSprite sprite(&sprAlloc);

    ol.makeCurrent();

    sprite.newOBJ(MSPRR_FRONT_BORDER, -32+1, -32, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32-1, -32, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32, -32+1, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32, -32-1, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT, -32, -32, r1->buffer, r1->size, r1->format);

    sprite.setIntrinsicScale(r1->scaleX, r1->scaleY);
    sprite.moveTo(128, 64);
    sprite.show();
#endif

    while (1) {
        /*
         * Run one normal frame.
         */
        while (game.run() != SBTHALT_FRAME_DRAWN);

#if 0
        /*
         * Run one sub-screen frame.
         */
        int haltCode = SBTHALT_FRAME_DRAWN;

        const RORoomId subRoom = RO_ROOM_RENDERER;

        renderData.copyFrom(&gameData);

        renderData.world->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
        renderData.world->setRobotXY(RO_OBJ_ROBOT_SCANNER_L,
                                     r1->centerX() - 6, r1->centerY() - 8);

        /* Draw robot power levels */
        //        text.moveTo(50, 60);
        //        text.setAlignment(text.CENTER);
        //        text.printf("%04x\n", gameData.robots.batteryAcc[2].get());

        do {
            haltCode = render.run();

            if (haltCode == SBTHALT_LOAD_ROOM_ID) {
                render.reg.al = subRoom;
            }

        } while (haltCode != SBTHALT_FRAME_DRAWN);

#endif
    }

    return 0;
}
