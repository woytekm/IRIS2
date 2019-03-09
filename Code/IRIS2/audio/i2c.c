#include<bcm2835.h>
#include "global.h"


uint8_t I2C_WriteByte(unsigned char address,char bdata)
{    
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    char data = bdata;
    return bcm2835_i2c_write(&data, 1);
}

/**
 *@brief Writes a byte value to a register address
 *@param reg Address of sensor register.
 *@param data Data byte to be written on register.
 *@return none
 */
uint8_t I2C_WriteByteRegister(unsigned char address,unsigned char reg,unsigned char data)
{
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    unsigned char wr_buf[2];

    wr_buf[0] = reg;
    wr_buf[1] = data;

    return bcm2835_i2c_write((const char *)wr_buf, 2);
}

/**
 *@brief Writes a word value (16 bit) to a register address.
 *@param reg Address of sensor register.
 *@param data Data word to be written on word size register.
 *@return none
 */
uint8_t I2C_WriteWordRegister(unsigned char address,unsigned char reg, unsigned char* data)
{
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    unsigned char wr_buf[3];
    
    wr_buf[0] = reg;
    wr_buf[1] = data[0];
    wr_buf[2] = data[1];

    return bcm2835_i2c_write((const char *)wr_buf, 3);
}

/**
 *@brief Writes a buffer array to the registers
 *@param reg    Address of sensor register, address autoincrements
 *@param data    Pointer to byte data buffer array
 *@param length    length of buffer array
 *@return none
 */
uint8_t I2C_WriteByteArray(unsigned char address,char reg, char* data, unsigned int length)
{
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    char* wr_buf = (char*) malloc(sizeof(char) * length);
    if (wr_buf==NULL) 
    {
        printf("Error allocating memory!\n"); //print an error message
    }
    
    wr_buf[0] = reg;
    for(unsigned int i = 1;i<length;i++)
    {
        wr_buf[i] = data[i];
    }

    return bcm2835_i2c_write((const char *)wr_buf, length);
}

/**
 *@brief Reads a byte from a register
 *@param reg Address of sensor register.
 *@return val Byte value of register.
 */
unsigned char I2C_ReadByteRegister(unsigned char address,char reg)
{
    char val = 0;
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    bcm2835_i2c_read_register_rs(&reg,&val,1);
    
    return val;
 }
 
 /**
 *@brief Initializes the I2C peripheral
 *@param reg    Address of sensor register, address autoincrements
 *@param *buffer    Pointer to byte data buffer array
 *@param length    length of buffer array
 *@return none
 */
void I2C_ReadByteArray(unsigned char address,char reg,char *buffer,unsigned int  length)
{    
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    bcm2835_i2c_read_register_rs(&reg,buffer,length);
}

 /**
 *@brief Readm result from a word length register
 *@param reg register to read from
 *@return val Word value of register
 */
unsigned int I2C_ReadWordRegisterRS(unsigned char address,char reg)
{
    bcm2835_i2c_setSlaveAddress(address);    //Set device address
    
       char cmd[1] = {reg}; 
    char receive[2] = {0};
    bcm2835_i2c_write_read_rs(cmd,1,receive,2);
    
    return (receive[0]<<8)|receive[1];
}

/**
 *@brief Read the value of a register that has already been select via the address pointer
 *@return Data Value of preset register
 */
unsigned int I2C_ReadWordPresetPointer(void)
{
    char val[2] = {0}; 
    bcm2835_i2c_read(val,2);
    unsigned int data = (val[0] << 8)|val[1];
    
    return data;
 }


