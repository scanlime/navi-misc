#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <loader-exit.h>

int main(void)
{
    irqInit();
    irqEnable(IRQ_VBLANK);
    powerSET(POWER_LCD | POWER_2D_B);
    videoSetMode(0);
    consoleDemoInit();

    iprintf("Hello World!\n"
	    "Press Start to exit.\n\n");

    while (1) {
	swiWaitForVBlank();
	scanKeys();

	if (keysHeld() == KEY_START) {
	    Loader_Exit9();
	    iprintf("No loader found!\n");
	}
    }

    return 0;
}
