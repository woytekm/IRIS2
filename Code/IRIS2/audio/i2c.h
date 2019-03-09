uint8_t I2C_WriteByte(unsigned char address,char bdata);
uint8_t I2C_WriteByteRegister(unsigned char address,unsigned char reg,unsigned char data);
uint8_t I2C_WriteWordRegister(unsigned char address,unsigned char reg, unsigned char* data);
uint8_t I2C_WriteByteArray(unsigned char address,char reg, char* data, unsigned int length);
unsigned char I2C_ReadByteRegister(unsigned char address,char reg);
void I2C_ReadByteArray(unsigned char address,char reg,char *buffer,unsigned int  length);
unsigned int I2C_ReadWordRegisterRS(unsigned char address,char reg);
unsigned int I2C_ReadWordPresetPointer(void);

