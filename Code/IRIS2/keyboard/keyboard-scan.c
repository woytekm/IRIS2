#include <string.h>
#include <bcm2835.h>

#include "keyboard.h"


uint8_t G_keymap;
uint16_t G_key_interval_ctr[KEYS_NO] = {0,0,0,0,0,0,0,0};
uint8_t G_keys[KEYS_NO] = {KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8};


uint8_t PL_keyboard_init(void)
 {

   if (!bcm2835_init())
     return 0;

   bcm2835_gpio_fsel(KEY1, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY2, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY3, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY4, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY5, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY6, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY7, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEY8, BCM2835_GPIO_FSEL_INPT);

   usleep(1000);

   return 1;

 }


uint8_t PL_keyboard_scan(unsigned char *message, uint8_t row, uint16_t times)
 {

   uint8_t i, keymap = 0;

   for(i=0; i<=KEYS_NO; i++)
    {
    if(!bcm2835_gpio_lev(G_keys[i]))
     {
      G_key_interval_ctr[i]++;
      if(G_key_interval_ctr[i] == THRESHOLD1)
       {
        keymap |= 1 << i;
        keymap |= 128;
        keymap += 1;
       }
      else if( ((G_key_interval_ctr[i] > THRESHOLD1) && (G_key_interval_ctr[i] < THRESHOLD2) ) && (G_key_interval_ctr[i] % KEY_DELAY_NORMAL == 0) )
       {
        keymap |= 1 << i;
        keymap |= 128;
        keymap += 1;
       }
      else if( ((G_key_interval_ctr[i] >= THRESHOLD2) && (G_key_interval_ctr[i] < THRESHOLD3) ) && (G_key_interval_ctr[i] % KEY_DELAY_FAST == 0) )  
       {
        keymap |= 1 << i;
        keymap |= 128;
        keymap += 1;
       }
      else if( (G_key_interval_ctr[i] >= THRESHOLD3) && (G_key_interval_ctr[i] % KEY_DELAY_FASTER == 0) )  // speed up option/value scrolling
       {
        keymap |= 1 << i;
        keymap |= 128;
        keymap += 1;
       }
     }
    else
     {
      if((G_key_interval_ctr[i]>0) && (G_key_interval_ctr[i]<THRESHOLD1))
        keymap |= 1 << i;
      G_key_interval_ctr[i] = 0;
     }

    }

   return keymap;

 }



