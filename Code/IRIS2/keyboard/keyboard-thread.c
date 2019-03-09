#include "global.h"
#include "keyboard.h"
#include "WEH001602-lib.h"
#include "tts.h"

void PL_dispatch_keyboard_event(uint8_t keyboard_state)
 {
  
  char debug_msg[255];
  char volume_msg[16];
  char stream_name_msg[16];

  uint8_t TTS_thread_msg[2]; // two bytes: 1 = operation type, 2 = stream index

  PL_debug("PL_dispatch_keyboard_event: keymap: %d",keyboard_state);

  if(keyboard_state == KEY_SETUP)
   {
    
    if(G_global_mode == GLOBAL_MODE_SETUP)  // go back from setup to normal
     {
      G_global_mode = GLOBAL_MODE_NORMAL;
      usleep(5000);
      G_display_mode_upper_row = G_dmur_save;
      G_display_mode_lower_row = G_dmlr_save;

      if(G_player_mode == PLAYER_STREAM)
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 15, "\xf6\0"); // little arrow indicating that stream start command was received
        pthread_mutex_unlock(&G_display_lock);
       }

      if(G_sleep_timer_active)
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 13, "\x04\0");  // set "sleep timer active" indicator
        pthread_mutex_unlock(&G_display_lock);
       }

      PL_debug("PL_dispatch_keyboard_event: went from setup to normal");

     }

    else if(G_global_mode == GLOBAL_MODE_NORMAL)  // start setup
     {
      G_dmur_save = G_display_mode_upper_row;
      G_dmlr_save = G_display_mode_lower_row;
      G_display_mode_upper_row = DISPLAY_MODE_SETUP;
      G_display_mode_lower_row = DISPLAY_MODE_SETUP;

      if(G_scroll_meta_active)
       {
        G_kill_meta_scroll = 1;
       }
      if(G_scroll_rss_active)
       {
        G_kill_rss_scroll = 1;
       }
      if(G_vu_active)
       {
        G_kill_vu = 1;
       }

      usleep(95000); // sleep one "cycle" of the clock thread
      pthread_mutex_lock(&G_display_lock);
      my_spi_WEH001602_clear();
      pthread_mutex_unlock(&G_display_lock);
      G_global_mode = GLOBAL_MODE_SETUP;
      PL_debug("PL_dispatch_keyboard_event: went from normal to setup");
     }

   }

  else if((keyboard_state == KEY_VOL_UP)||(keyboard_state == KEY_VOL_UP_LONG))
   {
    if(G_config.volume_level < 1)
     {
       G_config.volume_level += 0.02;
       sprintf(debug_msg,"PL_dispatch_keyboard_event: volume up (%1.2f) now",G_config.volume_level);
       PL_debug(debug_msg);
     }

    if(G_config.volume_level == 0.98)
     sprintf(volume_msg,"[volume max]",G_config.volume_level);
    else
     sprintf(volume_msg,"[volume %1.2f]",G_config.volume_level);

    pthread_mutex_lock(&G_display_lock);
    my_spi_WEH001602_out_text(TOP_ROW,"                ");
    my_spi_WEH001602_out_text(TOP_ROW,volume_msg);
    usleep(VOLUME_MSG_DLY);
    my_spi_WEH001602_out_text(TOP_ROW,"                ");
    pthread_mutex_unlock(&G_display_lock);
   }

  else if((keyboard_state == KEY_VOL_DOWN)||(keyboard_state == KEY_VOL_DOWN_LONG))
   {
    if(G_config.volume_level > 0.02)
     {
       G_config.volume_level -= 0.02;
       sprintf(debug_msg,"PL_dispatch_keyboard_event: volume down (%1.2f) now",G_config.volume_level);
       PL_debug(debug_msg);
     }

    if(G_config.volume_level == 0.02)
     sprintf(volume_msg,"[volume min]",G_config.volume_level);
    else
     sprintf(volume_msg,"[volume %1.2f]",G_config.volume_level);

    pthread_mutex_lock(&G_display_lock);
    my_spi_WEH001602_out_text(TOP_ROW,"                ");
    my_spi_WEH001602_out_text(TOP_ROW,volume_msg);
    usleep(VOLUME_MSG_DLY);
    my_spi_WEH001602_out_text(TOP_ROW,"                ");
    pthread_mutex_unlock(&G_display_lock);
   }

  else if(keyboard_state == KEY_SNOOZE_CENTER)
   {
    if((G_player_mode == PLAYER_STREAM) || (G_player_mode == ALARM_ACTIVE))
     {
      PL_debug("PL_dispatch_keyboard_event: SNOOZE_CENTER pressed and evaluated");
      if(G_snooze_state == SNOOZE_INACTIVE)
         G_snooze_state = SNOOZE_ACTIVE;
      else G_snooze_state = SNOOZE_RESET;
     }

   }

  else if((keyboard_state == KEY_RADIO_ONOFF_LONG) && (G_player_mode == PLAYER_STREAM))
   {
    if(G_config.bt_sink)
     {
      pthread_mutex_lock(&G_display_lock);
      my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "BT Sink active");
      sleep(1);
      my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
      pthread_mutex_unlock(&G_display_lock);
     }
    else
     {
      if(G_sleep_timer_active)
       {
         G_sleep_timer_active = 0; G_sleep_timer_counter = 0;
         pthread_mutex_lock(&G_display_lock);
         my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 13, " \0");  // clear "sleep timer active" indicator
         pthread_mutex_unlock(&G_display_lock);
         PL_debug("PL_dispatch_keyboard_event: sleep timer deactivated");
        }
      else
        {
         G_sleep_timer_active = 1; G_sleep_timer_counter = 0;
         pthread_mutex_lock(&G_display_lock);
         my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 13, "\x04\0");  // set "sleep timer active" indicator
         pthread_mutex_unlock(&G_display_lock);
         PL_debug("PL_dispatch_keyboard_event: sleep timer activated");
        }
     }
   }

  else if(keyboard_state == KEY_SNOOZE_LEFT)
   {
    if((G_display_mode_upper_row == DISPLAY_MODE_OFF) && (G_display_mode_save != DISPLAY_MODE_OFF))
     G_display_mode_upper_row = G_display_mode_save;
    else if((G_display_mode_upper_row == DISPLAY_MODE_OFF) && (G_display_mode_save == DISPLAY_MODE_OFF))
     G_display_mode_upper_row = DISPLAY_MODE_PLAYER_META;

    else if(G_display_mode_upper_row == DISPLAY_MODE_RSS)
     {
      if(G_scroll_rss_active)
        G_kill_rss_scroll = 1;
      G_display_mode_upper_row = DISPLAY_MODE_PLAYER_META;
     }
  
    else if( (G_display_mode_upper_row == DISPLAY_MODE_PLAYER_META) && (G_rss_feed_count > 0) )
     {
      if(G_scroll_meta_active)
       G_kill_meta_scroll = 1;
      G_display_mode_upper_row = DISPLAY_MODE_PLAYER_VU;
     }

    else if( (G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU) && (G_rss_feed_count > 0) )
     {
      G_display_mode_upper_row = DISPLAY_MODE_RSS;
      if(G_vu_active)
        G_kill_vu = 1;
     }

    else if( (G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU) && (G_rss_feed_count == 0) )
     {
      G_display_mode_upper_row = DISPLAY_MODE_PLAYER_META;
      if(G_vu_active)
        G_kill_vu = 1;
     }

   }

  else if(keyboard_state == KEY_SNOOZE_LEFT_LONG)
   {
    G_display_mode_save = G_display_mode_upper_row;
    G_display_mode_upper_row = DISPLAY_MODE_OFF;

    if(G_scroll_meta_active)
     G_kill_meta_scroll = 1;
    else if(G_scroll_rss_active)
     G_kill_rss_scroll = 1;
   }

 else if(keyboard_state == KEY_SNOOZE_CENTER_LONG)  // start/stop TTS function
  {
   if(G_TTS_state == TTS_STOPPED)
    {
     TTS_thread_msg[0] = TTS_READ_RSS_NOW;
     TTS_thread_msg[1] = G_rss_feed_index;
     PL_debug("PL_dispatch_keyboard_event: writing %d, %d to TTS thread receive pipe\n",TTS_thread_msg[0], TTS_thread_msg[1]);
     write(G_TTS_thread_input[1],&TTS_thread_msg,2);
    }
   else if(G_TTS_state == TTS_PLAYING)
    {
     TTS_thread_msg[0] = TTS_STOP;
     TTS_thread_msg[1] = 0;
     write(G_TTS_thread_input[1],&TTS_thread_msg,2);
    }
  }

 else if(keyboard_state == KEY_RADIO_ONOFF)
   {
    if(G_config.bt_sink)
     {
      pthread_mutex_lock(&G_display_lock);
      my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "BT Sink active");
      sleep(1);
      my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
      pthread_mutex_unlock(&G_display_lock);
     }
    else
     {
      if(G_player_mode == PLAYER_STREAM)
       {
        G_player_mode = PLAYER_STOP;
        G_sleep_timer_active = 0;
        G_sleep_timer_counter = 0;
        G_snooze_state = SNOOZE_RESET;

        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 13, " \0");  // clear "sleep timer active" indicator
        pthread_mutex_unlock(&G_display_lock);

        if(G_scroll_meta_active)
         G_kill_meta_scroll = 1;
       }
      else if(G_player_mode == PLAYER_STOP)
       {
        if(G_stream_count > 0)
          G_player_mode = PLAYER_STREAM;
        else
         {
           pthread_mutex_lock(&G_display_lock);
           my_spi_WEH001602_out_text(TOP_ROW,"                ");
           my_spi_WEH001602_out_text(TOP_ROW,"[ no stations! ]");
           usleep(70000);
           my_spi_WEH001602_out_text(TOP_ROW,"                ");
           pthread_mutex_unlock(&G_display_lock);
           return;
         }
       }
      else if(G_player_mode == ALARM_ACTIVE)
       {
        G_player_mode = PLAYER_STOP;
       }
      }

   }

  else if((keyboard_state == KEY_TUNING_DOWN) && (G_player_mode == PLAYER_STREAM) 
          && ((G_display_mode_upper_row == DISPLAY_MODE_PLAYER_META) || (G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU)) )
    {
     if(G_stream_index == 0)
      G_stream_index = G_stream_count-1;
     else
      G_stream_index--;
     if(G_scroll_meta_active)
      {
       G_kill_meta_scroll = 1;
       strcpy(G_current_stream_META,"\0");
      }
     G_search_direction = SEARCH_BACK;
    }

  else if( ((keyboard_state == KEY_TUNING_DOWN)||(keyboard_state == KEY_TUNING_DOWN_LONG))
           && (G_player_mode == PLAYER_STOP) && ((G_display_mode_upper_row == DISPLAY_MODE_PLAYER_META)||(G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU)) )
    {
     if(G_stream_count == 0)
      {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text(TOP_ROW,"                ");
        my_spi_WEH001602_out_text(TOP_ROW,"[ no stations! ]");
        usleep(70000);
        my_spi_WEH001602_out_text(TOP_ROW,"                ");
        pthread_mutex_unlock(&G_display_lock);
        return;
      }    

     if(G_stream_index == 0)
      G_stream_index = G_stream_count-1;
     else
      G_stream_index--;

     pthread_mutex_lock(&G_display_lock);
     my_spi_WEH001602_out_text(TOP_ROW,"                ");
     snprintf(stream_name_msg,16,"[%s]",G_streams[G_stream_index]->name);
     my_spi_WEH001602_out_text(TOP_ROW,stream_name_msg);
     usleep(80000);
     my_spi_WEH001602_out_text(TOP_ROW,"                ");
     pthread_mutex_unlock(&G_display_lock);

     G_search_direction = SEARCH_BACK;
    }

  else if((keyboard_state == KEY_TUNING_DOWN) && (G_display_mode_upper_row == DISPLAY_MODE_RSS))
    {
     if(G_rss_feed_index == 0)
      G_rss_feed_index = G_rss_feed_count-1;
     else
      G_rss_feed_index--;
     if(G_scroll_rss_active)
      G_kill_rss_scroll = 1;
     G_search_direction = SEARCH_BACK;
    }

  else if((keyboard_state == KEY_TUNING_UP) && (G_player_mode == PLAYER_STREAM) && ((G_display_mode_upper_row == DISPLAY_MODE_PLAYER_META)||(G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU)) )
    {

     if(G_stream_index == G_stream_count-1)
      G_stream_index = 0;
     else
      G_stream_index++;
     if(G_scroll_meta_active)
      {
       G_kill_meta_scroll = 1;
       strcpy(G_current_stream_META,"\0");
      }
     G_search_direction = SEARCH_FORWARD;
    }

  else if( ((keyboard_state == KEY_TUNING_UP)||(keyboard_state == KEY_TUNING_UP_LONG))
           && (G_player_mode == PLAYER_STOP) && ((G_display_mode_upper_row == DISPLAY_MODE_PLAYER_META)||(G_display_mode_upper_row == DISPLAY_MODE_PLAYER_VU)) )
    {

     if(G_stream_count == 0)
      {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text(TOP_ROW,"                ");
        my_spi_WEH001602_out_text(TOP_ROW,"[ no stations! ]");
        usleep(70000);
        my_spi_WEH001602_out_text(TOP_ROW,"                ");
        pthread_mutex_unlock(&G_display_lock);
        return;
      }

     if(G_stream_index == G_stream_count-1)
      G_stream_index = 0;
     else
      G_stream_index++;

     pthread_mutex_lock(&G_display_lock);
     my_spi_WEH001602_out_text(TOP_ROW,"                ");
     snprintf(stream_name_msg,16,"[%s]",G_streams[G_stream_index]->name);
     my_spi_WEH001602_out_text(TOP_ROW,stream_name_msg);
     usleep(50000);
     my_spi_WEH001602_out_text(TOP_ROW,"                ");
     pthread_mutex_unlock(&G_display_lock);

     G_search_direction = SEARCH_FORWARD;
    }

  else if((keyboard_state == KEY_TUNING_UP) && (G_display_mode_upper_row == DISPLAY_MODE_RSS))
    {
     if(G_rss_feed_index == G_rss_feed_count-1)
      G_rss_feed_index = 0;
     else
      G_rss_feed_index++;
     if(G_scroll_rss_active)
      G_kill_rss_scroll = 1;
     G_search_direction = SEARCH_FORWARD;
    }

 }

void PL_keyboard_thread(void)
 {

  uint8_t keyboard_state = 0, prev_keyboard_state = 0, encoder_event;
  int setup_input_pipe;

  setup_input_pipe = G_setup_key_input[1];


  if(!PL_keyboard_init())
   {
    PL_debug("PL_keyboard_thread: keyboard initialization error - exit");
    exit(0);
   }

  PL_debug("PL_keyboard_thread: entering polling loop");

  while(1)
   {

     keyboard_state = PL_keyboard_scan();

     if(keyboard_state != prev_keyboard_state)
      {
       //PL_debug("PL_keyboard_thread: new keyboard state: %d", keyboard_state);
       if(G_global_mode == GLOBAL_MODE_SETUP)
        {
         write(setup_input_pipe,&keyboard_state,1); // if we are in setup mode - pass keystroke to setup thread
         if(keyboard_state == KEY_SETUP)
          PL_dispatch_keyboard_event(keyboard_state);
        }
       else
        PL_dispatch_keyboard_event(keyboard_state);

       prev_keyboard_state = keyboard_state;

      }

     encoder_event = PL_get_encoder_event();

     if(encoder_event != ENCODER_NO_EVENT)
      if(G_global_mode != GLOBAL_MODE_SETUP)
        switch (encoder_event)
         {
           case ENCODER_TURN_LEFT:
                PL_dispatch_keyboard_event(KEY_VOL_DOWN);              
                break;
           case ENCODER_TURN_RIGHT:
                PL_dispatch_keyboard_event(KEY_VOL_UP);
                break;
         }
      else if(G_global_mode == GLOBAL_MODE_SETUP)
        switch (encoder_event)
         {
           case ENCODER_TURN_LEFT:
                keyboard_state = KEY_VOL_DOWN;
                write(setup_input_pipe,&keyboard_state,1);
                break;
           case ENCODER_TURN_RIGHT:
                keyboard_state = KEY_VOL_UP;
                write(setup_input_pipe,&keyboard_state,1);
                break;
         }

     usleep(KEYBOARD_POLLING_LOOP_USEC);

   }

 }
