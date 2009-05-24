/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Global hardware-specific initialization.
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
#include <new>

#include "hardware.h"
#include "panic.h"
#include "uiSubScreen.h"
#include "videoConvert.h"
#include "gfx_background.h"
#include "gfx_button_remote.h"

static void allocFailed() {
    PANIC(GENERIC_RT_ERROR, ("Memory allocation failure"));
}

extern "C" void *_sbrk_r(void *reent, int bytes) {
    /*
     * We override the default implementation of sbrk so we can PANIC
     * on allocation failure, and so we can redefine the end of the heap.
     *
     * The default sbrk in devkitARM's newlib is buggy. It uses the
     * stack pointer as its end-of-heap, which isn't right. It also
     * does not zero memory, which newlib expects. The stack isn't
     * even in mainmem, it's in DTCM. So we'd rather use the end of
     * the heap- but we also need to reserve space for the various IPC
     * regions that are at the end of main memory. The first such
     * region should be the one that our SoundEngine allocates.
     */

    const char *end = (char*) 0x027FE000;
    extern char begin asm ("__end__");
    static char *current;

    if (!current) {
        current = &begin;
    }

    void *result = (void*)current;
    current += bytes;

    if (current > end) {
        PANIC(GENERIC_RT_ERROR, ("Memory allocation failure\n\n"
                                 "sbrk(%d)\n"
                                 " min=%p\n"
                                 " cur=%p\n"
                                 " max=%p\n",
                                 bytes, &begin, current, end));
    }

    if (bytes > 0) {
        memset(result, 0, bytes);
    }

    return result;
}

void Hardware::init() {
    /*
     * Exception handlers
     */
    defaultExceptionHandler();
    std::set_new_handler(allocFailed);

    /*
     * Key repeat delay and rate
     */
    keysSetRepeat(15, 4);

    /*
     * Video mode, VRAM banks
     */
    lcdMainOnTop();
    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_5_2D);
    vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_SUB_SPRITE);

    /*
     * Palettes
     */

    static const uint32 paletteSize = 16 * sizeof(uint16_t);

    /* Sprite palette 0: VideoConvert */
    oamInit(&oamSub, SpriteMapping_1D_64, false);
    dmaCopy(VideoConvert::palette, SPRITE_PALETTE_SUB, paletteSize);

    /* Sprite palette 1: All black, for robot borders */
    dmaFillHalfWords(0x0000,
                     SPRITE_PALETTE_SUB + UIRobotIcon::OBJ_BORDER_PALETTE * 16,
                     paletteSize);

    /* Sprite palette 2: UI sprites */
    decompress(gfx_button_remotePal,
               SPRITE_PALETTE_SUB + UISpriteButton::OBJ_PALETTE * 16,
               LZ77Vram);

    /*
     * Backdrop image (sub BG3). The background's palette is shared
     * by the text layer (UIText) on both screens.
     */
    int subBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    decompress(gfx_backgroundBitmap, bgGetGfxPtr(subBg), LZ77Vram);
    decompress(gfx_backgroundPal, BG_PALETTE_SUB, LZ77Vram);
    decompress(gfx_backgroundPal, BG_PALETTE, LZ77Vram);
}
