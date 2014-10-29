// SMBus.cpp: implementation of the SMBus class.
//
//////////////////////////////////////////////////////////////////////
#include <linux/kernel.h>       /**< printk() */
#include <linux/io.h>
#include <linux/delay.h>

#include "SMBus.h"

//////////////////////////////////////////////////////////////////////
uint16_t m_SMBusMapIoAddr;

int devid; //for SMBus Devices ID
//////////////////////////////////////////////////////////////////////
// PCI Device Write/Read Function
//////////////////////////////////////////////////////////////////////
uint32_t PCI_Read(int size_t)
{
	uint32_t dwAddrVal;
	//GetPortVal(PCI_CONFIG_DATA,&dwAddrVal,size_t);
	dwAddrVal=inl(PCI_CONFIG_DATA);
    udelay(10);
	return dwAddrVal;
}

void PCI_Write(uint32_t dwDataVal,int size_t)
{
	//SetPortVal(PCI_CONFIG_ADDR, dwDataVal,size_t);
	outl(dwDataVal,PCI_CONFIG_ADDR);
	udelay(10);
}
//////////////////////////////////////////////////////////////////////
// Get VT8237 SMBus I/O Address
//////////////////////////////////////////////////////////////////////
#if 0
int PCI_AutoDetect()
{
    uint32_t dwResult;
    uint32_t dwIOAddr;
	int PCI_Function = 0;
    int PCI_Bus = 0;
    int PCI_Device = 0;
    devid =0;
	for (PCI_Bus = 0;PCI_Bus<5;PCI_Bus++)
	{

		for (PCI_Device = 0;PCI_Device<32;PCI_Device++)
		{

			for(PCI_Function = 0; PCI_Function<8 ; PCI_Function++ )
			{

				
                // 格式 31 Enabled 23:16 总线编号  15:11 设备编号 10:8 功能编号 7:2 配置空间寄存器编号 1:0 为0
				dwIOAddr = 0x80000000 + PCI_Bus*0x10000 + PCI_Device*0x800 + PCI_Function*0x100;

                // 将地址写到CONFIG_ADDR寄存器
				PCI_Write(dwIOAddr,4);

				dwResult = PCI_Read(4); // 读data数据寄存器，得到id
                if (dwResult != 0xffffffff) // not the bad one print it
                    printk("PCI Read result: 0x%08x ioaddr: 0x%08x\n", dwResult, dwIOAddr);

				if ((( dwResult & 0xFFFF) == VIA_VT8237_BUS_CTRL_VENDOR_ID )  &&
					(( dwResult >> 16 )   == VIA_VT8237_BUS_CTRL_DEVICE_ID ))
				{
					printk ("AutoDetect_IOBase : VIA VT8237 Bus Controller Found ... \n");
					return Get_VT8237_SMBusIoAddr(dwIOAddr);
				}

				if ((( dwResult & 0xFFFF) == VIA_CX700M_BUS_CTRL_VENDOR_ID )  &&
					(( dwResult >> 16 )   == VIA_CX700M_BUS_CTRL_DEVICE_ID ))
				{
					printk ("AutoDetect_IOBase : VIA VT8237 Bus Controller Found ... \n");
					return Get_CX700M_SMBusIoAddr(dwIOAddr);
				}

				if ((( dwResult & 0xFFFF) == INTEL_ICH4_VENDOR_ID )  &&
					(( dwResult >> 16 )   == INTEL_ICH4_DEVICE_ID ))
				{
					printk ("AutoDetect_IOBase : Intel ICH4 Chipset Found ...\n");
					PCI_Function=0;
					for (PCI_Function=0;PCI_Function<8;PCI_Function++)
					{
						dwIOAddr = dwIOAddr + PCI_Function*0x100;
						PCI_Write(dwIOAddr,4);
						dwResult = PCI_Read(4);

						if ((( dwResult & 0xFFFF) == INTEL_ICH4_SMBUS_VENDOR_ID )  &&
							(( dwResult >> 16   ) == INTEL_ICH4_SMBUS_DEVICE_ID ))
						{
							printk ("AutoDetect_IOBase : Intel ICH4 SMBus Controller Found ... \n");
							return Get_ICH4_SMBusIoAddr(dwIOAddr);
						}
					}
				}


				if ((( dwResult & 0xFFFF) == INTEL_ICH7_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_ICH7_SMBUS_DEVICE_ID ))						//	0x27DA
				{

                    printk("AutoDetect_IOBase : Intel ICH7 SMBus Controller Found ... \n");
					return Get_ICH7_SMBusIoAddr(dwIOAddr);
				}

                if ((( dwResult & 0xFFFF) == INTEL_ICH8_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_ICH8_SMBUS_DEVICE_ID ))						//	0x27DA
				{

                    printk("AutoDetect_IOBase : Intel ICH8 SMBus Controller Found ... \n");
					return Get_ICH8_SMBusIoAddr(dwIOAddr);
				}

				if ((( dwResult & 0xFFFF) == INTEL_ICH10_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_ICH10_SMBUS_DEVICE_ID ))						//	0x3A30
				{

                    printk("AutoDetect_IOBase : Intel ICH10 SMBus Controller Found ... \n");
					return Get_ICH10_SMBusIoAddr(dwIOAddr);
				}

				if ((( dwResult & 0xFFFF) == INTEL_QM67_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_QM67_SMBUS_DEVICE_ID ))						//	0x3A30
				{

                    printk("AutoDetect_IOBase : Intel QM67 SMBus Controller Found ... \n");
					return Get_QM67_SMBusIoAddr(dwIOAddr);
				}

				if ((( dwResult & 0xFFFF) == INTEL_QM77_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_QM77_SMBUS_DEVICE_ID ))						//	0x3A30
				{

                    printk("AutoDetect_IOBase : Intel QM77 SMBus Controller Found ... \n");
					return Get_QM77_SMBusIoAddr(dwIOAddr);
				}
				if ((( dwResult & 0xFFFF) == VIA_VX900_BUS_CTRL_VENDOR_ID )  &&					//	0x1106
					(( dwResult >> 16   ) == VIA_VX900_BUS_CTRL_DEVICE_ID ))					//	0x8410
				{

					printk("AutoDetect_IOBase : VIA VX900 SMBus Controller Found ... \n");
					devid = VIA_VX900_BUS_CTRL_DEVICE_ID;
					return Get_VX900_SMBusIoAddr(dwIOAddr);
				}
                // e3845 pci
				if ((( dwResult & 0xFFFF) == INTEL_E3815_SMBUS_VENDOR_ID )  &&					//	0x8086
					(( dwResult >> 16   ) == INTEL_E3815_SMBUS_DEVICE_ID ))						//	0x0F12
				{
					printk("AutoDetect_IOBase : Intel E3815 SMBus Controller Found IOAddr: %x ... \n", dwIOAddr);
					return Get_E3845_SMBusIoAddr(dwIOAddr);
				}
			}
		}
	}
	printk("AutoDetect_IOBase : Can't Found Any SMBus Controller!\n");
	return FALSE;
}
#endif

// 因此目前所在的平台的PCI地址是固定的，所以不像上面函数那样做
int PCI_DetectBaytrail(void)
{
    uint32_t dwResult;
    uint32_t dwIOAddr;
    int i = 0;
	// 格式 31 Enabled 23:16 总线编号  15:11 设备编号 10:8 功能编号 7:2 配置空间寄存器编号 1:0 为0
    //dwIOAddr = 0x80000000 + PCI_Bus*0x10000 + PCI_Device*0x800 + PCI_Function*0x100;
    dwIOAddr = 0x8000fb00;  // 直接指定
    // 将地址写到CONFIG_ADDR寄存器
    PCI_Write(dwIOAddr,4);

    dwResult = PCI_Read(4); // 读data数据寄存器，得到id
    
    // 检查10次
    for (i = 0; i < 10; i++)
    {
        // e3845 pci
        if ((( dwResult & 0xFFFF) == INTEL_E3815_SMBUS_VENDOR_ID )  &&					//	0x8086
            (( dwResult >> 16   ) == INTEL_E3815_SMBUS_DEVICE_ID ))						//	0x0F12
        {
            printk("AutoDetect_IOBase : Intel E3815 SMBus Controller Found IOAddr: %x ... \n", dwIOAddr);
            return Get_E3845_SMBusIoAddr(dwIOAddr);
        }
        udelay(10);
    }

	printk("AutoDetect_IOBase : Can't Found Any SMBus Controller!\n");
	return FALSE;
}

int Get_E3845_SMBusIoAddr(uint32_t dwIOAddr)
{
	uint32_t dwResult;
    
    // 从哪里得知这个地址的?
	PCI_Write(dwIOAddr+E3815_SMBUS_HOST_CONFIGUREE,4);

	dwResult = 	PCI_Read(2);

	//printk ("E3815 SMBus Host Configure = 0x%4X\n",dwResult );

	if ( dwResult & E3815_SMBUS_HOST_HST_EN )
	{
		//printk ("AutoDetect_IOBase : Intel E3815 SMBus Host Enable!\n");
	}
	else
	{
		//printk ("AutoDetect_IOBase : Intel E3815 SMBus Host Disable!\n");

		return FALSE;
	}

    // 不知为什么，把读取的地址取前面几位，而不是全部
	PCI_Write(dwIOAddr+E3815_SMBUS_HOST_IOBASE,4);

    dwResult = PCI_Read(4);

	m_SMBusMapIoAddr = 	(uint16_t) dwResult & 0xFFF0 ;  // why???

	printk ("AutoDetect_IOBase : Intel E3815 SMBus Host I/O Base = 0x%08x 0x%08x\n",dwResult, m_SMBusMapIoAddr );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// VT8237 SMBus Mapping I/O read/write
//////////////////////////////////////////////////////////////////////
void SMBusIoWrite(uint8_t byteOffset,uint8_t byteData)
{
	//SetPortVal(m_SMBusMapIoAddr + byteOffset, byteData,1);
	//printk("INbytedata is %x\n",byteData);
	outb(byteData,m_SMBusMapIoAddr + byteOffset);
	udelay(10);
}

uint8_t SMBusIoRead(uint8_t byteOffset)
{
	uint32_t dwAddrVal;

	dwAddrVal=inb(m_SMBusMapIoAddr + byteOffset);
	udelay(10);
	return (uint8_t)(dwAddrVal & 0x0FF);
}
//////////////////////////////////////////////////////////////////////
// Normal SMBus Function
//////////////////////////////////////////////////////////////////////
void SMBus_Clear() //Need to planing for Multi-Controller Architecture leon 2011/5/10
{
    uint8_t IoState = 0x00;
    do{
        switch(devid){
        case VIA_VX900_BUS_CTRL_DEVICE_ID:
            SMBusIoWrite(SMBHSTSTS ,0xDF);//For VX900 SMBus HST_STS Clear Code
            break;
        default:
            SMBusIoWrite(SMBHSTSTS ,0xFF);//For Normal SMBus HST_STS Clear Code
            break;
        }
        udelay(1);
        IoState=SMBusIoRead(SMBHSTSTS);
        udelay(1);
        IoState=IoState & 0x01;
    }while(IoState != 0);
	SMBusIoWrite(SMBHSTDAT0,0x00);
}

int SMBus_Wait ()
{
	int		timeout = SMBUS_TIMEOUT;
	uint32_t	dwValue;

	while (timeout--)
	{
		// I/O Delay
		udelay(1);
		// Read Host Status Register
		dwValue = SMBusIoRead(SMBHSTSTS) & 0x00FF;


		if ( dwValue & SMBHSTSTS_INTR )
		{
			//printk("SMBus Action Completion! %x\n",dwValue);
			return SMBUS_OK;
		}

		if ( dwValue & SMBHSTSTS_FAILED )
		{
//			printk("SMBus Action FAILED! %x\n",dwValue);
			return SMBHSTSTS_FAILED;
		}

		if (dwValue & SMBHSTSTS_COLLISION)
		{
//			printk("SMBus Action COLLISION! %x\n",dwValue);
			return SMBHSTSTS_COLLISION;
		}

		if (dwValue & SMBHSTSTS_ERROR)
		{
//			printk("SMBus Action ERROR! %x\n",dwValue);
			return SMBHSTSTS_ERROR;
		}
	}

	return SMBUS_BUSY;
}

int SMBus_Busy ()
{
	if ( (SMBusIoRead(SMBHSTSTS) & SMBHSTSTS_BUSY ) == 1 )
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////
// Normal SMBus Function
//////////////////////////////////////////////////////////////////////
int SMBus_CheckDevice (uint8_t byteDeviceAddress)
{
	int ret;

	// Clear Status First !
	SMBus_Clear();

	if (SMBus_Busy())
		return SMBUS_BUSY;

	SMBusIoWrite(SMBHSTADD  , byteDeviceAddress  & ~1 );
	SMBusIoWrite(SMBHSTCNT  , SMBHSTCNT_START | SMBHSTCNT_QUICK);

	ret = SMBus_Wait();

	if (ret == SMBUS_OK)
		return TRUE;
	else
		return FALSE;
}

int SMBus_WriteByte (uint8_t byteSlave,uint8_t pCmd,uint8_t  pByte)
{
	SMBus_Clear();

	if (SMBus_Busy())
		return SMBUS_BUSY;

	SMBusIoWrite(SMBHSTADD  , byteSlave & ~1 );
	SMBusIoWrite(SMBHSTCMD  , pCmd );
	SMBusIoWrite(SMBHSTDAT0 , pByte );
	SMBusIoWrite(SMBHSTCNT  , SMBHSTCNT_START | SMBHSTCNT_BYTE);

	return (int)SMBus_Wait();
}

int SMBus_ReadByte (uint8_t byteSlave,uint8_t pCmd,uint8_t *pByte)
{
    int ret = 0;
	SMBus_Clear();

	if (SMBus_Busy())
		return SMBUS_BUSY;

	SMBusIoWrite(SMBHSTADD  , byteSlave | 1 );
	SMBusIoWrite(SMBHSTCMD  , pCmd );
	SMBusIoWrite(SMBHSTCNT  , SMBHSTCNT_START | SMBHSTCNT_BYTE);

	ret = SMBus_Wait();

	if (ret == SMBUS_OK)
	{
		*pByte = (uint8_t)SMBusIoRead(SMBHSTDAT0) &0xFF;
	}

	return ret;
}
