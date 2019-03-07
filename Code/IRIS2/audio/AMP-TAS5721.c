#include "AMP-TAS5721.h"

uint8_t PL_HW_init_amp()
 {

  uint8_t i2c_read_buffer[8];
  uint8_t i2c_write_buffer[8];
  uint8_t i2c_result;

  G_TAS5731_detected = 0;
  bcm2835_gpio_fsel(AMP_TAS5721_HEADPHONE_AMP,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_FAULT_I2C,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_RESET,BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(AMP_TAS5721_PWR,BCM2835_GPIO_FSEL_OUTP); 

  bcm2835_gpio_write(AMP_TAS5721_HEADPHONE_AMP, LOW); // Disable internal headphones amplifier. AMP board is messed up here and will short headphones amp ouput, so this has to be permanently disabled or IC will overheat and fail
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_FAULT_I2C, HIGH);    // Set I2C address of TAS5731 to 0x36 (TAS5721_I2C_ADDR)
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_RESET, HIGH);        // Set reset to high before powering the chip down
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_PWR, LOW);           // Power the chip down
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_PWR, HIGH);          // Power the chip back up
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_RESET, LOW);         // Toggle reset (1)
  usleep(100);
  bcm2835_gpio_write(AMP_TAS5721_RESET, HIGH);        // Toggle reset (2)
  
  bcm2835_i2c_begin();
  bcm2835_i2c_setSlaveAddress(TAS5721_I2C_ADDR);

  i2c_result = bcm2835_i2c_read(&i2c_read_buffer,1);

  if(i2c_read_buffer[0] == 0x6c)
   {
     PL_debug("PL_HW_init_amp: detected TAS5721 at 0x%x",TAS5721_I2C_ADDR);
     G_TAS5731_detected = 1;
   } 
  else return 0;

  
  
 }

uint8_t PL_HW_set_amp_HW_volume(uint8_t HW_volume_L,uint8_t HW_volume_R,uint8_t HW_volume_M)
 {
 
 }

uint8_t PL_HW_enable_amp()
 {
 
 }

uint8_t PL_HW_shutdown_amp()
 {

 }

