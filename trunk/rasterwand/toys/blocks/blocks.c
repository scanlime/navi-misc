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
  char blocks[9][80];
  int i, j;

  int bs;
  int top, bottom;

  wand = open("/dev/usb/rwand0",O_WRONLY);
 
  for(i=0;i<9;i++)
  {
    for(j=0;j<80;j++)
    {
      blocks[i][j] = 0;
      switch(i)
      {
      case 0: 
	blocks[i][j] = 0xFF;
	break;
      case 1:
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
	break;
      case 2:
	bs = i;
	top = 0 ^ ONE ^ TWO ^ FIVE ^  SIX;
	bottom = 0 ^ THREE ^ FOUR ^ SEVEN ^ EIGHT;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 3:
	bs = i;
	top = 0^ ONE ^ TWO ^ THREE ^ SEVEN ^ EIGHT;
	bottom = 0 ^ FOUR ^ FIVE ^ SIX;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 4:
	bs = i;
	top = 0 ^ ONE ^ TWO ^ THREE ^ FOUR;
	bottom = 0 ^ FIVE ^ SIX ^ SEVEN ^ EIGHT;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 5:
	bs = i;
	top = 0 ^ ONE ^ TWO ^ THREE ^ FOUR ^ FIVE;
	bottom = 0 ^ SIX ^ SEVEN ^ EIGHT;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 6:
	bs = i;
	top = 0 ^ ONE ^ TWO ^ THREE ^ FOUR ^ FIVE ^ SIX;
	bottom = 0 ^ SEVEN ^ EIGHT;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 7:
	bs = i;
	top = 0 ^ ONE ^ TWO ^ THREE ^ FOUR ^ FIVE ^ SIX ^ SEVEN;
	bottom = 0 ^ EIGHT;
	if((j/bs)%2 == 0) blocks[i][j] = top;
	else blocks[i][j] = bottom;
	break;
      case 8:
	bs = 8;
	top = 0xFF;
	bottom = 0;
	if( (j/bs) % 2 == 0) blocks[8][j] = top;
	else blocks[8][j] = bottom;
	break;
      }
    }
  }
  
  for(i=0;i<81;i++)
  {
    write(wand,blocks[i%9],i);
    sleep(1);
  }
  close(wand);
}
