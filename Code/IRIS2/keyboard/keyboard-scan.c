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

   bcm2835_gpio_fsel(KEYPAD_DATA, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(KEYPAD_LATCH, BCM2835_GPIO_FSEL_OUTP);
   bcm2835_gpio_fsel(KEYPAD_CLK, BCM2835_GPIO_FSEL_OUTP);

   bcm2835_gpio_write(KEYPAD_CLK, LOW);
   bcm2835_gpio_write(KEYPAD_LATCH, HIGH);

   usleep(1000);

   return 1;

 }

void PL_keypad_shift_in(void)
 {

  uint8_t keypad_bitmap = 0, keypad_input, keypad_save = 0, i;
  
  bcm2835_gpio_write(KEYPAD_LATCH, LOW);
  usleep(1000);
  bcm2835_gpio_write(KEYPAD_LATCH, HIGH);
  usleep(1000);

  i = 0;
  keypad_input = bcm2835_gpio_lev(KEYPAD_DATA);
  G_keys[i] = keypad_input;

  usleep(1000);

  for(i=1;i<8;i++)
   {
     bcm2835_gpio_write(KEYPAD_CLK, HIGH);
     usleep(1000);
     keypad_input = bcm2835_gpio_lev(KEYPAD_DATA);
     bcm2835_gpio_write(KEYPAD_CLK, LOW);
     G_keys[i] = keypad_input;
     usleep(1000);
   }
 }

uint8_t PL_keyboard_scan(unsigned char *message, uint8_t row, uint16_t times)
 {

   uint8_t i, keymap = 0;

   PL_keypad_shift_in();  // G_keys is filled in with states of keys on the keypad

   for(i=0; i<=KEYS_NO; i++)
    {
    if(!G_keys[i])
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



