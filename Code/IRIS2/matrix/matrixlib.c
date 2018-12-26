#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "matrixlib.h"
#include "FreeSansBold9pt7b.h"
#include "FreeSans9pt7b.h"
#include "FreeMonoBold9pt7b.h"
#include "FreeSerifBold9pt7b.h"
#include "Picopixel.h"
#include "Org01.h"

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif


/* 
* external routines
*/ 

void m_clear()
 {
  bzero(mi_matrix_edit_buffer,MATRIXBUFLEN);
 }

void m_init(void)
 {
  mi_init_spi();
  bzero(&mi_matrix_commit_buffer,MATRIXBUFLEN);
  bzero(&mi_matrix_edit_buffer,MATRIXBUFLEN);
  gfxFont = &FreeSansBold9pt7b;
  _width = MATRIXLEN;
  _height = MATRIXHEIGHT;
  mi_debug = 0;
  m_clear();
 } 

void m_set_brightness(uint8_t brightness)
 {
  mi_brightness = brightness;
  mi_matrix_commit_buffer[MATRIXBUFLEN-1] = brightness;
  mi_commit_to_matrix();
 } 

void m_putpixel(uint8_t x, uint8_t y, uint16_t color)
 {
  uint16_t bufferpos;

  bufferpos = x + (y-1)*MATRIXLEN;
  mi_matrix_edit_buffer[bufferpos-1] = (uint8_t)color;
 }

/* all line, rectangle, arc, and font routines derived (ok, blantantly stolen) from Adafruit core GFX library */

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void m_putaline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color)
 {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            m_putpixel((uint8_t)y0, (uint8_t)x0, color);
        } else {
            m_putpixel((uint8_t)x0, (uint8_t)y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
 }

void m_putvline(int16_t x, int16_t y,int16_t h, uint16_t color) 
 {
    m_putaline(x, y, x, y+h-1, color);
 }

void m_puthline(int16_t x, int16_t y, int16_t w, uint16_t color) 
 {
   m_putaline(x, y, x+w-1, y, color);
 }

void m_putline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
 {
    if(x0 == x1){
        if(y0 > y1) _swap_int16_t(y0, y1);
        m_putvline(x0, y0, y1 - y0 + 1, color);
    } else if(y0 == y1){
        if(x0 > x1) _swap_int16_t(x0, x1);
        m_puthline(x0, y0, x1 - x0 + 1, color);
    } else {
        m_putaline(x0, y0, x1, y1, color);
    }
 }

void m_putrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
 {
    m_puthline(x, y, w, color);
    m_puthline(x, y+h-1, w, color);
    m_putvline(x, y, h, color);
    m_putvline(x+w-1, y, h, color);
 }

void m_putfillrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
 {
   int16_t i;

   for (i=x; i<x+w; i++) {
       m_putvline(i, y, h, color);
   }
 }


void m_putchar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
 {

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
    uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t  w  = pgm_read_byte(&glyph->width),
             h  = pgm_read_byte(&glyph->height);
    int8_t   xo = pgm_read_byte(&glyph->xOffset),
             yo = pgm_read_byte(&glyph->yOffset);
    uint8_t  xx, yy, bits = 0, bit = 0;
    int16_t  xo16 = 0, yo16 = 0;

    if(size > 1) {
        xo16 = xo;
        yo16 = yo;
     }

    for(yy=0; yy<h; yy++) 
     {
       for(xx=0; xx<w; xx++)   
        {
            if(!(bit++ & 7)) {
                bits = pgm_read_byte(&bitmap[bo++]);
                }
            if(bits & 0x80) {
                if(size == 1) {
                   m_putpixel(x+xo+xx, y+yo+yy, color);
                 } else {
                   m_putfillrect(x+(xo16+xx)*size, y+(yo16+yy)*size,
                    size, size, color);
                 }
              }
             bits <<= 1;
       }
    }

 }


void m_setcursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
}

void m_writechar(uint8_t c, uint8_t textsize, uint16_t textcolor, uint16_t textbgcolor)
 {

   uint8_t wrap = 1;

   if(c == '\n') 
    {
     cursor_x  = 0;
     cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } 
   else if(c != '\r') 
    {
     uint8_t first = pgm_read_byte(&gfxFont->first);
     if((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) 
       {
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c - first]);
        uint8_t   w = pgm_read_byte(&glyph->width),
                  h = pgm_read_byte(&glyph->height);
        if((w > 0) && (h > 0)) 
           { // Is there an associated bitmap?
            int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
            if(wrap && ((cursor_x + textsize * (xo + w)) > _width)) 
              {
                cursor_x  = 0;
                cursor_y += (int16_t)textsize * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
               }
             m_putchar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
           }
         cursor_x += (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
       }
     }

  }



void m_display(void)
 {
  mi_commit_edit_buffer();
  mi_matrix_edit_buffer[MATRIXBUFLEN-1] = mi_brightness;
  mi_commit_to_matrix();
 }

/*
* internal routines
*/

uint8_t mi_commit_to_matrix()
 {
  uint16_t cmp;
   
  bcm2835_spi_transfernb((char *)&mi_matrix_commit_buffer,(char *)&mi_spi_recv_buffer,MATRIXBUFLEN);
  cmp = memcmp(&mi_matrix_commit_buffer,&mi_spi_recv_buffer,MATRIXBUFLEN);
  if(cmp > 0)
   {
    return 0;
    mi_print_debug("matrix commit failed.\n");
   }
  else 
   {
    mi_print_debug("matrix commit ok.\n");
    return 1;
   }
 }

void mi_print_debug(char *text)
 {
  if(mi_debug == 1)
   printf(text);
 }

uint8_t mi_commit_edit_buffer(void)
 {
  memcpy(&mi_matrix_commit_buffer,&mi_matrix_edit_buffer,MATRIXBUFLEN);
 }

 
uint8_t mi_init_spi(void)
 {

  //if (!bcm2835_init())
  // {
  //  printf("bcm2835_init failed. Are you running as root??\n");
  //  return 1;
  // }

  if (!bcm2835_spi_begin())
   {
    printf("bcm2835_spi_begin failed. Are you running as root??\n");
    return 1;
   }

  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); // The default
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
 }


void mi_close_spi(void)
 {
  bcm2835_spi_end();
  bcm2835_close();
 }


