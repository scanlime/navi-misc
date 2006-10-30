#include "nds.h"
#include <nds/dma.h>
#include <nds/arm9/console.h> //basic print funcionality
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define  DMA_CHANNEL  3
#define  TCP_PORT     6502

void * sgIP_malloc(int size)
{ 
  return malloc(size);
}

void sgIP_free(void * ptr)
{
  free(ptr);
}

// wifi timer function, to update internals of sgIP
void Timer_50ms(void)
{
  Wifi_Timer(50);
}

// notification function to send fifo message to arm7
void arm9_synctoarm7()
{
  REG_IPC_FIFO_TX=0x87654321;
}

// interrupt handler to receive fifo messages from arm7
void arm9_fifo()
{
  u32 value = REG_IPC_FIFO_RX;
  if (value == 0x87654321)
    Wifi_Sync();
}

void video_init()
{
  powerSET(POWER_LCD | POWER_2D_B);
  lcdMainOnTop();
  videoSetMode(0);	//not using the main screen
  consoleDemoInit();
  iprintf("Wifi EZ4 Loader\nMicah Dowty <micah@navi.cx>\n\n");
}

void idle()
{
  vuint16 *vcount = 0x04000006;
  uint16 x;

  /* Wait for vertical blanking */
  while (*vcount > 192);
  while (*vcount < 192);

  /* Nothing else to do yet... */
}

void wifi_init()
{
  // send fifo message to initialize the arm7 wifi
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_SEND_CLEAR; // enable & clear FIFO

  u32 Wifi_pass= Wifi_Init(WIFIINIT_OPTION_USELED);
  REG_IPC_FIFO_TX=0x12345678;
  REG_IPC_FIFO_TX=Wifi_pass;
   	
  *((volatile u16 *)0x0400010E) = 0; // disable timer3

  irqInit(); 
  irqSet(IRQ_TIMER3, Timer_50ms); // setup timer IRQ
  irqEnable(IRQ_TIMER3);
  irqSet(IRQ_FIFO_NOT_EMPTY, arm9_fifo); // setup fifo IRQ
  irqEnable(IRQ_FIFO_NOT_EMPTY);
   	
  REG_IPC_FIFO_CR = IPC_FIFO_ENABLE | IPC_FIFO_RECV_IRQ; // enable FIFO IRQ
   	
  Wifi_SetSyncHandler(arm9_synctoarm7); // tell wifi lib to use our handler to notify arm7

  // set timer3
  *((volatile u16 *)0x0400010C) = -6553; // 6553.1 * 256 cycles = ~50ms;
  *((volatile u16 *)0x0400010E) = 0x00C2; // enable, irq, 1/256 clock
	       
  while(Wifi_CheckInit()==0) { // wait for arm7 to be initted successfully
    idle();
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

  sysSetCartOwner(1);              /* Give the ARM9 ownership of the GBA_BUS */
  ezflash4_set_write_protect(0);   /* Allow writes to the GBA "ROM", actually SRAM on the EZ4 */

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
    idle();
  }
  ioctl(conn, FIONBIO, &nonblock);
  iprintf("Accepted %d.%d.%d.%d:%d\n",
	  addr.sin_addr.s_addr & 0xFF,
	  (addr.sin_addr.s_addr >> 8) & 0xFF,
	  (addr.sin_addr.s_addr >> 16) & 0xFF,
	  (addr.sin_addr.s_addr >> 24) & 0xFF,
	  ntohs(addr.sin_port));

  offset = last_displayed_offset = 0;
  while (1) {
    int result = recv(conn, buffer + residual, sizeof buffer - residual, 0);
    if (result == 0) {
      close(conn);
      break;
    }
    if (result < 0) {
      idle();
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

  /* Finish up with the GBA bus */
  while (dmaBusy(DMA_CHANNEL));
  sysSetCartOwner(0);

  iprintf("\nPress any key to reboot\n");
  do {
    scanKeys();
  } while (!keysHeld());
  
  iprintf("Rebooting...\n");

  REG_IME = 0;  /* Disable IRQs */
  REG_IF = 0;

  /*
   * This sets up the ARM9 loop used by the EZ4, presumably.
   * This is different from the loop used by ndsmall.bin,
   * and I'm not sure if it's correct. This is from Moonshell.
   * Note that both the ARM7 and ARM9 code consistently refer
   * to this address (27FFE08) as the M3/SC boot vector
   * (as opposed to a GBAMP-style boot vector)
   *
   * This will put the ARM9 into reset. The ARM7 will notice
   * these values in memory and perform its corresponding
   * reset sequence.
   */
  *((vuint32*)0x027FFE08) = (u32)0xE59FF014;  // ldr pc, 0x027FFE24
  *((vuint32*)0x027FFE24) = (u32)0x027FFE08;  // Set ARM9 Loop address (M3/SC)

  swiSoftReset();

  return 0;
}
