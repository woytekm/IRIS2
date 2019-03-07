#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdint.h>

#include "global.h"
#include "curlget.h"
#include "newslist.h"
#include "tts.h"
#include "rss.h"


uint8_t PL_BASS_play_TTS_file(char *filename)
 {
  
    DWORD BASS_chan;
    DWORD chan_status;

    BASS_chan = BASS_StreamCreateFile(0,filename,0,0,0);

    BASS_ChannelSetAttribute(BASS_chan,BASS_ATTRIB_VOL,G_config.volume_level);
  
    PL_BASS_apply_TTS_parameq(BASS_chan);

    if(!BASS_chan) 
     {
      PL_debug("PL_BASS_play_TTS_file: BASS error while creating stream: %d. Aborting.\n", BASS_ErrorGetCode());
      return 0;
     }

    if(!BASS_ChannelPlay(BASS_chan,FALSE))
     {
      PL_debug("PL_BASS_play_TTS_file: BASS error while playing stream: %d. Aborting.\n", BASS_ErrorGetCode());
      return 0;
     }

    sleep(1);

    while(1)
      {
       chan_status = BASS_ChannelIsActive(BASS_chan);

       if(chan_status != BASS_ACTIVE_PLAYING)  
        break;

       if(G_config.volume_level != G_prev_volume_level)
        {
         BASS_ChannelSetAttribute(G_tts_chan,BASS_ATTRIB_VOL,G_config.volume_level);
         G_prev_volume_level = G_config.volume_level;
        }
       usleep(5000);
      }   
	  
    BASS_StreamFree(BASS_chan);
    return 1;
 }

 
void PL_TTS_play_rss_items(rss_item_t *rss_list_start)
 {

  rss_item_t *current_item;
  uint8_t end = 0, item_counter = 0;
  char *tts_cmd; 
  char tts_wave_filename[1024];
  uint8_t used_internal_spk = 0;
  int8_t output;

  current_item = rss_list_start;
 
  if((!G_amp_active)&&(!G_bt_connected))
   {
    PL_set_amp(1);
    used_internal_spk = 1;
    output = AUDIO_OUT_INTERNAL;
   }
  else
   output = AUDIO_OUT_DEFAULT;  // we assume that bluetooth speaker is connected to ALSA profile "default"
 
  if(!G_BASS_in_use)
   if(!BASS_Init(output,44100,0,0,NULL))
    {
     PL_debug("PL_TTS_play_rss_items: BASS init can't initialize device.");
     return;
    }

  G_BASS_in_use |= BASS_IN_USE_BY_TTS;

  BASS_SetConfig(BASS_CONFIG_CURVE_VOL,TRUE);

  PL_BASS_play_TTS_file("/tmp/intro-1.wav");
  PL_TTS_say_current_time();
  
  sleep(TTS_SEGMENT_DELAY);

  PL_BASS_play_TTS_file("/tmp/intro-2.wav");

  sleep(TTS_SEGMENT_DELAY);

  while(current_item)
   {
    sprintf(tts_wave_filename,"/tmp/tts-%d.wav",item_counter);

    PL_BASS_play_TTS_file(tts_wave_filename);
    
    sleep(TTS_INTER_NEWS_ITEM_DELAY);
	
    current_item = current_item->next;
    item_counter++;
   }

  PL_BASS_play_TTS_file("/tmp/outro-1.wav");
  PL_TTS_say_current_time();

  G_BASS_in_use ^= BASS_IN_USE_BY_TTS; // clear usage flag

  if(!G_BASS_in_use)
   {
    BASS_Free();
    if(used_internal_spk)
     PL_set_amp(0);
   }

 }

void PL_BASS_apply_TTS_parameq(DWORD chan)
 {

   BASS_DX8_PARAMEQ p;

   G_BASS_TTS_fx[0]=BASS_ChannelSetFX(chan,BASS_FX_DX8_PARAMEQ,0);
   G_BASS_TTS_fx[1]=BASS_ChannelSetFX(chan,BASS_FX_DX8_PARAMEQ,0);
   G_BASS_TTS_fx[2]=BASS_ChannelSetFX(chan,BASS_FX_DX8_PARAMEQ,0);

   p.fBandwidth=18;

   p.fGain=-10;
   p.fCenter=1000;
   BASS_FXSetParameters(G_BASS_TTS_fx[0],&p);

   p.fGain=15;
   p.fCenter=4000;
   BASS_FXSetParameters(G_BASS_TTS_fx[1],&p);

   p.fGain=5;
   p.fCenter=6000;
   BASS_FXSetParameters(G_BASS_TTS_fx[2],&p);

 }

void PL_prepare_RSS_TTS(rss_item_t *rss_list_start, char *feed_name)
 {
    char tts_cmd[1024];

    rss_item_t *current_item;
    uint8_t item_counter = 0;
	
    current_item = rss_list_start;	

    // if TTS takes too much CPU, use: "/usr/bin/slowdown 0.0005 $tts_cmd" in external commands
 
    sprintf(tts_cmd,"/usr/bin/pico2wave -w /tmp/intro-1.wav \"<speed level='%d'><pitch level='%d'><volume level='%d'>This is Iris RSS news service</speed></pitch></volume>\"",
            G_config.tts_speed,G_config.tts_pitch,G_config.tts_volume); 
    system(tts_cmd);

    sprintf(tts_cmd,"/usr/bin/pico2wave -w /tmp/intro-2.wav \"<speed level='%d'><pitch level='%d'><volume level='%d'>reading %s RSS channel</speed></pitch></volume>\"",
            G_config.tts_speed,G_config.tts_pitch,G_config.tts_volume,feed_name);
    system(tts_cmd);

    sprintf(tts_cmd,"/usr/bin/pico2wave -w /tmp/outro-1.wav \"<speed level='%d'><pitch level='%d'><volume level='%d'>This was Iris RSS news service</speed></pitch></volume>\"",
            G_config.tts_speed,G_config.tts_pitch,G_config.tts_volume);
    system(tts_cmd);
    
    while(current_item)
     {
       sprintf(tts_cmd,"/usr/bin/pico2wave -w /tmp/tts-%d.wav \"<speed level='%d'><pitch level='%d'><volume level='%d'>%s</speed></pitch></volume>\"",item_counter,
               G_config.tts_speed,G_config.tts_pitch,G_config.tts_volume,current_item->body); 
       system(tts_cmd);
       current_item = current_item->next;
       item_counter++;
      }
	
 } 
 
 void PL_cleanup_RSS_TTS(rss_item_t *rss_list_start)
 {
    char tts_filename[1024];
    rss_item_t *current_item;
    uint8_t item_counter = 0;
	
    current_item = rss_list_start;

    unlink("/tmp/intro-1.wav");
    unlink("/tmp/intro-2.wav");
    unlink("/tmp/outro-1.wav");
	
    while(current_item)
     {
       sprintf(tts_filename,"/tmp/tts-%d.wav",item_counter); 
       unlink(tts_filename);
       current_item = current_item->next;
       item_counter++;
      }
	
 } 

void PL_TTS_thread(void)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    rss_item_t *work_ptr, *news_list;
    uint8_t work_ctr = 0;

    uint8_t TTS_msg[2];

    struct curl_received_data my_data;
    uint32_t curl_data_size;
    uint8_t used_internal_spk = 0;
    int8_t output;

    LIBXML_TEST_VERSION

    G_rss_item_count = 0;

    G_TTS_state = TTS_STOPPED;

    PL_debug("TTS thread starting");

    while(1)
    {

     read(G_TTS_thread_input[0],&TTS_msg,2);

     PL_debug("PL_TTS_thread: got message %d, %d",TTS_msg[0], TTS_msg[1]);

     if((TTS_msg[0] != TTS_STOP) && (G_TTS_state == TTS_PLAYING))
      break;

     if(TTS_msg[0] == TTS_SAY_CURRENT_TIME)
      {

        if((!G_amp_active)&&(!G_bt_connected))
         {
          PL_set_amp(1);
          used_internal_spk = 1;
          output = AUDIO_OUT_INTERNAL;
         }
        else
         output = AUDIO_OUT_DEFAULT;  // we assume that bluetooth speaker is connected to ALSA profile "default"

        if(!G_BASS_in_use)
         if(!BASS_Init(output,44100,0,0,NULL))
          {
           PL_debug(stderr,"BASS init can't initialize device.");
           return;
          }

        G_BASS_in_use |= BASS_IN_USE_BY_TTS;
       
        if(G_BASS_in_use&BASS_IN_USE_BY_STREAM)
         {
          BASS_ChannelSlideAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level-0.1,300);
          while (BASS_ChannelIsSliding(G_stream_chan,0)) usleep(1000);
         }

        G_TTS_state = TTS_PLAYING;
      
        usleep(10000);
        PL_TTS_say_current_time();
        usleep(10000);

       if(G_BASS_in_use&BASS_IN_USE_BY_STREAM)
        {
         BASS_ChannelSlideAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level,300);
         while (BASS_ChannelIsSliding(G_stream_chan,0)) usleep(1000);
        }

        G_BASS_in_use ^= BASS_IN_USE_BY_TTS; // clear usage flag
        G_TTS_state = TTS_STOPPED;

        if(!G_BASS_in_use)
         {
          BASS_Free();
          if(used_internal_spk)
           {
            PL_set_amp(0);
            used_internal_spk = 0;
           }
         }
      }

     if((TTS_msg[0] == TTS_READ_RSS_NOW) || (TTS_msg[0] == TTS_READ_RSS_AFTER_10SEC))
      { 
 
        sleep(1);       

        xmlInitParser();
 
        if(TTS_msg[0] == TTS_READ_RSS_AFTER_10SEC)
	 sleep(10);
	 
        PL_debug("PL_TTS_thread: reading current RSS chanel (%s)...\n",G_rss_feeds[TTS_msg[1]]->name);
        G_TTS_state = TTS_PREPARING;
        curl_data_size = curl_get_url_into_memory(G_rss_feeds[TTS_msg[1]]->url,&my_data);
        doc = xmlReadMemory(my_data.data, curl_data_size, "rss.xml", NULL, 0);

        if (doc == NULL) {
           PL_debug("PL_TTS_thread: error: could not parse curl buffer\n");
        }

        /*Get the root element node */
        root_element = xmlDocGetRootElement(doc);

        parse_xml_rss(root_element,PREPARE_FOR_TTS);

	//(fcntl(G_TTS_thread_input[0], F_SETFL, O_NONBLOCK) == -1)
        //(fcntl(G_TTS_thread_input[0], F_SETFL, ~O_NONBLOCK) == -1)
		
	PL_prepare_RSS_TTS(G_rss_news_list,G_rss_feeds[TTS_msg[1]]->name);
		
	BASS_ChannelSlideAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level-0.2,300);
        while (BASS_ChannelIsSliding(G_stream_chan,0)) usleep(1000);
	  
        G_TTS_state = TTS_PLAYING; 
        PL_TTS_play_rss_items(G_rss_news_list);

	BASS_ChannelSlideAttribute(G_stream_chan,BASS_ATTRIB_VOL,G_config.volume_level,300);
        while (BASS_ChannelIsSliding(G_stream_chan,0)) usleep(1000);
				
	PL_cleanup_RSS_TTS(G_rss_news_list);

        destroy_rss_news_list(G_rss_news_list);
		
        /*free the document */
        xmlFreeDoc(doc);
        xmlCleanupParser();
        bzero(&TTS_msg,2);
        G_TTS_state = TTS_STOPPED;
        PL_debug("PL_TTS_thread: finished playing RSS channel.\n");
      }
    }

}


