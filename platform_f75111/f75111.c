// SMBus.cpp: implementation of the SMBus class.
//
//////////////////////////////////////////////////////////////////////

#include <linux/module.h>
#include <linux/kernel.h>       /**< printk() */
#include <linux/delay.h>

#include "f75111.h"

static uint8_t f75111_address = 0x9c;

int F75111_Init()
{
    

	if(PCI_DetectBaytrail())
	{
		if(SMBus_CheckDevice(f75111_address))
		{
			SMBus_WriteByte(f75111_address,GPIO1X_CONTROL_MODE  ,0x00);
			SMBus_WriteByte(f75111_address,GPIO3X_CONTROL_MODE  ,0x00);
			SMBus_WriteByte(f75111_address,GPIO2X_CONTROL_MODE  ,0xFF);

			SMBus_WriteByte(f75111_address,GPIO2X_OUTPUT_DRIVING,0xFF);
			SMBus_WriteByte(f75111_address,F75111_CONFIGURATION,0x07);
			SMBus_WriteByte(f75111_address,0x06,0x04);
            printk("PCI init Baytrail bus OK!\n");
			return 0;
		}
	}
	else
    {
        printk("PCI init Baytrail bus error.\n");
    }
	return -1;
}

uint8_t F75111_GetDigitalInput ()
{
	uint8_t byteGPIO1X = 0;
	uint8_t byteGPIO3X = 0;
	uint8_t byteData   = 0;

	SMBus_ReadByte(f75111_address,GPIO1X_INPUT_DATA,&byteGPIO1X) ;
	SMBus_ReadByte(f75111_address,GPIO3X_INPUT_DATA,&byteGPIO3X) ;
	byteGPIO1X = byteGPIO1X  & 0xF0;
	byteGPIO3X = byteGPIO3X  & 0x0F;

	byteData = ( byteGPIO1X & 0x10 )? byteData + 0x01 : byteData;
	byteData = ( byteGPIO1X & 0x80 )? byteData + 0x02 : byteData;
	byteData = ( byteGPIO1X & 0x40 )? byteData + 0x04 : byteData;
	byteData = ( byteGPIO3X & 0x01 )? byteData + 0x08 : byteData;

	byteData = ( byteGPIO3X & 0x02 )? byteData + 0x10 : byteData;
	byteData = ( byteGPIO3X & 0x04 )? byteData + 0x20 : byteData;
	byteData = ( byteGPIO3X & 0x08 )? byteData + 0x40 : byteData;
	byteData = ( byteGPIO1X & 0x20 )? byteData + 0x80 : byteData;

	return byteData;
}

void F75111_SetDigitalOutput(uint8_t byteValue)
{
	uint8_t byteData = 0;

	byteData = (byteValue & 0x01 )? byteData + 0x01 : byteData;
	byteData = (byteValue & 0x02 )? byteData + 0x02 : byteData;
	byteData = (byteValue & 0x04 )? byteData + 0x04 : byteData;
	byteData = (byteValue & 0x80 )? byteData + 0x08 : byteData;
	byteData = (byteValue & 0x40 )? byteData + 0x10 : byteData;
	byteData = (byteValue & 0x20 )? byteData + 0x20 : byteData;
	byteData = (byteValue & 0x10 )? byteData + 0x40 : byteData;
	byteData = (byteValue & 0x08 )? byteData + 0x80 : byteData;

	SMBus_WriteByte(f75111_address,GPIO2X_OUTPUT_DATA,byteData);
	udelay(10);
}

void F75111_SetWDTEnable (uint8_t byteTimer)
{
    printk("My new wdt...\n");
    SMBus_WriteByte(f75111_address,WDT_TIMER_RANGE  ,byteTimer);
    udelay(10);
    SMBus_WriteByte(f75111_address,WDT_CONFIGURATION,1<<MYWDT_STS_WD_TMOUT | 1<<MYWDT_ENABLE | 0<<MYWDT_UNIT | 1<<MYWDT_PULSE | 2<<MYWDT_PSWIDTH);
}

void F75111_SetWDTDisable ()
{
    printk("My new disable wdt...\n");
	SMBus_WriteByte(f75111_address,WDT_CONFIGURATION,0x00);
}

void F75111_SetWDTFeed (uint8_t byteTimer)
{
    printk("My new feed wdt %d...\n", byteTimer);
	SMBus_WriteByte(f75111_address,WDT_TIMER_RANGE, byteTimer);
}
