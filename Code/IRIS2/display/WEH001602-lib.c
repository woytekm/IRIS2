//
// Winstar WEH001602 OLED display code for raspberry pi  
// This is heavily based on ehajo.de code (c) Hannes Jochriem, 2013
// (http://dokuwiki.ehajo.de/artikel:displays:weh001602_spi_democode)
//
// raspi adaptation by woytekm
//

#include <string.h>

#include "global.h"
#include "WEH001602-lib.h"

//
// we are using SPI pins as normal GPIO pins
//

void my_spi_toggle_clk(void)
 {
  bcm2835_gpio_write(MY_CLK, G_clkval);
  usleep(1);
  G_clkval = 1 - G_clkval;
  bcm2835_gpio_write(MY_CLK, G_clkval);
  usleep(1);
  G_clkval = 1 - G_clkval;
 }

void my_spi_WEH001602_clear(void)
 {
   my_spi_WEH001602_out_cmd(0b00000001);  // Display clear
 }

void my_spi_WEH001602_out_cmd(uint8_t cmdbyte)
 {
   uint8_t i;

   bcm2835_gpio_write(MY_CS0, LOW);
    
   bcm2835_gpio_write(MY_MOSI, LOW);  /* send RS and RW bits - both 0 */
   my_spi_toggle_clk();
   bcm2835_gpio_write(MY_MOSI, LOW);
   my_spi_toggle_clk();

   for(i=0;i<8;i++)
    {
     if(cmdbyte & 128)
      bcm2835_gpio_write(MY_MOSI, HIGH);
     else
      bcm2835_gpio_write(MY_MOSI, LOW);
     my_spi_toggle_clk();
     cmdbyte = cmdbyte << 1;
    }

    bcm2835_gpio_write(MY_CS0, HIGH);
  }


void my_spi_WEH001602_out_data(uint8_t cmdbyte)
 {
  uint8_t i;

  bcm2835_gpio_write(MY_CS0, LOW);

  bcm2835_gpio_write(MY_MOSI, HIGH);  /* send RS=1 and RW=0 bits */
  my_spi_toggle_clk();
  bcm2835_gpio_write(MY_MOSI, LOW);
  my_spi_toggle_clk();

  for(i=0;i<8;i++)
   {
    if(cmdbyte & 128)
     bcm2835_gpio_write(MY_MOSI, HIGH);
    else
     bcm2835_gpio_write(MY_MOSI, LOW);
    my_spi_toggle_clk();
    cmdbyte = cmdbyte << 1;
   }

  bcm2835_gpio_write(MY_CS0, HIGH);
 }


//
// 64-bit bitmap variable should contain 8 8-bit rows of character definition 
// where first 3 bits are 0 and next 5 bits define pixels in subsequent rows
//

void my_spi_WEH001602_def_char(uint8_t char_idx, uint64_t bitmap)
 {

   uint8_t i, row;
   uint64_t mask = 255;
 
   if(char_idx > 7) 
    return;

   my_spi_WEH001602_out_cmd(0x40 + (char_idx*8));

   bcm2835_delay(10);

   for(i=0; i < 8; i++)
    {
     row = bitmap & mask;
     my_spi_WEH001602_out_data(row);
     bitmap = bitmap >> 8;
    }

 }

uint8_t my_spi_WEH001602_init_character(void)
 {
  my_spi_WEH001602_out_cmd(0b00010111);
 }

uint8_t my_spi_WEH001602_init_graphics(void)
 {
  my_spi_WEH001602_out_cmd(0b00011111);  // set graphic mode
 }

uint8_t my_spi_WEH001602_out_pixel_pattern(uint8_t pattern,uint8_t x, uint8_t y)
 {
  my_spi_WEH001602_out_cmd(128+x);
  if(y == 0)
   my_spi_WEH001602_out_cmd(0b01000000);
  else if(y == 1)
   my_spi_WEH001602_out_cmd(0b01000001);
  my_spi_WEH001602_out_data(pattern);
 }



uint8_t my_spi_WEH001602_init(void)
 {

   bcm2835_gpio_fsel(MY_MISO, BCM2835_GPIO_FSEL_INPT);
   bcm2835_gpio_fsel(MY_MOSI, BCM2835_GPIO_FSEL_OUTP);
   bcm2835_gpio_fsel(MY_CLK, BCM2835_GPIO_FSEL_OUTP);
   bcm2835_gpio_fsel(MY_CS0, BCM2835_GPIO_FSEL_OUTP);

   bcm2835_gpio_write(MY_CLK, HIGH);
   bcm2835_gpio_write(MY_CS0, HIGH);
   bcm2835_gpio_write(MY_MOSI, LOW);

   G_clkval = 0;

   usleep(100);

   my_spi_WEH001602_out_cmd(0b00000001);  // Display clear
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00111011);  // Function set: 8bit, 2 Zeilen, 5x8 Punkte, Westeurop (table 2). Charset
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00010011); // power off
   usleep(5000);
   my_spi_WEH001602_out_cmd(0b00001000);  // Display off. Display an, Cursor aus, Blinken aus.
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00000010);  // Display home
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00000110);  // Entry mode: Dekrement, no shift.
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00010100);
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00010111); // power on, character mode
   usleep(5000);
   my_spi_WEH001602_out_cmd(0b00001100);  // Display on. Display an, Cursor aus, Blinken aus.
   usleep(5000);

   my_spi_WEH001602_out_cmd(0b00111011);  // Function set: 8bit, 2 Zeilen, 5x8 Punkte, Westeurop (table 2). Charset
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00001100);  // Display on. Display an, Cursor aus, Blinken aus.
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00000001);  // Display clear
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00000010);  // Display home
   bcm2835_delay(10);
   my_spi_WEH001602_out_cmd(0b00000110);  // Entry mode: Dekrement, no shift.


   //my_spi_WEH001602_def_char(1,0x0000000E0A0E0000); // VU segment
   my_spi_WEH001602_def_char(1,0x00000E0A0A0A0E00); // VU segment
   my_spi_WEH001602_def_char(2,0x001F181515151B1F); // small negative Q or B (depends on stream buffer fill - redefined in player-thread.c)
   my_spi_WEH001602_def_char(4,0x001F17171317111F); // small negative F
   my_spi_WEH001602_def_char(3,0x001F17111011171F); // small vertical arrow ("play" indicator)
   my_spi_WEH001602_def_char(5,0x0000041F0E0E0E04); // bell icon (alarm active)
   my_spi_WEH001602_def_char(6,0x0000000000000000); // wifi signal (dynamically redefined in wifi_signal.c)
   my_spi_WEH001602_def_char(7,0x00FF1B1B1B1B11FF); // small negative T
   my_spi_WEH001602_def_char(0,0x0004160D060D1604); // bluetooth icon
  
   PL_debug("my_spi_WEH001602_init: display init complete");
 
   return 1;

 }

void my_spi_WEH001602_out_text(uint8_t row, unsigned char *text)
 {
  char zeichen;
 
  if(row == 0)
   my_spi_WEH001602_out_cmd((0x80)+0);
  else
   my_spi_WEH001602_out_cmd((0x80)+64);	
	
  while ((zeichen = *text++)) 
   {	
    my_spi_WEH001602_out_data(zeichen);
   }
 }

void my_spi_WEH001602_out_text_at_col(uint8_t row, uint8_t col, unsigned char *text)
 {
  char zeichen;

  if(row == 0)
   my_spi_WEH001602_out_cmd((0x80)+0+col);
  else
   my_spi_WEH001602_out_cmd((0x80)+64+col);

  if(text[0] == 0)
   {
    my_spi_WEH001602_out_data(0);
    return;
   }
  else
   {
    while ((zeichen = *text++))
     my_spi_WEH001602_out_data(zeichen);
   }
 }

void my_spi_WEH001602_move_cursor_at_col(uint8_t row, uint8_t col)
 {

  if(row == 0)
   my_spi_WEH001602_out_cmd((0x80)+0+col);
  else
   my_spi_WEH001602_out_cmd((0x80)+64+col);

 }

void my_spi_WEH001602_scroll_rss_once(unsigned char *message, uint8_t row)
 {

   uint16_t end = 0, msg_chr_counter = 0;
   uint16_t scroll_pos = 0;
   uint16_t msglen, i;
   char scroll_sec[DISPLAY_WIDTH];
   char scroll_sec_empty[DISPLAY_WIDTH];

   G_scroll_rss_active = 1;

   msglen = strlen(message);
    
   memset(scroll_sec,' ',DISPLAY_WIDTH);
   memset(scroll_sec_empty,' ',DISPLAY_WIDTH);

   while(!end)   
    {
   
     for(i=0; i <= DISPLAY_WIDTH; i++)
      {

       if(i < DISPLAY_WIDTH-1)
        scroll_sec[i] = scroll_sec[i+1];

       if(i == DISPLAY_WIDTH-1)
        {
         if(msg_chr_counter == msglen)
          scroll_sec[i] = ' ';
         else
          scroll_sec[i] = message[msg_chr_counter++];
        }

      } 

     pthread_mutex_lock(&G_display_lock);
     my_spi_WEH001602_out_text(row,scroll_sec);
     pthread_mutex_unlock(&G_display_lock);

     scroll_pos++;

     if(G_kill_rss_scroll)
      {
       G_kill_rss_scroll = 0;
       pthread_mutex_lock(&G_display_lock);
       my_spi_WEH001602_out_text(row,"                ");
       pthread_mutex_unlock(&G_display_lock);
       end = 1;
       G_scroll_rss_active = 0;
       return;
      }

     usleep(DISPLAY_SCROLL_DELAY);

     if(scroll_pos == msglen+DISPLAY_WIDTH+2)
      end = 1;

    }

    G_scroll_rss_active = 0;
 
 }

void my_spi_WEH001602_scroll_meta_once(unsigned char *message, uint8_t row)
 {

   uint16_t end = 0, msg_chr_counter = 0;
   uint16_t scroll_pos = 0;
   uint16_t msglen, i;
   char scroll_sec[DISPLAY_WIDTH];
   char scroll_sec_empty[DISPLAY_WIDTH];

   G_scroll_meta_active = 1;

   msglen = strlen(message);
    
   memset(scroll_sec,' ',DISPLAY_WIDTH);
   memset(scroll_sec_empty,' ',DISPLAY_WIDTH);

   while(!end)   
    {
   
     for(i=0; i <= DISPLAY_WIDTH; i++)
      {

       if(i < DISPLAY_WIDTH-1)
        scroll_sec[i] = scroll_sec[i+1];

       if(i == DISPLAY_WIDTH-1)
        {
         if(msg_chr_counter == msglen)
          scroll_sec[i] = ' ';
         else
          scroll_sec[i] = message[msg_chr_counter++];
        }

      } 

     pthread_mutex_lock(&G_display_lock);
     my_spi_WEH001602_out_text(row,scroll_sec);
     pthread_mutex_unlock(&G_display_lock);

     scroll_pos++;

     if(G_kill_meta_scroll)
      {
       G_kill_meta_scroll = 0;
       pthread_mutex_lock(&G_display_lock);
       my_spi_WEH001602_out_text(row,"                ");
       pthread_mutex_unlock(&G_display_lock);
       end = 1;
       G_scroll_meta_active = 0;
       return;
      }

     usleep(DISPLAY_SCROLL_DELAY);

     if(scroll_pos == msglen+DISPLAY_WIDTH+2)
      end = 1;

    }

    G_scroll_meta_active = 0;
 
 }

void my_spi_WEH001602_scroll_text_once(unsigned char *message, uint8_t row)
 {

   uint16_t end = 0, msg_chr_counter = 0;
   uint16_t scroll_pos = 0;
   uint16_t msglen, i;
   char scroll_sec[DISPLAY_WIDTH];
   char scroll_sec_empty[DISPLAY_WIDTH];

   msglen = strlen(message);
    
   memset(scroll_sec,' ',DISPLAY_WIDTH);
   memset(scroll_sec_empty,' ',DISPLAY_WIDTH);

   while(!end)   
    {
   
     for(i=0; i <= DISPLAY_WIDTH; i++)
      {

       if(i < DISPLAY_WIDTH-1)
        scroll_sec[i] = scroll_sec[i+1];

       if(i == DISPLAY_WIDTH-1)
        {
         if(msg_chr_counter == msglen)
          scroll_sec[i] = ' ';
         else
          scroll_sec[i] = message[msg_chr_counter++];
        }

      } 

     pthread_mutex_lock(&G_display_lock);
     my_spi_WEH001602_out_text(row,scroll_sec);
     pthread_mutex_unlock(&G_display_lock);

     scroll_pos++;

     usleep(DISPLAY_SCROLL_DELAY);

     if(scroll_pos == msglen+DISPLAY_WIDTH+2)
      end = 1;

    }
 
 }

void  my_spi_WEH001602_scroll_text(unsigned char *message, uint8_t row, uint16_t times)
 {
   uint16_t end = 0, scroll_pos,msglen,copy_len1, copy_len2;
   uint16_t scrolled = 0;
   
   char scroll_sec[DISPLAY_WIDTH];

   msglen = strlen(message);
   scroll_pos = 0;

   copy_len1 = DISPLAY_WIDTH;

   while(!end)
   {

    if( (scroll_pos+DISPLAY_WIDTH) == msglen)
     scrolled++;

    if( (scroll_pos+DISPLAY_WIDTH) >= msglen)
     copy_len1 = msglen - scroll_pos;

    if(scroll_pos == msglen)
     {
      scroll_pos = 0;
      copy_len1 = DISPLAY_WIDTH;
     }

    memcpy(scroll_sec, message+scroll_pos, copy_len1);
    
    if(copy_len1 < DISPLAY_WIDTH)  // wrap message
     {
       copy_len2 = DISPLAY_WIDTH - copy_len1; 
       memcpy(scroll_sec+copy_len1, message, copy_len2);
     }

    my_spi_WEH001602_out_text(row,scroll_sec);

    usleep(DISPLAY_SCROLL_DELAY); 

    scroll_pos++;

    if(scrolled == times)
     {
      my_spi_WEH001602_out_cmd(0b00000001);
      return;
     } 
   } 
 }


// end

