/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * main7.c - Main ARM7 source file for the Wifi EEPROM dumper
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
#include "ipc-messages.h"


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

void vblank_irq_handler()
{
    Wifi_Update();
    IPC->buttons = REG_KEYXY;
    IPC->heartbeat++;
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

