#include "global.h"
#include "system.h"


void PL_init(void)
 {

  PL_debug("PL_init: starting.");

  if (!bcm2835_init())
  {
   fprintf(stderr,"fatal: cannot initialize BCM2835 library!\n");
   exit(0);
  }

  G_iris_task_count = 1;

  G_debug_info_active = 0;

  G_alarms = NULL;

  G_global_mode = GLOBAL_MODE_NORMAL;

  G_player_mode = PLAYER_STOP;

  G_config.snooze_timer_time = 120;

  G_config.tell_time_when_on = TELL_TIME_STAY_SILENT;

  G_config.tell_time_start_at = 7;    // say hour even when turned off, start at this hour
  G_config.tell_time_stop_at = 0;      // say hour even when turned off, stop at this hour

  G_display_mode_upper_row = DISPLAY_MODE_PLAYER_META;
  G_display_mode_lower_row = DISPLAY_MODE_CLOCK;

  G_scroll_meta_active = 0;
  G_kill_meta_scroll = 0;

  G_scroll_rss_active = 0;
  G_kill_rss_scroll = 0;

  G_vu_active = 0;
  G_kill_vu = 0;

  G_config.sleep_timer_time = 40; // 40 minutes

  G_config.volume_level = G_prev_volume_level = 0.5;

  G_display_mode_save = G_display_mode_upper_row;

  bcm2835_gpio_fsel(INTERNAL_AMP_CONTROL_PIN, BCM2835_GPIO_FSEL_OUTP);
  G_internal_amp_active = 0;  // should be turned off at startup
  PL_set_internal_amp(0);

  pipe(G_setup_key_input);
  pipe(G_TTS_thread_input);

  if(!my_spi_WEH001602_init())
   {
     fprintf(stderr,"fatal: error initializing display library!\n");
     exit(0);
   }

  G_stream_index = 0;
  G_stream_count = 0;

  G_rss_feed_index = 0;
  G_rss_feed_count = 0;
  G_alarm_sound_count = 0;
  G_alarm_sound_index = 0;

  G_wakeup = 0;

  strcpy(G_config.wireless_ssid,"none");
  strcpy(G_config.wireless_password,"none");
  strcpy(G_config.wireless_driver," "); // leave this at autodetect
  G_config.wireless_type = 0;
  G_config.use_ntp = 0;
  strcpy(G_config.ntp_server,"pool.ntp.org");
  strcpy(G_config.timezone,"UTC");
  strcpy(G_config.ip_address,"0.0.0.0");
  strcpy(G_config.ip_netmask,"0.0.0.0");
  strcpy(G_config.ip_gateway,"0.0.0.0");
  G_config.ssh_access = 1;
  G_config.www_access = 1;
  G_config.websrv_mode = WEBSRV_SECURE;
  G_config.basic_menu = 0;
  G_config.debug = 0;
  G_config.bt_spk = 0;
  G_config.bt_sink = 0;
  G_config.bt_speaker[0] = 0x0;
  G_config.bt_source[0] = 0x0;
  G_config.autoplay = 0;
  G_config.tts_speed = 100;
  G_config.tts_pitch = 100;
  G_config.tts_volume = 100;

  PL_debug("PL_init: global variables inited");

 }



void PL_debug(char *fmt, ...)
 {
  #define MAX_LOG_MSG_LEN 2048
  va_list ap;
  char timestr[255];
  char message[MAX_LOG_MSG_LEN];
  uint16_t total_msg_len;

  if(!G_debug_info_active) return;

  if(G_logfile_handle > 0) /* logfile handle open and ready to write */
   {
    struct timeval tv;
    struct timezone tz;
    struct tm tm;

    gettimeofday(&tv, &tz);
    localtime_r(&tv.tv_sec,&tm);
    asctime_r(&tm,timestr);

    timestr[strlen(timestr)-1] = 0x0;   /* get rid of newline at the end of asctime result */
    total_msg_len = strlen(timestr) + strlen(fmt) + 10;

    snprintf(message,MAX_LOG_MSG_LEN,"%s %s\n",timestr,fmt);

    va_start(ap, message);
    vfprintf(G_logfile_handle, message, ap);
    fflush(G_logfile_handle);
    va_end(ap);
   }

 }


iris_task_info_t *PL_start_task(uint8_t task_id, void (*task_function)(), void *task_args, int scheduling_policy, int priority)
 {

   uint8_t setstack_err=0;
   pthread_attr_t new_task_attr;
   struct sched_param new_task_sched_param;
   iris_task_info_t *new_task;

   new_task = malloc(sizeof(iris_task_info_t));

   pthread_attr_init(&new_task_attr);
   pthread_attr_setdetachstate(&new_task_attr, PTHREAD_CREATE_DETACHED);

   if( (setstack_err = pthread_attr_setstacksize(&new_task_attr, IRIS_DEFAULT_THREAD_STACK_SIZE)) != 0)
    PL_debug("PL_start_task: warning: cannot set thread stack size to %dk (%d)",
              IRIS_DEFAULT_THREAD_STACK_SIZE,setstack_err);

   if(pipe(new_task->task_input) == -1)
     PL_debug("PL_start_task: warning: cannot create create input pipe for task %x",task_function);

  if(pipe(new_task->task_output) == -1)
     PL_debug("PL_start_task: warning: cannot create create output pipe for task %x",task_function);

   if(pthread_create(&new_task->task_id, &new_task_attr, task_function, task_args))
    {
     PL_debug("PL_start_task: cannot start task %x",task_function);
     return -1;
    }

   new_task_sched_param.sched_priority = priority;

   if(pthread_setschedparam(new_task->task_id, scheduling_policy, &new_task_sched_param))
    PL_debug("PL_start_task: warning: cannot set scheduling policy for task %x",task_function);

   G_iris_task_count++;

   return new_task;
 }

