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


void PL_HW_amp_control_thread()
 {

   uint8_t HW_params[4];
   uint8_t HW_params_prev[4];

   PL_HW_set_amp_HW_volume(G_config.HW_volume_L,G_config.HW_volume_R,G_config.HW_volume_S,G_config.HW_mastervol);

   while(1)
    {

     HW_params[0] = G_config.HW_volume_L;
     HW_params[1] = G_config.HW_volume_R;
     HW_params[2] = G_config.HW_volume_S;
     HW_params[3] = G_config.HW_mastervol;

      if(memcmp(HW_params,HW_params_prev,sizeof(HW_params)))
       {
        PL_HW_set_amp_HW_volume(G_config.HW_volume_L,G_config.HW_volume_R,G_config.HW_volume_S,G_config.HW_mastervol);
       }
     
     HW_params_prev[0] = G_config.HW_volume_L;
     HW_params_prev[1] = G_config.HW_volume_R;
     HW_params_prev[2] = G_config.HW_volume_S;
     HW_params_prev[3] = G_config.HW_mastervol;

     usleep(10000);
    }

 }


