#include <bcm2835.h>

#define KEYS_NO 8

enum keypad_keys { KEY1,KEY2,KEY3,KEY4,KEY5,KEY6,KEY7,KEY8 };

#define KEYPAD_CLK RPI_BPLUS_GPIO_J8_13
#define KEYPAD_LATCH RPI_BPLUS_GPIO_J8_15
#define KEYPAD_DATA RPI_BPLUS_GPIO_J8_16

#define ENCODER_L RPI_BPLUS_GPIO_J8_11
#define ENCODER_R RPI_BPLUS_GPIO_J8_07

void PL_keyboard_thread(void);

#define KEY_RADIO_ONOFF        1
#define KEY_RADIO_ONOFF_LONG   130
#define KEY_SETUP              2
#define KEY_SETUP_LONG         131
#define KEY_TUNING_DOWN        4
#define KEY_TUNING_DOWN_LONG   133
#define KEY_TUNING_UP          8
#define KEY_TUNING_UP_LONG     137
#define KEY_VOL_DOWN           64
#define KEY_VOL_DOWN_LONG      193
#define KEY_VOL_UP             128
#define KEY_VOL_UP_LONG        129
#define KEY_SNOOZE_LEFT        16
#define KEY_SNOOZE_LEFT_LONG   145
#define KEY_SNOOZE_CENTER      32
#define KEY_SNOOZE_CENTER_LONG 161

#define KEY_DELAY_NORMAL 100
#define KEY_DELAY_FAST 50
#define KEY_DELAY_FASTER 20

#define THRESHOLD1 50
#define THRESHOLD2 230
#define THRESHOLD3 400

#define ENCODER_TURN_RIGHT 1
#define ENCODER_TURN_LEFT  2
#define ENCODER_NO_EVENT   0

