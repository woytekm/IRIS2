#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>

#include <bcm2835.h>

#include <netinet/ether.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/un.h>

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <curl/curl.h>

#include <pthread.h>

#include <bass.h>
#include <bass_aac.h>
#include <bassflac.h>

#include <microhttpd.h>

#include <linux/sched.h>

#define AUDIO_OUT_DEFAULT -1  // for ALSA bluetooth output 
//#define AUDIO_OUT_INTERNAL 1  // BASS detects BCM2835 internal audio as #2
#define AUDIO_OUT_INTERNAL 2  // I2S audio (?)


#define WEBSRV_PORT 8443
#define WEBSRV_KEY_FILE "/etc/iris_websrv_key.pem"
#define WEBSRV_CERT_FILE "/etc/iris_websrv_cert.pem"
#define WEBSRV_ADMIN_LOGIN "root"
#define WEBSRV_SECURE 10
#define WEBSRV_HTTP 20
#define WEBSRV_DOCUMENT_ROOT "/usr/share/iris/htdocs"

// basic REST API:

// POST   /api/play/stream/0  - play stream at current index
// DELETE /api/play/stream/0  - stop current stream
// GET    /api/play/stream/0  - check stream status
// POST   /api/play/stream/<stream idx> - play particular stream
// GET    /api/stream  - get stream list
// POST   /api/stream/ - add new stream at the end
// PUT    /api/stream/<stream idx> - change stream data at this index
// DELETE /api/stream/<stream idx> - delete stream at this index
// GET    /api/volume/0 - get main volume
// PUT    /api/volume/0 - change main volume

#define WEBSRV_API_URL                "api"
#define WEBSRV_API_RES_PLAY_STREAM    "play/stream"
#define WEBSRV_API_RES_STREAM         "stream"
#define WEBSRV_API_RES_RSS            "rss"
#define WEBSRV_API_RES_VOL            "volume"

#define WEBSRV_ADMIN_URL              "admin"

#define DISPLAY_MODE_OFF 0
#define DISPLAY_MODE_CLOCK 1
#define DISPLAY_MODE_RSS 2
#define DISPLAY_MODE_PLAYER_META 4
#define DISPLAY_MODE_PLAYER_VU 8
#define DISPLAY_MODE_WEATHER 16
#define DISPLAY_MODE_STREAM_STATS 32
#define DISPLAY_MODE_DATE 64
#define DISPLAY_MODE_CLOCK_DATE 128
#define DISPLAY_MODE_SETUP 255

#define GLOBAL_MODE_NORMAL 1
#define GLOBAL_MODE_SETUP 2
#define GLOBAL_MODE_SPECTRUM 3

#define PLAYER_STOP 10
#define PLAYER_STREAM 20
#define ALARM_ACTIVE 30

#define DISPLAY_SCROLL_DELAY 75000
#define VU_METER_REFRESH_DELAY 32000

#define INTERNAL_AMP_CONTROL_PIN RPI_BPLUS_GPIO_J8_07

#define KEYBOARD_POLLING_LOOP_USEC 4000

#define ENOUGH_BUFFERING 600

#define MAX_STREAMS 128
#define MAX_RSS 16

#define SEARCH_FORWARD 1
#define SEARCH_BACK 2

#define NVRAM_SIZE 8192
#define NVRAM_READ_SIZE 4096
#define NVRAM_WRITE_SIZE 4096

#define IRIS_COMMAND_SOCKET "/var/run/iris_cmd"
#define ALARM_SOUNDS_PATH "/usr/share/iris/alarm_sounds"
#define NVRAM "/dev/nvram"  
#define FAKE_NVRAM "/usr/share/iris/nvram"

unsigned char G_config_data[NVRAM_SIZE]; // raw config data blob
unsigned char G_working_config[NVRAM_SIZE]; // working buffer for building and writing raw config

#define ALARM_ON 1
#define ALARM_OFF 0

#define SNOOZE_INACTIVE 0
#define SNOOZE_RESET 1
#define SNOOZE_ACTIVE 2

#define ALARM_TYPE_STREAM 0
#define ALARM_TYPE_RSS_AND_STREAM 1
#define ALARM_TYPE_ALARM_SOUND 2

typedef struct _alarm_sound_t {
 unsigned char filename[255];
 uint8_t loop_it;
 uint8_t options;
 } alarm_sound_t;

alarm_sound_t G_alarm_sounds[255];
uint8_t G_alarm_sound_count;

typedef struct _alarm_data_t {
 uint8_t hour;
 uint8_t minute;
 uint8_t days_of_week;
 uint8_t alarm_state;
 uint8_t alarm_type;
 uint8_t sound_index;
 uint16_t triggered_today;
 struct _alarm_data_t *next;
 } alarm_data_t;

alarm_data_t *G_alarms;
alarm_data_t *G_last_alarm;

// ! there should be at least two alarm structs allocated after init and config read
//   these two will be accessible from unit's front panel setup menu
//   rest will be accessible only from web panel/nvram cli

typedef struct _setup_menu_item_t {
  unsigned char *title;
  void *variable;
  uint8_t var_type;
  uint32_t var_min;
  uint32_t var_max;
  struct _setup_menu_t *prev;
  struct _setup_menu_t *next;
 } setup_menu_item_t;

setup_menu_item_t *G_setup_menu;
setup_menu_item_t *G_last_setup_menu_item;

int G_setup_key_input[2];  // IPC pipe
int G_TTS_thread_input[2]; // IPC pipe

HFX G_BASS_TTS_fx[3];      // parametric EQ for TTS channel
HFX G_BASS_stream_fx[3];   // parametric EQ for radio stream

pthread_mutex_t G_display_lock;

#define DEBUG_FILE "/tmp/debug.log"
uint8_t G_debug_info_active;
FILE *G_logfile_handle;

uint8_t G_amp_active;

struct tm *G_tm;

float   G_prev_volume_level;
int8_t G_prev_output_device;
uint8_t G_global_mode;
uint8_t G_display_mode_upper_row;
uint8_t G_display_mode_lower_row;
uint8_t G_dmur_save, G_dmlr_save;
uint8_t G_display_mode_save;
uint8_t G_player_mode;
uint8_t G_stream_index;
uint8_t G_rss_feed_index;
uint8_t G_alarm_sound_index;
uint8_t G_stream_count;
uint8_t G_rss_feed_count;

uint8_t G_TTS_state;
uint8_t G_tts_channel_status;
uint8_t G_stream_channel_status;

uint8_t G_bt_source_connected;
uint8_t G_bt_connected;


char G_bt_found_title[128];
char G_bt_found_album[128];
char G_bt_found_artist[128];


#define BASS_IN_USE_BY_TTS    1
#define BASS_IN_USE_BY_STREAM 2
#define BASS_IN_USE_BY_ALARM  4
#define BASS_IN_USE_BY_OTHER  8

#define TTS_SEGMENT_DELAY 1 // in seconds
#define TTS_INTER_NEWS_ITEM_DELAY 2 // in seconds
#define TTS_TEMP_DIR "/tmp/"

#define TELL_TIME_STAY_SILENT 0
#define TELL_TIME_WHEN_ON 1
#define TELL_TIME_ALL_THE_TIME 2

#define NVRAM_PERSISTENCE_DATA_OFFSET 24576
#define PERSISTENCE_CHECK_LOOP_DLY 20  // in seconds
#define PERSISTENCE_DATA_SIG 6767

#define VOLUME_MSG_DLY 90000

uint8_t G_BASS_in_use;

DWORD G_stream_chan, G_tts_chan; // BASS sound channels

uint8_t G_search_direction; // station search direction

uint8_t G_sleep_timer_active;
uint16_t G_sleep_timer_counter;

uint32_t G_wakeup;
uint32_t G_global_timer;

uint8_t G_snooze_state;

uint8_t G_scroll_meta_active;
uint8_t G_kill_meta_scroll;

uint8_t G_scroll_rss_active;
uint8_t G_kill_rss_scroll;

uint8_t G_vu_active;
uint8_t G_kill_vu;

uint8_t G_amp_detected;

char G_current_stream_META[1024];

// basic task management and IPC

#define MAX_TASK 		20

#define TASK_DISPLAY 		1
#define TASK_PLAYER		2
#define TASK_ALARM		3
#define TASK_KEYBOARD		4
#define TASK_CLOCK 		5
#define TASK_SNOOZE		6
#define TASK_SETUP		7
#define TASK_RSS		8
#define TASK_TTS		9
#define TASK_TTS_INDICATOR	10
#define TASK_PLAYER_DISPLAY	11
#define TASK_ALARM_INDICATOR	12
#define TASK_WIFI_INDICATOR	13
#define TASK_BT_INDICATOR	14
#define TASK_SNOOZE_TIMER       15
#define TASK_PERSISTENCE        16
#define TASK_EXT_CMDS           17
#define TASK_WEBSRV             18
#define TASK_MATRIX             19
#define TASK_AMP_HW_CTRL        20
#define TASK_BTCTRL             21
#define TASK_BTPLAYERCTRL       22
#define TASK_MATRIX_DISPLAY     23
#define TASK_WEATHER_API        24
#define TASK_WEATHER_SCENERY    25



typedef struct _iris_task_info_t {
  pthread_t task_id;
  int task_input[2];
  int task_output[2];
  uint32_t task_alive_timestamp;
 } iris_task_info_t;

iris_task_info_t *G_iris_tasks[MAX_TASK];

uint8_t G_iris_task_count;

typedef struct _iris_task_message_t {
  uint8_t message_type;
  uint16_t message_options;
 } iris_task_message_t;

// define persistence data

typedef struct _iris_persistence_data_t {
  uint16_t signature;
  uint8_t stream_index;
  uint8_t rss_index;
  uint8_t player_mode;
  uint8_t display_mode_1;
  uint8_t global_mode;
  int8_t last_audio_output;
  float volume_level;
} iris_persistence_data_t;

typedef struct _iris_config_t {
    float volume_level;
    uint32_t sleep_timer_time;     // sleep fader time in seconds
    uint32_t snooze_timer_time;    // snooze pause length in usec (?)
    uint8_t tell_time_when_on;      // tell time at every full hour when on (playing stream)
    uint8_t tell_time_start_at;
    uint8_t tell_time_stop_at;
    uint8_t default_alarm_sound;
    unsigned char wireless_ssid[255];
    unsigned char wireless_password[255];
    unsigned char wireless_driver[255];
    uint8_t wireless_type;
    unsigned char ip_address[16];
    unsigned char ip_netmask[16];
    unsigned char ip_gateway[16];
    uint8_t ssh_access;
    uint8_t www_access;
    uint8_t websrv_mode;
    uint8_t use_ntp;
    uint8_t basic_menu;
    uint8_t autoplay;
    unsigned char ntp_server[255];
    unsigned char timezone[255];   
    uint8_t bt_spk;
    uint8_t bt_sink;
    unsigned char bt_pair_with[255];
    unsigned char bt_source[20];
    unsigned char bt_speaker[20];
    uint8_t debug;
    uint8_t wifi_signal_level_debug;
    uint8_t tts_speed;
    uint8_t tts_pitch;
    uint8_t tts_volume;
    uint8_t matrix_brightness;
    uint8_t HW_volume_L;
    uint8_t HW_volume_R;
    uint8_t HW_volume_S;
    uint8_t HW_mastervol;
    uint8_t FFT_base_color;
    uint8_t FFT_marker_color;
    uint8_t FFT_gradient_color;
    uint8_t matrix_clock_color;
    uint8_t get_weather;
    uint8_t matrix_temperature;
    uint8_t matrix_weather;
    unsigned char weather_location[255];
    unsigned char rapidapi_weather_key[255];
} iris_config_t;

iris_config_t G_config;

uint8_t G_last_weather_API_call_status;
double G_temperature;
uint8_t G_windspeed;
typedef enum {OVERCAST_CLOUDS, BROKEN_CLOUDS, SCATTERED_CLOUDS, FEW_CLOUDS, CLEAR_SKY, 
                LIGHT_RAIN, MODERATE_RAIN, HEAVY_RAIN, STORM, LIGHT_SNOW, SNOW, MODERATE_SNOW, HEAVY_SNOW, RAIN_SNOW, FOG, UNKNOWN} weather_t; 

weather_t G_weather;

typedef struct _rss_feed_t {
  unsigned char *name;
  unsigned char *url;
 } rss_feed_t;

#define STREAM_TYPE_MP3 1
#define STREAM_TYPE_AAC 2
#define STREAM_TYPE_FLAC 3

typedef struct _radio_stream_t {
  unsigned char *name;
  uint8_t type;
  unsigned char *url;
} radio_stream_t;


radio_stream_t *G_streams[MAX_STREAMS];
rss_feed_t *G_rss_feeds[MAX_RSS];

uint8_t G_matrix_analyser_mode;
uint8_t G_matrix_analyser_state;
uint8_t G_matrix_analyser_color_scheme;
uint8_t G_matrix_brightness;
uint8_t G_clear_matrix;

uint8_t G_matrix_mode;

uint8_t G_bt_pair_now;

#define MATRIX_MODE_CLOCK 1
#define MATRIX_MODE_ANALYSER 2
#define MATRIX_MODE_BLANK 3

#define ALSA_DEFAULT_OUT 2
#define ALSA_DEFAULT_IN 1

// include end

