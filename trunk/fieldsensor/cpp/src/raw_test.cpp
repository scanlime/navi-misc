#include <FieldSensor.h>

int main(void) {
  FieldSensor s;
  std::vector<float> readings;

  while (1) {
    readings = s.readPacket();
    for (std::vector<float>::iterator i=readings.begin(); i!=readings.end(); i++)
      printf("%0.9f ", *i);
    printf("\n");
  }

  return 0;
}
