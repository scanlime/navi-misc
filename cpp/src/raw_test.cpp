#include <FieldSensor.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  FieldSensor s;
  VECTOR readings, oldReadings;

  while (1) {
    oldReadings = readings;
    readings = s.getRawData();
    for (VECTOR::iterator i=readings.begin();i!=readings.end();i++)
      printf("%0.9f ", *i);
    if (oldReadings != readings)
      printf(" ***");
    printf("\n");
    usleep(1000);
  }

  return 0;
}
