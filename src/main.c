/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * main.c - Initialization and command line interface
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include "config.h"
#include "platform.h"

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#ifdef HAVE_FORK
#include <unistd.h>
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
#include "remote-server.h"
#include "batch-image-render.h"
#include "gui-util.h"

#ifdef HAVE_GNET
#include "cluster-model.h"
#include "discovery-server.h"
#endif

static void usage                  (char          **argv);
static void animation_render_main  (IterativeMap   *map,
				    Animation      *animation,
				    const gchar    *filename,
				    double          quality);
static void acquire_console        (void);
#ifdef HAVE_GNET
static void daemonize_to_pidfile   (const char* filename);
#endif


int main(int argc, char ** argv) {
    IterativeMap* map;
    Animation* animation;
    gboolean animate = FALSE;
    gboolean have_gtk;
    gboolean verbose = FALSE;
    gboolean hidden = FALSE;
    enum {INTERACTIVE, RENDER, SCREENSAVER, REMOTE} mode = INTERACTIVE;
    const gchar *outputFile = NULL;
    const gchar *pidfile = NULL;
    int c, option_index=0;
    double quality = 1.0;
#ifdef HAVE_GNET
    int port_number = FYRE_DEFAULT_PORT;
#endif
    GError *error = NULL;

    math_init();
    g_type_init();
    have_gtk = gtk_init_check(&argc, &argv);

#ifdef HAVE_GNET
    gnet_init();
#  ifdef WIN32
    gnet_ipv6_set_policy(GIPV6_POLICY_IPV4_ONLY);
#  endif
#endif

    map = ITERATIVE_MAP(de_jong_new());
    animation = animation_new();

    while (1) {
	static struct option long_options[] = {
	    {"help",         0, NULL, 'h'},
	    {"read",         1, NULL, 'i'},
	    {"animate",      1, NULL, 'n'},
	    {"output",       1, NULL, 'o'},
	    {"param",        1, NULL, 'p'},
	    {"size",         1, NULL, 's'},
	    {"oversample",   1, NULL, 'S'},
	    {"quality",      1, NULL, 'q'},
	    {"remote",       0, NULL, 'r'},
	    {"verbose",      0, NULL, 'v'},
	    {"port",         1, NULL, 'P'},
	    {"cluster",      1, NULL, 'c'},
	    {"auto-cluster", 0, NULL, 'C'},
	    {"screensaver",  0, NULL, 1000},   /* Undocumented, still experimental */
	    {"hidden",       0, NULL, 1001},
	    {"chdir",        1, NULL, 1002},   /* Undocumented, used by win32 file associations */
	    {"pidfile",      1, NULL, 1003},
	    {"version",      0, NULL, 1004},
	    {NULL},
	};
	c = getopt_long(argc, argv, "hi:n:o:p:s:S:q:rvP:c:C",
			long_options, &option_index);
	if (c == -1)
	    break;

	switch (c) {

	case 'i':
	{
	    histogram_imager_load_image_file(HISTOGRAM_IMAGER(map), optarg, &error);
	    break;
        }

	case 'n':
	{
	    GtkTreeIter iter;

	    animation_load_file(animation, optarg);
	    animate = TRUE;
	    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(animation->model), &iter);
	    animation_keyframe_load(animation, &iter, PARAMETER_HOLDER(map));
	    break;
	}

	case 'o':
	    mode = RENDER;
	    outputFile = optarg;
	    break;

	case 'p':
	    parameter_holder_load_string(PARAMETER_HOLDER(map), optarg);
	    break;

	case 's':
	    parameter_holder_set(PARAMETER_HOLDER(map), "size" , optarg);
	    break;

	case 'S':
	    parameter_holder_set(PARAMETER_HOLDER(map), "oversample", optarg);
	    break;

	case 'q':
	    quality = atof(optarg);
	    break;

	case 'v':
	    verbose = TRUE;
	    break;

#ifdef HAVE_GNET
	case 'c':
	    {
		ClusterModel *cluster = cluster_model_get(map, TRUE);
		cluster_model_add_nodes(cluster, optarg);
	    }
	    break;
	case 'C':
	    {
		ClusterModel *cluster = cluster_model_get(map, TRUE);
		cluster_model_enable_discovery(cluster);
	    }
	    break;
	case 'r':
	    mode = REMOTE;
	    break;
	case 'P':
	    port_number = atol(optarg);
	    break;
#else
	case 'c':
	case 'C':
	case 'P':
	    fprintf(stderr,
		    "This Fyre binary was compiled without gnet support.\n"
		    "Cluster support is not available.\n");
	    break;
	case 'r':
	    fprintf(stderr,
		    "This Fyre binary was compiled without gnet support.\n"
		    "Cluster support is not available.\n");
	    exit(1);
	    break;
#endif

	case 1000: /* --screensaver */
	    mode = SCREENSAVER;
	    break;

	case 1001: /* --hidden */
	    hidden = TRUE;
	    break;

	case 1002: /* --chdir */
	    chdir(optarg);
	    break;

	case 1003: /* --pidfile */
	    pidfile = optarg;
	    break;

	case 1004: /* --version */
	    printf("%s\n", VERSION);
	    return 0;

	case 'h':
	default:
	    usage(argv);
	    return 1;
	}
    }

    if (optind + 1 < argc) {
	usage(argv);
	return 1;
    }

    if (optind != argc) {
	char *ext = strrchr (argv[optind], '.');
	if (ext) {
	    if (g_strcasecmp(ext, ".png") == 0) {
		histogram_imager_load_image_file(HISTOGRAM_IMAGER(map), argv[optind], &error);
	    } else if (g_strcasecmp(ext, ".fa") == 0) {
	        GtkTreeIter iter;

		animation_load_file(animation, argv[optind]);
		animate = TRUE;
	        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(animation->model), &iter);
	        animation_keyframe_load(animation, &iter, PARAMETER_HOLDER(map));
	    } else {
	        usage(argv);
	        return 1;
	    }
	} else {
	    usage(argv);
	    return 1;
	}
    }

    switch (mode) {

    case INTERACTIVE: {
	Explorer *explorer;

	if (!have_gtk) {
	    fprintf(stderr, "GTK intiailization failed, can't start in interactive mode\n");
	    return 1;
	}
	explorer = explorer_new (map, animation);
	if (error) {
	    GtkWidget *dialog, *label;
	    gchar *text;

	    dialog = glade_xml_get_widget (explorer->xml, "error dialog");
	    label = glade_xml_get_widget (explorer->xml, "error label");

	    text = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">Error!</span>\n\n%s", error->message);
	    gtk_label_set_markup (GTK_LABEL (label), text);
	    g_free (text);
	    g_error_free (error);

	    gtk_dialog_run (GTK_DIALOG (dialog));
	    gtk_widget_hide (dialog);
	}
	gtk_main();
	break;
    }

    case RENDER: {
	acquire_console();
	if (error) {
	    g_print ("Error: %s\n", error->message);
	    g_error_free (error);
	}
	if (animate)
	    animation_render_main (map, animation, outputFile, quality);
	else
	    batch_image_render (map, outputFile, quality);
	break;
    }

    case REMOTE: {
#ifdef HAVE_GNET
        if (verbose) {
	    acquire_console();
	}
	else {
	    daemonize_to_pidfile(pidfile);
	}
	if (!hidden)
	    discovery_server_new(FYRE_DEFAULT_SERVICE, port_number);
	remote_server_main_loop(port_number, have_gtk, verbose);
#else
	fprintf(stderr,
		"This Fyre binary was compiled without gnet support.\n"
		"Remote control mode is not available.\n");
#endif
	break;
    }

    case SCREENSAVER: {
	ScreenSaver* screensaver;
	GtkWidget* window;

	if (!have_gtk) {
	    fprintf(stderr, "GTK intiailization failed, can't start in screensaver mode\n");
	    return 1;
	}

	screensaver = screensaver_new(map, animation);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	fyre_set_icon_later(window);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_title(GTK_WINDOW(window), "Fyre Screensaver");
	gtk_container_add(GTK_CONTAINER(window), screensaver->view);
	gtk_widget_show_all(window);

	gtk_main();
	break;
    }
    }

    return 0;
}

static void usage(char **argv) {
    acquire_console();
    fprintf(stderr,
	    "Usage: %s [options] [file]\n"
	    "Interactive exploration and high quality rendering of chaotic maps\n"
	    "\n"
	    "Actions:\n"
	    "  -i, --read FILE         Load all parameters from the tEXt chunk of any\n"
	    "                            .png image file generated by this program.\n"
	    "  -n, --animate FILE      Load an animation from FILE. If an output file is\n"
	    "                            also specified, this renders the animation.\n"
	    "  -o, --output FILE       Instead of presenting an interactive GUI, render\n"
	    "                            an image or animation with the provided settings\n"
	    "                            noninteractively, and store it in FILE.\n"
	    "  -h, --help              Display this text.\n"
	    "  --version               Show the version number and exit.\n"
	    "\n"
	    "Clustering:\n"
	    "  -c. --cluster LIST      Use a rendering cluster, given as a comma-separated\n"
	    "                            list of hosts, optionally of the form host:port.\n"
	    "  -C, --auto-cluster      Automatically search for cluster nodes, adding them\n"
            "                            as they become available.\n"
	    "  -r, --remote            Remote control mode. Fyre will listen by default on\n"
	    "                            port 7931 for commands, and can act as a rendering\n"
	    "                            server in a cluster.\n"
	    "  -P, --port N            Set the TCP port number used for remote control mode.\n"
	    "  -v, --verbose           In remote control mode, display status messages on the\n"
	    "                            console and don't run as a daemon.\n"
	    "  --hidden                In remote control mode, don't reply to broadcast\n"
	    "                            requests for detecting available Fyre servers.\n"
	    "  --pidfile FILE          When running in the background under a UNIX-like OS,\n"
	    "                            save the new process ID to this file.\n"
	    "\n"
	    "Parameters:\n"
	    "  -p, --param KEY=VALUE   Set a calculation or rendering parameter, using the\n"
	    "                            same key/value format used to store parameters in\n"
	    "                            image metadata.\n"
	    "\n"
	    "Quality:\n"
	    "  -s, --size X[xY]        Set the image size in pixels. If only one value is\n"
	    "                            given, a square image is produced\n"
	    "  -S, --oversample SCALE  Calculate the image at some integer multiple of the\n"
	    "                            output resolution, downsampling when generating the\n"
	    "                            final image. This improves the quality of sharp\n"
	    "                            edges on most images, but will increase memory usage\n"
	    "                            quadratically. Recommended values are between 1\n"
	    "                            (no oversampling) and 4 (heavy oversampling)\n"
	    "  -q, --quality QUALITY   In noninteractive rendering, set the quality level at\n"
	    "                            which we stop rendering. Larger numbers give\n"
	    "                            smoother and more detailed results, but increase\n"
	    "                            running time. The default of 1.0 gives roughly one\n"
	    "                            histogram sample for every final image sample.\n",
	    argv[0]);
}

static void animation_render_main (IterativeMap *map,
				   Animation    *animation,
				   const gchar  *filename,
				   double        quality) {
    const double frame_rate = 24;
    AnimationIter iter;
    ParameterHolderPair frame;
    guint frame_count = 0;
    gboolean continuation;
    double current_quality;

    AviWriter *avi = avi_writer_new(fopen(filename, "wb"),
				    HISTOGRAM_IMAGER(map)->width,
				    HISTOGRAM_IMAGER(map)->height,
				    frame_rate);

    animation_iter_get_first(animation, &iter);
    frame.a = PARAMETER_HOLDER(de_jong_new());
    frame.b = PARAMETER_HOLDER(de_jong_new());

    while (animation_iter_read_frame(animation, &iter, &frame, frame_rate)) {

	continuation = FALSE;
	do {
	    /* Calculate 0.5 seconds between quality updates. This is lower than the
	     * batch image rendering default of 2.0 seconds, to better handle
	     * lower-quality animations where the individual frames go quicker.
	     */
	    iterative_map_calculate_motion_timed(map, 0.5, continuation,
						 PARAMETER_INTERPOLATOR(parameter_holder_interpolate_linear),
						 &frame);
	    current_quality = histogram_imager_compute_quality(HISTOGRAM_IMAGER(map));

	    printf("\rFrame %d, %e iterations, %.04f quality", frame_count,
		   map->iterations, current_quality);
	    fflush(stdout);

	    continuation = TRUE;
	} while (current_quality < quality);

	histogram_imager_update_image(HISTOGRAM_IMAGER(map));
	avi_writer_append_frame(avi, HISTOGRAM_IMAGER(map)->image);

	/* Move to the next line for each frame.
	 * Updates within a frame overwrite that line.
	 */
	printf("\n");

	frame_count++;
    }

    avi_writer_close(avi);
}


/* Daemonize this process, saving the new PID to a file if a name
 * is specified. No pidfile is written if the filename is NULL.
 */
#ifdef HAVE_GNET
#ifdef HAVE_FORK
static void daemonize_to_pidfile(const char* filename)
{
    FILE* f = NULL;

    if (filename) {
	/* Open the file before our current directory and console disappear */
	f = fopen(filename, "w");
	if (!f)
	    printf("Can't open pidfile '%s'\n", filename);
    }

    if (daemon(0, 0) < 0) {
	perror("daemon");
	exit(1);
    }

    if (f) {
	fprintf(f, "%d", getpid());
	fclose(f);
    }
}
#else
static void daemonize_to_pidfile(const char* filename) {}
#endif /* HAVE_FORK */
#endif /* HAVE_GNET */


#ifdef WIN32
static int console_running = 1;

void sleep_at_exit()
{
    printf("\nFinished.\n");
    while (console_running) {
        Sleep(100);
        fgetc(stdin);
    }
}

BOOL console_control_handler(DWORD control_type)
{
    switch (control_type) {

    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        console_running = 0;
        break;

    }
    return FALSE;
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

    SetConsoleCtrlHandler((PHANDLER_ROUTINE) console_control_handler, TRUE);
    atexit(sleep_at_exit);
}
#else
static void acquire_console() {}
#endif

/* The End */
