#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <xmms/plugin.h>

static void plugin_init(void);
static void plugin_cleanup(void);
static void plugin_render_freq(gint16 data[2][256]);

#define NUM_COLUMNS 32

static int fd;
pthread_t my_thread;

struct column {
  int total;
  int n_samples;
} columns[NUM_COLUMNS];

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
  struct column *c;
  unsigned char levels[9] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

  while (fd) {
    for (i=0; i<NUM_COLUMNS; i++) {
      c = &columns[i];
      if (c->n_samples) {
	level = c->total / c->n_samples / 180;
	c->total = c->n_samples = 0;

	if (level > 8)
	  level = 8;
	frame[i] = levels[level];
      }
      else {
	/* No data */
	frame[i] = 0x24;
      }
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
  /* Scale the given frequency data down, both temporally and spatially, to match our output resolution */
  int i, colnum;
  struct column *c;

  for (i=0; i<256; i++) {
    colnum = i/3;
    if (colnum >= NUM_COLUMNS)
      break;
    c = &columns[colnum];
    c->total += data[0][i];
    c->n_samples++;
  }
}

/* The End */
