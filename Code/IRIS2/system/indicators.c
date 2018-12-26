#include "global.h"
#include "WEH001602-lib.h"

void PL_bt_indicator(void)
 {

   struct stat f_info;
   int stat_err;

   PL_debug("PL_bt_indicator: starting");

   while(1)
    {

     if( (G_global_mode == GLOBAL_MODE_NORMAL) && G_config.use_bt)
      {
        system("/usr/sbin/iris-check-bt-speaker.sh");    

        pthread_mutex_lock(&G_display_lock);
       
        if(stat("/tmp/bt_speaker.connected",&f_info) == -1)
         {
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 1, " \0"); 
          G_bt_connected = 0;
         }
        else
         {
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 1, "\0\0");
          G_bt_connected = 1;
         }

       pthread_mutex_unlock(&G_display_lock);
      }

     sleep(3);

    }

 }
