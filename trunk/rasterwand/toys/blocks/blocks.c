#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define ZERO     0x00
#define ONE      0X01 
#define TWO      0x02
#define THREE    0x04
#define FOUR     0x08
#define FIVE     0x10
#define SIX      0x20
#define SEVEN    0x40
#define EIGHT    0x80

int main(int argc, char **argv)
{
  int wand;
  char blocks[8][80];
  int i, j;

  int bs;

  wand = open("/dev/usb/rwand0",O_WRONLY);
 
  for(i=0;i<8;i++)
  {
    for(j=0;j<80;j++)
    {
      blocks[i][j] = 0;
      if((i+j)%2)
      {
	blocks[i][j] ^= ONE;
	blocks[i][j] ^= THREE;
	blocks[i][j] ^= FIVE;
	blocks[i][j] ^= SEVEN;
      }
      else
      {
	blocks[i][j] ^= TWO;
	blocks[i][j] ^= FOUR;
	blocks[i][j] ^= SIX;
	blocks[i][j]^= EIGHT;
      }
    }
  }
  
  for(i=0;;i++)
  {
    write(wand,blocks[i%8],80);
  }
  close(wand);
}
