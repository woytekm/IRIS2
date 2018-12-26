#include <stdint.h>
#include "WEH001602-lib.h"


main(int argc, char **argv)
 {
   if (!bcm2835_init())
   {
    printf("fatal: cannot initialize BCM2835 library!\n");
    exit(0);
   }
   my_spi_WEH001602_init();
   my_spi_WEH001602_out_text(atoi(argv[1]), argv[2]);
 }
