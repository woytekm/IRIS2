#include "global.h"
#include "audio.h"
#include "AMP-TAS5721.h"

void PL_set_amp(uint8_t mode)
 {
   if(mode==1)
    {
     PL_HW_enable_amp();
     G_amp_active = 1;
    }
   else if(mode==0)
    {
     PL_HW_shutdown_amp();
     G_amp_active = 0;
    }
 }

uint8_t PL_init_amp()
 {
   uint8_t result;
   G_config.HW_volume_L = AMP_INIT_SATELLITE_GAIN;
   G_config.HW_volume_R = AMP_INIT_SATELLITE_GAIN;
   G_config.HW_volume_S = AMP_INIT_SUBWOOFER_GAIN;
   G_config.HW_mastervol =  AMP_INIT_MASTERVOL;

   if(!PL_HW_init_amp()) return 0;
   if(!PL_HW_set_amp_HW_volume(G_config.HW_volume_L,G_config.HW_volume_R,G_config.HW_volume_S,G_config.HW_mastervol)) return 0;
   return 1;
 }


