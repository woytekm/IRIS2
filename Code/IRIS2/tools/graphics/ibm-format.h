// IBM format is 64 byte header starting with "IBM" and then bitmap description, then two rows of 100 uint8_t one
// after the other - one vertical pixel line per uint8_t. This format allows for easy displaying of RAW graphic
// on WEH001602 16x2 OLED display in graphic mode

#define IBM_HEADER_LEN 64
