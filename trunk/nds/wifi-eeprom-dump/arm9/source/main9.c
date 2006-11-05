/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * main9.c - Main ARM9 source file for the Wifi EEPROM dumper
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
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ipc-messages.h"
#include "cardme.h"

#define  WIFI_TIMER_MS  50
#define  TCP_PORT       4000

typedef struct {
    int fd;
    unsigned char buffer[512];
    int card_type;
    int card_size;
    int address;
} Connection;


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

    iprintf("Wifi EEPROM Dumper\nMicah Dowty <micah@navi.cx>\n\n");
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

int wifi_listen()
{
    struct sockaddr_in addr;
    int s;
    uint32 my_ip;

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_PORT);
    s = socket(AF_INET, SOCK_STREAM, 0);
    bind(s, (struct sockaddr *) &addr, sizeof addr);
    listen(s, 2);

    my_ip = Wifi_GetIP();
    iprintf("Listening on %d.%d.%d.%d:%d\n",
	    my_ip & 0xFF,
	    (my_ip >> 8) & 0xFF,
	    (my_ip >> 16) & 0xFF,
	    (my_ip >> 24) & 0xFF,
	    TCP_PORT);

    return s;
}

void connection_accept(Connection *self, int socket)
{
    struct sockaddr_in addr;
    int addrSize;

    memset(self, 0, sizeof *self);

    self->fd = accept(socket, (struct sockaddr *) &addr, &addrSize);
    iprintf("\nAccepted %d.%d.%d.%d:%d\n",
	    addr.sin_addr.s_addr & 0xFF,
	    (addr.sin_addr.s_addr >> 8) & 0xFF,
	    (addr.sin_addr.s_addr >> 16) & 0xFF,
	    (addr.sin_addr.s_addr >> 24) & 0xFF,
	    ntohs(addr.sin_port));
}

int main(void)
{
    int server;

    video_init();
    wifi_init();
    while (!wifi_connect());
    server = wifi_listen();

    while (1) {
	static Connection conn;
	int i;
	uint32 data_goal;
	
	connection_accept(&conn, server);

	conn.card_type = cardmeGetType();
	iprintf("Card type %d\n", conn.card_type);
	if (conn.card_type <= 0) {
	    close(conn.fd);
	    continue;
	}

	conn.card_size = cardmeSize(conn.card_type);
	iprintf("%d byte EEPROM\n", conn.card_size);

	for (conn.address = 0;
	     conn.address < conn.card_size;
	     conn.address += sizeof conn.buffer) {
	    int sent = 0;

	    cardmeReadEeprom(conn.address, conn.buffer, sizeof conn.buffer, conn.card_type);

	    data_goal = Wifi_GetStats(WSTAT_TXDATABYTES) + sizeof conn.buffer;

	    do {
		sent += send(conn.fd, conn.buffer + sent, sizeof conn.buffer - sent, 0);
	    } while (sent < sizeof conn.buffer);

	    /*
	     * dswifi's TCP stack is buggy... or sometihing. It's retransmitting every packet
	     * several times. This seems to help a little. Also, this should help work around
	     * the fact that dswifi doesn't flush any buffers on close().
	     */	   
	    while (Wifi_GetStats(WSTAT_TXDATABYTES) < data_goal) {
		swiWaitForVBlank();
	    }

	    iprintf("(%d bytes)\x1b[60D", conn.address);
	}	
	iprintf("Complete.        \n");

	/*
	 * Wait for the other end to close the connection.
	 * Closing it immediately seems to just lose data in
	 * dswifi's buffers...
	 */
	while (recv(conn.fd, conn.buffer, sizeof conn.buffer, 0) > 0);
	close(conn.fd);
    }

    return 0;
}
