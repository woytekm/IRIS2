#include <bass.h>
#include <bcm2835.h>
#include <stdio.h>

typedef struct _dev_info_t{
   char name[255];
   char driver[255];
  } dev_info_t;

main()
{

  int a, count=0, i, end = 0;;
  BASS_DEVICEINFO info;

  dev_info_t devices[255];

  char soundfile[] = "test.wav";
  DWORD BASS_chan;
  DWORD chan_status;

  bcm2835_init();

  printf("************** device detection ***************\n");

  for (a=0; BASS_GetDeviceInfo(a, &info); a++)
    if (info.flags&BASS_DEVICE_ENABLED) // device is enabled
     {
        printf("found device: %s, %s ",info.name, info.driver);

        if(info.flags&BASS_DEVICE_ENABLED)
         printf(" enabled\n");
        else
         printf(" disabled\n");

        if(info.name != NULL) 
         strcpy(devices[count].name,info.name);

        if(info.driver != NULL)
         strcpy(devices[count].driver,info.driver);

        count++;

     }

   printf("\n************** sound test ***************\n");
   bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_07, HIGH);

   for(i = 1; i < count; i++)
    {
     printf("playing sound on device #%d (%s, %s)\n",i ,devices[i].name, devices[i].driver);
  
     end = 0;
   
     if(!BASS_Init(i,44100,0,0,NULL)) 
     {
      printf("BASS init can't initialize device #%d\n",i);
      continue;
     }

     BASS_chan = BASS_StreamCreateFile(0,soundfile,0,0,BASS_STREAM_AUTOFREE);

    if(!BASS_ChannelPlay(BASS_chan,FALSE))
    {
     printf("BASS error while playing file: %d. Aborting", BASS_ErrorGetCode());
     continue;
    }

    while(!end)
     {
      chan_status = BASS_ChannelIsActive(BASS_chan);
      if(chan_status != BASS_ACTIVE_PLAYING)
       end = 1;
     }

    BASS_Free();

    sleep(1);

    }
   
   bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_07, LOW);

 }
