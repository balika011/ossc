#include <it6613_i2c.h>
#include "system.h"
#include "i2c_opencores.h"
#include "it6613.h"

inline uint32_t read_it2(uint32_t regaddr)
{
	I2C_start(I2CA_BASE, IT_BASE, 0);
	I2C_write(I2CA_BASE, regaddr, 0);
	I2C_start(I2CA_BASE, IT_BASE, 1);
	return I2C_read(I2CA_BASE, 1);
}

inline void write_it2(uint32_t regaddr, uint8_t data)
{
	I2C_start(I2CA_BASE, IT_BASE, 0);
	I2C_write(I2CA_BASE, regaddr, 0);
	I2C_write(I2CA_BASE, data, 1);
}

uint8_t I2C_Read_Byte(uint8_t Addr, uint8_t RegAddr)
{
	I2C_start(I2CA_BASE, Addr, 0);
	I2C_write(I2CA_BASE, RegAddr, 0);
	I2C_start(I2CA_BASE, Addr, 1);
	return I2C_read(I2CA_BASE, 1);
}

SYS_STATUS I2C_Write_Byte(uint8_t Addr, uint8_t RegAddr, uint8_t Data)
{
	I2C_start(I2CA_BASE, Addr, 0);
	I2C_write(I2CA_BASE, RegAddr, 0);
	I2C_write(I2CA_BASE, Data, 1);
	return 0;
}

SYS_STATUS I2C_Read_ByteN(uint8_t Addr, uint8_t RegAddr, uint8_t *pData, int N)
{
	int i;

	for (i = 0; i < N; i++)
		pData[i] = I2C_Read_Byte(Addr, RegAddr + i);

	return 0;
}

SYS_STATUS I2C_Write_ByteN(uint8_t Addr, uint8_t RegAddr, uint8_t *pData, int N)
{
	int i;

	for (i = 0; i < N; i++)
		I2C_Write_Byte(Addr, RegAddr + i, pData[i]);

	return 0;
}

uint8_t HDMITX_ReadI2C_Byte(uint8_t RegAddr)
{
	return read_it2(RegAddr);
}

SYS_STATUS HDMITX_WriteI2C_Byte(uint8_t RegAddr, uint8_t val)
{
	write_it2(RegAddr, val);
	return 0;
}

SYS_STATUS HDMITX_ReadI2C_ByteN(uint8_t RegAddr, uint8_t *pData, int N)
{
	int i;

	for (i = 0; i < N; i++)
		pData[i] = HDMITX_ReadI2C_Byte(RegAddr + i);

	return 0;
}

SYS_STATUS HDMITX_WriteI2C_ByteN(uint8_t RegAddr, uint8_t *pData, int N)
{
	int i;

	for (i = 0; i < N; i++)
		HDMITX_WriteI2C_Byte(RegAddr + i, pData[i]);

	return 0;
}