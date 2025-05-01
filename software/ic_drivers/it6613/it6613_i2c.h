#ifndef _IT6613_I2C_H
#define _IT6613_I2C_H

#include <it6613_types.h>

uint8_t I2C_Read_Byte(uint8_t Addr, uint8_t RegAddr);
SYS_STATUS I2C_Write_Byte(uint8_t Addr, uint8_t RegAddr, uint8_t Data);
SYS_STATUS I2C_Read_ByteN(uint8_t Addr, uint8_t RegAddr, uint8_t *pData, int N);
SYS_STATUS I2C_Write_ByteN(uint8_t Addr, uint8_t RegAddr, uint8_t *pData, int N);

uint8_t HDMITX_ReadI2C_Byte(uint8_t RegAddr);
SYS_STATUS HDMITX_WriteI2C_Byte(uint8_t RegAddr, uint8_t val);
SYS_STATUS HDMITX_ReadI2C_ByteN(uint8_t RegAddr, uint8_t *pData, int N);
SYS_STATUS HDMITX_WriteI2C_ByteN(uint8_t RegAddr, uint8_t *pData, int N);

#endif