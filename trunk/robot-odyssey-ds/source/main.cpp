/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Hardware emulation and environment support code for sbt86, an
 * experimental 8086 -> C static binary translator.
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

#include "sbt86.h"
#include "videoConvert.h"
#include "soundEngine.h"

SBT_DECL_PROCESS(LabEXE);

int
main(int argc, char **argv)
{
    consoleDemoInit();

    /*
     * We'll be using VRAM banks A and B for a double-buffered framebuffer.
     * Assign them directly to the LCD controller, bypassing the graphics engine.
     */
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);

    iprintf("Robot Odyssey DS\n"
            "(Work In Progress)\n"
            "Micah Dowty <micah@navi.cx>\n"
            "---------------------------\n");

    static LabEXE lab;

    lab.exec("30");
    lab.run();

    return 0;
}
