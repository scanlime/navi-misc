#include <FieldSensor.h>
#include <stdio.h>

int main(void) {
  FieldSensor s;
  double readings[8];

  while (1) {
    s.readPacket(readings);
    for (int i=0;i<8;i++)
      printf("%0.9f ", readings[i]);
    printf("\n");
  }

  return 0;
}
