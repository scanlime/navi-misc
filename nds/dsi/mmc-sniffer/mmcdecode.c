/*
 * MMC protocol decoder for Saxoscope.
 *
 * Assumes a slightly modified saxoscope HDL, which
 * gives us one byte on every rising edge of CLK.
 * That byte contains DAT[3:0], CMD, and an error bit.
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <stdint.h>

static void cmd_state_machine(int cmd);
static void dat_state_machine(int dat);
static void dat_received(uint8_t *buffer, int size);

int main()
{
  while (1) {
    uint8_t byte = fgetc(stdin);
    uint8_t dat = byte & 0x0F;
    uint8_t cmd = byte & 0x10;
    uint8_t err = byte & 0x80;

    if (err) {
      printf("[***] Error: Hardware buffer overflow, data was lost!\n");
    }

    cmd_state_machine(cmd);
    dat_state_machine(dat);
  }

  return 0;
}

void cmd_state_machine(int cmd)
{
  static uint64_t buffer;
  static int numBits;
  static enum {
    S_IDLE,
    S_CMD,
  } state = S_IDLE;

  switch (state) {

  case S_IDLE:
    if (cmd) {
      /* Still idle */
      return;
    }
    /* Otherwise, it's a start bit. Fall through. */
    state = S_CMD;

  case S_CMD:
    buffer = buffer << 1;
    numBits++;
    if (cmd) {
      buffer |= 1;
    }

    if (numBits == 48) {      
      printf("[CMD] tb=%d CMD%-2d arg=%8x crc=%02x end=%d\n",
	     (int)((buffer >> 46) & 0x1),
	     (int)((buffer >> 40) & 0x3f),
	     (int)((buffer >> 8) & 0xffffffff),
	     (int)((buffer >> 1) & 0x7f),
	     (int)((buffer >> 0) & 0x1));

      numBits = 0;
      buffer = 0;
      state = S_IDLE;
    }
    break;

  }
}

void dat_state_machine(int dat)
{
  /* Note: 16 clock cycles (8 bytes) of CRC16 */
  static const int blockSize = 0x208;
  static uint8_t buffer[0x208];
  static int byteNum = 0;

  static enum {
    S_IDLE,
    S_START,
    S_DATA_LOW,
    S_DATA_HIGH,
    S_STOP,
  } state = S_IDLE;

  switch (state) {

  case S_IDLE:
    if (dat == 0) {
      /* Start bit */
      state = S_START;
    } else if (dat == 0x0F) {
      /* Idle */
    } else if (dat == 0x0E) {
      /* Busy response for some commands. Ignore. */
    } else {
      printf("[***] Bad DAT byte in idle state (%02x)\n", dat);
    }
    break;

  case S_START:
    byteNum = 0;
    state = S_DATA_HIGH;
    break;

  case S_DATA_HIGH:
    buffer[byteNum] = dat << 4;
    state = S_DATA_LOW;
    break;

  case S_DATA_LOW:
    buffer[byteNum] |= dat;
    byteNum++;
    if (byteNum < blockSize) {
      state = S_DATA_HIGH;
    } else {
      state = S_STOP;
    }
    break;

  case S_STOP:
    dat_received(buffer, blockSize);
    if (dat != 0x0F) {
      printf("[***] Bad DAT stop condition (%02x)\n", dat);
    }
    state = S_IDLE;
    break;

  }
}

void dat_received(uint8_t *buffer, int size)
{
  int offset = 0;

  while (offset < size) {
    int i;
    int lineSize = 16;
    int maxLineSize = size - offset;

    if (lineSize > maxLineSize) {
      lineSize = maxLineSize;
    }

    printf("[DAT] %03x:", offset);

    for (i = 0; i < lineSize; i++) {
      printf(" %02x", buffer[offset + i]);
    }

    printf(" : ");

    for (i = 0; i < lineSize; i++) {
      uint8_t byte = buffer[offset + i];
      if (byte < ' ' || byte > '~') {
	byte = '.';
      }
      printf("%c", byte); 
    }

    printf("\n");
  
    offset += lineSize;
  }
}
