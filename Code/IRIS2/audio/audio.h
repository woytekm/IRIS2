/* abstraction from hardware for upper layers  - this needs to be appropriate to whatever AMP module is used  */

#include "AMP-TAS5721.h"
#define AMP_INIT_SATELLITE_GAIN AMP_TAS5721_INIT_SATELLITE_GAIN 
#define AMP_INIT_SUBWOOFER_GAIN AMP_TAS5721_INIT_SUBWOOFER_GAIN
#define AMP_INIT_MASTERVOL      AMP_TAS5721_INIT_MASTERVOL

uint8_t PL_HW_init_amp();
uint8_t PL_HW_set_amp_HW_volume(uint8_t HW_volume_L,uint8_t HW_volume_R,uint8_t HW_volume_M, uint8_t HW_mastervol);
uint8_t PL_HW_enable_amp();
uint8_t PL_HW_shutdown_amp();
void    PL_HW_amp_control_thread();


