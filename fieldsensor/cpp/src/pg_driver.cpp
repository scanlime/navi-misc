#include <FieldSensor.h>
#include <unistd.h>
#include <iostream>
using namespace std;
extern "C" {
#include <picogui.h>
}

int main(int argc, char **argv) {
  try {
    FieldSensor s;
    pg_client_trigger trig;
    pgInit(argc,argv);
    
    while (1) {
      Vector3 v = s.getPosition();
      memset(&trig, 0, sizeof(trig));
      
      trig.content.type = PG_TRIGGER_MOTIONTRACKER;
      trig.content.u.motion.bodyPart = PG_BODYPART_EITHER_HAND;
      trig.content.u.motion.position[0] = (u32) (v[0]*0xFFFFFFFF);
      trig.content.u.motion.position[1] = (u32) (v[1]*0xFFFFFFFF);
      trig.content.u.motion.position[2] = (u32) (v[2]*0xFFFFFFFF);

      pgInFilterSend(&trig);
      pgFlushRequests();
      
      usleep(1000000 / 60);
    }
  }
  catch (annie::Exception &e) {
    cout << e.what() << endl;
  }

  return 0;
}
