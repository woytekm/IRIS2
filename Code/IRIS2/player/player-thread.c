#include "global.h"
#include "version.h"
#include "WEH001602-lib.h"
#include "tts.h"
#include "matrix.h"


void PL_set_internal_amp(uint8_t mode)
 {
   if(mode==1)
    {
     bcm2835_gpio_write(INTERNAL_AMP_CONTROL_PIN, HIGH);    
     G_internal_amp_active = 1;
    }
   else if(mode==0)
    {
     bcm2835_gpio_write(INTERNAL_AMP_CONTROL_PIN, LOW);
     G_internal_amp_active = 0;
    }
 }

void PL_disp_channel_VU(uint8_t row, uint8_t left_level, uint8_t right_level)
 {
   int8_t i,j=0,chr;
   char display_string[17];

   display_string[16] = 0x0;

   //display_string[7] = 0xF6;
   //display_string[8] = 0xF7;
   
   display_string[7] = ']';
   display_string[8] = '[';

   for(i=6; i>=0; i--)
    {
     chr = 0x01;
     j++;
     if(j>left_level)
      chr = ' ';
     display_string[i] = chr;
    }

   j=0;

   for(i=9; i<=15; i++)
    {
     chr = 0x01;
     j++;
     if(j>right_level)
      chr = ' ';
     display_string[i] = chr;
    }
 
   pthread_mutex_lock(&G_display_lock);
   my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, display_string);
   pthread_mutex_unlock(&G_display_lock);   

 }


void PL_player_display_thread(void)
 {

   char *meta_copy;
   char greeting_str[25];
   DWORD channel_level;
   uint16_t left_level;
   uint16_t right_level;

   PL_debug("PL_player_display_thread: starting");

   sprintf(greeting_str,">> Iris v%d.%d <<",IRIS_VER_MAJOR, IRIS_VER_MINOR);
   

   while(1)
    {

      if(G_kill_vu)
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                \0");
        pthread_mutex_unlock(&G_display_lock);

        G_kill_vu = 0; // does not do anything - implemented for future use
        G_vu_active = 0;
       }

     if((G_config.bt_sink) && (G_player_mode == PLAYER_STOP))
       {
        my_spi_WEH001602_scroll_meta_once("BT Sink active", TOP_ROW);
        sleep(1);
       }
     if((G_display_mode_upper_row ==  DISPLAY_MODE_PLAYER_META) && (G_player_mode == PLAYER_STREAM))
       {
        meta_copy = strdup(G_current_stream_META);
        my_spi_WEH001602_scroll_meta_once(meta_copy, TOP_ROW);
        free(meta_copy);
       }
     else if((G_display_mode_lower_row ==  DISPLAY_MODE_PLAYER_META) && (G_player_mode == PLAYER_STREAM))
      {
       meta_copy = strdup(G_current_stream_META);
       my_spi_WEH001602_scroll_meta_once(G_current_stream_META, BOTTOM_ROW);
       free(meta_copy);
      }
     else if((G_display_mode_upper_row ==  DISPLAY_MODE_PLAYER_VU) && (G_player_mode == PLAYER_STREAM))
      {
       G_vu_active = 1;

       if(G_stream_channel_status == BASS_ACTIVE_PLAYING)
        channel_level = BASS_ChannelGetLevel(G_stream_chan);
       else
        channel_level = 0;

       left_level = (LOWORD(channel_level)/4681); // 4681 because we are dividing 32768 (max value per channel) onto 7 display segments
       right_level = (HIWORD(channel_level)/4681);

       PL_disp_channel_VU(1,left_level,right_level);

       usleep(VU_METER_REFRESH_DELAY);
      } 

     usleep(10000);

    }

  }


unsigned char *PL_BASS_get_icy_name(unsigned char *tag_stream)
 {
  uint8_t double_null, end = 0, found;
  uint16_t pos = 0;

  while(!end)
   {
    if((tag_stream[pos] == 0x0) && (tag_stream[pos+1] == 0x0))
     break;
    if(strstr(&tag_stream[pos],"icy-name:"))
     return &tag_stream[pos+9];
    pos++;
   }

  return NULL;
 }

void PL_BASS_print_http_header(unsigned char *http_tags)
 {
  uint8_t double_null, end = 0, found;
  uint16_t pos = 0;

  while(!end)
   {
    if((http_tags[pos] == 0x0) && (http_tags[pos+1] == 0x0))
     break;
    if(http_tags[pos] == '\0')
     printf(",");
    else
     printf("%c",http_tags[pos]);
    pos++;
   }

 }
 
void PL_BASS_parse_meta(HSYNC handle, DWORD G_stream_channel, DWORD data, void *user)
 {
  const char *meta_string, *p, *p1;
  meta_string = BASS_ChannelGetTags(G_stream_channel,BASS_TAG_META);
  p=strstr(meta_string,"StreamTitle='");
  if (p) 
   {
     p=strdup(p+13);
     strchr(p,';')[-1]=0;
     p1 = malloc(strlen(p) + strlen(G_streams[G_stream_index]->url) + 10);
     sprintf(p1,"%s [%s] ",p, G_streams[G_stream_index]->url);
     strcpy(G_current_stream_META,p1);
     free(p);
     free(p1);
   }
  else 
   strcpy(G_current_stream_META,'\0');
 } 

BOOL PL_RecordProc(HRECORD handle, const void *buffer, DWORD length, void *user)
{
    BASS_StreamPutData(G_stream_chan, buffer, length); // pass the data to the stream
    return TRUE; // continue recording
}
 
void PL_player_thread(void)
 {
  DWORD act,time,level,buffer_fill,buffer_fill_prev,buffering_timer;
  BOOL ismod;
  QWORD pos;
  uint8_t playing, bt_sink_inited, current_stream_index, failure_count = 0, cant_init = 0;
  int BASS_err = 0;
  uint16_t tag_check_interval = 0, additional_init_delay = 0;
  const char *http_tags, *stream_tags, *other_tags, *p, *p1, *icy_name;
  uint8_t used_internal_spk = 0;
  int8_t output;
  
  HRECORD rchan;
 
  PL_debug("PL_player_thread: starting");
 
  // check the correct BASS was loaded
  if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
    PL_debug("PL_player_thread: an incorrect version of BASS was loaded.");
    return;
  }
 
  while(1)
    {       

      playing = 0;
      bt_sink_inited = 0;
 
      G_prev_volume_level = G_config.volume_level;

      while((G_player_mode!=PLAYER_STREAM) && (!G_config.bt_sink))
       usleep(60*1000); 

      while(G_config.bt_sink)
       {
        if(!bt_sink_inited)
         {
          output = AUDIO_OUT_INTERNAL;
          BASS_Init(output,44100,0,0,NULL);
          BASS_RecordInit(0);
          BASS_RecordSetInput(0,BASS_INPUT_ON,1);
          rchan=BASS_RecordStart(44100,2,0,&PL_RecordProc,0);
          G_stream_chan = BASS_StreamCreate(44100, output, 0, STREAMPROC_PUSH, NULL);
          G_config.volume_level = 0.10;        // turn down volume on bluetooth sink as we don't know what is set on bluetooth source
          BASS_ChannelSetAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level);
          BASS_ChannelPlay(G_stream_chan,0);
          bt_sink_inited = 1;
         }

        if(G_config.volume_level != G_prev_volume_level)
          BASS_ChannelSetAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level);

        G_prev_volume_level = G_config.volume_level;

        usleep(90*1000);
       }
      if(bt_sink_inited)
       {
        BASS_ChannelStop(rchan);
        BASS_RecordFree();
        BASS_ChannelStop(G_stream_chan); 
        BASS_StreamFree(G_stream_chan);
        BASS_Free();
        bt_sink_inited = 0;
        G_config.volume_level = 0.40;        // set volume back to some sane level for internet streams
        continue;
       }
    
      pthread_mutex_lock(&G_display_lock);
      my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 15, "\xf6\0"); // arrow indicating that stream start command was received
      pthread_mutex_unlock(&G_display_lock); 
  
      // setup output device

       if((!G_internal_amp_active)&&(!G_bt_connected))
        {
         PL_debug("PL_player_thread: turning on internal speaker amp");
         PL_set_internal_amp(1);
         used_internal_spk = 1;
         output = AUDIO_OUT_INTERNAL;
        }
       else
        output = AUDIO_OUT_DEFAULT; // this should be BT speaker

      if(!G_BASS_in_use)
       if(!BASS_Init(output,44100,0,0,NULL))
        {
         PL_debug("PL_player_thread: BASS error: can't initialize playback device.");
         cant_init++;
         
         additional_init_delay = cant_init * 40000;
 
         usleep(10*1000 + additional_init_delay);
         if(cant_init == 3)
          {
           G_player_mode = PLAYER_STOP;
           PL_debug("PL_player_thread: couldn't initialize playback device - stop.");
           pthread_mutex_lock(&G_display_lock);
           my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 14, " \0");
           my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 15, " \0");
           my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "  [device error]  ");
           sleep(1);
           my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
           pthread_mutex_unlock(&G_display_lock);
          }
         continue;
        }
       
      cant_init = 0;

      G_BASS_in_use |= BASS_IN_USE_BY_STREAM;
     
      BASS_SetConfig(BASS_CONFIG_NET_BUFFER, 6500); 
      BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, 10000);
      BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,2); // parse stream URL's in *.pls and *.m3u  urls
      BASS_SetConfig(BASS_CONFIG_CURVE_VOL,TRUE); // logarythmic volume scale 

      current_stream_index = G_stream_index;

      // try streaming the file/url

      PL_debug("PL_player_thread: trying to stream: %s (stream type: %d)",G_streams[G_stream_index]->url, G_streams[G_stream_index]->type);

      switch(G_streams[G_stream_index]->type)
       {

        case STREAM_TYPE_MP3: // this is also for OGG streams
          G_stream_chan=BASS_StreamCreateURL(G_streams[G_stream_index]->url,0,BASS_SAMPLE_LOOP|BASS_SAMPLE_FLOAT,0,0);
          break;

        case STREAM_TYPE_AAC:
          G_stream_chan=BASS_AAC_StreamCreateURL(G_streams[G_stream_index]->url,0,BASS_SAMPLE_LOOP|BASS_SAMPLE_FLOAT,0,0);
          break;

        case STREAM_TYPE_FLAC:
          G_stream_chan=BASS_FLAC_StreamCreateURL(G_streams[G_stream_index]->url,0,BASS_SAMPLE_LOOP|BASS_SAMPLE_FLOAT,0,0);
          break;
  
        default:
         PL_debug("PL_player_thread: unknown stream type specified (%d) - stop.",G_streams[G_stream_index]->type);
         G_player_mode = PLAYER_STOP;
         continue;

       }

      if (G_stream_chan)
        pos=BASS_ChannelGetLength(G_stream_chan,BASS_POS_BYTE);
      else
       {

          // Ok, we have failed to create the stream. It's no big deal if player was started by the user. It's a different thing 
          // if player start was triggered by the clock thread (this is an alarm). If we fail to start stream - owner of the device will not be able wake up, 
          // so now we have to fall back to triggering local alarm sound instead of radio stream - do this if G_wakeup equals 1.

          BASS_err = BASS_ErrorGetCode();
          usleep(1000);
          pthread_mutex_lock(&G_display_lock);
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 15, " \0"); // clear arrow 
          my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, " [stream error] "); 
          sleep(1);
          my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                ");
          pthread_mutex_unlock(&G_display_lock);
          G_player_mode = PLAYER_STOP;
          PL_debug("PL_player_thread: couldn't create stream %s (BASS error %d). player stop.",G_streams[G_stream_index]->url, BASS_err);

          if(G_wakeup)
           {
            G_player_mode = PLAYER_STOP;
            usleep(1000);
            G_player_mode = ALARM_ACTIVE;  // Wake up alarm thread. It will clear G_wakeup flag by itself.
           }  

          continue;
       }

       PL_debug("PL_player_thread: stream %s created successfully",G_streams[G_stream_index]->url);
       PL_debug("PL_player_thread: setting stream parameters");
       BASS_ChannelSetSync(G_stream_chan,BASS_SYNC_META,0,&PL_BASS_parse_meta,0);
    
       if(G_TTS_state == TTS_PLAYING)
         BASS_ChannelSetAttribute(G_stream_chan,BASS_ATTRIB_VOL,0.35);
       else
         BASS_ChannelSetAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level);

       PL_debug("PL_player_thread: playing stream");
       BASS_ChannelPlay(G_stream_chan,FALSE);
       
       p1 = malloc(strlen(G_streams[G_stream_index]->url) + 10);
       sprintf(p1,"[%s] ",G_streams[G_stream_index]->url);
       strcpy(G_current_stream_META,p1);
       free(p1);

       playing = 1;
       
       sleep(1);
  
       BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, 40000); // 40 sec. buffering timeout

       if(G_wakeup)
        G_wakeup = 1;
 
       while ((G_player_mode!=PLAYER_STOP) && (G_stream_index == current_stream_index))
         {

          G_stream_channel_status = BASS_ChannelIsActive(G_stream_chan);

          if( (G_stream_channel_status != BASS_ACTIVE_PLAYING) && (G_stream_channel_status != BASS_ACTIVE_STALLED) )  // stream is not playing - restart
           {
            sleep(1);  // throttle this a bit just in case
            PL_debug("PL_player_thread: stream stopped by itself... trying to restart");
            failure_count++;

            if(failure_count > 3) // stop player after 3 consecutive BASS failures
             {
              PL_debug("PL_player_thread: 3 consecutive BASS failures - giving up - stopping stream.");
              G_player_mode = PLAYER_STOP;
              failure_count = 0;

              usleep(1000);

              if(G_wakeup)
               G_player_mode = ALARM_ACTIVE;
             }
    
            break;
           }

          failure_count = 0;

          level=BASS_ChannelGetLevel(G_stream_chan);
          pos=BASS_ChannelGetPosition(G_stream_chan,BASS_POS_BYTE);
          time=BASS_ChannelBytes2Seconds(G_stream_chan,pos);
          buffer_fill=BASS_StreamGetFilePosition(G_stream_chan,BASS_FILEPOS_BUFFER)*100/BASS_StreamGetFilePosition(G_stream_chan,BASS_FILEPOS_END); 

          if(G_TTS_state != TTS_PLAYING)
           if(G_config.volume_level != G_prev_volume_level)
            {
             char debug_str[255];
             sprintf(debug_str,"PL_player_thread: changing volume to: %1.2f",G_config.volume_level);
             PL_debug(debug_str);

             BASS_ChannelSetAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level);
             G_prev_volume_level = G_config.volume_level;
            }
 
          if(tag_check_interval == 30)
           {
             //printf("player-thread: buffer fill %d%\n",buffer_fill);
             stream_tags = BASS_ChannelGetTags(G_stream_chan,BASS_TAG_META);
             other_tags = BASS_ChannelGetTags(G_stream_chan,BASS_TAG_ICY);

             http_tags = BASS_ChannelGetTags(G_stream_chan, BASS_TAG_HTTP);

             if(http_tags != NULL) { icy_name = PL_BASS_get_icy_name(http_tags); }
             if(other_tags != NULL) { icy_name = PL_BASS_get_icy_name(other_tags); }

             tag_check_interval = 0;
             if(stream_tags != NULL)
              {
               p=strstr(stream_tags,"StreamTitle='");
               if (p) {
                p=strdup(p+13);
                strchr(p,';')[-1]=0;

                if(icy_name == NULL)
                 {
                  p1 = malloc(strlen(p) + strlen(G_streams[G_stream_index]->url) + 10);
                  sprintf(p1,"%s [%s] ",p, G_streams[G_stream_index]->url);
                 }
                else
                 {
                  p1 = malloc(strlen(p) + strlen(icy_name) + 10);
                  sprintf(p1,"%s [%s] ",p, icy_name);
                 }

                strcpy(G_current_stream_META,p1);
                free(p);
                free(p1);
             }
             //free(stream_tags);
            }
           }
 
     if(buffer_fill < 30)  
       {
         buffering_timer++;
         if(G_global_mode == GLOBAL_MODE_NORMAL)
          {
           pthread_mutex_lock(&G_display_lock);
           my_spi_WEH001602_def_char(2,0x001F13151315131F);
           my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 14, "\x2\0");  // print "buffering" indicator
           pthread_mutex_unlock(&G_display_lock);
          }
       }

     if((buffer_fill < 50) && (buffer_fill == buffer_fill_prev)) // stalled?
      buffering_timer++;

     if((buffer_fill > 40) && (buffer_fill < 70))
       {
        buffering_timer = 0; // reset buffering timer
        if(G_global_mode == GLOBAL_MODE_NORMAL)
         {
          pthread_mutex_lock(&G_display_lock);
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 14, " \0");
          pthread_mutex_unlock(&G_display_lock);
         }
       }

     if(buffer_fill > 70)
       {
        buffering_timer = 0; // reset buffering timer
        if(G_global_mode == GLOBAL_MODE_NORMAL)
         {
          pthread_mutex_lock(&G_display_lock);
          my_spi_WEH001602_def_char(2,0x001F181515151B1F);
          my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 14, "\x2\0"); // print "quality stream" indicator (buffer filled more than 70%)
          pthread_mutex_unlock(&G_display_lock);
         }
       }

     if(buffering_timer > ENOUGH_BUFFERING)
       {
        PL_debug("PL_player_thread: bufering takes too long - restarting stream");
        break;
       }

     if(G_config.bt_sink) // BT Sink engaged
      {
       G_player_mode = PLAYER_STOP;
       G_sleep_timer_active = 0;
       G_sleep_timer_counter = 0;
       G_snooze_state = SNOOZE_RESET;
      }

     buffer_fill_prev = buffer_fill;


     // if we are started by alarm - check if we are actually playing something (check if play loop goes on)
     // if play loop goes on through 50 iterations (stream seems stable) - clear alarm flag 
     // this will prevent starting alarm sound on stream failure.

     if(G_wakeup)
      G_wakeup++;

     if(G_wakeup > 50)
      G_wakeup = 0;

     tag_check_interval++;

     usleep(90*1000);

     //PL_debug("PL_player_thread: play loop: (buffer_fill = %d, buffering_timer = %d)",buffer_fill, buffering_timer); 

   } // end of play loop
 
   if(playing) // cleanup and go to the next stream, or wait for play command
    {
      PL_debug("PL_player_thread: stopping stream and cleaning up...");
      // fade-out to avoid a "click"
      BASS_ChannelSlideAttribute(G_stream_chan,BASS_ATTRIB_VOL,0,300);
      // wait for slide to finish
      while (BASS_ChannelIsSliding(G_stream_chan,0)) usleep(1000);
        BASS_ChannelStop(G_stream_chan);
      BASS_StreamFree(G_stream_chan);

      if(G_matrix_mode == MATRIX_MODE_ANALYSER)
       G_clear_matrix = 1;

      strcpy(G_current_stream_META," ");
      if(G_global_mode == GLOBAL_MODE_NORMAL)
       {
        pthread_mutex_lock(&G_display_lock);
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 14, " \0");
        my_spi_WEH001602_out_text_at_col(BOTTOM_ROW, 15, " \0");
        my_spi_WEH001602_out_text_at_col(TOP_ROW, 0, "                \0");
        pthread_mutex_unlock(&G_display_lock);
       }

      G_BASS_in_use ^= BASS_IN_USE_BY_STREAM; // clear channel usage flag
    
      if(!G_BASS_in_use)
       {
        PL_debug("PL_player_thread: stopping BASS");
        BASS_Free();
        if(used_internal_spk)
         {
          PL_debug("PL_player_thread: turning off internal speaker");
          PL_set_internal_amp(0);
          used_internal_spk = 0;
         }
       }
      
      G_prev_output_device = output;
      G_stream_channel_status = BASS_ACTIVE_STOPPED;
      buffering_timer = 0;

      usleep(50000);  
     
    }
 
   }
 
 }


