#include <FieldSensor.h>
#include <alsa/asoundlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  FieldSensor s;
  Vector3 v;
  snd_seq_t *seq;
  int port;
  snd_seq_event_t ev;

  int dest_client = argv[1] ? atoi(argv[1]) : 128, dest_port = 0;

  snd_seq_open(&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
  snd_seq_set_client_name(seq, "Digital Theremin");
  port = snd_seq_create_simple_port(seq, "Digital Theremin Output",
				    SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
				    SND_SEQ_PORT_TYPE_APPLICATION);

  snd_seq_connect_to(seq, port, dest_client, dest_port);



  while (1) {
    v = s.getPosition();
    
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_source(&ev, port);
    snd_seq_ev_set_dest(&ev, dest_client, dest_port);

    snd_seq_ev_set_pitchbend(&ev, 0, ((int) (v[0] * 16383 - 8192)));
    snd_seq_event_output(seq,&ev);

    snd_seq_ev_set_controller(&ev, 0, 1, ((int) -(v[1] * 127 - 127)));
    snd_seq_ev_set_controller(&ev, 0, 2, ((int) -(v[2] * 127 - 127)));
    snd_seq_event_output(seq,&ev);

    snd_seq_drain_output(seq);

    usleep(1);
  }

  return 0;
}
