#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <xmms/plugin.h>
#include <rwand_dev.h>
#include <math.h>

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
  struct timeval then, now;
  float dt;

  gettimeofday(&then, NULL);

  while (fd) {
    /* Get the time step for this frame, in seconds */
    gettimeofday(&now, NULL);
    dt = (now.tv_sec - then.tv_sec) + (now.tv_usec - then.tv_usec)/1000000.0;
    then = now;

    for (i=0; i<NUM_COLUMNS; i++) {
      /* Scale our columns logarithmically and draw them to the frame */
      if (columns[i] > 256) {
	level = log((columns[i]>>8)) * scale;
	if (level > 8)
	  level = 8;
	frame[i] = levels[level];
      }
      else {
	frame[i] = 0;
      }

      /* Decay our original column values linearly, giving an exponential
       * decay in the displayed values after the log() above.
       */
      columns[i] -= dt * 14285;
      if (columns[i] < 0)
	columns[i] = 0;
    }

    /* Write this frame, blocking until it blits to the front buffer */
    write(fd, frame, sizeof(frame));
  }
  return NULL;
}

static void plugin_init(void) {
  struct rwand_settings s;

  fd = open("/dev/rwand0", O_WRONLY);
  if (fd < 0) {
    perror("open");
    return;
  }

  /* Set up the display parameters to give us a more spectrum-analyzer-looking
   * output. This makes our pixels fairly wide, so they look like the LEDs
   * on a stereo. We also skew the display toward the right so when it's dominated
   * by low frequencies it looks centered.
   */
  ioctl(fd, RWANDIO_GET_SETTINGS, &s);
  s.display_center = 0xFFFF * 0.53;
  s.display_width  = 0xFFFF * 0.45;
  s.duty_cycle     = 0xFFFF * 0.66;
  ioctl(fd, RWANDIO_PUT_SETTINGS, &s);

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
