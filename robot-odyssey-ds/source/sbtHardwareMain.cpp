/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Main implementation of SBTHardware, which emulates video using the
 * Nintendo DS's primary screen.
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
#include "panic.h"
#include "sbtHardwareMain.h"
#include "soundEngine.h"
#include "videoConvert.h"


void SBTHardwareMain::reset()
{
    SBTHardwareCommon::reset();

    // Mode 5: Two tiled 'text' layers, two extended background layers
    videoSetMode(MODE_5_2D);

    /*
     * We need a total of 192 kB of memory for the front and back
     * buffers.  Map banks A and B consecutively- this gives us 256 kB.
     */
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankB(VRAM_B_MAIN_BG_0x06020000);

    bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    backbuffer= bgGetGfxPtr(bg);
    bgSetMapBase(bg, MAP_BASE_OFFSET);
}

void SBTHardwareMain::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    VideoConvert::scaleCGAto256(framebuffer, backbuffer);

    if (!(keysHeld() & KEY_SELECT)) {
        int i = 5;
        while (i--) {
            swiWaitForVBlank();
        }
    }

    /* The current frontbuffer will be the next backbuffer. */
    backbuffer= bgGetGfxPtr(bg);

    /* Toggle between displaying the top 96kB and the bottom 96kB */
    bgSetMapBase(bg, bgGetMapBase(bg) ^ MAP_BASE_OFFSET);

    SBTHardwareCommon::drawScreen(proc, framebuffer);
}

void SBTHardwareMain::writeSpeakerTimestamp(uint32_t timestamp)
{
    SoundEngine::writeSpeakerTimestamp(timestamp);
}

void SBTHardwareMain::pollKeys()
{
    unsigned int i;
    static const struct {
        uint16_t key;
        uint16_t code;
    } keyTable[] = {

        { KEY_UP    | KEY_R,  0x4800 | '8' },
        { KEY_DOWN  | KEY_R,  0x5000 | '2' },
        { KEY_LEFT  | KEY_R,  0x4B00 | '4' },
        { KEY_RIGHT | KEY_R,  0x4D00 | '6' },

        { KEY_UP,    0x4800 },
        { KEY_DOWN,  0x5000 },
        { KEY_LEFT,  0x4B00 },
        { KEY_RIGHT, 0x4D00 },

        { KEY_B, ' ' },
        { KEY_A, 'S' },
        { KEY_X, 'C' },
        { KEY_Y, 'T' },
        { KEY_L, 'R' },
    };

    scanKeys();

    for (i = 0; i < (sizeof keyTable / sizeof keyTable[0]); i++) {
        if ((keysHeld() & keyTable[i].key) == keyTable[i].key) {
            keycode = keyTable[i].code;
            return;
        }
    }
}
