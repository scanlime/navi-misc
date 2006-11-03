/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * main9.c - Main ARM9 source file for the Wifi EZ4 Loader
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
#include <nds/dma.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ez4loader.h"

#define  DMA_CHANNEL    3
#define  WIFI_TIMER_MS  50
#define  TCP_PORT       6502

void * sgIP_malloc(int size)
{ 
    return malloc(size);
}

void sgIP_free(void * ptr)
{
    free(ptr);
}

void wifi_timer_handler(void)
{
    Wifi_Timer(WIFI_TIMER_MS);
}

void wifi_sync_handler()
{
    /* This is called by libdswifi in order to invoke Wifi_Sync() on the ARM7 */
    REG_IPC_FIFO_TX = IPC_MSG_WIFI_SYNC;
}


void fifo_irq_handler()
{
    uint32 message = REG_IPC_FIFO_RX;
    switch (message) {

    case IPC_MSG_WIFI_SYNC:
	Wifi_Sync();

    }
}

void vblank_irq_handler()
{
    Wifi_Update();
}

void video_init()
{
    powerSET(POWER_LCD | POWER_2D_B);
    lcdMainOnBottom();
    videoSetMode(0);
    consoleDemoInit();

    iprintf("Wifi EZ4 Loader\nMicah Dowty <micah@navi.cx>\n\n");
}

void wifi_init()
{
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

    /*
     * Our half of the ARM7 Wifi handshaking:
     * Send it a pointer to the shared memory area for Wifi.
     */
    REG_IPC_FIFO_TX = IPC_MSG_WIFI_INIT;
    REG_IPC_FIFO_TX = Wifi_Init(WIFIINIT_OPTION_USELED);

    /* Disable TIMER3 before setting up its IRQ handler */
    TIMER3_CR = 0;

    irqInit(); 
    irqSet(IRQ_VBLANK, vblank_irq_handler);
    irqEnable(IRQ_VBLANK);
    irqSet(IRQ_TIMER3, wifi_timer_handler);
    irqEnable(IRQ_TIMER3);
    irqSet(IRQ_FIFO_NOT_EMPTY, fifo_irq_handler);
    irqEnable(IRQ_FIFO_NOT_EMPTY);

    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ;
    Wifi_SetSyncHandler(wifi_sync_handler);

    /* Program TIMER3 for 50ms intervals */
    TIMER3_DATA = TIMER_FREQ_256(1000 / WIFI_TIMER_MS);
    TIMER3_CR = TIMER_DIV_256 | TIMER_IRQ_REQ;

    /* Poll for the ARM7 init to finish */
    while (!Wifi_CheckInit()) {
	swiWaitForVBlank();
    }
}

int wifi_connect()
{
    iprintf("Connecting with WFC settings...\n");
  
    /* Begin connecting asynchronously */
    Wifi_AutoConnect();

    while (1) {
	switch (Wifi_AssocStatus()) {

	case ASSOCSTATUS_ASSOCIATED:
	    return 1;

	case ASSOCSTATUS_CANNOTCONNECT:
	    return 0;
	}
    }
}

void ezflash4_set_write_protect(int enable)
{
    /*
     * This magical sequence of writes does in fact
     * appear to lock/unlock the PSRAM on the EZFlash 4
     * Lite Deluxe. This is a slightly modified version
     * of the sequence appearing in Moonshell's EZ4
     * NDS loader.
     *
     * Of course, this sequence also corrupts various
     * values in ROM if WP is already off- so
     * it's really only useful eith enable==0.
     */
    *(vuint16 *)0x9fe0000 = 0xd200;
    *(vuint16 *)0x8000000 = 0x1500;
    *(vuint16 *)0x8020000 = 0xd200;
    *(vuint16 *)0x8040000 = 0x1500;
    *(vuint16 *)0x9C40000 = enable ? 0xd200 : 0x1500;
    *(vuint16 *)0x9fc0000 = 0x1500;
}

void arm9_reboot(void)
{
    /* Disable IRQs */
    REG_IME = 0;
    REG_IF = 0;

    /* A little video resetting */
    videoSetMode(0);
    videoSetModeSub(0);

    /*
     * Initialize the ARM9 loop.
     * Our ARM7 is polling for this- as soon as we set the
     * entry point address here, arm7_reboot() will begin.
     */
    BOOT_NDS_HEADER->arm9_loop = BOOT_ARM9_LOOP_INSTRUCTION;
    BOOT_NDS_HEADER->arm9_execAddr = (void*) &BOOT_NDS_HEADER->arm9_loop;

    swiSoftReset();
}

int main(void)
{
    struct sockaddr_in addr;
    int s, conn, addrSize;
    int nonblock = 1;
    static unsigned char buffer[64*1024];
    uint32 my_ip;
    uint32 offset = 0;
    uint residual = 0;
    uint32 last_displayed_offset = 0;

    video_init();
    wifi_init();
    while (!wifi_connect());
    my_ip = Wifi_GetIP();

    /*
     * Listen for TCP/IP connections
     */
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    s = socket(AF_INET, SOCK_STREAM, 0);
    bind(s, (struct sockaddr *) &addr, sizeof addr);
    listen(s, 2);
    ioctl(s, FIONBIO, &nonblock);

    iprintf("Listening on %d.%d.%d.%d:%d\n",
	    my_ip & 0xFF,
	    (my_ip >> 8) & 0xFF,
	    (my_ip >> 16) & 0xFF,
	    (my_ip >> 24) & 0xFF,
	    TCP_PORT);

    while (1) {
	conn = accept(s, (struct sockaddr *) &addr, &addrSize);
	if (conn >= 0) {
	    break;
	}
	swiWaitForVBlank();
    }
    ioctl(conn, FIONBIO, &nonblock);
    iprintf("Accepted %d.%d.%d.%d:%d\n",
	    addr.sin_addr.s_addr & 0xFF,
	    (addr.sin_addr.s_addr >> 8) & 0xFF,
	    (addr.sin_addr.s_addr >> 16) & 0xFF,
	    (addr.sin_addr.s_addr >> 24) & 0xFF,
	    ntohs(addr.sin_port));

    sysSetCartOwner(1);              /* Give the ARM9 ownership of the GBA_BUS */
    ezflash4_set_write_protect(0);   /* Allow writes to the GBA "ROM", actually SRAM on the EZ4 */

    offset = last_displayed_offset = 0;
    while (1) {
	int result = recv(conn, buffer + residual, sizeof buffer - residual, 0);
	if (result == 0) {
	    close(conn);
	    break;
	}
	if (result < 0) {
	    swiWaitForVBlank();
	    continue;
	}

	/*
	 * Start an asynchronous DMA transfer into the PSRAM.
	 * This will round down to the nearest half-word.
	 */
	while (dmaBusy(DMA_CHANNEL));
	dmaCopyHalfWordsAsynch(DMA_CHANNEL, buffer, offset + (void*)GBA_BUS, result);
	offset += result;

	/*
	 * If we had a leftover byte after the DMA, save it for later
	 */
	if (result & 1) {
	    buffer[0] = buffer[result-1];
	    residual = 1;
	} else {
	    residual = 0;
	}

	if (offset - last_displayed_offset > 1024) {
	    iprintf("(%d bytes)\x1b[60D", offset);
	    last_displayed_offset = offset;
	}
    }
      
    iprintf("Finished. (%d bytes)\n", offset);

    /*Wifi_Init(WIFIINIT_OPTION_USELED); Finish up with the GBA bus */
    while (dmaBusy(DMA_CHANNEL));
    sysSetCartOwner(0);

    iprintf("\nPress any key to reboot\n");
    do {
	scanKeys();
    } while (!keysHeld());
  
    iprintf("Rebooting...\n");
    arm9_reboot();

    return 0;
}
