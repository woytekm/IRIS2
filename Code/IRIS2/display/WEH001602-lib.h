#define MY_MISO RPI_BPLUS_GPIO_J8_29
#define MY_MOSI RPI_BPLUS_GPIO_J8_31
#define MY_CLK RPI_BPLUS_GPIO_J8_18
#define MY_CS0 RPI_BPLUS_GPIO_J8_33

#define DISPLAY_WIDTH 16

#define TOP_ROW 0
#define BOTTOM_ROW 1

uint8_t G_clkval;

void my_spi_toggle_clk(void);
void my_spi_WEH001602_out_cmd(uint8_t cmdbyte);
void my_spi_WEH001602_out_data(uint8_t cmdbyte);
uint8_t my_spi_WEH001602_init(void);
void my_spi_WEH001602_out_text(uint8_t row, unsigned char *text);
void my_spi_WEH001602_scroll_text_once(unsigned char *message, uint8_t row);
void my_spi_WEH001602_scroll_text(unsigned char *message, uint8_t row, uint16_t times);

