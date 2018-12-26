#include "global.h"
#include "tts.h"

void PL_TTS_say_current_time(void)
 {
  char tts_cmd[1024];
  char time_str[512];

  PL_current_time_to_text(&time_str);

  sprintf(tts_cmd,"/usr/bin/pico2wave -w /tmp/current-time.wav \"%s\"",time_str);
  
  system(tts_cmd);

  PL_BASS_play_TTS_file("/tmp/current-time.wav");

  unlink("/tmp/current-time.wav");

 }

void PL_current_time_to_text(char *input_buffer)
 {

   struct tm *tm1;
   time_t now;
   uint8_t hour, minutes, am = 0, pm = 0; 
   char text_buffer[512];

   char hour_str[256];
   char minute_str[256];

   time(&now);
   tm1 = localtime(&now);
   hour = tm1->tm_hour;
   minutes = tm1->tm_min;

   if(hour > 12)
    {
     hour = hour - 12; // convert hour 
     pm = 1;
    }
   else
    am = 1;

   if(minutes == 15)
    {
     if(hour == 0)
      sprintf(text_buffer,"it's quarter past midnight");
     else
      sprintf(text_buffer,"it's quarter past %d",hour);
    }
   else if(minutes == 30)
    {
     if(hour == 0)
      sprintf(text_buffer,"it's half past midnight");
     else
      sprintf(text_buffer,"it's half past %d", hour);
    }
   else if(minutes == 45)
    {
     if((hour == 11) && pm)
      hour = 0;
     else hour++;

     if(hour == 0)
      sprintf(text_buffer,"it's quarter to midnight");
     else
      sprintf(text_buffer,"it's quarter to %d", hour);
    }
   else if(hour == 0)
    {
     if(minutes == 0)
      sprintf(text_buffer,"it's midnight");
     else if(minutes < 30)
       sprintf(text_buffer,"it's %d minutes past midnight",minutes);
     else
       sprintf(text_buffer,"it's %d minutes to one");
     }
   else if(minutes == 0)
    {
     sprintf(text_buffer,"it's %d o clock",hour);
    }
   else 
    if(minutes == 1)
      {
       if(hour == 0)
        sprintf(text_buffer,"it's one minute past midnight");
       else
        sprintf(text_buffer,"it's one minute past %d",hour);
      }
    else if(minutes < 30)
      {
       if(hour == 0)
        sprintf(text_buffer,"it's %d minutes past midnight",minutes);
       else
        sprintf(text_buffer,"it's %d minutes past %d",minutes,hour); 
      }
   else if((60 - minutes) > 1)
     {
      if((hour == 11) && pm)
        hour == 0;
      else hour++;

      if(hour == 0)
       sprintf(text_buffer,"it's %d minutes to midnight",60 - minutes);
      else
       sprintf(text_buffer,"it's %d minutes to %d",60 - minutes,hour);
     }
    else
     {
      if((hour == 11) && pm)
        hour == 0;
      else hour++;

      if(hour == 0)
       sprintf(text_buffer,"it's one minute to midnight");
      else
       sprintf(text_buffer,"it's one minute to %d",hour);
     }

   sprintf(input_buffer,"<speed level='%d'><pitch level='%d'><volume level='%d'>%s</speed></pitch></volume>",G_config.tts_speed,G_config.tts_pitch,G_config.tts_volume,text_buffer);

 }


