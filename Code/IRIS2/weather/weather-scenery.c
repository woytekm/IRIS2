#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "global.h"

#include "broken-clouds-day-16f.h"
#include "broken-clouds-night-16f.h"
#include "clear-sky-day-11f.h"
#include "clear-sky-night-11f.h"
#include "few-clouds-day-32F.h"
#include "few-clouds-night-32F.h"
#include "heavy-rain-16f.h"
#include "light-rain-16f.h"
#include "moderate-rain-16f.h"
#include "overcast-clouds-16f.h"
#include "scattered-clouds-day-16F.h"
#include "scattered-clouds-night-16F.h"
#include "storm-16f.h"
#include "no-weather-1f.h"



void PL_weather_scenery_thread(void)
 {
  uint8_t cleared, i, animation_start = 1;

  while(1)
   {

    if(G_matrix_mode == MATRIX_MODE_CLOCK)
     {
      cleared = 0;
      if(animation_start)
       {
        i = 0;
        animation_start = 0;
       }
      if(G_config.get_weather)
       {
       if(G_config.matrix_weather)
        {
         if(G_last_weather_API_call_status = 1)
          {
           switch(G_weather)
            { 
             case FEW_CLOUDS: 
              if((G_tm->tm_hour > 20) && (G_tm->tm_hour < 5))  
               {
                 display_bitmap_at(few_clouds_night_32f_data[i++],FEW_CLOUDS_NIGHT_32F_FRAME_WIDTH, FEW_CLOUDS_NIGHT_32F_FRAME_HEIGHT,1,17);
                 if(i>=FEW_CLOUDS_NIGHT_32F_FRAME_COUNT) i = 0;
               }
              else
               {
                 display_bitmap_at(few_clouds_day_16f_data[i++],FEW_CLOUDS_DAY_16F_FRAME_WIDTH, FEW_CLOUDS_DAY_16F_FRAME_HEIGHT,1,17);
                 if(i>=FEW_CLOUDS_DAY_16F_FRAME_COUNT) i = 0;
               } 
              break;

             case BROKEN_CLOUDS:

               if((G_tm->tm_hour > 20) && (G_tm->tm_hour < 5))
                {
                  display_bitmap_at(broken_clouds_night_16f_data[i++],BROKEN_CLOUDS_NIGHT_16F_FRAME_WIDTH, BROKEN_CLOUDS_NIGHT_16F_FRAME_HEIGHT,1,17);
                  if(i>=BROKEN_CLOUDS_NIGHT_16F_FRAME_COUNT) i = 0;
                }
               else
                {
                  display_bitmap_at(broken_clouds_day_16f_data[i++],BROKEN_CLOUDS_DAY_16F_FRAME_WIDTH, BROKEN_CLOUDS_DAY_16F_FRAME_HEIGHT,1,17);
                  if(i>=BROKEN_CLOUDS_DAY_16F_FRAME_COUNT) i = 0;
                }
              break;

             case CLEAR_SKY: 
               if((G_tm->tm_hour > 20) && (G_tm->tm_hour < 5))
                {
                  display_bitmap_at(clear_sky_night_11f_data[i++],CLEAR_SKY_NIGHT_11F_FRAME_WIDTH, CLEAR_SKY_NIGHT_11F_FRAME_HEIGHT,1,17);
                  if(i>=CLEAR_SKY_NIGHT_11F_FRAME_COUNT) i = 0;
                }
               else
                {
                  display_bitmap_at(clear_sky_day_11f_data[i++],CLEAR_SKY_DAY_11F_FRAME_WIDTH, CLEAR_SKY_DAY_11F_FRAME_HEIGHT,1,17);
                  if(i>=CLEAR_SKY_DAY_11F_FRAME_COUNT) i = 0;
                }
              break;

             case HEAVY_RAIN:
                  display_bitmap_at(heavy_rain_16f_data[i++],HEAVY_RAIN_16F_FRAME_WIDTH, HEAVY_RAIN_16F_FRAME_HEIGHT,1,17);
                  if(i>=HEAVY_RAIN_16F_FRAME_COUNT) i = 0;
              break;

              case MODERATE_RAIN:
                   display_bitmap_at(moderate_rain_16f_data[i++],MODERATE_RAIN_16F_FRAME_WIDTH, MODERATE_RAIN_16F_FRAME_HEIGHT,1,17);
                   if(i>=MODERATE_RAIN_16F_FRAME_COUNT) i = 0;
               break;

              case LIGHT_RAIN:
                   display_bitmap_at(light_rain_16f_data[i++],LIGHT_RAIN_16F_FRAME_WIDTH, LIGHT_RAIN_16F_FRAME_HEIGHT,1,17);
                   if(i>=LIGHT_RAIN_16F_FRAME_COUNT) i = 0;
               break;

              case STORM:
                   display_bitmap_at(storm_16f_data[i++],STORM_16F_FRAME_WIDTH, STORM_16F_FRAME_HEIGHT,1,17);
                   if(i>=STORM_16F_FRAME_COUNT) i = 0;
               break;

              case OVERCAST_CLOUDS:
                  display_bitmap_at(overcast_clouds_16f_data[i++],OVERCAST_CLOUDS_16F_FRAME_WIDTH, OVERCAST_CLOUDS_16F_FRAME_HEIGHT,1,17);
                  if(i>=OVERCAST_CLOUDS_16F_FRAME_COUNT) i = 0;
              break;

              case SCATTERED_CLOUDS:
               if((G_tm->tm_hour > 20) && (G_tm->tm_hour < 5))
                {
                  display_bitmap_at(scattered_clouds_night_16f_data[i++],SCATTERED_CLOUDS_NIGHT_16F_FRAME_WIDTH, SCATTERED_CLOUDS_NIGHT_16F_FRAME_HEIGHT,1,17);
                  if(i>=SCATTERED_CLOUDS_NIGHT_16F_FRAME_COUNT) i = 0;
                }
               else
                {
                  display_bitmap_at(scattered_clouds_day_16f_data[i++],SCATTERED_CLOUDS_DAY_16F_FRAME_WIDTH, SCATTERED_CLOUDS_DAY_16F_FRAME_HEIGHT,1,17);
                  if(i>=SCATTERED_CLOUDS_DAY_16F_FRAME_COUNT) i = 0;
                }
              break;
            
              case UNKNOWN:
               display_bitmap_at(no_weather_1f_data[0],NO_WEATHER_FRAME_WIDTH, NO_WEATHER_FRAME_HEIGHT,1,17);
               break;

            }

           usleep(150000);
          }
        }
       }
     }
    else if(!cleared)
     {
      m_clear();
      cleared = 1;
      animation_start = 1;
      sleep(1);
     }
   }
 }


