#include <FieldSensor.h>
#include <stdio.h>

int main(void) {
  FieldSensor s;
  VECTOR readings;

  while (1) {
    readings = s.readPacket();
    for (VECTOR::iterator i=readings.begin();i!=readings.end();i++)
      printf("%0.9f ", *i);
    printf("\n");
  }

  return 0;
}
