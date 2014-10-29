// SMBus.h: interface for the SMBus class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SMBUS_H
#define SMBUS_H

#define FALSE 0
#define TRUE 1

//----------------------------------------------------------------------------------
//	PCI Device Read/Write I/O
//----------------------------------------------------------------------------------
#define PCI_CONFIG_ADDR							0xCF8
#define PCI_CONFIG_DATA							0xCFC
//----------------------------------------------------------------------------------
//	Intel Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define INTEL_ICH4_VENDOR_ID					0x8086
#define INTEL_ICH4_DEVICE_ID					0x24C0

#define INTEL_ICH4_SMBUS_VENDOR_ID				0x8086
#define INTEL_ICH4_SMBUS_DEVICE_ID				0x24C3
//----------------------------------------------------------------------------------
#define       ICH4_SMBUS_HOST_IOBASE			0x20
//----------------------------------------------------------------------------------
#define       ICH4_SMBUS_HOST_CONFIGUREE		0x40
#define       ICH4_SMBUS_HOST_HST_EN			0x01
#define       ICH4_SMBUS_HOST_SMI_EN			0x02
#define       ICH4_SMBUS_HOST_I2C_EN			0x04

//----------------------------------------------------------------------------------
//	Intel 945 ICH7 Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define		INTEL_ICH7_VENDOR_ID				0x8086
#define		INTEL_ICH7_DEVICE_ID				0x2448

#define		INTEL_ICH7_SMBUS_VENDOR_ID			0x8086
#define		INTEL_ICH7_SMBUS_DEVICE_ID			0x27DA
//----------------------------------------------------------------------------------
#define		ICH7_SMBUS_HOST_IOBASE				0x20
//----------------------------------------------------------------------------------
#define		ICH7_SMBUS_HOST_CONFIGUREE			0x40
#define		ICH7_SMBUS_HOST_HST_EN				0x01
#define		ICH7_SMBUS_HOST_SMI_EN				0x02
#define		ICH7_SMBUS_HOST_I2C_EN				0x04

//----------------------------------------------------------------------------------
//	Intel 510&525 ICH8 Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define		INTEL_ICH8_VENDOR_ID				0x8086
#define		INTEL_ICH8_DEVICE_ID				0x244E

#define		INTEL_ICH8_SMBUS_VENDOR_ID			0x8086
#define		INTEL_ICH8_SMBUS_DEVICE_ID			0x283E
//----------------------------------------------------------------------------------
#define		ICH8_SMBUS_HOST_IOBASE				0x20
//----------------------------------------------------------------------------------
#define		ICH8_SMBUS_HOST_CONFIGUREE			0x40
#define		ICH8_SMBUS_HOST_HST_EN				0x01
#define		ICH8_SMBUS_HOST_SMI_EN				0x02
#define		ICH8_SMBUS_HOST_I2C_EN				0x04

//----------------------------------------------------------------------------------
//	Intel 275 ICH10 Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define		INTEL_ICH10_VENDOR_ID				0x8086
#define		INTEL_ICH10_DEVICE_ID				0x244E

#define		INTEL_ICH10_SMBUS_VENDOR_ID			0x8086
#define		INTEL_ICH10_SMBUS_DEVICE_ID			0x3A30
//----------------------------------------------------------------------------------
#define		ICH10_SMBUS_HOST_IOBASE				0x20
//----------------------------------------------------------------------------------
#define		ICH10_SMBUS_HOST_CONFIGUREE			0x40
#define		ICH10_SMBUS_HOST_HST_EN				0x01
#define		ICH10_SMBUS_HOST_SMI_EN				0x02
#define		ICH10_SMBUS_HOST_I2C_EN				0x04
//----------------------------------------------------------------------------------
//	Intel 650 QM67(6 series) Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define		INTEL_QM67_VENDOR_ID				0x8086
#define		INTEL_QM67_DEVICE_ID				0x1C10

#define		INTEL_QM67_SMBUS_VENDOR_ID			0x8086
#define		INTEL_QM67_SMBUS_DEVICE_ID			0x1C22
//----------------------------------------------------------------------------------
#define		QM67_SMBUS_HOST_IOBASE				0x20
//----------------------------------------------------------------------------------
#define		QM67_SMBUS_HOST_CONFIGUREE			0x40
#define		QM67_SMBUS_HOST_HST_EN				0x01
#define		QM67_SMBUS_HOST_SMI_EN				0x02
#define		QM67_SMBUS_HOST_I2C_EN				0x04
//----------------------------------------------------------------------------------
//	Intel 770 QM77(7 series) Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define		INTEL_QM77_SMBUS_VENDOR_ID			0x8086
#define		INTEL_QM77_SMBUS_DEVICE_ID			0x1E22
//----------------------------------------------------------------------------------
#define		QM77_SMBUS_HOST_IOBASE				0x20
//----------------------------------------------------------------------------------
#define		QM77_SMBUS_HOST_CONFIGUREE			0x40
#define		QM77_SMBUS_HOST_HST_EN				0x01
#define		QM77_SMBUS_HOST_SMI_EN				0x02
#define		QM77_SMBUS_HOST_I2C_EN				0x04

//----------------------------------------------------------------------------------
//	VIA VT8237 Vendor ID & Device ID
//----------------------------------------------------------------------------------
#define VIA_VT8237_BUS_CTRL_VENDOR_ID			0x1106
#define VIA_VT8237_BUS_CTRL_DEVICE_ID			0x3227
//----------------------------------------------------------------------------------
#define		VT8237_SMBUS_HOST_IOBASE			0xD0
//----------------------------------------------------------------------------------
#define		VT8237_SMBUS_HOST_CONFIGURE			0xD2
#define		VT8237_SMBUS_HOST_CONTROLER_ENABLE	0x01
#define		VT8237_SMBUS_HOST_INTERRUPT_ENABLE	0x02
#define		VT8237_SMBUS_HOST_INTERRUPT_TYPE	0x08
//----------------------------------------------------------------------------------
#define VIA_CX700M_BUS_CTRL_VENDOR_ID			0x1106
#define VIA_CX700M_BUS_CTRL_DEVICE_ID			0x8324
//----------------------------------------------------------------------------------
#define		CX700M_SMBUS_HOST_IOBASE				0xD0
//----------------------------------------------------------------------------------
#define		CX700M_SMBUS_HOST_CONFIGURE			0xD2
#define		CX700M_SMBUS_HOST_CONTROLER_ENABLE	0x01
#define		CX700M_SMBUS_HOST_INTERRUPT_ENABLE	0x02
#define		CX700M_SMBUS_HOST_INTERRUPT_TYPE	0x08
#define		CX700M_SMBUS_HOST_CLOCK				0x04
//----------------------------------------------------------------------------------
//	VIA VX900 Vendor ID / Devices ID & Bus-Specific configuration register
//----------------------------------------------------------------------------------
#define VIA_VX900_BUS_CTRL_VENDOR_ID			0x1106
#define VIA_VX900_BUS_CTRL_DEVICE_ID			0x8410
#define	VX900_SMBUS_HOST_IOBASE					0xD0
#define	VX900_SMBUS_HOST_CONFIGURE				0xD2
#define	VX900_SMBUS_HOST_CONTROLER_ENABLE		0x01
//----------------------------------------------------------------------------------
//	Intel Bay Trail-l E3815 Vendor ID & Device ID
// 使用这个
//----------------------------------------------------------------------------------
#define     INTEL_E3815_SMBUS_VENDOR_ID         0x8086
#define     INTEL_E3815_SMBUS_DEVICE_ID         0x0F12
//----------------------------------------------------------------------------------
#define     E3815_SMBUS_HOST_IOBASE             0x20
//----------------------------------------------------------------------------------
#define     E3815_SMBUS_HOST_CONFIGUREE         0x40
#define		E3815_SMBUS_HOST_HST_EN				0x01
#define		E3815_SMBUS_HOST_SMI_EN				0x02
#define		E3815_SMBUS_HOST_I2C_EN				0x04

//-----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//		Register of VIA & Intel
//----------------------------------------------------------------------------------
#define		SMBHSTSTS			0x00	// SMBus Host  Status Register Offset
#define		SMBHSTSTS_BUSY		0x01	// SMBus Host -> 0000-0001 Busy
#define		SMBHSTSTS_INTR		0x02	// SMBus Host -> 0000-0010 Interrupt / complection
#define		SMBHSTSTS_ERROR		0x04	// SMBus Host -> 0000-0100 Error
#define		SMBHSTSTS_COLLISION	0x08	// SMBus Host -> 0000-1000 Collistion
#define		SMBHSTSTS_FAILED	0x10	// SMBus Host -> 0001-0000 Failed
//----------------------------------------------------------------------------------
#define		SMBHSTCNT			0x02	// SMBus Host Contorl Register Offset
#define		SMBHSTCNT_KILL		0x02	// SMBus Host Contorl -> 0000 0010 Kill
#define		SMBHSTCNT_QUICK		0x00	// SMBus Host Contorl -> 0000 0000 quick (default)
#define		SMBHSTCNT_SENDRECV	0x04	// SMBus Host Contorl -> 0000 0100 Byte
#define		SMBHSTCNT_BYTE		0x08	// SMBus Host Contorl -> 0000 1000 Byte Data
#define		SMBHSTCNT_WORD		0x0c	// SMBus Host Contorl -> 0000 1100 Word Data
#define		SMBHSTCNT_BLOCK		0x14	// SMBus Host Contorl -> 0001 0100 Block
#define		SMBHSTCNT_START		0x40	// SMBus Host Contorl -> 0100 0000 Start
//----------------------------------------------------------------------------------
#define	SMBHSTCMD				0x03	// SMBus Host Command		Register Offset
#define	SMBHSTADD				0x04	// SMBus Host Address		Register Offset
#define	SMBHSTDAT0				0x05	// SMBus Host Data0			Register Offset
										// SMBus Host Block Counter	Register Offset
#define	SMBHSTDAT1				0x06	// SMBus Host Data1			Register Offset
#define	SMBBLKDAT				0x07	// SMBus Host Block	Data	Register Offset
//----------------------------------------------------------------------------------
//		Register of VIA only
//----------------------------------------------------------------------------------
#define	SMBHSLVSTS			0x01		// SMBus Slave Status Register Offset
//----------------------------------------------------------------------------------
#define	SMBSLVCNT			0x08		// SMBus Slave  Control
#define	SMBSHDWCMD			0x09		// SMBus Shadow Command
#define	SMBSLVEVT			0x0a		// SMBus Slave  Event
#define	SMBSLVDAT			0x0c		// SMBus Slave  Data
//----------------------------------------------------------------------------------
//      SMBus Bus Status Code
//----------------------------------------------------------------------------------
#define SMBUS_OK			0x0			// SMBUS OK
#define SMBUS_BUSY			0x1			// SMBUS BUSY
#define SMBUS_INT			0x2			// SMBUS INTR
#define SMBUS_ERROR			0x4			// SMBUS ERROR
//----------------------------------------------------------------------------------
#define	SMBUS_TIMEOUT		100
//----------------------------------------------------------------------------------
extern uint16_t m_SMBusMapIoAddr;

uint32_t	PCI_Read (int   size_t);
void	PCI_Write(uint32_t dwDataVal,int size_t);

//int	PCI_AutoDetect(void);
int PCI_DetectBaytrail(void);

int	Get_VT8237_SMBusIoAddr(uint32_t dwIOAddr);
int	Get_CX700M_SMBusIoAddr(uint32_t dwIOAddr);
int	Get_ICH4_SMBusIoAddr(uint32_t dwIOAddr);
int	Get_ICH7_SMBusIoAddr(uint32_t dwIOAddr);
int	Get_ICH8_SMBusIoAddr(uint32_t dwIOAddr);
int	Get_VX900_SMBusIoAddr(uint32_t dwIOAddr); //Add for VX900 SMbus controller
int    Get_ICH10_SMBusIoAddr(uint32_t dwIOAddr);
int    Get_QM67_SMBusIoAddr(uint32_t dwIOAddr);
int    Get_QM77_SMBusIoAddr(uint32_t dwIOAddr);
int    Get_E3815_SMBusIoAddr(uint32_t dwIOAddr);
uint8_t	SMBusIoRead (uint8_t byteOffset);
void	SMBusIoWrite(uint8_t byteOffset,uint8_t byteData);

void	SMBus_Clear(void);
int		SMBus_Wait (void);
int	SMBus_Busy (void);

int	SMBus_CheckDevice	(uint8_t byteDeviceAddress);
int		SMBus_WriteByte		(uint8_t byteSlave,uint8_t pCmd,uint8_t  pByte);
int		SMBus_ReadByte		(uint8_t byteSlave,uint8_t pCmd,uint8_t *pByte);

#endif