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
#include <map>

#include "sbt86.h"
#include "hwMain.h"
#include "uiSubScreen.h"
#include "uiMessageBox.h"
#include "uiList.h"
#include "hardware.h"
#include "saveData.h"
#include "videoConvert.h"


int main() {
    Hardware::init();

    SaveData sd;

    HwMainInteractive *hwMain = new HwMainInteractive();
    SBTProcess *game = new GameEXE(hwMain);

    if (!sd.init()) {
        UIMessageBox *mb = new UIMessageBox(sd.getInitErrorMessage());
        UIFade fader(SUB);
        fader.hide();
        mb->objects.push_back(&fader);
        mb->run();
        delete mb;
    }

    UISavedGameList *games = new UISavedGameList(&sd, "Load Game");
    if (games->isEmpty()) {
        UIMessageBox *mb = new UIMessageBox("Can't find any saved games!");
        mb->run();
        delete mb;
    } else {
        SaveFile file = games->run();
        delete games;
        file.loadGame(game, hwMain);
    }

    ROData gameData(game);
    UISubScreen *subScreen = new UISubScreen(&gameData, hwMain);
    UIFade gameFader(MAIN);
    subScreen->objects.push_back(&gameFader);
    gameFader.hide();
    subScreen->activate();

    while (1) {
        switch (game->run()) {

        case SBTHALT_FRAME_DRAWN:
            subScreen->renderFrame();
            break;

        }
    }

    return 0;
}
