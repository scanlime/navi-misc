/*
 * main.c - Initialization and command line interface
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include "de-jong.h"
#include "animation.h"
#include "explorer.h"
#include "avi-writer.h"
#include "screensaver.h"
#include "config.h"

static void usage                  (char       **argv);
static void animation_render_main  (DeJong      *dejong,
				    Animation   *animation,
				    const gchar *filename,
				    gulong       target_density);
static void image_render_main      (DeJong      *dejong,
				    const gchar *filename,
				    gulong       target_density);
static void acquire_console        (void);

int main(int argc, char ** argv) {
  DeJong* dejong;
  Animation* animation;
  gboolean animate = FALSE;
  enum {INTERACTIVE, RENDER, SCREENSAVER} mode = INTERACTIVE;
  const gchar *outputFile = NULL;
  int c, option_index=0;
  gulong target_density = 10000;

  g_random_set_seed(time(NULL));
  g_type_init();

  dejong = de_jong_new();
  animation = animation_new();

  while (1) {
    static struct option long_options[] = {
      {"help",        0, NULL, 'h'},
      {"read",        1, NULL, 'i'},
      {"output",      1, NULL, 'o'},
      {"zoom",        1, NULL, 'z'},
      {"rotation",    1, NULL, 'r'},
      {"blur-radius", 1, NULL, 1000},
      {"blur-ratio",  1, NULL, 1001},
      {"exposure",    1, NULL, 'e'},
      {"gamma",       1, NULL, 'g'},
      {"foreground",  1, NULL, 1002},
      {"background",  1, NULL, 1003},
      {"size",        1, NULL, 's'},
      {"density",     1, NULL, 'd'},
      {"clamped",     0, NULL, 1004},
      {"oversample",  1, NULL, 1005},
      {"tileable",    0, NULL, 1006},
      {"fg-alpha",    1, NULL, 1007},
      {"bg-alpha",    1, NULL, 1008},
      {"animate",     1, NULL, 'n'},
      {"screensaver", 0, NULL, 1009},
      {NULL},
    };
    c = getopt_long(argc, argv, "hi:o:a:b:c:d:x:y:z:r:e:g:s:t:n:",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {

    case 'a':  parameter_holder_set(PARAMETER_HOLDER(dejong), "a",              optarg); break;
    case 'b':  parameter_holder_set(PARAMETER_HOLDER(dejong), "b",              optarg); break;
    case 'c':  parameter_holder_set(PARAMETER_HOLDER(dejong), "c",              optarg); break;
    case 'd':  parameter_holder_set(PARAMETER_HOLDER(dejong), "d",              optarg); break;
    case 'x':  parameter_holder_set(PARAMETER_HOLDER(dejong), "xoffset",        optarg); break;
    case 'y':  parameter_holder_set(PARAMETER_HOLDER(dejong), "yoffset",        optarg); break;
    case 'z':  parameter_holder_set(PARAMETER_HOLDER(dejong), "zoom",           optarg); break;
    case 'r':  parameter_holder_set(PARAMETER_HOLDER(dejong), "rotation",       optarg); break;
    case 'e':  parameter_holder_set(PARAMETER_HOLDER(dejong), "exposure",       optarg); break;
    case 'g':  parameter_holder_set(PARAMETER_HOLDER(dejong), "gamma",          optarg); break;
    case 's':  parameter_holder_set(PARAMETER_HOLDER(dejong), "size" ,          optarg); break;
    case 1000: parameter_holder_set(PARAMETER_HOLDER(dejong), "blur_radius",    optarg); break;
    case 1001: parameter_holder_set(PARAMETER_HOLDER(dejong), "blur_ratio",     optarg); break;
    case 1002: parameter_holder_set(PARAMETER_HOLDER(dejong), "fgcolor",        optarg); break;
    case 1003: parameter_holder_set(PARAMETER_HOLDER(dejong), "bgcolor",        optarg); break;
    case 1004: parameter_holder_set(PARAMETER_HOLDER(dejong), "clamped",        "1");    break;
    case 1005: parameter_holder_set(PARAMETER_HOLDER(dejong), "oversample",     optarg); break;
    case 1006: parameter_holder_set(PARAMETER_HOLDER(dejong), "tileable",       "1");    break;
    case 1007: parameter_holder_set(PARAMETER_HOLDER(dejong), "fgalpha",        optarg); break;
    case 1008: parameter_holder_set(PARAMETER_HOLDER(dejong), "bgalpha",        optarg); break;

    case 'i':
      histogram_imager_load_image_file(HISTOGRAM_IMAGER(dejong), optarg);
      break;

    case 'o':
      mode = RENDER;
      outputFile = optarg;
      break;

    case 'n':
      animation_load_file(animation, optarg);
      animate = TRUE;
      break;

    case 1009:
      mode = SCREENSAVER;
      break;

    case 't':
      target_density = atol(optarg);
      break;

    case 'h':
    default:
      usage(argv);
      return 1;
    }
  }

  if (optind < argc) {
    usage(argv);
    return 1;
  }

  switch (mode) {

  case INTERACTIVE:
    gtk_init(&argc, &argv);
    explorer_new(ITERATIVE_MAP(dejong), animation);
    gtk_main();
    break;

  case RENDER:
    acquire_console();
    if (animate)
      animation_render_main(dejong, animation, outputFile, target_density);
    else
      image_render_main(dejong, outputFile, target_density);
    break;

  case SCREENSAVER:
    gtk_init(&argc, &argv);
    screensaver_new(ITERATIVE_MAP(dejong), animation);
    gtk_main();
    break;
  }

  return 0;
}

static void usage(char **argv) {
  acquire_console();
  fprintf(stderr,
     "Usage: %s [options]\n"
	 "Interactive exploration of the Peter de Jong attractor\n"
	 "\n"
	 "Actions:\n"
	 "  -i, --read FILE       Load all parameters from the tEXt chunk of any\n"
	 "                          .png image file generated by this program.\n"
	 "  -n, --animate FILE    Load an animation from FILE. If an output file is\n"
	 "                          also specified, this renders the animation.\n"
	 "  -o, --output FILE     Instead of presenting an interactive GUI, render\n"
	 "                          an image or animation with the provided settings\n"
	 "                          noninteractively, and store it in FILE.\n"
	 "\n"
	 "Parameters:\n"
	 "  -a VALUE              Set the 'a' parameter\n"
	 "  -b VALUE              Set the 'b' parameter\n"
	 "  -c VALUE              Set the 'c' parameter\n"
	 "  -d VALUE              Set the 'd' parameter\n"
	 "  -x OFFSET             Set the X offest\n"
	 "  -y OFFSET             Set the Y offset\n"
	 "  -z, --zoom ZOOM       Set the zoom factor\n"
         "  -r, --rotation RADS   Set the rotation, in radians\n"
	 "  --blur-radius RADIUS  Set the blur radius\n"
	 "  --blur-ratio RATIO    Set the blur ratio\n"
	 "  --tileable            Generate a tileable image by wrapping at the edges\n"
	 "\n"
	 "Rendering:\n"
	 "  -e, --exposure EXP    Set the image exposure\n"
	 "  -g, --gamma GAMMA     Set the image gamma correction\n"
	 "  --foreground COLOR    Set the foreground color, specified as a color name\n"
	 "                          or in #RRGGBB hexadecimal format\n"
	 "  --background COLOR    Set the background color, specified as a color name\n"
	 "                          or in #RRGGBB hexadecimal format\n"
	 "  --fg-alpha ALPHA      Set the foreground alpha, between 0 (transparent)\n"
	 "                          and 65535 (completely opaque)\n"
	 "  --bg-alpha ALPHA      Set the background alpha, between 0 (transparent)\n"
	 "                          and 65535 (completely opaque)\n"
	 "  --clamped             Clamp the image to the foreground color, rather than\n"
	 "                          allowing more intense pixels to have other values\n"
	 "\n"
	 "Quality:\n"
	 "  -s, --size X[xY]      Set the image size in pixels. If only one value is\n"
	 "                          given, a square image is produced\n"
	 "  --oversample SCALE    Calculate the image at some integer multiple of the\n"
	 "                          output resolution, downsampling when generating the\n"
	 "                          final image. This improves the quality of sharp\n"
	 "                          edges on most images, but will increase memory usage\n"
	 "                          quadratically. Recommended values are between 1\n"
	 "                          (no oversampling) and 4 (heavy oversampling)\n"
	 "  -t, --density DENSITY In noninteractive rendering, set the peak density\n"
	 "                          to stop rendering at. Larger numbers give smoother\n"
	 "                          and more detailed results, but increase running time\n"
	 "                          linearly\n",
	 argv[0]);
}

static void image_render_main (DeJong     *dejong,
			       const char *filename,
			       gulong      target_density) {
  /* Main function for noninteractive rendering. This renders an image with the
   * current settings until render.current_density reaches target_density. We show helpful
   * progress doodads on stdout while the poor user has to wait.
   */
  float elapsed, remaining;

  while (HISTOGRAM_IMAGER(dejong)->peak_density < target_density) {
    iterative_map_calculate(ITERATIVE_MAP(dejong), 1000000);

    /* This should be a fairly accurate time estimate, since (asymptotically at least)
     * current_density increases linearly with the number of iterations performed.
     * Elapsed time and time remaining are in seconds.
     */
    elapsed = histogram_imager_get_elapsed_time(HISTOGRAM_IMAGER(dejong));
    remaining = elapsed * target_density / HISTOGRAM_IMAGER(dejong)->peak_density - elapsed;

    /* After each batch of iterations, show the percent completion, number
     * of iterations (in scientific notation), iterations per second,
     * density / target density, and elapsed time / remaining time.
     */
    printf("%6.02f%%   %.3e   %.2e/sec   %6ld / %ld   %02d:%02d:%02d / %02d:%02d:%02d\n",
	   100.0 * HISTOGRAM_IMAGER(dejong)->peak_density / target_density,
	   ITERATIVE_MAP(dejong)->iterations, ITERATIVE_MAP(dejong)->iterations / elapsed,
	   HISTOGRAM_IMAGER(dejong)->peak_density, target_density,
	   ((int)elapsed) / (60*60), (((int)elapsed) / 60) % 60, ((int)elapsed)%60,
	   ((int)remaining) / (60*60), (((int)remaining) / 60) % 60, ((int)remaining)%60);
  }

#ifdef HAVE_EXR
  /* Save as an OpenEXR file if it has a .exr extension, otherwise use PNG */
  if (strlen(filename) > 4 && strcmp(".exr", filename + strlen(filename) - 4)==0) {
    printf("Creating OpenEXR image...\n");
    exr_save_image_file(HISTOGRAM_IMAGER(dejong), filename);
  }
  else
#endif
  {
    printf("Creating PNG image...\n");
    histogram_imager_save_image_file(HISTOGRAM_IMAGER(dejong), filename);
  }
}

static void animation_render_main (DeJong      *dejong,
				   Animation   *animation,
				   const gchar *filename,
				   gulong      target_density) {
  const double frame_rate = 24;
  AnimationIter iter;
  ParameterHolderPair frame;
  guint frame_count = 0;
  gboolean continuation;
  AviWriter *avi = avi_writer_new(fopen(filename, "wb"),
				  HISTOGRAM_IMAGER(dejong)->width,
				  HISTOGRAM_IMAGER(dejong)->height,
				  frame_rate);

  animation_iter_get_first(animation, &iter);
  frame.a = PARAMETER_HOLDER(de_jong_new());
  frame.b = PARAMETER_HOLDER(de_jong_new());

  while (animation_iter_read_frame(animation, &iter, &frame, frame_rate)) {

    continuation = FALSE;
    do {
      iterative_map_calculate_motion(ITERATIVE_MAP(dejong), 100000, continuation,
			       PARAMETER_INTERPOLATOR(parameter_holder_interpolate_linear),
			       &frame);
      printf("Frame %d, %e iterations, %ld density\n", frame_count, ITERATIVE_MAP(dejong)->iterations, HISTOGRAM_IMAGER(dejong)->peak_density);
      continuation = TRUE;
    } while (HISTOGRAM_IMAGER(dejong)->peak_density < target_density);


    histogram_imager_update_image(HISTOGRAM_IMAGER(dejong));
    avi_writer_append_frame(avi, HISTOGRAM_IMAGER(dejong)->image);

    frame_count++;
  }

  avi_writer_close(avi);
}

#ifdef WIN32
void sleep_at_exit()
{
  printf("\nFinished.\n");
  while (1)
    sleep(10);
}

static void acquire_console()
{
  /* This will give us a new console window- a little disconcerting
   * when running Fyre from the command line, but still usable. We
   * have to use a little bit of black magic to reattach that to stdout
   * and stderr.
   */
  HANDLE hfile;
  int fd;
  FILE *file;

  AllocConsole();
  hfile = GetStdHandle(STD_OUTPUT_HANDLE);
  fd = _open_osfhandle((LONG) hfile, O_WRONLY);
  file = fdopen(fd, "w");
  *stdout = *file;
  *stderr = *file;
  
  atexit(sleep_at_exit);
}
#else
static void acquire_console() {}
#endif

/* The End */
