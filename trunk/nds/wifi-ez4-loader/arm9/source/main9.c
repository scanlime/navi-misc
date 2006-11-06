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
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ez4loader.h"

/*
 * Control values for the EZFlash IV.
 *
 * These values, as well as the magic sequence of writes
 * which transfer them to the EZ4, were first discovered in
 * the Moonshell source but later verified by disassembling
 * the EZ4's firmware. All values here were observed in the
 * firmware, but most are of unknown purpose.
 */
#define EZ4CTRL_GBAROM_WRITE_DISABLE   0xD200    /* Write protect the GBAROM, corrupting it in the process :( */
#define EZ4CTRL_GBAROM_WRITE_ENABLE    0x1500
#define EZ4CTRL_UNKNOWN1_A             0xA200    /* This pair was discovered at 0x114384 in ezfla_up.bin */
#define EZ4CTRL_UNKNOWN1_B             0x2A00
#define EZ4CTRL_UNKNOWN2_A             0xA500    /* Discovered at 0x114314 in ezfla_up.bin */
#define EZ4CTRL_UNKNOWN2_B             0x5A00

/*
 * This is ndsmall.bin, a 512-byte ARM7 bootloader which copies a
 * .nds file into main memory and begins running it. It is typically
 * prepended to a .nds file to yield a .ds.gba file. All code
 * in this bootloader is relocatable, but it does include a
 * reference to the start address of the original .NDS file:
 * by default, 512 bytes into the GBAROM. The address of this constant
 * is defined here, so we can relocate the bootloader at runtime.
 */
#include "ndsmall_bin.h"
const uint32 ndsmall_nds_offset = 0x01a8;

/*
 * This parameter block lets us communicate with the image we've loaded.
 * It sits near the top of NDSHeader, in the reserved area. It's above
 * the last address that ndsmall.bin copies.
 *
 * The loaded process can verify our presence by checking that
 * entryPoint == ~entryPointComplement. It can jump back to the bootloader
 * by resetting the ARM9 to a PassMe loop and the ARM7 to this address.
 */
struct {
    uint32 entryPoint;
    uint32 entryPointComplement;
} *LoaderParameterBlock = (void*) 0x027FFF84;

/* A copy of the ARM7 binary image, in memory that this processor can access */
static unsigned char local_arm7_image[0x10000];

#define  WIFI_TIMER_MS  50
#define  TCP_PORT       6502

typedef struct {
    int fd;
    unsigned char buffer[4096];
    uint32 buffer_len;
    uint32 bytes_received;
    uint32 header_len;
    uint32 boot_mode;
    void *image_destination;
    union {
	tNDSHeader nds;
	tGBAHeader gba;
    } __attribute__ ((__packed__)) header;
} Connection;

extern void sgIP_Init();


void * sgIP_malloc(int size)
{ 
    return malloc(size);
}

void sgIP_free(void * ptr)
{
    free(ptr);
}

static void wifi_timer_handler(void)
{
    Wifi_Timer(WIFI_TIMER_MS);
}

static void wifi_sync_handler()
{
    /* This is called by libdswifi in order to invoke Wifi_Sync() on the ARM7 */
    REG_IPC_FIFO_TX = IPC_MSG_WIFI_SYNC;
}


static void fifo_irq_handler()
{
    uint32 message = REG_IPC_FIFO_RX;
    switch (message) {

    case IPC_MSG_WIFI_SYNC:
	Wifi_Sync();

    }
}

static void vblank_irq_handler()
{
    Wifi_Update();
}

static uint32 fifo_rx_wait()
{
    /* Perform a blocking read from the FIFO */
    while (REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
    return REG_IPC_FIFO_RX;
}

static void video_init()
{
    powerSET(POWER_LCD | POWER_2D_B);
    lcdMainOnBottom();
    videoSetMode(0);
    consoleDemoInit();

    iprintf("Wifi EZ4 Loader\nMicah Dowty <micah@navi.cx>\n\n");
}

uint32 Wifi_Init_Hack(uint32 flags)
{
    /*
     * XXX: This is a reimplementation of dswifi's Wifi_Init().
     *      For some reason, Wifi_Init() can crash after a soft-reboot.
     *      This version doesn't- and the only noticeable difference
     *      is that we use memset() rather than a for loop to clear
     *      Wifi_Data_Struct.
     */
    struct WIFI_MAINSTRUCT {
	u16 curChannel, reqChannel;
	u16 curMode, reqMode;
	u16 authlevel,authctr;
	u32 flags9, flags7, reqPacketFlags;
    };
    extern struct WIFI_MAINSTRUCT Wifi_Data_Struct;
    extern struct WIFI_MAINSTRUCT *WifiData;
    uint32 wifiSize = 0x00009f04;

    memset(&Wifi_Data_Struct, 0, wifiSize);
    WifiData = (struct WIFI_MAINSTRUCT*) (((uint32) &Wifi_Data_Struct) | 0x00400000);
    sgIP_Init();
    WifiData->flags9 = 1 + WIFIINIT_OPTION_USELED;
    return (uint32) &Wifi_Data_Struct;
}

static void wifi_init()
{
    REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR;

    /*
     * This is not wifi-related, obviously, but for convenience
     * we're abusing the existing ARM7 handshaking here. Ask
     * the ARM7 to make a copy of its own code into main memory.
     */
    REG_IPC_FIFO_TX = IPC_MSG_COPY_SELF;
    REG_IPC_FIFO_TX = (uint32) local_arm7_image;

    /*
     * Wait until the ARM7 is ready for us to Wifi_Init()
     */
    while (fifo_rx_wait() != IPC_MSG_WIFI_INIT);

    /*
     * Our half of the ARM7 Wifi handshaking:
     * Send it a pointer to the shared memory area for Wifi.
     */
    REG_IPC_FIFO_TX = IPC_MSG_WIFI_INIT;
    REG_IPC_FIFO_TX = Wifi_Init_Hack(WIFIINIT_OPTION_USELED);

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

static int wifi_connect()
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

static int wifi_listen()
{
    struct sockaddr_in addr = { 0 };
    int s;
    uint32 my_ip;

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

static void connection_accept(Connection *self, int socket)
{
    struct sockaddr_in addr;
    int addrSize;

    self->fd = accept(socket, (struct sockaddr *) &addr, &addrSize);
    iprintf("\nAccepted %d.%d.%d.%d:%d\n",
	    addr.sin_addr.s_addr & 0xFF,
	    (addr.sin_addr.s_addr >> 8) & 0xFF,
	    (addr.sin_addr.s_addr >> 16) & 0xFF,
	    (addr.sin_addr.s_addr >> 24) & 0xFF,
	    ntohs(addr.sin_port));
}

static int connection_read_header(Connection *self)
{
    while (self->header_len != sizeof self->header) {
	int result = recv(self->fd, 
			  self->header_len + (void*)&self->header,
			  sizeof self->header - self->header_len, 0);
	if (result <= 0) {
	    return 0;
	}

	self->bytes_received += result;
	self->header_len += result;
    }

    return 1;
}

static void connection_read_image(Connection *self)
{
    /*
     * Copy the header followed by the rest of the ROM image into
     * self->image_destination, until the connection is closed.
     */
    
    /* This assumes our header is not an odd number of bytes */
    swiCopy(&self->header, self->image_destination,
	    COPY_MODE_HWORD | (sizeof self->header / sizeof(uint16)));
    self->image_destination += sizeof self->header;

    while (1) {
	int result;
	uint32 block_size;
	
	iprintf("(%d bytes)\x1b[60D", self->bytes_received);

	result = recv(self->fd,
		      self->buffer + self->buffer_len,
		      sizeof self->buffer - self->buffer_len,
		      0);
	if (result <= 0) {
	    break;
	}
	self->bytes_received += result;

	/*
	 * Round down to the nearest even number of bytes for
	 * the copy. Store an extra byte, if we have one,
	 * at the beginning of the buffer.
	 */
	block_size = result & ~1;
	self->buffer_len = result & 1;

	swiCopy(self->buffer, self->image_destination,
		COPY_MODE_HWORD | (block_size / sizeof(uint16)));
	self->image_destination += block_size;

	if (self->buffer_len) {
	    self->buffer[0] = self->buffer[result - 1];
	}
    }

    if (self->buffer_len) {
	uint32 block_size = (self->buffer_len + 1) & ~1;
	swiCopy(self->buffer, self->image_destination,
		COPY_MODE_HWORD | (block_size / sizeof(uint16)));
	self->image_destination += block_size;
    }
      
    iprintf("Finished. (%d bytes)\n", self->bytes_received);
}

static char *format_game_title(char *str, int len)
{
    /*
     * Format a game title, given as a counted string,
     * into a static buffer. This strips out nonprintable
     * characters and NUL-terminates the string. If the game
     * title appears valid, returns a string with embedded
     * quotes. Otherwise, returns the empty string.
     */
    static char buffer[32];
    char *dest = buffer;
    int printable_chars = 0;

    if (len > sizeof buffer - 3) {
	len = sizeof buffer - 3;
    }

    *(dest++) = '"';

    while (len) {
	char c = *str;
	if (c == '\0') {
	    break;
	}
	if (c >= ' ' && c <= '~') {
	    *(dest++) = c;
	}
	printable_chars++;
	str++;
	len--;
    }

    if (printable_chars) {
	*(dest++) = '"';
	*(dest++) = '\0';
	return buffer;
    }

    return "";
}

static int validate_gba_header(tGBAHeader *hdr)
{
    if (hdr->is96h != 0x96) {
	return 0;
    }

    if ((hdr->entryPoint & 0xFFFFFF00) != 0xea000000) {
	return 0;
    }

    return 1;
}

static int validate_nds_header(tNDSHeader *hdr)
{
    /* NB: Don't use romSize for validation, it's wrong on many PAlib games */

    if (hdr->headerSize < sizeof *hdr) {
	return 0;
    }

    if (hdr->arm9romSource < hdr->headerSize ||
	hdr->arm7romSource < hdr->headerSize) {
	return 0;
    }

    /* Make sure the execute addresses are within the copied images */
    if (hdr->arm9executeAddress < hdr->arm9destination ||
	hdr->arm9executeAddress >= (hdr->arm9destination + hdr->arm9binarySize) ||
	hdr->arm7executeAddress < hdr->arm7destination ||
	hdr->arm7executeAddress >= (hdr->arm7destination + hdr->arm7binarySize)) {
	return 0;
    }

    /*
     * Enforce that the load addresses are between 0x2000000 and 0x4000000.
     * This allows code loaded into Main, shared, or private RAM and prevents
     * a corrupted NDS header from clobbering registers.
     */
    if (hdr->arm9destination < 0x2000000 ||
	(hdr->arm9destination + hdr->arm9binarySize) >= 0x4000000 ||
	hdr->arm7destination < 0x2000000 ||
	(hdr->arm7destination + hdr->arm7binarySize) >= 0x4000000) {
	return 0;
    }

    return 1;
}

static void connection_identify_rom(Connection *self)
{
    char *title, *image_type;

    self->image_destination = GBAROM;
    self->boot_mode = IPC_MSG_REBOOT_NDS;

    if (validate_nds_header(&self->header.nds)) {
	/*
	 * NDS header looks fine. Prepend our default GBA loader.
	 *
	 * Note: Currently it seems like a better idea to validate
	 *       NDS headers first. By default, ndstool produces headers
	 *       which look like both NDS and GBA headers, but are actually
	 *       only useful as NDS.
	 */
	title = format_game_title(self->header.nds.gameTitle,
				  sizeof self->header.nds.gameTitle);
	image_type = ".nds";

	swiCopy(ndsmall_bin, self->image_destination,
		COPY_MODE_HWORD | (ndsmall_bin_size / sizeof(uint16)));
	self->image_destination += ndsmall_bin_size;
    } else if (validate_gba_header(&self->header.gba)) {
	/*
	 * GBA header seems valid. This will always load directly
	 * to the GBAROM memory, but we should detect whether it's
	 * a real GBA game or a loader for a DS game.
	 */
	title = format_game_title(self->header.gba.title,
				  sizeof self->header.gba.title);

	if (memcmp(self->header.gba.gamecode, "PASS", 
		   sizeof self->header.gba.gamecode)) {
	    image_type = ".gba";
	    self->boot_mode = IPC_MSG_REBOOT_GBA;
	} else {
	    image_type = ".ds.gba";
	}
    } else {
	/*
	 * Unknown! Dump it to ROM and hope for the best- it
	 * might just be a bare ARM7 binary.
	 */
	title = "";
	image_type = "unknown";
    }

    iprintf("Detected %s image\n%s\n\n", image_type, title);
}

static uint32 gbarom_detect_writable_size(void)
{
    /*
     * Perform a binary search to determine the amount of
     * writable SRAM mapped into the GBAROM area.
     */
    const uint32 max_words = 0x01000000;
    uint32 byte_size = 0;
    uint32 current_bits = 0;
    uint32 next_bit = max_words >> 1;

    while (next_bit) {
	uint32 offset_under_test = current_bits | next_bit;
	int i;
	static const uint16 testPattern[] = {
	    0x0000, 0xFFFF, 0x5555, 0xAAAA, 0x1234, 0x0000,
	};
	
	for (i = sizeof testPattern / sizeof testPattern[0] - 1; i >= 0; i--) {
	    GBA_BUS[offset_under_test] = testPattern[i];

	    /* Detect aliasing */
	    GBA_BUS[current_bits] = 0xFFFF;
	    
	    if (GBA_BUS[offset_under_test] != testPattern[i]) {
		break;
	    }
	}

	if (i < 0) {
	    /* Test succeeded. The size now includes this address. */
	    current_bits = offset_under_test;
	    next_bit >>= 1;
	    byte_size = (current_bits + 1) << 1;
	} else {
	    /* Test failed, stop here */
	    break;
	}
    }

    return byte_size;
}

static void ezflash4_control(uint16 value)
{
    GBA_BUS[0xff0000] = 0xd200;
    GBA_BUS[0x000000] = 0x1500;
    GBA_BUS[0x010000] = 0xd200;
    GBA_BUS[0x020000] = 0x1500;
    GBA_BUS[0xe20000] = value;
    GBA_BUS[0xfe0000] = 0x1500;
}

static void gbarom_init(void)
{
    static uint32 patched_loader[512 / sizeof(uint32)];
    static tNDSHeader patched_header;
    uint32 available_space;
    uint32 loader_size;
    void *loader_ptr;

    sysSetCartOwner(BUS_OWNER_ARM9);
    ezflash4_control(EZ4CTRL_GBAROM_WRITE_ENABLE);

    available_space = gbarom_detect_writable_size();

    /*
     * Carve out a chunk of GBAROM space at the very top,
     * and make a copy of ourselves there. We'll patch the
     * ndsmall.bin bootloader in order to change its NDS
     * source address, so that it can run in-place.
     *
     * We place the entry point address in an agreed-upon
     * location in main RAM.
     */
    
    /* First, calculate how much space we'll need, including extra for worst-case padding */
    loader_size = ndsmall_bin_size + sizeof(tNDSHeader) +
	NDSHeader.arm9binarySize + NDSHeader.arm7binarySize;
    loader_size = (loader_size + 2*3) & ~3;
    
    /* Reserve that space at the very top of the GBAROM */
    available_space -= loader_size;
    loader_ptr = available_space + (void*)GBAROM;
    iprintf("%d kB program space\n\n", available_space / 1024);

    /* Store the entry point */
    LoaderParameterBlock->entryPoint = (uint32) loader_ptr;
    LoaderParameterBlock->entryPointComplement = ~(uint32) loader_ptr;

    /* First in: a patched copy of the ndsmall bootloader */
    swiCopy(ndsmall_bin, patched_loader,
	    COPY_MODE_HWORD | (ndsmall_bin_size / sizeof(uint16)));
    patched_loader[ndsmall_nds_offset / sizeof(patched_loader[0])] =
	(uint32) (loader_ptr + ndsmall_bin_size);
    swiCopy(patched_loader, loader_ptr,
	    COPY_MODE_HWORD | (ndsmall_bin_size / sizeof(uint16)));
    loader_ptr += ndsmall_bin_size;

    /*
     * Next, a patched copy of our NDS header. We change the ROM addresses,
     * since we'll be copying the header, ARM7 code, and ARM9 code consecutively.
     */
    swiCopy(&NDSHeader, &patched_header,
	    sizeof patched_header / sizeof(uint32));
    patched_header.arm7binarySize = (patched_header.arm7binarySize + 3) & ~3;
    patched_header.arm9binarySize = (patched_header.arm9binarySize + 3) & ~3;
    patched_header.arm7romSource = sizeof patched_header;
    patched_header.arm9romSource = patched_header.arm7romSource + patched_header.arm7binarySize;
    swiCopy(&patched_header, loader_ptr,
	    COPY_MODE_HWORD | (sizeof patched_header / sizeof(uint16)));
    loader_ptr += sizeof patched_header;

    /*
     * Now we can copy in our own executable code
     */
    swiCopy(local_arm7_image, loader_ptr,
	    COPY_MODE_HWORD | (patched_header.arm7binarySize / sizeof(uint16)));
    loader_ptr += patched_header.arm7binarySize;
    swiCopy((void*) patched_header.arm9destination, loader_ptr,
	    COPY_MODE_HWORD | (patched_header.arm9binarySize / sizeof(uint16)));
}

static void arm9_reboot(uint32 boot_mode)
{
    iprintf("Rebooting...\n");

    sysSetCartOwner(BUS_OWNER_ARM7);

    /* Disable IRQs */
    REG_IME = 0;
    REG_IF = 0;

    if (boot_mode == IPC_MSG_REBOOT_GBA) {
	/*
	 * Map and clear the VRAM, to eliminate nasty flickery border
	 * artifacts in GBA mode.
	 */
	uint16 *vram;
    
	videoSetMode(MODE_FB0);
	vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);

	for (vram = VRAM_A; vram < VRAM_E; vram++) {
	    *vram = 0;
	}
    }

    /* Reset video modes */
    videoSetMode(0);
    videoSetModeSub(0);

    if (boot_mode == IPC_MSG_REBOOT_GBA &&
	PersonalData->gbaScreen) {
	/* GBA on lower screen */
	lcdMainOnBottom();
    } else {
	lcdMainOnTop();
    }

    /* Reset the ARM7 entry point to the beginning of the GBA ROM */
    NDSHeader.arm7executeAddress = (uint32) GBAROM;

    /*
     * Initialize the ARM9 loop.
     * The BIOS will jump us to arm9executeAddress, which
     * points to our loop instruction which jumps to arm9executeAddress...
     */
    *BOOT_ARM9_LOOP_ADDRESS = BOOT_ARM9_LOOP_INSTRUCTION;
    NDSHeader.arm9executeAddress = (uint32) BOOT_ARM9_LOOP_ADDRESS;

    DC_FlushAll();
    IC_InvalidateAll();

    /*
     * Tell the ARM7 to reboot now, wait for it to acknowledge.
     * This is mostly as a sanity check for arm7_reboot, so we
     * don't end up rebooting one core but not the other.
     */
    REG_IPC_FIFO_TX = boot_mode;
    do {
	while (REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY);
    } while (REG_IPC_FIFO_RX != IPC_MSG_ACK_REBOOT);

    swiSoftReset();
}

int main(void)
{
    int server;

    video_init();
    wifi_init();
    gbarom_init();

    while (!wifi_connect());
    server = wifi_listen();

    while (1) {
	static Connection conn = { 0 };
	connection_accept(&conn, server);

	if (connection_read_header(&conn)) {
	    connection_identify_rom(&conn);
	    connection_read_image(&conn);

	    iprintf("\nPress any key to reboot\n");
	    do {
		swiWaitForVBlank();
		scanKeys();
	    } while (!keysHeld());
  
	    arm9_reboot(conn.boot_mode);
	}

	close(conn.fd);
    }

    return 0;
}
