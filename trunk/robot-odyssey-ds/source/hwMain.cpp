/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Implementations of SBTHardware which emulate video using the
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
#include "hwMain.h"
#include "soundEngine.h"
#include "videoConvert.h"


//********************************************************** HWMain


HwMain::HwMain() {
    bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    backbuffer = bgGetGfxPtr(bg);
    bgSetMapBase(bg, MAP_BASE_OFFSET);
}

void HwMain::setOpacity(int opacity, uint16_t bgColor) {
    BG_PALETTE[0] = bgColor;
    REG_BLDCNT = (BLEND_ALPHA |
                  BLEND_SRC_BG3 |
                  BLEND_DST_BACKDROP);
    REG_BLDALPHA = opacity | (OPACITY_MAX - opacity) << 8;
}

void HwMain::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    VideoConvert::scaleCGAto256(framebuffer, backbuffer);

    /* The current frontbuffer will be the next backbuffer. */
    backbuffer = bgGetGfxPtr(bg);

    /* Toggle between displaying the top 96kB and the bottom 96kB */
    bgSetMapBase(bg, bgGetMapBase(bg) ^ MAP_BASE_OFFSET);

    HwCommon::drawScreen(proc, framebuffer);
}


//********************************************************** HWMainInteractive


void HwMainInteractive::writeSpeakerTimestamp(uint32_t timestamp) {
    SoundEngine::writeSpeakerTimestamp(timestamp);
}

void HwMainInteractive::pollKeys(SBTProcess *proc) {
    unsigned int i;
    static const struct {
        uint16_t key;
        uint16_t code;
    } keyTable[] = {

        { KEY_UP    | KEY_Y,  0x4800 | '8' },
        { KEY_DOWN  | KEY_Y,  0x5000 | '2' },
        { KEY_LEFT  | KEY_Y,  0x4B00 | '4' },
        { KEY_RIGHT | KEY_Y,  0x4D00 | '6' },

        { KEY_UP,    0x4800 },
        { KEY_DOWN,  0x5000 },
        { KEY_LEFT,  0x4B00 },
        { KEY_RIGHT, 0x4D00 },

        { KEY_B, ' ' },
    };

    HwCommon::pollKeys(proc);

    for (i = 0; i < (sizeof keyTable / sizeof keyTable[0]); i++) {
        uint16_t keys = keyTable[i].key;
        if ((keysHeld() & keys) == keys) {
            keycode = keyTable[i].code;
            return;
        }
    }
}
