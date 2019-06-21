#include "global.h"
#include "version.h"
#include "system.h"
#include "plclock.h"
#include "rss.h"
#include "player.h"
#include "keyboard.h"
#include "WEH001602-lib.h"
#include "setup.h"
#include "tts.h"
#include "config.h"
#include "alarm.h"
#include "wifi_signal.h"
#include "audio.h"
#include "bluetooth.h"
#include "matrix.h"
#include "weather.h"


main(int argc, char **argv)
 {

  iris_persistence_data_t persistence_data;
  int pid;
 
  if(argc > 1)
   if(strstr(argv[1], "-d") != NULL)
    {
     G_debug_info_active = 1;
     G_logfile_handle = fopen(DEBUG_FILE,"w");
    }
   else
    G_debug_info_active = 0;
 
  PL_debug("main: Iris version %d.%d. Compiled on: %s. Starting...", IRIS_VER_MAJOR, IRIS_VER_MINOR, IRIS_COMPILE_TIME);

  PL_init();
 
  pid = fork(); 
  if (pid < 0)
   {
      /* Could not fork  */
     exit(EXIT_FAILURE);
   }
  if (pid > 0)
   {
     /* Child created ok, so exit parent process  */
     exit(EXIT_SUCCESS);
   }

  PL_load_config(&G_config_data);
  PL_parse_config_buffer(&G_config_data);

  PL_debug("main: starting Iris tasks");

  G_iris_tasks[TASK_CLOCK] = PL_start_task(TASK_CLOCK, PL_clock_thread, NULL, SCHED_RR, 90);
  G_iris_tasks[TASK_SNOOZE] = PL_start_task(TASK_SNOOZE, PL_snooze_timer_thread, NULL, SCHED_RR, 70);
  G_iris_tasks[TASK_RSS] = PL_start_task(TASK_RSS, PL_rss_reader_thread, NULL, SCHED_RR, 85);
  G_iris_tasks[TASK_PLAYER] = PL_start_task(TASK_PLAYER, PL_player_thread, NULL, SCHED_RR, 86);
  G_iris_tasks[TASK_KEYBOARD] = PL_start_task(TASK_KEYBOARD, PL_keyboard_thread, NULL, SCHED_RR, 87); 
  G_iris_tasks[TASK_PLAYER_DISPLAY] = PL_start_task(TASK_PLAYER_DISPLAY, PL_player_display_thread, NULL, SCHED_RR, 95);
  G_iris_tasks[TASK_SETUP] = PL_start_task(TASK_SETUP, PL_setup_thread, NULL, SCHED_RR, 80);
  G_iris_tasks[TASK_TTS_INDICATOR] = PL_start_task(TASK_TTS_INDICATOR, PL_TTS_indicator_thread, NULL, SCHED_RR, 54);
  G_iris_tasks[TASK_ALARM] = PL_start_task(TASK_ALARM, PL_alarm_thread, NULL, SCHED_RR, 53);
  G_iris_tasks[TASK_ALARM_INDICATOR] = PL_start_task(TASK_ALARM_INDICATOR, PL_alarm_indicator_thread, NULL, SCHED_RR, 52);
  G_iris_tasks[TASK_EXT_CMDS] = PL_start_task(TASK_EXT_CMDS, PL_command_thread, NULL, SCHED_RR, 40);
  G_iris_tasks[TASK_TTS] = PL_start_task(TASK_TTS, PL_TTS_thread, NULL, SCHED_BATCH, 0);
  G_iris_tasks[TASK_WIFI_INDICATOR] = PL_start_task(TASK_WIFI_INDICATOR, PL_wifi_signal_thread, NULL, SCHED_BATCH, 0);
  G_iris_tasks[TASK_BT_INDICATOR] = PL_start_task(TASK_BT_INDICATOR, PL_bt_indicator, NULL, SCHED_BATCH, 0);
  G_iris_tasks[TASK_MATRIX] = PL_start_task(TASK_MATRIX, PL_matrix_analyser_thread, NULL, SCHED_RR, 96);
  G_iris_tasks[TASK_AMP_HW_CTRL] = PL_start_task(TASK_AMP_HW_CTRL, PL_HW_amp_control_thread, NULL, SCHED_RR, 97);
  G_iris_tasks[TASK_BTCTRL] = PL_start_task(TASK_BTCTRL, PL_bluetoothctl_proxy_thread, NULL, SCHED_RR, 98);
  G_iris_tasks[TASK_BTPLAYERCTRL] = PL_start_task(TASK_BTPLAYERCTRL, PL_bluetoothplayer_proxy_thread, NULL, SCHED_RR, 99);
  G_iris_tasks[TASK_MATRIX_DISPLAY] = PL_start_task(TASK_MATRIX_DISPLAY, PL_matrix_display_thread, NULL, SCHED_RR, 95);
  G_iris_tasks[TASK_WEATHER_API] = PL_start_task(TASK_WEATHER_API, PL_weather_api_thread, NULL, SCHED_BATCH, 0);
  G_iris_tasks[TASK_WEATHER_SCENERY] = PL_start_task(TASK_WEATHER_SCENERY, PL_weather_scenery_thread, NULL, SCHED_RR, 81);

  if(G_config.www_access)
   G_iris_tasks[TASK_WEBSRV] = PL_start_task(TASK_WEBSRV, PL_websrv_thread, NULL, SCHED_BATCH, 0);

  usleep(1000);

  if(PL_load_persistence_data(&persistence_data))
   {
    PL_debug("main: syncing persistence data");
    PL_keep_persistence(&persistence_data);
   }
  else
    PL_debug("main: persistence data not found or corrupted");

  G_iris_tasks[TASK_PERSISTENCE] = PL_start_task(TASK_PERSISTENCE, PL_persistence_thread, NULL, SCHED_BATCH,0);

  if(!G_config.bt_spk)
   sleep(4);
  else
   sleep(20);  // we are trying to connect with BT speaker now - wait a bit more

  if(G_config.autoplay)
   {
    if(G_player_mode == PLAYER_STOP)  // if user already started playback - abort autoplay
     {
      PL_debug("main: starting autoplay");
      G_player_mode = PLAYER_STREAM;
     }
   }

  usleep(1000);
 
  PL_debug("main: startup complete");
 
  while(1) { G_global_timer++; sleep(1); }  

 }


