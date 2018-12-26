#include "global.h"


main()
 {

   unsigned char buffer[NVRAM_SIZE];

   printf("reading config\n");
   PL_load_config(buffer);

   printf("parsing config\n");
   PL_parse_config_buffer(buffer);


 }
