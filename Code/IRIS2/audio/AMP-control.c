
void PL_set_amp(uint8_t mode)
 {
   if(mode==1)
    {
     PL_HW_init_amp();
     PL_HW_set_amp_HW_volume(G_HW_volume_L,G_HW_volume_R,G_HW_volume_M);
     PL_HW_enable_amp();
     G_amp_active = 1;
    }
   else if(mode==0)
    {
     PL_HW_shutdown_amp();
     G_amp_active = 0;
    }
 }


