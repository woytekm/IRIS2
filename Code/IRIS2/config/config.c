#include "global.h"
#include "config.h"

void PL_load_config_defaults(void)
 {
   PL_debug("PL_load_config_defaults: no nvram and no fake nvram found - using minimum config defaults!");
   // most of config values are pre-inited for sanity in PL_init()
   // init some additional defaults here
 }

void PL_parse_alarm_sound_def(unsigned char *alarm_sound_def)
 {
	uint8_t loop_it, options;
	unsigned char *token;
	
	token = strtok(alarm_sound_def,",");

        if(token == NULL)
         {
          PL_debug("PL_parse_alarm_sound_def: cannot parse entry - seems incomplete");
          return;
         }
        else
	 strncpy(G_alarm_sounds[G_alarm_sound_count].filename, token, 255);
	
	token = strtok(NULL,",");
        if(token == NULL)
         {
          PL_debug("PL_parse_alarm_sound_def: cannot parse entry - seems incomplete");
          return;
         }
        else
	  loop_it = atoi(token);

	if((loop_it == 0) || (loop_it == 1))
	  G_alarm_sounds[G_alarm_sound_count].loop_it = loop_it;
        else 
	 {PL_debug("PL_parse_alarm_sound_def: invalid sound specification"); G_alarm_sound_count--; return;}
  
        token = strtok(NULL,",");
        if(token == NULL)
         {
          PL_debug("PL_parse_alarm_sound_def: cannot parse entry - seems incomplete");
          return;
         }
        else
	 options = atoi(token);

	if((options >= 0) && (options < 255))
	  G_alarm_sounds[G_alarm_sound_count].options = options;
        else 
	 {PL_debug("PL_parse_alarm_sound_def: invalid sound specification"); return;}

        PL_debug("PL_parse_alarm_sound_def: parsed %s, %d, %d",G_alarm_sounds[G_alarm_sound_count].filename,
	          G_alarm_sounds[G_alarm_sound_count].loop_it, G_alarm_sounds[G_alarm_sound_count].options);

        G_alarm_sound_count++;
			  
 }

void PL_parse_stream_spec(unsigned char *stream_spec)
 {
   unsigned char *token;
   const char *comma = ",";
   uint8_t tmp_val;
 
   G_streams[G_stream_count] = malloc(sizeof(radio_stream_t));
 
   token = strtok(stream_spec,",");

   if(token == NULL)
    {
     G_streams[G_stream_count]->name = malloc(1);
     G_streams[G_stream_count]->name[0] = 0x0;
    }
   else
    {
     G_streams[G_stream_count]->name = malloc(strlen(token)+1);
     strcpy(G_streams[G_stream_count]->name, token);
    }

   token = strtok(NULL,",");

   if(token == NULL)
    {
     PL_debug("PL_parse_stream_spec: invalid stream specification - ignoring this stream");
     free(G_streams[G_stream_count]);
     return;
    }

   tmp_val = atoi(token);

   if((tmp_val < 1) || (tmp_val > 3))
    {
     PL_debug("PL_parse_stream_spec: invalid stream type %d - ignoring this stream",tmp_val);
     free(G_streams[G_stream_count]);
     return;
    }

   G_streams[G_stream_count]->type = tmp_val;

   token = strtok(NULL,",");

   if(token == NULL)
    {
     PL_debug("PL_parse_stream_spec: invalid stream specification - ignoring this stream");
     free(G_streams[G_stream_count]);
     return;
    }

   G_streams[G_stream_count]->url = malloc(strlen(token)+1);
   strcpy(G_streams[G_stream_count]->url, token);

   PL_debug("PL_parse_stream_spec: parsed stream spec: %s, %d, %s",G_streams[G_stream_count]->name, G_streams[G_stream_count]->type, G_streams[G_stream_count]->url);

   G_stream_count++;

 }
 
void PL_parse_alarm_spec(unsigned char *alarm_str)
 {

     uint8_t  days_bitmap, alarm_state, alarm_type, sound_index;
     unsigned char *token;
     const char *comma = ",";
     struct tm timespec;

     token = strtok(alarm_str,",");

     if((token == NULL) || strptime(token,"%H:%M", &timespec) == NULL)
        {
         PL_debug("cannot parse time: %s - ignoring", token);
         return;
        }

     token = strtok(NULL,",");

     if((token == NULL) || (days_bitmap = atoi(token)) > 127)
        {
         PL_debug("invalid day spec bitmap: %d - ignoring alarm",days_bitmap);
         return;
        }

     token = strtok(NULL,",");

     if(token == NULL)
       {
         PL_debug("invalid alarm spec: %s - ignoring alarm",alarm_str);
         return;
       }
     else
      {
       alarm_state = atoi(token);
       if((alarm_state != 0) && (alarm_state != 1))
        {
         PL_debug("invalid alarm state specification: %d - ignoring alarm",alarm_state);
         return;
        }
      }

    token = strtok(NULL,",");

    if(token != NULL)
     {
      alarm_type = atoi(token);
      if(alarm_type > 128) alarm_type = 0;
     }
    else alarm_type = 0;

   token = strtok(NULL,",");

    if(token != NULL)
     {
      sound_index = atoi(token);
      if(sound_index > 254) sound_index = 1;
     }
    else sound_index = 1;
    


     if(G_alarms == NULL)
        {
         G_alarms = PL_new_alarm(timespec.tm_hour, timespec.tm_min, days_bitmap, alarm_state, alarm_type, sound_index);
         G_last_alarm = G_alarms;
        }
     else
        {
         G_last_alarm->next = PL_new_alarm(timespec.tm_hour, timespec.tm_min, days_bitmap, alarm_state, alarm_type, sound_index);
         G_last_alarm = G_last_alarm->next;
        }

     PL_debug("PL_parse_alarm_spec: parsed alarm spec: %d:%d, %d, %d, %d, %d",timespec.tm_hour, timespec.tm_min, days_bitmap, alarm_state, alarm_type, sound_index);

  }

uint8_t PL_build_config(unsigned char *config_buffer)
 {
  uint32_t config_buffer_pos = 0;
  unsigned char config_line[1024];
  rss_feed_t *next_feed;
  alarm_data_t *next_alarm;
  uint8_t i;
  time_t curr_time;

  curr_time = time(NULL);

  PL_debug("PL_build_config: building config.");

  curr_time = time(NULL); 
  sprintf(config_line,"# iris nvram data :: written by Iris on %s \n",asctime(localtime(&curr_time)));

  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  for(i = 0; i < G_stream_count; i++)
   {
    sprintf(config_line,"stream=%s,%d,%s\n",G_streams[i]->name,G_streams[i]->type,G_streams[i]->url);
    sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
    config_buffer_pos += strlen(config_line);
   }
   
  for(i = 0; i < G_rss_feed_count; i++)
   {
    sprintf(config_line,"rss=%s,%s\n",G_rss_feeds[i]->name, G_rss_feeds[i]->url);
    sprintf(&config_buffer[config_buffer_pos],"%s",config_line);	  
	config_buffer_pos += strlen(config_line);
   }
  
  for(i = 0; i < G_alarm_sound_count; i++)
   {
    sprintf(config_line,"alarm_sound=%s,%d,%d\n",G_alarm_sounds[i].filename, G_alarm_sounds[i].loop_it,G_alarm_sounds[i].options);
    sprintf(&config_buffer[config_buffer_pos],"%s",config_line);	  
	config_buffer_pos += strlen(config_line);
   }
   
  next_alarm = G_alarms;
  
  while(next_alarm != NULL) 
   {
    sprintf(config_line,"alarm=%02d:%02d,%d,%d,%d,%d\n",next_alarm->hour, next_alarm->minute, next_alarm->days_of_week, 
                                                 next_alarm->alarm_state, next_alarm->alarm_type, next_alarm->sound_index);	
    sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
    config_buffer_pos += strlen(config_line);
    next_alarm = next_alarm->next;
   }
  
  sprintf(config_line,"sleep_timer=%d\n",G_config.sleep_timer_time);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"snooze_timer=%d\n",G_config.snooze_timer_time);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);
  
  sprintf(config_line,"volume=%1.2f\n",G_config.volume_level);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);
  
  sprintf(config_line,"tell_time=%d\n",G_config.tell_time_when_on);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"wifi_ssid=%s\n",G_config.wireless_ssid);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"wifi_password=%s\n",G_config.wireless_password);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"wifi_driver=%s\n",G_config.wireless_driver);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"wifi_type=%d\n",G_config.wireless_type);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"ip_address=%s\n",G_config.ip_address);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"ip_netmask=%s\n",G_config.ip_netmask);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"ip_gateway=%s\n",G_config.ip_gateway);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"ssh_access=%d\n",G_config.ssh_access);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"www_access=%d\n",G_config.www_access);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"timezone=%s\n",G_config.timezone);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"use_ntp=%d\n",G_config.use_ntp);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"ntp_server=%s\n",G_config.ntp_server);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"debug=%d\n",G_config.debug);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"basic_menu=%d\n",G_config.basic_menu);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"autoplay=%d\n",G_config.autoplay);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"use_bt=%d\n",G_config.use_bt);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"tts_speed=%d\n",G_config.tts_speed);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"tts_pitch=%d\n",G_config.tts_pitch);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  sprintf(config_line,"tts_volume=%d\n",G_config.tts_volume);
  sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
  config_buffer_pos += strlen(config_line);

  if(strlen(G_config.bt_speaker) >= 10)
   {
    sprintf(config_line,"bt_speaker=%s\n",G_config.bt_speaker);
    sprintf(&config_buffer[config_buffer_pos],"%s",config_line);
    config_buffer_pos += strlen(config_line);
   }

  return 1;

 }   
  
uint8_t PL_save_config(void)
 {
  struct stat file_check;
  int nvram_desc;
  uint32_t byte_offset = 0;
  uint16_t wrote;

  PL_build_config(&G_working_config);

   if(stat(NVRAM,&file_check) == 0)
    {
      PL_debug("PL_save_config: writing real nvram");
      nvram_desc = open(NVRAM,O_WRONLY);
      while(byte_offset < NVRAM_SIZE)
       {
        PL_debug("PL_save_config: writing at %d",byte_offset);
        wrote = write(nvram_desc,G_working_config + byte_offset,NVRAM_WRITE_SIZE);
        byte_offset += NVRAM_WRITE_SIZE;
       }
      close(nvram_desc);
    }
   else
    {
      if(stat(FAKE_NVRAM,&file_check) == 0)
       {
        PL_debug("PL_save_config: writing fake vnram (file)");
        nvram_desc = open(FAKE_NVRAM,O_WRONLY);
        while(byte_offset < NVRAM_SIZE)
         {
          PL_debug("PL_save_config: writing at %d",byte_offset);
          wrote = write(nvram_desc,G_working_config + byte_offset,NVRAM_WRITE_SIZE);
          byte_offset += NVRAM_READ_SIZE;
         }
        close(nvram_desc);
       }
    }

  PL_debug("PL_save_config: wrote %d bytes to nvram variable space.",wrote);

  return 1;
 }

void PL_load_config(unsigned char *config_buffer)
 {

   struct stat file_check;
   int nvram_desc;
   uint32_t byte_offset = 0;

   if(stat(NVRAM,&file_check) == 0)
    {
      PL_debug("PL_load_config: reading real nvram");
      nvram_desc = open(NVRAM,O_RDONLY);
      while(byte_offset < NVRAM_SIZE)
       {
        PL_debug("PL_load_config: reading at %d",byte_offset);
        read(nvram_desc,config_buffer + byte_offset,NVRAM_READ_SIZE);
        byte_offset += NVRAM_READ_SIZE;
       }
      close(nvram_desc);
    }
   else
    {
      if(stat(FAKE_NVRAM,&file_check) == 0)
       {
        PL_debug("PL_load_config: reading from fake vnram (file)");
        nvram_desc = open(FAKE_NVRAM,O_RDONLY);
        while(byte_offset < NVRAM_SIZE)
         {
          PL_debug("PL_load_config: reading at %d",byte_offset);
          read(nvram_desc,config_buffer + byte_offset,NVRAM_READ_SIZE);
          byte_offset += NVRAM_READ_SIZE;
         }
        close(nvram_desc);
       }
      else
       PL_load_config_defaults();
    }

   config_buffer[NVRAM_SIZE-1] = 0x0; // terminate config buffer

 }


unsigned char *PL_get_next_config_line(unsigned char *config_buffer, uint32_t config_buffer_offset)
 {

  uint32_t index = 0;
  unsigned char *new_line;

  while(config_buffer[config_buffer_offset + index] != 0x0)
   {
    if(config_buffer[config_buffer_offset + index] == '\n')
     {
      if(index == 0)  // empty string, but return it anyway
       {
        new_line = malloc(2);
        new_line[0] = ' ';
        new_line[1] = 0x0;
        return new_line;
       }
      else
       {
         new_line = malloc(index + 1);
         strncpy(new_line, &config_buffer[config_buffer_offset], index);
         new_line[index] = 0x0;
         return new_line;
       }
      }
     index++;
    }

  return NULL;

 }

void PL_parse_config_av(unsigned char *avpair)
 {
   uint16_t av_index = 0;
   uint16_t avlen;

   uint8_t attrib_val_present = 0, days_bitmap;
   uint16_t attrib = 0;
   uint16_t stream_name_len,feed_name_len,time_descr_len,tmp_val;
   unsigned char *value, *comma_pos, *time_to_parse;
   const char comma[2] = {',',0x0};
   struct tm timespec;

   avlen = strlen(avpair);

   while(av_index < avlen)
    {

      if(avpair[av_index] == '#')  // comment starts
       {
         if((attrib_val_present) && (strlen(value) > 0))
          avpair[av_index] = 0x0;
         break;
       }

      if((avpair[av_index] == '=') && !attrib_val_present)  // first '=' sign marks attribute - value assignment
       {
        attrib_val_present = 1;
        value = &avpair[av_index + 1];
       }

      if(!attrib_val_present)
       {
        attrib += tolower(avpair[av_index]);
       }

     av_index++;

    }

   if((attrib_val_present) && (strlen(value) > 0))
    {

     switch(attrib)
     {

      case CONFIG_ATTR_STREAM:
       PL_parse_stream_spec(value);
       break;

      case CONFIG_ATTR_RSS:
       comma_pos = strstr(value,&comma);
       if(comma_pos == NULL)
        {
         PL_debug("PL_parse_config_av: invalid rss feed description in config: %s - ignoring",value);
         break;
        }
       feed_name_len = strlen(value) - strlen(comma_pos);
       G_rss_feeds[G_rss_feed_count] = malloc(sizeof(rss_feed_t));
       if(feed_name_len == 0)
        {
         G_rss_feeds[G_rss_feed_count]->name = malloc(2);
         G_rss_feeds[G_rss_feed_count]->name[0] = ' ';
         G_rss_feeds[G_rss_feed_count]->name[1] = 0x0;
        }
       else
        {
         G_rss_feeds[G_rss_feed_count]->name = malloc(feed_name_len + 1);
         strncpy(G_rss_feeds[G_rss_feed_count]->name,value,feed_name_len);
         G_rss_feeds[G_rss_feed_count]->name[feed_name_len] = 0x0;
        }
       G_rss_feeds[G_rss_feed_count]->url = malloc(strlen(value) - feed_name_len + 1 );
       strcpy(G_rss_feeds[G_rss_feed_count]->url,value + feed_name_len + 1);
       PL_debug("PL_parse_config_av: parsed rss feed descr: [%s]",G_rss_feeds[G_rss_feed_count]->name);
       PL_debug("PL_parse_config_av: parsed rss feed URL: [%s]",G_rss_feeds[G_rss_feed_count]->url);
       G_rss_feed_count++;
       break;

      case CONFIG_ATTR_ALARM:
       PL_parse_alarm_spec(value);
       break;

      case CONFIG_ATTR_VOLUME:
       G_config.volume_level = atof(value);
       char debug_str[255];
       sprintf(debug_str,"PL_parse_config_av: parsed volume level: %1.2f",G_config.volume_level);
       PL_debug(debug_str);
       break;

      case CONFIG_ATTR_SLEEP_TIMER:
       tmp_val = atoi(value);
       if((tmp_val > 86400) || (tmp_val < 1))
         PL_debug("PL_parse_config_av: invalid sleep timer specification: %s - ignoring",value);
       else
        {
         G_config.sleep_timer_time = tmp_val;
         PL_debug("PL_parse_config_av: parsed sleep timer: %d",G_config.sleep_timer_time);
        }
       break;

      case CONFIG_ATTR_SNOOZE_TIMER:
       tmp_val = atoi(value);
       if((tmp_val > 600) || (tmp_val < 10))
         PL_debug("PL_parse_config_av: invalid snooze timer specification: %s - ignoring",value);
       else
        {
         G_config.snooze_timer_time = tmp_val;
        }
       break;

      case CONFIG_ATTR_TELL_TIME:
       tmp_val = atoi(value);
       if((tmp_val >= 0) || (tmp_val <= 2))
        G_config.tell_time_when_on = tmp_val;
       PL_debug("PL_parse_config_av: parsed time telling flag: %d",G_config.tell_time_when_on);
       break;

      case CONFIG_ATTR_USE_BT:
       tmp_val = atoi(value);
       if((tmp_val == 0) || (tmp_val == 1))
        G_config.use_bt = tmp_val;
       PL_debug("PL_parse_config_av: parsed use bluetooth flag: %d",G_config.use_bt);
       break;

      case CONFIG_ATTR_BT_SPEAKER:
       strncpy(G_config.bt_speaker,value,20);
       PL_debug("PL_parse_config_av: parsed bluetooth speaker address: [%s]",G_config.bt_speaker);
       break;

      case CONFIG_ATTR_TIMEZONE:
       strncpy(G_config.timezone,value,255);
       PL_debug("PL_parse_config_av: parsed timezone: [%s]",G_config.timezone);
       break;

      case CONFIG_ATTR_WIFI_SSID:
       strncpy(G_config.wireless_ssid,value,255);
       PL_debug("PL_parse_config_av: parsed wireless SSID: [%s]",G_config.wireless_ssid);
       break;

      case CONFIG_ATTR_WIFI_PSK:
       strncpy(G_config.wireless_password,value,255);
       PL_debug("PL_parse_config_av: parsed wireless PSK: [XXXXX]");
       break;

      case CONFIG_ATTR_WIFI_DRIVER:
       strncpy(G_config.wireless_driver,value,255);
       PL_debug("PL_parse_config_av: parsed wireless driver: [%s]",G_config.wireless_driver);
       break;

      case CONFIG_ATTR_WIFI_TYPE:
       G_config.wireless_type = atoi(value);
       PL_debug("PL_parse_config_av: parsed wireless type: %d",G_config.wireless_type);
       break;

      case CONFIG_ATTR_IP_ADDR:
       strcpy(G_config.ip_address,value);
       PL_debug("PL_parse_config_av: parsed ip address: [%s]");
       break;

      case CONFIG_ATTR_IP_MASK:
       strcpy(G_config.ip_netmask,value);
       PL_debug("PL_parse_config_av: parsed ip netmask: [%s]");
       break;

      case CONFIG_ATTR_IP_GW:
       strcpy(G_config.ip_gateway,value);
       PL_debug("PL_parse_config_av: parsed ip gateway: [%s]");
       break;

      case CONFIG_ATTR_USE_NTP:
       G_config.use_ntp = atoi(value);
       PL_debug("PL_parse_config_av: parsed NTP on/off flag: %d",G_config.use_ntp);
       break;

      case CONFIG_ATTR_NTP_SERVER:
       strcpy(G_config.ntp_server,value);
       PL_debug("PL_parse_config_av: parsed NTP server: [%s]",G_config.ntp_server);
       break;

      case CONFIG_ATTR_SSH_ACCESS:
       G_config.ssh_access = atoi(value);
       PL_debug("PL_parse_config_av: parsed SSH access flag: %d",G_config.ssh_access);
       break;

      case CONFIG_ATTR_WWW_ACCESS:
       G_config.www_access = atoi(value);
       PL_debug("PL_parse_config_av: parsed www access flag: %d",G_config.www_access);
       break;

      case CONFIG_ATTR_DEBUG:
       G_config.debug = atoi(value);
       PL_debug("PL_parse_config_av: parsed debug flag: %d",G_config.debug);
       break;

      case CONFIG_ATTR_BASIC_MENU:
       G_config.basic_menu = atoi(value);
       PL_debug("PL_parse_config_av: parsed basic menu flag: %d",G_config.basic_menu);
       break;

      case CONFIG_ATTR_AUTOPLAY:
       G_config.autoplay = atoi(value);
       PL_debug("PL_parse_config_av: parsed autoplay flag: %d",G_config.autoplay);
       break;

      case CONFIG_ATTR_TTS_SPEED:
       tmp_val = atoi(value);
       if((tmp_val>79) && (tmp_val<121))
        {
         G_config.tts_speed = tmp_val;
         PL_debug("PL_parse_config_av: parsed TTS speed: %d",G_config.tts_speed);
        }
       else 
        PL_debug("PL_parse_config_av: invalid TTS speed: %d",tmp_val);
       break;

      case CONFIG_ATTR_TTS_PITCH:
       tmp_val = atoi(value);
       if((tmp_val>79) && (tmp_val<121))
        {
         G_config.tts_pitch = tmp_val;
         PL_debug("PL_parse_config_av: parsed TTS pitch: %d",G_config.tts_pitch);
        }
       else
        PL_debug("PL_parse_config_av: invalid TTS pitch: %d",tmp_val);
       break;

      case CONFIG_ATTR_TTS_VOL:
       tmp_val = atoi(value);
       if((tmp_val>79) && (tmp_val<121))
        {
         G_config.tts_volume = tmp_val;
         PL_debug("PL_parse_config_av: parsed TTS volume: %d",G_config.tts_volume);
        }
       else
        PL_debug("PL_parse_config_av: invalid TTS volume: %d",tmp_val);
       break;
  
      case CONFIG_ATTR_ALARM_SOUND:
	   PL_parse_alarm_sound_def(value);
       PL_debug("PL_parse_config_av: parsed alarm sound definition");	   
       break;
	   
      default:
       PL_debug("PL_parse_config_av: unknown config attribute %d",attrib);

     }
    }

 }
 
void PL_parse_config_buffer(unsigned char *config_buffer)
 {

  uint32_t buffer_offset = 0;
  unsigned char *next_config_av;

  while(buffer_offset < NVRAM_SIZE-1)
   {
    next_config_av = NULL;
    next_config_av = PL_get_next_config_line(config_buffer, buffer_offset);
    if(next_config_av != NULL)
     {
      buffer_offset += strlen(next_config_av);
      PL_parse_config_av(next_config_av);
      free(next_config_av); // this was malloc'ed in PL_get_next_config_line()
     }
    else
     {
      PL_debug("PL_parse_config_buffer: parsed entire config buffer");
      return;
     }
   }

 }


