#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "i2c_opencores.h"
#include "it6613.h"

volatile uint8_t cur_bank;

inline void select_bank_it(uint8_t bank) {
	cur_bank = bank;
	I2C_start(I2CA_BASE, IT_BASE, 0);
	I2C_write(I2CA_BASE, IT_CURBANK, 0);
	I2C_write(I2CA_BASE, cur_bank, 1);
}

inline uint32_t read_it(uint32_t regaddr) {
	if ((regaddr > 0xFF) && (cur_bank == 0))
		select_bank_it(1);
	else if ((regaddr <= 0xFF) && (cur_bank == 1))
		select_bank_it(0);

	I2C_start(I2CA_BASE, IT_BASE, 0);
	I2C_write(I2CA_BASE, (regaddr & 0xFF), 1);
	I2C_start(I2CA_BASE, IT_BASE, 1);
	return I2C_read(I2CA_BASE,1);
}

inline void write_it(uint32_t regaddr, uint8_t data) {
	if ((regaddr > 0xFF) && (cur_bank == 0))
		select_bank_it(1);
	else if ((regaddr <= 0xFF) && (cur_bank == 1))
		select_bank_it(0);

	I2C_start(I2CA_BASE, IT_BASE, 0);
	I2C_write(I2CA_BASE, (regaddr & 0xFF), 0);
	I2C_write(I2CA_BASE, data, 1);
}

int init_it() {
	uint32_t vendor_id, device_id;
	uint32_t i;

	cur_bank = 0;
	select_bank_it(cur_bank);

	vendor_id = read_it(IT_VENDORID);
	device_id = read_it(IT_DEVICEID);

	printf("VEN: 0x%.2lX, DEV: 0x%.2lX\n", vendor_id, device_id);

	if (!((vendor_id == IT6613_VENDORID) && (device_id == IT6613_DEVICEID)))
		return -1;

	write_it(REG_TX_SW_RST,B_REF_RST|B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST) ;
	usleep(1000);
    write_it(REG_TX_SW_RST,B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST) ;

	write_it(IT_DRIVECTRL, 0x10);

	write_it(IT_HDMIMODE, 0);
	for (i=0xC1; i<=0xD0; i++)
		write_it(i, 0);

	write_it(IT_OUTCOLOR, 0);

	// enable video
	//HDMITX_WriteI2C_Byte(REG_TX_SW_RST,B_VID_RST|B_AUD_RST|B_AREF_RST|B_HDCP_RST) ;

	write_it(REG_TX_SW_RST,          B_AUD_RST|B_AREF_RST|B_HDCP_RST) ;
	usleep(1000);



	return 0;
}
