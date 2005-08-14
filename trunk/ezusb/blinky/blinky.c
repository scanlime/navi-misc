#include "regezusb.h"
#include <string.h>
#include <stdio.h>

void Led_Init() {
   PORTCCFG = 0;
   OUTC = 0x00;
   OEC = 0x08;
}

void Led_Flash() {
   OUTC ^= 0x08;
}

void main()
{
   unsigned int i;

   Led_Init();

   while (1) {
      Led_Flash();
      for (i=0; i<40000; i++);
   }
}

