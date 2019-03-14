#include "global.h"
#include "WEH001602-lib.h"
#include "tts.h"

alarm_data_t *PL_new_alarm(uint8_t hour, uint8_t minute, uint8_t days_of_week, uint8_t al_state, uint8_t al_type, uint8_t sound_idx)
 {

  alarm_data_t *new_alrm;

  new_alrm = malloc(sizeof(alarm_data_t));

  new_alrm->hour = hour;
  new_alrm->minute = minute;
  new_alrm->days_of_week = days_of_week;
  new_alrm->alarm_state = al_state;
  new_alrm->alarm_type = al_type;
  new_alrm->sound_index = sound_idx;
  new_alrm->triggered_today = 0;
  new_alrm->next = NULL;

  return new_alrm;

 }

void PL_clear_alarms_trigger_flag(void)
 {
  alarm_data_t *next_alarm;

  next_alarm = G_alarms;

  while(next_alarm != NULL)
   {
    next_alarm->triggered_today = 0;
    next_alarm = next_alarm->next;
   }

 }

void PL_clock_thread(void)
 {

   time_t now;
   unsigned char timestr[11];
   uint8_t flip = 0, some_alarms_active, prev_hour = 25,i;
   alarm_data_t *alarm;
   uint8_t TTS_thread_msg[2];
   uint8_t x_first,x_second,x_third,x_fourth;
   
   while(1)
     {

      time(&now);
      G_tm = localtime(&now);

      if(G_matrix_mode == MATRIX_MODE_CLOCK)
       {
         if(i == 1)
          {
            sprintf(timestr,"%2d:%02d",G_tm->tm_hour,G_tm->tm_min);
            i = 0;
          }
         else
          {
            i = 1;
            sprintf(timestr,"%2d %02d",G_tm->tm_hour,G_tm->tm_min);
          }

         m_clear();
         if(timestr[0] == ' ')
          x_first = 8;
         else if(timestr[0] == '1')
          x_first = 10;
         else if(timestr[0] == '2')
          x_first = 10;

         m_setcursor(x_first,15);
         m_writechar(timestr[0],1,18,0);

         if(timestr[0] == ' ')
           x_second = 16;
         else if(timestr[0] == '1')
           x_second = 18;
         else if(timestr[0] == '2')
           x_second = 20;

         m_setcursor(x_second,15);

         m_writechar(timestr[1],1,18,0);  // second digit
         m_writechar(timestr[2],1,18,0);  // semicolon

         if(timestr[0] == '1')
           x_third = 32;
         else if(timestr[0] == '2')
           x_third = 32;
         else if(timestr[0] == ' ')
           x_third = 31;
         
         m_setcursor(x_third,15);
         m_writechar(timestr[3],1,18,0);
         m_writechar(timestr[4],1,18,0);
         m_display();

       }

      if(G_global_mode == GLOBAL_MODE_SETUP)
       {
        usleep(950000);
        continue;
       }

      if(G_display_mode_lower_row == DISPLAY_MODE_CLOCK_DATE)
       {
        if(flip)
         sprintf(timestr,"%02d:%02d %02d/%02d/%02d",G_tm->tm_hour, G_tm->tm_min, G_tm->tm_mday, G_tm->tm_mon, G_tm->tm_year-100);
        else if( (G_player_mode == PLAYER_STREAM) || (G_display_mode_lower_row == DISPLAY_MODE_RSS) )
         sprintf(timestr,"%02d %02d %02d/%02d/%02d",G_tm->tm_hour, G_tm->tm_min, G_tm->tm_mday, G_tm->tm_mon, G_tm->tm_year-100);
        else
         sprintf(timestr,"                       "); // crude screen saver - this should be substituted with large graphic clock moving across the screen when unit is idle
       }

      if(G_display_mode_lower_row == DISPLAY_MODE_CLOCK)
       {
        if(flip)
         sprintf(timestr,"%02d:%02d",G_tm->tm_hour, G_tm->tm_min);
        else if( (G_player_mode == PLAYER_STREAM) || (G_display_mode_lower_row == DISPLAY_MODE_RSS) || G_config.bt_sink )
         sprintf(timestr,"%02d %02d",G_tm->tm_hour, G_tm->tm_min);
        else
         sprintf(timestr,"     "); // crude screen saver - this should be substituted with large graphic clock moving across the screen when unit is idle

        if(G_matrix_mode == MATRIX_MODE_CLOCK)  // blank upper display if matrix is displaying clock
         sprintf(timestr,"     ");

       }
	   
      if(G_display_mode_lower_row == DISPLAY_MODE_CLOCK_DATE)
       {
        flip = 1 - flip;
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 1, &timestr);
        pthread_mutex_unlock(&G_display_lock);
       }
      else if(G_display_mode_lower_row == DISPLAY_MODE_CLOCK)
       {
        flip = 1 - flip;
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 5, &timestr);
        pthread_mutex_unlock(&G_display_lock);
       }

     if((G_tm->tm_min == 0) && (G_tm->tm_sec > 5) && (G_tm->tm_hour != prev_hour))  // this should be triggered once an hour (5 sec. after full hour)
      {
       if( ((G_player_mode == PLAYER_STREAM) && (G_config.tell_time_when_on == TELL_TIME_WHEN_ON)) 
           || (G_config.tell_time_when_on == TELL_TIME_ALL_THE_TIME) )
         if(G_TTS_state == TTS_STOPPED)
          {
           PL_debug("PL_clock_thread: telling the time at full hour.");
           TTS_thread_msg[0] = TTS_SAY_CURRENT_TIME;
           TTS_thread_msg[1] = 0; // does not matter
           write(G_TTS_thread_input[1],&TTS_thread_msg,2);
          }
        prev_hour = G_tm->tm_hour;
      }
     
     if(G_sleep_timer_active)
       G_sleep_timer_counter++;

     if(G_sleep_timer_counter == (G_config.sleep_timer_time*60))
      {
       G_player_mode = PLAYER_STOP;      
       G_sleep_timer_active = 0; G_sleep_timer_counter = 0;
       pthread_mutex_lock(&G_display_lock);
       my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 13, " \0");  // clear "sleep timer active" indicator
       pthread_mutex_unlock(&G_display_lock);
       PL_debug("PL_clock_thread: sleep timer reached programmed value %d min. - streaming stopped (if was active)",G_config.sleep_timer_time);
      }


     alarm = G_alarms;
     some_alarms_active = 0;

     while(alarm != NULL)
      {

        if(alarm->alarm_state == ALARM_ON)
         {
          some_alarms_active = 1;
          if( (alarm->hour == G_tm->tm_hour) && (alarm->minute == G_tm->tm_min) && ( ((alarm->days_of_week >> G_tm->tm_wday)&1) == 1) && (alarm->triggered_today != G_tm->tm_yday) )
           {
            PL_debug("PL_clock_thread: alarm triggered at %02d:%02d, alarm type=%d",alarm->hour, alarm->minute, alarm->alarm_type);
            
            alarm->triggered_today = G_tm->tm_yday;

            G_wakeup = 1;

            if(alarm->alarm_type == ALARM_TYPE_STREAM)
             {
             if(G_player_mode == PLAYER_STOP)
              G_player_mode = PLAYER_STREAM; 
             }
  	    else if(alarm->alarm_type == ALARM_TYPE_RSS_AND_STREAM)
  	     {
              if(G_player_mode == PLAYER_STOP)
               {
                G_player_mode = PLAYER_STREAM;
	        if(G_TTS_state == TTS_STOPPED)
                 {
                  TTS_thread_msg[0] = TTS_READ_RSS_NOW;
                  TTS_thread_msg[1] = G_rss_feed_index; 
                  write(G_TTS_thread_input[1],&TTS_thread_msg,2);
                 }
                }
	      }
            else if(alarm->alarm_type == ALARM_TYPE_ALARM_SOUND)
	     {
              PL_debug("PL_clock_thread: triggering alarm sound");
            
              if(alarm->sound_index <= G_alarm_sound_count)
               G_alarm_sound_index = alarm->sound_index;
              else
               G_alarm_sound_index = 0;

	      if(G_player_mode == PLAYER_STOP)
		G_player_mode = ALARM_ACTIVE;
             }

           }
         }

        alarm = alarm->next;

       }
     
      if(G_display_mode_lower_row == DISPLAY_MODE_CLOCK) 
       {
        pthread_mutex_lock(&G_display_lock);
        if(some_alarms_active)
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 11, "\x05\0");
        else
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 11, " \0");
        pthread_mutex_unlock(&G_display_lock);
       }

      usleep(950000);

    }

 }
