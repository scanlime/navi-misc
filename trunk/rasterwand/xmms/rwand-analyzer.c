#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <xmms/plugin.h>

static void plugin_init(void);
static void plugin_cleanup(void);
static void plugin_render_freq(gint16 data[2][256]);

#define NUM_COLUMNS 16

static int fd;
pthread_t my_thread;
int columns[NUM_COLUMNS];

VisPlugin plugin_vp = {
  .description         = "Raster Wand spectrum analyzer",
  .num_freq_chs_wanted = 1,
  .init                = plugin_init,
  .cleanup             = plugin_cleanup,
  .render_freq         = plugin_render_freq,
};

VisPlugin *get_vplugin_info(void) {
  return &plugin_vp;
}

void *refresh_thread(void *foo) {
  unsigned char frame[NUM_COLUMNS];
  int i, level;
  unsigned char levels[9] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
  const float scale = 14 / log(256);

  while (fd) {
    for (i=0; i<NUM_COLUMNS; i++) {
      if (columns[i] > 256) {
	level = log((columns[i]>>8)) * scale;
	if (level > 8)
	  level = 8;
	frame[i] = levels[level];
      }
      else {
	frame[i] = 0;
      }

      columns[i] -= 1500;
      if (columns[i] < 0)
	columns[i] = 0;
    }
    write(fd, frame, sizeof(frame));
  }
  return NULL;
}

static void plugin_init(void) {
  fd = open("/dev/usb/rwand0", O_WRONLY);
  if (fd < 0) {
    perror("open");
    return;
  }

  pthread_create(&my_thread,NULL,refresh_thread,NULL);
}

static void plugin_cleanup(void) {
  close(fd);
  fd = 0;
}

static void plugin_render_freq(gint16 data[2][256]) {
  int i, colnum;
  int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

  colnum = 0;
  for (i=0; i<256; i++) {
    while (xscale[colnum] < i)
      colnum++;

    if (data[0][i] > columns[colnum])
      columns[colnum] = data[0][i];
  }
}

/* The End */
