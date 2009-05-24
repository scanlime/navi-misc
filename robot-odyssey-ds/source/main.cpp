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

SBT_DECL_PROCESS(MenuEXE);
SBT_DECL_PROCESS(LabEXE);
SBT_DECL_PROCESS(GameEXE);
SBT_DECL_PROCESS(TutorialEXE);

int main() {
    Hardware::init();

    HwMain *hwMain = new HwMain();
    SBTProcess *game = new GameEXE(hwMain);

    SaveData sd;

    if (!sd.init()) {
        UIMessageBox *mb = new UIMessageBox(sd.getInitErrorMessage());
        UIFade fader(fader.SUB);
        fader.hide();
        mb->objects.push_back(&fader);
        mb->run();
        delete mb;
    } else {
        SaveType gameSaves(&sd, ".gsv");
        SaveFileList saves;
        SaveFileList::iterator iter;

        gameSaves.listFiles(saves, true);

        UIListWithRobot *list = new UIListWithRobot();

        for (iter = saves.begin(); iter != saves.end(); iter++) {
            UIFileListItem *item = new UIFileListItem();

            if (iter->isNew()) {
                item->setText(item->TEXT_CENTER, "New File");

            } else if (iter->getSize() == sizeof(ROSavedGame)) {
                char buf[80];
                time_t ts = iter->getTimestamp();
                strftime(buf, sizeof buf, "%Y-%m-%d %H:%M", gmtime(&ts));
                item->setText(item->TEXT_BOTTOM_RIGHT, "%s", buf);

                ROSavedGame *save = new ROSavedGame;
                if (iter->read(save, sizeof *save)) {
                    item->setText(item->TEXT_BOTTOM_LEFT, "%s",
                                  save->getWorldName());
                }
                delete save;

                item->setText(item->TEXT_TOP_LEFT, "%s", iter->getName());

                item->file = &*iter;
            }

            list->append(item);
        }

        list->show();
        list->activate();

        UIListItem *current = list->getCurrentItem();

        while (!list->isHidden()) {
            if (list->getCurrentItem() != current) {
                current = list->getCurrentItem();
                UIFileListItem *fileItem = static_cast<UIFileListItem*>(current);

                fileItem->file->read(&hwMain->fs.saveFile, sizeof hwMain->fs.saveFile);
                game->exec("99");
            }

            game->run();
        }

        list->deactivate();
        delete list;
    }

    ROData gameData(game);
    UISubScreen *subScreen = new UISubScreen(&gameData, hwMain);
    UIFade gameFader(gameFader.MAIN);
    subScreen->objects.push_back(&gameFader);
    gameFader.hide();
    subScreen->activate();

    while (1) {
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
