#include <annie.h>
#include <FieldSensor.h>

int main(void) {
  FieldSensor s;
  annie::TwoLayerNetwork net("data/current.net");
  double packet[8];
  annie::VECTOR position;

  while (1) {
    s.readPacket(packet);
    position = net.getOutput(packet);
    printf("%f %f %f\n", position[0], position[1], position[2]);
  }

  return 0;
}
