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
#include "hwSpriteScraper.h"
#include "videoConvert.h"
#include "roData.h"
#include "mSprite.h"
#include "uiBase.h"
#include "textRenderer.h"
#include "gfx_background.h"
#include "gfx_button_remote.h"

SBT_DECL_PROCESS(LabEXE);
SBT_DECL_PROCESS(GameEXE);
SBT_DECL_PROCESS(TutorialEXE);
SBT_DECL_PROCESS(RendererEXE);

static TextRenderer text;


class MyObject : public UIObject {
    virtual void handleInput(const UIInputState &input) {
        text.moveTo(10,10);
        text.setAlignment(text.LEFT);
        text.printf("keys %08x\n", input.keysHeld);
    }
};

static void subScreenInit(void) {
    videoSetModeSub(MODE_5_2D);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    vramSetBankD(VRAM_D_SUB_SPRITE);

    oamInit(&oamSub, SpriteMapping_1D_64, false);
    memcpy(SPRITE_PALETTE_SUB, VideoConvert::palette, sizeof VideoConvert::palette);

    int subBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    decompress(gfx_backgroundBitmap, bgGetGfxPtr(subBg), LZ77Vram);
    decompress(gfx_backgroundPal, BG_PALETTE_SUB, LZ77Vram);
}

int main() {
    defaultExceptionHandler();
    subScreenInit();

    UIObjectList ol;
    MSpriteAllocator sprAlloc(&oamSub);

    text.init();
    text.setAlignment(text.CENTER);
    text.moveTo(128, 64);
    text.printf("Hello World!\n");
    text.printf("I am a text\n");
    text.printf("Font is Copyright ~ 2001\n");
    text.printf("http://www.orgdot.com\n");

    static HwMain hwMain;
    static GameEXE game;
    hwMain.reset();
    game.hardware = &hwMain;
    game.exec("");

    static HwSpriteScraper hwSub;
    static RendererEXE render;
    hwSub.reset();
    render.hardware = &hwSub;
    render.exec();

    ROData gameData(&game);
    ROData renderData(&render);

    MSprite button(&sprAlloc);
    /* Enough room for 4 images */
    uint16_t *buttonImg = oamAllocateGfx(&oamSub, SpriteSize_64x64,
                                         SpriteColorFormat_16Color);
    button.newOBJ(MSPRR_UI, 0, 0, buttonImg, SpriteSize_32x32,
                  SpriteColorFormat_16Color)->entry->palette = 2;
    decompress(gfx_button_remoteTiles, buttonImg, LZ77Vram);
    decompress(gfx_button_remotePal, SPRITE_PALETTE_SUB + 2*16, LZ77Vram);
    button.moveTo(10,120);
    button.show();

    SpriteScraperRect *r1 = hwSub.allocRect(&oamSub);
    MSprite sprite(&sprAlloc);

    ol.objects.push_back(new MyObject);
    ol.makeCurrent();

    sprite.newOBJ(MSPRR_FRONT_BORDER, -32+1, -32, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32-1, -32, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32, -32+1, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT_BORDER, -32, -32-1, r1->buffer, r1->size, r1->format)->entry->palette = 1;
    sprite.newOBJ(MSPRR_FRONT, -32, -32, r1->buffer, r1->size, r1->format);

    sprite.setIntrinsicScale(r1->scaleX, r1->scaleY);
    sprite.moveTo(50, 50);
    sprite.show();

    

    while (1) {
        touchPosition touch;
        touchRead(&touch);
        if (touch.rawx && touch.rawy) {
            static int angle = 0;
            angle += 0x500;
            int scale = 0x100 - (abs(sinLerp(angle)) >> 6);
            sprite.moveTo(touch.px, touch.py);
            sprite.setScale(scale, scale);
            sprite.show();
        }

        static int frame;
        frame++;
        button.obj[0].setGfx(buttonImg + ((frame & 3) << 8));

        /*
         * Run one normal frame.
         */
        while (game.run() != SBTHALT_FRAME_DRAWN);

        /*
         * Run one sub-screen frame.
         */
        int haltCode = SBTHALT_FRAME_DRAWN;

        const RORoomId subRoom = RO_ROOM_RENDERER;

        renderData.copyFrom(&gameData);

        renderData.world->setRobotRoom(RO_OBJ_ROBOT_SCANNER_L, subRoom);
        renderData.world->setRobotXY(RO_OBJ_ROBOT_SCANNER_L,
                                     r1->centerX() - 6, 192 - r1->centerY() - 8);

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

        oamUpdate(&oamSub);
    }

    return 0;
}
