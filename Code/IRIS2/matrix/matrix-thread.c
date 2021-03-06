#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "matrix.h"
#include "matrixlib.h"
#include "global.h"


void PL_avg_FFT_256_to_64_with_32_scale(float *fft_256,uint8_t *fft_64_32)
 {
  uint16_t i,j=8,scaler,scaler_l1,scaler_l2;
  float wrk;

  if(G_config.bt_sink)
   {
    scaler = 3000;
    scaler_l1 = 800;
    scaler_l2 = 1900;
   }
  else
   {
    scaler = 310;
    scaler_l1 = 100;
    scaler_l2 = 250;
   }

  fft_64_32[0] = (uint8_t)(((fft_256[0]+fft_256[1])/2)*scaler_l1);
  fft_64_32[1] = (uint8_t)(((fft_256[2]+fft_256[3])/2)*scaler_l1);
  fft_64_32[2] = (uint8_t)(((fft_256[4]+fft_256[5])/2)*scaler_l1);
  fft_64_32[3] = (uint8_t)(((fft_256[6]+fft_256[7])/2)*scaler_l1);

  fft_64_32[4] = (uint8_t)(((fft_256[8]+fft_256[9])/2)*scaler_l2);
  fft_64_32[5] = (uint8_t)(((fft_256[10]+fft_256[11])/2)*scaler_l2);
  fft_64_32[6] = (uint8_t)(((fft_256[12]+fft_256[13])/2)*scaler_l2);
  fft_64_32[7] = (uint8_t)(((fft_256[14]+fft_256[15])/2)*scaler_l2);

  if(G_config.bt_sink)
    scaler = 3000;
  else
    scaler = 310;

  for(i=16; i<240; i = i+4)
   {
    wrk = (fft_256[i]+fft_256[i+1]+fft_256[i+2]+fft_256[i+3])/4;
    fft_64_32[j] = (uint8_t)(wrk*scaler);
    j++;
   }

  fft_64_32[62] = (uint8_t)(((fft_256[238]+fft_256[239]+fft_256[240]+fft_256[241]+fft_256[242]+fft_256[243]+fft_256[244]+fft_256[245])/8)*scaler);
  fft_64_32[63] = (uint8_t)(((fft_256[246]+fft_256[247]+fft_256[248]+fft_256[249]+fft_256[250]+fft_256[251]+fft_256[252]+fft_256[253])/8)*scaler);
  fft_64_32[64] = (uint8_t)(fft_256[254]*scaler);

 }


void PL_avg_FFT_256_to_64_with_16_scale(float *fft_256,uint8_t *fft_64_16)
 {
  uint16_t i,j=0,scaler;
  float wrk;

  fft_64_16[j++] = (fft_256[0]+fft_256[1])/2;
  fft_64_16[j++] = (fft_256[2]+fft_256[3])/2;
  fft_64_16[j++] = (fft_256[4]+fft_256[5])/2;
  fft_64_16[j++] = (fft_256[6]+fft_256[7])/2;

  if(G_config.bt_sink)
    scaler = 400;
  else
    scaler = 160;

  for(i=8; i<247; i = i+4)
   {
    wrk = (fft_256[i]+fft_256[i+1]+fft_256[i+2]+fft_256[i+3])/4;
    fft_64_16[j] = (uint8_t)(wrk*scaler);
    j++;
   }

 }

void PL_matrix_analyser_display(float *fft_buffer, uint8_t mode, uint8_t color_scheme)
 {
  uint8_t i,val;
  uint8_t FFT_values_64_scaled[128]; // test! it should be 64!

  m_clear();

  if(mode == MATRIX_ANALYSER_MODE_MONO_HOLD)
   {
    PL_avg_FFT_256_to_64_with_32_scale(fft_buffer,&FFT_values_64_scaled);
   }

  for(i=1; i<65; i++)
   {
    if(FFT_values_64_scaled[i] > 30) val=30;
     else val = FFT_values_64_scaled[i];
    if(val)
     {
      m_putvline(i,(31-val),val+1,G_config.FFT_base_color);
      m_putpixel(i,(31-val),G_config.FFT_marker_color);
     }
   }

 }


void PL_matrix_analyser_thread(void)
 {

  float fft[512];
  uint16_t i;
  uint8_t cleared = 0;

  G_matrix_analyser_mode = MATRIX_ANALYSER_MODE_MONO_HOLD;
  G_matrix_mode = MATRIX_MODE_CLOCK;
  G_matrix_analyser_color_scheme = MATRIX_ANALYSER_COLOR_01;
  G_clear_matrix = 0;

  while(1)
   {

     if((((G_player_mode != PLAYER_STREAM)&&(!G_config.bt_sink)) && (!cleared)) || G_clear_matrix )
      {
       m_clear();
       bzero(&fft,sizeof(fft));
       cleared = 1;
       G_clear_matrix = 0;
       G_matrix_mode = MATRIX_MODE_CLOCK;
      }

     if( (G_player_mode == PLAYER_STREAM) || (G_config.bt_sink) )
      {

       G_matrix_mode = MATRIX_MODE_ANALYSER;

       if(G_matrix_analyser_mode == MATRIX_ANALYSER_MODE_MONO_HOLD)
        BASS_ChannelGetData(G_stream_chan, fft, BASS_DATA_FFT512);
       if(G_matrix_analyser_mode == MATRIX_ANALYSER_MODE_STEREO_HOLD)
        BASS_ChannelGetData(G_stream_chan, fft, BASS_DATA_FFT512|BASS_DATA_FFT_INDIVIDUAL);

       PL_matrix_analyser_display(fft,G_matrix_analyser_mode,G_matrix_analyser_color_scheme);
       cleared = 0;
      }
     else if(G_matrix_mode == MATRIX_MODE_BLANK)
      {
       m_clear();
       bzero(&fft,sizeof(fft));
      }
     // else if(G_matrix_mode == MATRIX_MODE_CLOCK) - serviced in plclock.c

     //usleep(15000);
     usleep(20000);

   }

 }


