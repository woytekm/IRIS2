#include <bcm2835.h>

#define KEYS_NO 8

#define KEY1 RPI_BPLUS_GPIO_J8_31
#define KEY2 RPI_BPLUS_GPIO_J8_32
#define KEY3 RPI_BPLUS_GPIO_J8_33
#define KEY4 RPI_BPLUS_GPIO_J8_35
#define KEY5 RPI_BPLUS_GPIO_J8_36
#define KEY6 RPI_BPLUS_GPIO_J8_37
#define KEY7 RPI_BPLUS_GPIO_J8_38
#define KEY8 RPI_BPLUS_GPIO_J8_40

void PL_keyboard_thread(void);

#define KEY_RADIO_ONOFF        2
#define KEY_RADIO_ONOFF_LONG   131
#define KEY_SETUP              4
#define KEY_SETUP_LONG         133
#define KEY_TUNING_DOWN        32
#define KEY_TUNING_DOWN_LONG   161
#define KEY_TUNING_UP          64
#define KEY_TUNING_UP_LONG     193
#define KEY_VOL_DOWN           128
#define KEY_VOL_DOWN_LONG      129
#define KEY_VOL_UP             8
#define KEY_VOL_UP_LONG        137
#define KEY_SNOOZE_LEFT        1
#define KEY_SNOOZE_LEFT_LONG   130
#define KEY_SNOOZE_CENTER      16
#define KEY_SNOOZE_CENTER_LONG 145

#define KEY_DELAY_NORMAL 100
#define KEY_DELAY_FAST 50
#define KEY_DELAY_FASTER 20

#define THRESHOLD1 100
#define THRESHOLD2 500
#define THRESHOLD3 1000


