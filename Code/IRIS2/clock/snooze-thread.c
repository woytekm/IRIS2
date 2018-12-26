#include "global.h"

void PL_snooze_timer_thread(void)
 {

  uint16_t snooze_seconds;
  uint8_t snooze_savestate;
  uint8_t save_mode;

  while(1)
   {

    if(G_snooze_state != snooze_savestate)  // something happened
     {

      if(G_snooze_state == SNOOZE_RESET)
       {
        snooze_seconds = 0;
        G_snooze_state = SNOOZE_INACTIVE;
       }

      else if(G_snooze_state == SNOOZE_ACTIVE)
       {
		save_mode = G_player_mode;
        G_player_mode = PLAYER_STOP;
       }

     }
    
    if(G_snooze_state == SNOOZE_ACTIVE)
     {
      snooze_seconds++;
      if(snooze_seconds >= G_config.snooze_timer_time)
       {
        G_snooze_state = SNOOZE_INACTIVE;
        snooze_seconds = 0;
        G_player_mode = save_mode;
       }
     }

    snooze_savestate = G_snooze_state;

    sleep(1);
   }

 }
