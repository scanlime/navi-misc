/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Entry point for Robot Odyssey DS.
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
#include <fat.h>
#include <dirent.h>

#include "sbt86.h"
#include "hwMain.h"
#include "uiSubScreen.h"
#include "hardware.h"

SBT_DECL_PROCESS(MenuEXE);
SBT_DECL_PROCESS(LabEXE);
SBT_DECL_PROCESS(GameEXE);
SBT_DECL_PROCESS(TutorialEXE);

int main() {
    Hardware::init();

    HwMain *hwMain = new HwMain();
    SBTProcess *game = new GameEXE(hwMain, "99");

    ROData gameData(game);

    UISubScreen *subScreen = new UISubScreen(&gameData, hwMain);
    subScreen->activate();

    if (!fatInitDefault()) {
        subScreen->text.printf("No filesystem!\n");
    } else {
        subScreen->text.printf("Filesystem OK\n");
#if 0
        DIR *d = opendir("/");
        if (d) {
            struct dirent *pent;
            while ((pent = readdir(d))) {
                subScreen->text.printf("%s\n", pent->d_name);
            }
        }
#endif
    }

    while (1) {

        static int i;
        i++;

        if ((i & 7) == 0) {
            subScreen->deactivate();
            delete subScreen;
            subScreen = new UISubScreen(&gameData, hwMain);
            subScreen->activate();
        }

        subScreen->text.drawFrame(Rect(20, 20, 120, 30));
        subScreen->text.setBackgroundOpaque(true);
        subScreen->text.moveTo(25,25);
        subScreen->text.printf("%d %p\n", i, subScreen);
        subScreen->text.blit();

        switch (game->run() & SBTHALT_MASK_CODE) {

        case SBTHALT_FRAME_DRAWN:
            subScreen->renderFrame();
            break;

        case SBTHALT_DOS_EXIT:
            subScreen->text.printf("DOS EXIT\n");
            while (1);
            break;
        }
    }

    return 0;
}
