#include <stdio.h>
#include <stdlib.h>

void main()
 {

  if (!bcm2835_init())
  {
   printf("fatal: cannot initialize BCM2835 library!\n");
   exit(0);
  }

  if(!PL_init_amp())
   {
    printf("can't init amplifier!\n");
    exit(0);
   }

  PL_set_amp(1);

  system("mpg321 --gain 10 crowd-cheering.mp3");

  PL_set_amp(0);  

  bcm2835_close();

 }

