/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Robot Odyssey DS: Main program for the ARM7.
 *
 * This does the usual libnds things (polling input, handling FIFO
 * commands), plus we start up our SoundEngine on the ARM7.
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
#include "soundEngine.h"

static void
isrVCount()
{
    inputGetAndSend();
}

int
main()
{
    irqInit();
    fifoInit();
    readUserSettings();
    initClockIRQ();
    SetYtrigger(80);
    installSystemFIFO();

    irqSet(IRQ_VCOUNT, isrVCount);
    irqEnable(IRQ_VCOUNT);

    SoundEngine::init();

    while (1) {
        swiWaitForVBlank();
    }

    return 0;
}
