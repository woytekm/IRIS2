#include <bcm2835.h>
#include "global.h"
#include "audio.h"
#include "AMP-TAS5721.h"
#include "i2c.h"

uint8_t PL_HW_init_amp()
 {

  uint8_t i2c_read_buffer[8];
  uint8_t i2c_readchar;
  uint8_t i2c_result;
  uint8_t tas5721_subchannel_mixer_21_setup[4] = {0x00,0x00,0x42,0x03};

  PL_debug("PL_HW_init_amp(TAS5721): init");

  G_TAS5721_detected = 0;
  G_amp_detected = 0;

  bcm2835_gpio_fsel(AMP_TAS5721_HEADPHONE_AMP,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_FAULT_I2C,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_RESET,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_PWR,BCM2835_GPIO_FSEL_OUTP); 

  bcm2835_gpio_write(AMP_TAS5721_HEADPHONE_AMP, LOW); // Disable internal headphones amplifier. AMP board is messed up here and will short headphones amp ouput, so this has to be permanently disabled or IC will overheat and fail
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_FAULT_I2C, HIGH);    // Set I2C address of TAS5731 to 0x36 7-bit, or 0x1b 8-bit (TAS5721_I2C_ADDR)
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_RESET, HIGH);        // Set reset to high before powering the chip down
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_PWR, LOW);           // Power the chip down
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_PWR, HIGH);          // Power the chip back up
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_RESET, LOW);         // Toggle reset (1)
  usleep(1000);
  bcm2835_gpio_write(AMP_TAS5721_RESET, HIGH);        // Toggle reset (2)
  usleep(10000);

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(TAS5721_I2C_BAUDRATE);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  i2c_readchar = I2C_ReadByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CLKCTRL);  // we should get TAS5721_I2C_ID here

  if(i2c_readchar == TAS5721_I2C_ID)
   {
     PL_debug("PL_HW_init_amp(TAS5721): detected TAS5721 at 0x%x",TAS5721_I2C_ADDR);
     G_TAS5721_detected = 1;
     G_amp_detected = 1;
   } 
  else 
   {
     printf("PL_HW_init_amp: TAS5721 not detected. i2c detect returned: 0x%x\n",i2c_readchar);
     return 0;
   }

  // TAS5721 is detected - let's init it and set it up for 2.1 mode

  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_OSCTRIM,0x00);  // oscillator factory trim
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_OSCTRIM,0x00,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SERDAT,0x03);   // I2S word lenght = 16bit
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SERDAT,0x03,i2c_result);
  i2c_result = I2C_WriteByteArray(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CH4SRC,(char *)&tas5721_subchannel_mixer_21_setup,4);  // set sub channel mixer to 2.1 mode
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_CH4SRC,tas5721_subchannel_mixer_21_setup[0],i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SOFTMUTE,0x07); // mute all channels
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SOFTMUTE,0x07,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CH1VOL,AMP_INIT_SATELLITE_GAIN);    // set gain for subsequent channels
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_CH1VOL,AMP_INIT_SATELLITE_GAIN,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CH2VOL,AMP_INIT_SUBWOOFER_GAIN);
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_CH2VOL,AMP_INIT_SUBWOOFER_GAIN,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_MASTERVOL,AMP_INIT_MASTERVOL);
  PL_debug("PL_HW_init_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_MASTERVOL,AMP_INIT_MASTERVOL,i2c_result);

  bcm2835_i2c_end();

  return 1;
  
 }

uint8_t PL_HW_set_amp_HW_volume(uint8_t HW_volume_L,uint8_t HW_volume_R,uint8_t HW_volume_S,uint8_t HW_mastervol)
 {
  uint8_t i2c_result;

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(TAS5721_I2C_BAUDRATE);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CH1VOL,HW_volume_L);    // set gain for subsequent channels
  PL_debug("PL_HW_set_volume(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_CH1VOL,HW_volume_L,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_CH2VOL,HW_volume_S);
  PL_debug("PL_HW_set_volume(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_CH2VOL,HW_volume_S,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_MASTERVOL,HW_mastervol);
  PL_debug("PL_HW_set_volume(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_MASTERVOL,HW_mastervol,i2c_result);

  bcm2835_i2c_end();

  return 1;
 }

uint8_t PL_HW_enable_amp()
 {
  uint8_t i2c_result;

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(TAS5721_I2C_BAUDRATE);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SYSCTRL2,0x84);
  PL_debug("PL_HW_enable_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SYSCTRL2,0x84,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SOFTMUTE,0x00);
  PL_debug("PL_HW_enable_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SOFTMUTE,0x00,i2c_result);

  PL_HW_set_amp_HW_volume(G_config.HW_volume_L,G_config.HW_volume_R,G_config.HW_volume_S,G_config.HW_mastervol);

  bcm2835_i2c_end();
  return 1;
 }

uint8_t PL_HW_shutdown_amp()
 {
  uint8_t i2c_result;

  bcm2835_i2c_begin();
  bcm2835_i2c_set_baudrate(TAS5721_I2C_BAUDRATE);
  bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_2500);

  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SOFTMUTE,0x07);
  PL_debug("PL_HW_shutdown_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SOFTMUTE,0x07,i2c_result);
  i2c_result = I2C_WriteByteRegister(TAS5721_I2C_ADDR,TAS5721_I2C_REG_SYSCTRL2,0x04);
  PL_debug("PL_HW_shutdown_amp(TAS5721): 0x%x, 0x%x, I2C write result: %d",TAS5721_I2C_REG_SYSCTRL2,0x04,i2c_result);

  bcm2835_i2c_end();
  return 1;
 }

