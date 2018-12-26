#include "global.h"
#include "tts.h"
#include "WEH001602-lib.h"

void PL_TTS_indicator_thread(void)
 {

  uint8_t tts_indicator = 0;

  while(1)
   {

    if(G_TTS_state == TTS_PLAYING)
      if(!tts_indicator) tts_indicator = 1;

    if(G_TTS_state == TTS_PREPARING)
     tts_indicator = 1 - tts_indicator;

    if(G_TTS_state == TTS_STOPPED)
     tts_indicator = 0;

    if(tts_indicator)
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 2, "\x7\0"); 
        pthread_mutex_unlock(&G_display_lock);
       }
    else
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 2, " \0");
        pthread_mutex_unlock(&G_display_lock);
       }

    usleep(230000);

   }

 }
