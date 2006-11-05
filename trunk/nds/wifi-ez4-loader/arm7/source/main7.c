/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * main7.c - Main ARM7 source file for the Wifi EZ4 Loader
 *
 * Copyright (C) 2006 Micah Dowty <micah@navi.cx>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <nds.h>
#include <nds/bios.h>
#include <dswifi7.h>
#include "ez4loader.h"

/* Internal functions from libdswifi7 */
void SetLedState(int state);
void Wifi_Stop();
void Wifi_Shutdown();

static int gba_mode_flag = 0;

void wifi_sync_handler()
{
    /* This is called by libdswifi in order to invoke Wifi_Sync() on the ARM9 */
    REG_IPC_FIFO_TX = IPC_MSG_WIFI_SYNC;
}

void fifo_irq_handler()
{
    uint32 message = REG_IPC_FIFO_RX;
    switch (message) {

    case IPC_MSG_WIFI_SYNC:
	Wifi_Sync();
	break;

    case IPC_MSG_GBA_MODE:
	gba_mode_flag = 1;
	break;

    }
}

uint32 fifo_rx_wait()
{
    /* Perform a blocking read from the FIFO */
    while (REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY) {
	swiWaitForVBlank();
    }
    return REG_IPC_FIFO_RX;
}

void arm7_reboot()
{
    uint32 addr;
    int pmflags;
    
    /* Turn off the wifi */
    Wifi_Stop();
    Wifi_Shutdown();
    POWER_CR = 1;

    /* Default power management flags */
    pmflags = PM_SOUND_PWR | PM_SOUND_VOL | 
	PM_BACKLIGHT_TOP | PM_BACKLIGHT_BOTTOM;

    if (gba_mode_flag) {
	/* Turn off the unused screen in GBA mode */
	if (PersonalData->gbaScreen) {
	    pmflags &= ~PM_BACKLIGHT_TOP;
	} else {
	    pmflags &= ~PM_BACKLIGHT_BOTTOM;
	}
    }

    writePowerManagement(0, pmflags);

    /* Stop blinking the LED */
    SetLedState(0);

    /* Disable IRQs */
    REG_IME = 0;
    REG_IF = 0;
          
    /* Zero out DMA channel registers */
    for (addr = 0x040000B0; addr <= 0x040000E0; addr += 4) {
	*(vuint32*)addr = 0;
    }

    /* Zero out timer registers */
    for (addr = 0x04000100; addr <= 0x04000110; addr += 2) {
	*(vuint16*)addr = 0;
    }

    if (gba_mode_flag) {
	swiSwitchToGBAMode();
    } else {
	swiSoftReset();
    }
}

void vblank_irq_handler()
{
    Wifi_Update();
    IPC->buttons = REG_KEYXY;
    IPC->heartbeat++;

    /*
     * If the ARM9 is stuck in its reboot loop,
     * begin the ARM7's half of the reboot process.
     */
    if (NDSHeader.arm9executeAddress == (uint32) BOOT_ARM9_LOOP_ADDRESS) {
	arm7_reboot();
    }
}

int main()
{
    irqInit();
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

    /* We're using the top screen only */
    writePowerManagement(0, PM_BACKLIGHT_TOP);

    /*
     * Initialize interrupts that can occur before Wifi is ready
     */
    irqSet(IRQ_VBLANK, vblank_irq_handler);
    irqEnable(IRQ_VBLANK);
    irqSet(IRQ_WIFI, Wifi_Interrupt);
    irqEnable(IRQ_WIFI);

    /*
     * Initialization handshake with the ARM9
     */
    while (fifo_rx_wait() != IPC_MSG_WIFI_INIT);
    Wifi_Init(fifo_rx_wait());

    /*
     * Pass all future FIFO commands to the interrupt handler
     */
    irqSet(IRQ_FIFO_NOT_EMPTY, fifo_irq_handler);
    irqEnable(IRQ_FIFO_NOT_EMPTY);
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;

    /*
     * Now it's safe for libdswifi to send its own messages to the ARM9
     */
    Wifi_SetSyncHandler(wifi_sync_handler);

    /*
     * Main loop. Everything from now on happens via an IRQ handler.
     */
    while (1) {
	swiWaitForVBlank();
    }
}

