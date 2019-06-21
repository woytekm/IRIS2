#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "matrix.h"
#include "matrixlib.h"
#include "global.h"


void PL_matrix_display_thread(void)
 {

  m_init();
  m_clear();

  while(1)
   {
     m_set_brightness(G_config.matrix_brightness);
     m_display();
     usleep(20000);
   }

 }


