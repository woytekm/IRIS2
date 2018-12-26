#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

main()
 {

  uint8_t keyboard_state, prev_keyboard_state = 0;

  if(!PL_keyboard_init())
   {
    printf("keyboard test program initialization error.\n");
    exit(0);
   }

  while(1)
   {
    
     keyboard_state = PL_keyboard_scan();
     if(keyboard_state != prev_keyboard_state)
      {
       printf("keyboard_state: %d\n",keyboard_state);
       prev_keyboard_state = keyboard_state;
      }

     usleep(5000);

   }

 }
