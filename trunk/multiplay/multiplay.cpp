#include <gst/gst.h>
#include <signal.h>
#include <iostream>

bool intrack;
bool quit;

void playsong(char *song);
void interrupt(int sig);
void alarms(int sig);

int main(int argc, char **argv) {
  gst_init(&argc, &argv);
  if(argc < 2) {
    g_print("usage: %s <filename>\n", argv[0]);
    exit(-1);
  }

  quit = false;

  signal(SIGINT, interrupt);
  signal(SIGALRM, alarms);
  
  for(int i = 1; i < argc; i++)
    playsong(argv[i]);
}

void playsong(char *song) {
  intrack = true;
  
  std::cout << "playing " << song << std::endl;
  
  GstElement *pipeline, *filesrc, *dec, *stereosplit;
  GstElement *netsink, *localsink;

  pipeline = gst_pipeline_new ("pipeline");
  filesrc = gst_element_factory_make("filesrc", "disk_source");
  g_object_set(G_OBJECT(filesrc), "location", song, NULL);
  char *ext = strrchr(song, '.');
  if(ext == NULL)
    return;
  if(strcmp(ext, ".mp3") == 0) {
    dec = gst_element_factory_make("mad", "decoder");
  } else if(strcmp(ext, ".ogg") == 0) {
    dec = gst_element_factory_make("vorbisdec", "decoder");
  } else {
    return;
  }
  stereosplit = gst_element_factory_make("stereosplit", "splitter");

  localsink = gst_element_factory_make("osssink", "local-sink");
  g_object_set(G_OBJECT(localsink), "frequency", 48000, NULL);

  netsink = gst_element_factory_make("osssink", "net-sink");
  g_object_set(G_OBJECT(netsink), "frequency", 48000, "device", "/dev/sound/dsp2", NULL);

  gst_bin_add_many(GST_BIN(pipeline), filesrc, dec, stereosplit, localsink, netsink, NULL);
  gst_element_connect_many(filesrc, dec, stereosplit, NULL);
  gst_pad_connect(gst_element_get_pad(stereosplit, "src1"), gst_element_get_pad(netsink, "sink"));
  gst_pad_connect(gst_element_get_pad(stereosplit, "src2"), gst_element_get_pad(localsink, "sink"));

  gst_element_set_state(pipeline, GST_STATE_PLAYING);
  while(gst_bin_iterate(GST_BIN(pipeline))) {
    if(!intrack) {
      break;
    }
  }
  gst_element_set_state(pipeline, GST_STATE_NULL);

  gst_object_unref(GST_OBJECT(pipeline));
}

void interrupt(int sig) {
  if(quit) {
    exit(0);
  }
  quit = true;
  alarm(1);
  intrack = false;
}

void alarms(int sig) {
  quit = false;
}
