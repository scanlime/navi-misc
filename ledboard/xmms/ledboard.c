/* This horrible mess of cruft was thrown together by Micah Dowty
 * to be somewhat of a visualization plugin for his LED array.
 */


#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termios.h>
#include <sys/select.h>
#include <string.h>
#include <pthread.h>

#include "xmms/plugin.h"
#include "xmms/util.h"

#define NUM_BANDS 5
#define HEIGHT 8

struct termios options;

static int fd;
pthread_t my_thread;
unsigned char ledregs[42];
gint16 freq_data[2][256];

static void sanalyzer_init(void);
static void sanalyzer_cleanup(void);
static void sanalyzer_playback_start(void);
static void sanalyzer_playback_stop(void);
static void sanalyzer_render_freq(gint16 data[2][256]);


/************************************** Brightness functions */

/* Sort of like a really low-res spectrogram */
int red_fn(gint16 data[2][256], int x, int y) {
  static float flow[5][8];
  int i,j;
  int freq_taps[5] = {32, 48, 64, 96, 128};

  if (!(x || y)) {

    /* Flow the whole thing upwards */
    for (i=7;i>0;i--)
      for (j=0;j<5;j++)
	flow[j][i] = flow[j][i-1];

    /* Stick new values in the bottom row */
    for (j=0;j<5;j++) {
      float x = data[0][freq_taps[j]] / 60.0;

      if (x > flow[j][0])
	flow[j][0] = x;
      else
	flow[j][0] -= 1.6;
      
      if (flow[j][0] < 0)
	flow[j][0] = 0;

      if (flow[j][0] > 15)
	flow[j][0] = 15;
    }
  }

  return flow[x][y];
}

/* VU meter for channel 0 */
int yellow_fn(gint16 data[2][256], int i) {
  int j;
  float total;
  int bright;
  static int fade[8];

  if (!i) {
    total = 0;
    for (j=0;j<256;j++)
      total += data[0][j];
    total /= 15000.0;
    
    for (i=0;i<8;i++) {
      if (7-i < total)
	bright =  10;
      else if (i < total+1)
	bright = (total-7+i) * 15.0;
      else
	bright = 0;
    
      if (bright > fade[i])
	fade[i] = bright;
      else if (fade[i] > 0)
	fade[i]-=0.9;
      else
	fade[i] = 0;
    }    
    i=0;
  }

  return fade[i];
}

/* VU meter for channel 1 */
int green_fn(gint16 data[2][256], int i) {
  int j;
  float total;
  int bright;
  static int fade[8];

  if (!i) {
    total = 0;
    for (j=0;j<256;j++)
      total += data[0][j];
    total /= 15000.0;
    
    for (i=0;i<8;i++) {
      if (i < total)
	bright =  10;
      else if (i < total+1)
	bright = (total-i) * 15.0;
      else
	bright = 0;

    
      if (bright > fade[i])
	fade[i] = bright;
      else if (fade[i] > 0)
	fade[i]-=0.5;
      else
	fade[i] = 0;
    }
    i=0;
  }

  return fade[i];
}

/* White indicates treble beat */
int white_fn(gint16 data[2][256], int i) {
  static float beat;
  float total, avg_total;
  static float samples[32];
  static int current_sample;
  int j;

  if (!i) {
    /* Average current volume */
    total = 0;
    for (j=64;j<256;j++)
      total += data[0][j];
    
    /* Store the previous 32 */
    if (current_sample >= 32)
      current_sample = 0;
    samples[current_sample] = total;
    current_sample++;
    
    /* Average the previous 32 */
    avg_total = 0;
    for (j=0;j<16;j++)
      avg_total += samples[j];
    avg_total /= 32;
    
    if (total > avg_total+18000)
      beat = 15;
    else if (beat>0)
      beat -= 0.8;
    else
      beat = 0;
  }

  return beat;
}

/* Blue is bass beat */
int blue_fn(gint16 data[2][256], int i) {
  static float beat;
  float total, avg_total;
  static float samples[32];
  static int current_sample;
  int j;

  if (!i) {
    /* Average current volume from low end of spectrum */
    total = 0;
    for (j=0;j<32;j++)
      total += data[0][j];
    
    /* Store the previous 32 */
    if (current_sample >= 32)
      current_sample = 0;
    samples[current_sample] = total;
    current_sample++;
    
    /* Average the previous 32 */
    avg_total = 0;
    for (j=0;j<32;j++)
      avg_total += samples[j];
    avg_total /= 32;
    
    if (total > avg_total+21000)
      beat = 15;
    else if (beat>0)
      beat -= 0.5;
    else
      beat = 0;
  }    

  return beat;
}

/* Just throw some stuff together for the side panel... */
int side_fn(gint16 data[2][256], int i) {
  switch (i) {

  case 0:        /* White LED follows the white LEDs on top*/
    return white_fn(data,1);

  case 2:        /* Blue LEDs follow the other blue LEDs.. */
  case 4:
  case 6:
    return blue_fn(data,1);

    /* The red and green LEDs follow the top row of the red grid */
  case 1:
    return red_fn(data,0,7);
  case 3:
    return red_fn(data,1,7);
  case 5:
    return red_fn(data,2,7);
  case 7:
    return red_fn(data,3,7);



  default:
    return 0;
  }
}

/************************************** Cruft */

VisPlugin sanalyzer_vp =
{
	NULL,
	NULL,
	0,
	"LED board visualizer", /* Description */
	0,
	1,		
	sanalyzer_init, /* init */
	sanalyzer_cleanup, /* cleanup */
	NULL, /* about */
	NULL, /* configure */
	NULL, /* disable_plugin */
	sanalyzer_playback_start, /* playback_start */
	sanalyzer_playback_stop, /* playback_stop */
	NULL, /* render_pcm */
	sanalyzer_render_freq  /* render_freq */
};

VisPlugin *get_vplugin_info(void)
{
	return &sanalyzer_vp;
}


void write_another_byte(int device, int byte) {
  char buffer[2];
  buffer[0] = (device << 5) | 0x10 | (byte>>4);
  buffer[1] = (device << 5) | 0x10 | (byte&0x0F);
  write(fd,buffer,2);
}

void write_nop(void) {
  char buffer[8];
  memset(buffer,0,sizeof(buffer));
  write(fd,buffer,sizeof(buffer));
}


void write_byte(int device, int byte) {
  char buffer[2];
  buffer[0] = (device << 5)        | (byte >> 4);               /* Set LED board address */
  buffer[1] = (device << 5) | 0x10 | (byte & 0x0F);
  write(fd,buffer,2);
}

void write_string(int device, char *str) {
  while (*str) {
    write_byte(device,*str);
    write_nop();
    str++;
  }
}

/* Refresh the LED array using data in ledregs[] 
 *
 * Send only the changed data plus one preexisting byte in each update
 */
void *refresh_thread(void *foo) {
  int i;
  int addr = 0;
  fd_set wfd;
  int old_addr = 0;
  unsigned char prev_ledregs[42];

  while (1) {
    FD_ZERO(&wfd);
    FD_SET(fd,&wfd);
    select(fd+1,NULL,&wfd,NULL,NULL);
    write_nop();
    write_nop();
    write_nop();
    write_nop();

    for (i=0;i<sizeof(ledregs);i++) {
      /* Partial updating broke... */
      //      if (prev_ledregs[i]==ledregs[i])
      //	continue;
      if (i!=addr) {
	write_byte(5,i);
	addr = i;
      }
      write_another_byte(5,ledregs[i]);
      prev_ledregs[i] = ledregs[i];
      addr++;
    }

    old_addr++;
    if (old_addr > sizeof(ledregs))
      old_addr = 0;
  }
  return NULL;
}

static void sanalyzer_init(void)
{
  int i;

  /********** Init the serial port */
  
  fd = open("/dev/ttyS0",O_WRONLY | O_NOCTTY | O_NDELAY);
  if (fd<0) {
    perror("open");
  }
  
  //  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL,0) & ~O_NONBLOCK);

  tcgetattr(fd, &options);
  cfsetispeed(&options, B57600);        /* 57600bps */
  options.c_cflag |= (CLOCAL | CREAD);  /* Enable receiver and set the local mode */
  options.c_cflag &= ~PARENB;           /* None parity */
  options.c_cflag &= ~CSIZE;            /* Mask the character bit size */
  options.c_cflag |= CS8;               /* 8 data bits */
  options.c_cflag &= ~CSTOPB;           /* 1 stop bits */
  options.c_cflag &= ~CRTSCTS;          /* Disable hardware flow control */
  options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */
  /* Set raw input and output */
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag &= ~OPOST;
  tcsetattr(fd, TCSANOW, &options);     /* set parameters */

  /* Initialize LED registers */
  memset(ledregs,0,sizeof(ledregs));
  ledregs[0x24] = 15;
  ledregs[0x28] = 1;
  ledregs[0x29] = 0x25;

  /* Put something up on the LCD */
  write_nop();           /* Synchronize */
  write_byte(2, 0x0C);   /* Control mode */
  write_byte(4, 0x01);   /* Clear LCD */
  write_nop();           /* Wait for clear to complete */
  write_byte(2, 0x0D);   /* Data mode */
  write_string(4, "XMMS Plugin");
  for (i=0;i<50;i++)
    write_nop();         /* Fill buffer up until thread starts */

  /* Start a thread to handle serial output */
  pthread_create(&my_thread,NULL,refresh_thread,NULL);
}

static void sanalyzer_cleanup(void)
{
  close(fd);
}

static void sanalyzer_playback_start(void)
{
}


static void sanalyzer_playback_stop(void)
{
}


static void sanalyzer_render_freq(gint16 data[2][256])
{
  int i;

  /* Write the side LEDs */
  for (i=0;i<4;i++) {
    ledregs[i] = (side_fn(data,i*2) << 4);
    ledregs[i] |= side_fn(data,i*2+1);
  }  

  /* Write red LED grid */
  for (i=0;i<20;i++) {
    ledregs[i+4] = (red_fn(data,i>>2,(i&3)*2) << 4);
    ledregs[i+4] |= red_fn(data,i>>2,(i&3)*2+1);
  }

  /* Write green LED bar */
  for (i=0;i<4;i++) {
    ledregs[i+24] = (green_fn(data,i*2) << 4);
    ledregs[i+24] |= green_fn(data,i*2+1);
  }

  /* Write yellow LED bar */
  for (i=0;i<4;i++) {
    ledregs[i+28] = (yellow_fn(data,i*2) << 4);
    ledregs[i+28] |= yellow_fn(data,i*2+1);
  }

  /* Write blue and white LEDs */
  for (i=0;i<4;i++)
    ledregs[i+32] = (blue_fn(data,i) << 4) | white_fn(data,i);

  return;			
}
