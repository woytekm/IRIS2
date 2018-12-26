//
// convert RAW image file to IBM (Iris Bitmap)
// RAW file should be black and white 100x16 bitmap 
//
// IBM format is 64 byte header starting with "IBM" and then bitmap description, then two rows of 100 uint8_t one 
// after the other - one vertical pixel line per uint8_t. This format allows for easy displaying of RAW data 
// on WEH001602 16x2 OLED display in graphic mode
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <string.h>

#include "ibm-format.h"

int main(int argc, char **argv)
 {

   uint8_t *f_buffer;
   int fd,ofd;
   uint16_t filelen;
   struct stat f_info;
   uint16_t x=0,y=0,i;
   uint8_t row,x_counter,y_counter;
   uint8_t bitmap_row_1[100];
   uint8_t bitmap_row_2[100];
   uint8_t hlen;
   char ibm_header[IBM_HEADER_LEN];

   bzero(&bitmap_row_1,100);
   bzero(&bitmap_row_2,100);

   if(argc<2)
    {
      printf("not enough parameters: give raw 16x100 b/w bitmap file name.\n");
      exit(0);
    }

   if(stat(argv[1],&f_info) == -1)
    {
      printf("file open error.\n");
      exit(0);
    }

   filelen = f_info.st_size;

   if(filelen != 1600)
    printf("this file has wrong size... should be 1600 bytes... trying anyway...\n");

   f_buffer = malloc(filelen);

   fd = open(argv[1],O_RDONLY);

   if( (read(fd,f_buffer,filelen)) != filelen)
    {
     printf("file read error.\n");
     exit(0);
    }

   close(fd);

   x_counter = y_counter = 0;
   row = 1;

   for(x=0;x<1599;x++)
    {

     if(f_buffer[x] == 0xff) f_buffer[x] = 1;
     if(x_counter == 100)
      {
       x_counter = 0;
       y_counter++;
      }

     if(y_counter == 8)
      {
       y_counter = 0;
       row=2;
      }

     if(row == 1)
      {
       bitmap_row_1[x_counter] |= (f_buffer[x] << y_counter);
       //printf("%x \n",(f_buffer[x] << y_counter));
      }
     else
      {
       bitmap_row_2[x_counter] |= (f_buffer[x] << y_counter);
      }

     x_counter++;

    }

 // for(i=0;i<99;i++)
 //  printf("%x ",bitmap_row_1[i]);

 // printf("\n");

 // for(i=0;i<99;i++)
 //  printf("%x ",bitmap_row_2[i]);

   ofd = open("bitmap.ibm",O_WRONLY|O_CREAT);
   if(ofd == -1)
    {
     printf("output file write error (%d)\n",errno);
     exit(0);
    }
 
   if(argc==3)
    snprintf(ibm_header,64,"IBM [%s",argv[2]);
   else
    snprintf(ibm_header,64,"IBM [no description");

   hlen = strlen(ibm_header);
 
   if(hlen < IBM_HEADER_LEN) 
    {
     for(i=1; i<(IBM_HEADER_LEN-hlen);i++)
      ibm_header[hlen+i] = ' ';
    } 
    
  ibm_header[IBM_HEADER_LEN-1] = ']';
  
  write(ofd,(void*)&ibm_header,64);
  write(ofd,(void *)&bitmap_row_1,100);
  write(ofd,(void *)&bitmap_row_2,100);
  close(ofd);


 }

