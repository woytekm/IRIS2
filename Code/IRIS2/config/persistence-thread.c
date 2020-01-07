#include "global.h"

uint8_t PL_load_persistence_data(iris_persistence_data_t *persistence_data)
 {
    int nvram_fd;
    uint8_t readed;

    nvram_fd = open(NVRAM,O_RDONLY);
    lseek(nvram_fd,NVRAM_PERSISTENCE_DATA_OFFSET,SEEK_SET);
    readed = read(nvram_fd, (void *)persistence_data, sizeof(iris_persistence_data_t));
    close(nvram_fd);
    if( (readed == sizeof(iris_persistence_data_t)) && (persistence_data->signature == PERSISTENCE_DATA_SIG) )
     {
       PL_debug("PL_load_persistence_data: loaded successfully %d bytes of persistence data: %d,%d,%d,%d",readed,persistence_data->stream_index,
                 persistence_data->rss_index, persistence_data->display_mode_1, persistence_data->last_audio_output);
       return readed;
     }
    else
     {
      PL_debug("PL_load_persistence_data: cannot load persistence - read error or no signature");
      return 0;
     }
 }

void PL_keep_persistence(iris_persistence_data_t *persistence_data)
 {

   // if saved stream index is higher than overall stream count loaded from config file - don't restore that 
   if(G_stream_count > persistence_data->stream_index)
     G_stream_index = persistence_data->stream_index;

   G_rss_feed_index = persistence_data->rss_index;
   G_config.volume_level = persistence_data->volume_level;
   G_display_mode_upper_row = persistence_data->display_mode_1;
   G_prev_output_device = persistence_data->last_audio_output;
   PL_debug("PL_keep_persistence: restored: %d,%d,%d,%d",G_stream_index, G_rss_feed_index, 
             G_display_mode_upper_row, G_prev_output_device);

 }

uint8_t PL_check_persistence_data(iris_persistence_data_t *persistence_data)
 {
   if(G_global_mode != GLOBAL_MODE_SETUP)
    {
     if( (persistence_data->stream_index != G_stream_index) || 
       (persistence_data->rss_index != G_rss_feed_index) ||
       (persistence_data->volume_level != G_config.volume_level) ||
       (persistence_data->player_mode != G_player_mode) ||
       (persistence_data->display_mode_1 != G_display_mode_upper_row) ||
       (persistence_data->last_audio_output != G_prev_output_device) ||
       (persistence_data->signature != PERSISTENCE_DATA_SIG) )
        return 0;
     else
       return 1;
    }
   else
    return 1; // don't check persistence when in setup
 }

void PL_sync_persistence_data(iris_persistence_data_t *persistence_data)
 {
   persistence_data->signature = PERSISTENCE_DATA_SIG;
   persistence_data->stream_index = G_stream_index;
   persistence_data->rss_index = G_rss_feed_index;
   persistence_data->volume_level = G_config.volume_level;
   persistence_data->display_mode_1 = G_display_mode_upper_row;
   persistence_data->last_audio_output = G_prev_output_device;
   if(G_player_mode == ALARM_ACTIVE)
    persistence_data->player_mode = PLAYER_STOP;
   else
    persistence_data->player_mode = G_player_mode;
  }

void PL_persistence_thread(void)
 {

  iris_persistence_data_t persistence_data;
  int nvram_fd;
  uint8_t wrote;

  PL_debug("PL_persistence_thread: starting");

  PL_load_persistence_data((void *)&persistence_data);

  while(1)
   {

    if(G_player_mode != ALARM_ACTIVE)
     {
      if(!PL_check_persistence_data(&persistence_data))
       {
        PL_sync_persistence_data(&persistence_data);
        nvram_fd = open(NVRAM,O_RDWR);
        lseek(nvram_fd,NVRAM_PERSISTENCE_DATA_OFFSET,SEEK_SET);
        wrote = write(nvram_fd, (void *)&persistence_data, sizeof(iris_persistence_data_t));
        PL_debug("PL_persistence_thread: synced %d bytes of persistence data to nvram", wrote);
       }
     }

    sleep(PERSISTENCE_CHECK_LOOP_DLY);

   }

 }


