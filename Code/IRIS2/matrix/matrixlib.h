#include <bcm2835.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "gfxfont.h"

#define MATRIXBUFLEN 2049
#define FRAME_SIZE 2048

#define MATRIXLEN 64
#define MATRIXHEIGHT 32

char mi_matrix_edit_buffer[MATRIXBUFLEN];
char mi_matrix_commit_buffer[MATRIXBUFLEN];
char mi_spi_recv_buffer[MATRIXBUFLEN];
uint8_t mi_brightness;
uint8_t mi_debug;

const GFXfont *gfxFont;
const GFXfont *gfxFontSmall;

uint16_t cursor_x;
uint16_t cursor_y;
uint16_t _width;
uint16_t _height;

void m_clear(void);
void m_init(void);
void m_set_brightness(uint8_t brightness);
void m_putpixel(uint8_t x, uint8_t y, uint16_t color);
void m_putaline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t color);
void m_putvline(int16_t x, int16_t y,int16_t h, uint16_t color);
void puthline(int16_t x, int16_t y, int16_t w, uint16_t color);
void m_putline(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void m_putrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void m_putfillrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void m_display(void);
uint8_t mi_commit_to_matrix(void);
uint8_t mi_commit_edit_buffer(void);
uint8_t mi_init_spi(void);
void mi_close_spi(void);
void mi_print_debug(char *text);
