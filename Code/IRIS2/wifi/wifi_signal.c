#include "global.h"
#include "wireless.h"
#include "WEH001602-lib.h"


void PL_wifi_signal_thread()
 {

  int goterr = 0;
  char ap_addr_outcome[255];
  char signal_indicator[2];
  uint8_t connection_test;
  uint8_t signal_level;

  FILE * f;
  
  PL_debug("PL_wifi_signal_thread: starting");

  while(1)
   {

    signal_level = 0;

    // iwconfig shell wrapper - a lot less work that wrestling with wireless tools code (which can easily go out of date)
    // big disadvantge of system() solution is that it's hogging CPU 

    system("/usr/sbin/iris-wifi-signal-level.sh");
    
    f = fopen("/tmp/wifi.signal.level", "r");

    if(f==NULL)
      signal_level = 0;
    else
     {
      fscanf(f, "%d", &signal_level);     
      fclose(f);
     }
   
    if(signal_level > 4) signal_level = 0;

    pthread_mutex_lock(&G_display_lock);
    switch (signal_level)
     {
      case 0:
        my_spi_WEH001602_def_char(6,0x0010001000100010); // wifi signal 0
        break;
      case 1:
        my_spi_WEH001602_def_char(6,0x0018001000100010); // wifi signal 1
        break;
      case 2:
        my_spi_WEH001602_def_char(6,0x0018001C00100010); // wifi signal 2
        break;
      case 3:
        my_spi_WEH001602_def_char(6,0x0018001C001E0010); // wifi signal 3
        break;
      case 4:
        my_spi_WEH001602_def_char(6,0x0018001C001E001F); // wifi signal 4
        break;
     }

    pthread_mutex_unlock(&G_display_lock);

    sprintf(signal_indicator,"\x6\0");

    if(G_global_mode == GLOBAL_MODE_NORMAL)
     {
      pthread_mutex_lock(&G_display_lock);
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 0, &signal_indicator);

      pthread_mutex_unlock(&G_display_lock);
     }

    sleep(6);

   }

 }


