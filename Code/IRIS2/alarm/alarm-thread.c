#include "global.h"
#include "WEH001602-lib.h"

void PL_alarm_indicator_thread(void)
 {
   uint8_t was_active;
   uint8_t flip = 0;

   while(1)
    {
       if(G_player_mode == ALARM_ACTIVE)
        {
         was_active = 1;

         if(flip)
          {
           pthread_mutex_lock(&G_display_lock);
           my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "    [ALARM!]    ");
           pthread_mutex_unlock(&G_display_lock);           
          }
         else
          {
           pthread_mutex_lock(&G_display_lock);
           my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
           pthread_mutex_unlock(&G_display_lock);
          }

         flip = 1 - flip;

        }
       else
        {
         if(was_active)
          {
           if(flip == 0)
            {
             pthread_mutex_lock(&G_display_lock);
             my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
             pthread_mutex_unlock(&G_display_lock);
            }
           was_active = 0;
          }
        }

     usleep(70000);

    }
 }


void PL_run_alarm(uint8_t alarm_index)
 {
   uint32_t alarm_duration;
   char soundfile[2048];
   DWORD BASS_chan;
   DWORD chan_status;

   sprintf(soundfile,"%s/%s", ALARM_SOUNDS_PATH, G_alarm_sounds[alarm_index].filename);
   
   if(!G_internal_amp_active)
    PL_set_internal_amp(1);
 
   if(!G_BASS_in_use)
    if(!BASS_Init(AUDIO_OUT_INTERNAL,44100,0,0,NULL)) // always play alarm sound on internal speaker
     {
      PL_debug("PL_run_alarm: BASS init can't initialize device");
      return;
     } 
	 
   G_BASS_in_use |= BASS_IN_USE_BY_ALARM; 

   BASS_SetConfig(BASS_CONFIG_CURVE_VOL,TRUE);

   if(G_alarm_sounds[alarm_index].loop_it == 1)
    {
     BASS_chan = BASS_StreamCreateFile(0,soundfile,0,0,BASS_SAMPLE_LOOP);
     PL_debug("PL_run_alarm: looping %s",soundfile);
    }
   else 
    { 
     BASS_chan = BASS_StreamCreateFile(0,soundfile,0,0,0);
     PL_debug("PL_run_alarm: playing once %s",soundfile);
    }
   
   BASS_ChannelSetAttribute(BASS_chan,BASS_ATTRIB_VOL,0);

   if(!BASS_ChannelPlay(BASS_chan,FALSE))
    {
     PL_debug("PL_run_alarm: BASS error while playing alarm sound: %d. Aborting", BASS_ErrorGetCode());
     G_player_mode = PLAYER_STOP;
     return 0;
    }
  
   #define ALARM_DURATION 2000000
   #define ALARM_LOUDNESS_INCREASE_INTERVAL 110000
    
   BASS_ChannelSlideAttribute(BASS_chan,BASS_ATTRIB_VOL,G_config.volume_level+0.4,ALARM_LOUDNESS_INCREASE_INTERVAL);
	 
   sleep(1);
   
   while(1)
      {
       chan_status = BASS_ChannelIsActive(BASS_chan);

       if(chan_status != BASS_ACTIVE_PLAYING)  
        {
         G_player_mode = PLAYER_STOP;
         PL_debug("PL_run_alarm: alarm end - BASS stopped");
         break;
        }
	
       if(G_player_mode != ALARM_ACTIVE)
        {
         PL_debug("PL_run_alarm: alarm end - turned off");
	 break;
        }

       if(alarm_duration >= ALARM_DURATION)
        {
         G_player_mode = PLAYER_STOP;
         PL_debug("PL_run_alarm: alarm end - reached alarm duration");
         break;
        }
	
       alarm_duration++;
	   
       usleep(15000);
      } 
	  
    BASS_StreamFree(BASS_chan);
	
    G_BASS_in_use ^= BASS_IN_USE_BY_ALARM; // clear flag
    
    if(!G_BASS_in_use)
     {
      PL_debug("PL_run_alarm: Stopping BASS and turning off internal speaker");
      BASS_Free();
      PL_set_internal_amp(0);
     } 
    else
     PL_debug("PL_run_alarm: G_BASS_in_use=%d", G_BASS_in_use);

    return;
	
 } 

void PL_alarm_thread(void)
 {

   PL_debug("PL_alarm_thread: starting");

   while (1)
    {   

     while(G_player_mode != ALARM_ACTIVE)
      { 
       usleep(9000);
      }

     // alarm was triggered
     // clear alarm flag
     G_wakeup = 0;

     if(G_alarm_sound_count > 0) 
       PL_run_alarm(G_alarm_sound_index);
     else
      PL_debug("PL_alarm_thread: no alarm sounds! cannot start alarm!");

     sleep(1);

    }
   
 }


