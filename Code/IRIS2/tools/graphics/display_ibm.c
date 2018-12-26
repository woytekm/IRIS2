#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "ibm-format.h"

#include "WEH001602-lib.h"

main(int argc, char **argv)
 {

   uint8_t row_1[100];
   uint8_t row_2[100];
   int fd;

   if(argc < 2)
    {
     printf("give IBM filename\n");
     exit(0);
    }

   fd = open(argv[1],O_RDONLY);

   lseek(fd,IBM_HEADER_LEN,SEEK_SET);

   if( (read(fd,&row_1,100)) != 100)
    {
     printf("read error!\n");
     exit(0);
    }

   if( (read(fd,&row_2,100)) != 100)
    {
     printf("read error!\n");
     exit(0);
    }

   uint8_t x,y;

   if (!bcm2835_init())
   {
    printf("fatal: cannot initialize BCM2835 library!\n");
    exit(0);
   }
   
   my_spi_WEH001602_init_dark();
   my_spi_WEH001602_init_graphics();

   for(x = 0; x < 80; x++)
    my_spi_WEH001602_out_pixel_pattern(row_1[x],x,0);

  for(x = 0; x < 80; x++)
    my_spi_WEH001602_out_pixel_pattern(row_2[x],x,1);

  my_spi_WEH001602_out_cmd(0b00001100); // activate the screen once the bitmap is loaded

  bcm2835_close();

 }
