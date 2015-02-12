/*
 * 	The Virtual Support Module(VSM) for virtulize the PCI configure  
 * 	space. so user can access the PCI configure space directly as
 *	a normal multi-function PCI device which following the PCI-2.2 spec.
 *
 * Author : jlliu <liujl@lemote.com>
 * Modified by Yanhua <yanh@lemote.com> to port to U-Boot
 */

/* FOR INCLUDING THE RX REGISTERS */
#define	SB_RX
// NOTE THE IDE DMA OPERATION, BASE ADDR CONFIG RIGHT OR WRONG???

#include <common.h>
#include <pci.h>
#include <asm/addrspace.h>

#include "bonito.h"
#include "cs5536.h"
#include "cs5536_pci.h"
/*
 * rdmsr : read 64bits data from the cs5536 MSR register
 */
void _rdmsr(u32 msr, u32 *hi, u32 *lo)
{
	u32 type = 0x00000;
	u32 addr;
	
	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xf4;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*(volatile u32 *)((u32 )(BONITO_PCICFG) | (addr & 0xfffc)) = msr;
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xf8;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*lo = *(volatile u32 *)((u32)(BONITO_PCICFG)| (addr & 0xfffc));
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xfc;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*hi = *(volatile u32 *)((u32)(BONITO_PCICFG) | (addr & 0xfffc));
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	return;	
}

/*
 * wrmsr : write 64bits data to the cs5536 MSR register
 */
void _wrmsr(u32 msr, u32 hi, u32 lo)
{
	u32 type = 0x00000;
	u32 addr;
	
	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xf4;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*(volatile u32 *)((u32)(BONITO_PCICFG)| (addr & 0xfffc)) = msr;
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xf8;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*(volatile u32 *)((u32)(BONITO_PCICFG) | (addr & 0xfffc)) = lo;
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	addr = (PCI_BUS_CS5536 << 16) | (1 << (PCI_IDSEL_CS5536 + 11) ) | (0 << 8) | 0xfc;
	BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT | PCI_STATUS_REC_TARGET_ABORT;
	BONITO_PCIMAP_CFG = (addr >> 16) | type;
	*(volatile u32 *)((u32)(BONITO_PCICFG) | (addr & 0xfffc)) = hi;
	if (BONITO_PCICMD & PCI_STATUS_REC_MASTER_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_MASTER_ABORT;
	}
	if (BONITO_PCICMD & PCI_STATUS_REC_TARGET_ABORT) {
		BONITO_PCICMD |= PCI_STATUS_REC_TARGET_ABORT;
	}

	return;	
}

/*
 * divil_lbar_enable_disable : enable/disable the divil module bar space.
 */
static void divil_lbar_enable_disable(int enable)
{
	u32 hi, lo;
	
	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_IRQ), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_IRQ), hi, lo);

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_SMB), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_SMB), hi, lo);

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_GPIO), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_GPIO), hi, lo);

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), hi, lo);

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_PMS), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_PMS), hi, lo);

	_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_ACPI), &hi, &lo);
	if(enable)
		hi |= 0x01;
	else
		hi &= ~0x01;
	_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_ACPI), hi, lo);

	return;
}

/******************************************************************************/

/*
 * The following functions are not implemented in  pmon.
 */
static void pci_flash_write_reg(int reg, u32 value)
{
	return;
}

static u32 pci_flash_read_reg(int reg)
{
	return 0xffffffff;
}

/*******************************************************************************/

/*
 * isa_write : isa write transfering.
 * WE assume that the ISA is not the BUS MASTER. 
 */
static void pci_isa_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	u32 temp;
	u32 softcom;

	_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
	softcom = lo;
	
	switch(reg){
		case PCI_COMMAND:
			// command
			if( value & PCI_COMMAND_IO) {
				divil_lbar_enable_disable(1);
			}else{
				divil_lbar_enable_disable(0);
			}
			/* BUS MASTER : is it 0 for SB???  yes...*/
			/* PER response enable or disable. */
			if( value & PCI_COMMAND_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				lo |= SB_PARE_ERR_EN;
				_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);			
			}else{
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				lo &= ~SB_PARE_ERR_EN;
				_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);			
			}
			// status
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			temp = lo & 0x0000ffff;
			if( ((value >> 16) & PCI_STATUS_SIG_TARGET_ABORT) && 
				(lo & SB_SYSE_ERR_EN) ){
				temp |= SB_SYSE_ERR_FLAG;
			}
			if (((value >> 16) & PCI_STATUS_REC_TARGET_ABORT) &&
				(lo & SB_TAR_ERR_EN) ){
				temp |= SB_TAR_ERR_FLAG;
			}
			if (((value >> 16) & PCI_STATUS_REC_MASTER_ABORT) &&
				(lo & SB_MAR_ERR_EN) ){
				temp |= SB_MAR_ERR_FLAG;
			}
			if (((value >> 16) & PCI_STATUS_DETECTED_PARITY) &&
				(lo & SB_PARE_ERR_EN) ){
				temp |= SB_PARE_ERR_FLAG; 
			}
			lo = temp;
			_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
			break;
		case PCI_CACHE_LINE_SIZE:
			value &= 0x0000ff00;
			_rdmsr(SB_MSR_REG(SB_CTRL), &hi, &lo);
			hi &= 0xffffff00;
			hi |= (value >> 8);
			_wrmsr(SB_MSR_REG(SB_CTRL), hi, lo);
			break;
		case PCI_BASE_ADDRESS_0:
			if (value == PCI_BAR_RANGE_MASK) {
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_SMB_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				hi = 0x0000f001;
				lo = value & 0x0000fff8;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_SMB), hi, lo);
#ifdef	SB_RX
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_SMB_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_SMB_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R0), hi, lo);
#endif				
			}
			break;
		case PCI_BASE_ADDRESS_1:
			if (value == PCI_BAR_RANGE_MASK) {
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_GPIO_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				hi = 0x0000f001;
				lo = value & 0x0000ff00;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_GPIO), hi, lo);
#ifdef	SB_RX				
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_GPIO_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_GPIO_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R1), hi, lo);
#endif				
			}
			break;
		case PCI_BASE_ADDRESS_2:
			if (value == PCI_BAR_RANGE_MASK) {
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_MFGPT_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				hi = 0x0000f001;
				lo = value & 0x0000ffc0;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), hi, lo);
#ifdef SB_RX				
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_MFGPT_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_MFGPT_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R2), hi, lo);
#endif				
			}
			break;
		case PCI_BASE_ADDRESS_3:
			if (value == PCI_BAR_RANGE_MASK) {
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_IRQ_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				hi = 0x0000f001;
				lo = value & 0x0000ffe0;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_IRQ), hi, lo);
#ifdef	SB_RX				
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_IRQ_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_IRQ_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R3), hi, lo);
#endif				
			}
			break;
		case PCI_BASE_ADDRESS_4:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_PMS_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				hi = 0x0000f001;
				lo = value & 0x0000ff80;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_PMS), hi, lo);
#ifdef 	SB_RX				
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_PMS_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_PMS_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R4), hi, lo);
#endif				
			}
			break;
		case PCI_BASE_ADDRESS_5:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_ACPI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else {
				hi = 0x0000f001;
				lo = value & 0x0000ffe0;
				_wrmsr(DIVIL_MSR_REG(DIVIL_LBAR_ACPI), hi, lo);
#ifdef 	SB_RX				
				hi = ((value & 0x000ffffc) << 12) | ((CS5536_ACPI_LENGTH - 4) << 12) | 0x01;
				//hi = ((value & 0x000fffff) << 12) | ((CS5536_ACPI_LENGTH) << 12) | 0x03;
				lo = ((value & 0x000ffffc) << 12) | 0x01;
				_wrmsr(SB_MSR_REG(SB_R5), hi, lo);
#endif				
			}
			break;
		default :
			break;			
	}
	
	return;
}

/*
 * isa_read : isa read transfering.
 * we assume that the ISA is not the BUS MASTER. 
 */
static u32 pci_isa_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = 0;
			_rdmsr(SB_MSR_REG(SB_CAP), &hi, &lo);	/* jlliu : should get the correct value. */
			if( (lo != 0x0) && (lo != 0xffffffff) ){
				conf_data = (CS5536_ISA_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			}
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND			
			_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_SMB), &hi, &lo);
			if(hi & 0x01){
				conf_data |= PCI_COMMAND_IO;
			}
			conf_data |= PCI_COMMAND_SPECIAL;
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_EN){
				conf_data |= PCI_COMMAND_PARITY;
			}else{
				conf_data &= ~PCI_COMMAND_PARITY;
			}
			// STATUS	
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_DEVSEL_MEDIUM << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16); 
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_SYSE_ERR_FLAG)
				conf_data |= (PCI_STATUS_SIG_TARGET_ABORT << 16);
			if(lo & SB_TAR_ERR_FLAG)
				conf_data |= (PCI_STATUS_REC_TARGET_ABORT << 16);
			if(lo & SB_MAR_ERR_FLAG)
				conf_data |= (PCI_STATUS_REC_MASTER_ABORT << 16);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_DETECTED_PARITY << 16);
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(GLCP_MSR_REG(GLCP_CHIP_REV_ID), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_ISA_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			_rdmsr(SB_MSR_REG(SB_CTRL), &hi, &lo);
			hi &= 0x000000f8;
			conf_data = ( 0 << 24) | (0x80 << 16) |
				(hi << 8) | PCI_NORMAL_CACHE_LINE_SIZE;
			break;
		/*
		 * we only use the LBAR of DIVIL, no RCONF used. 
		 * all of them are IO space.
		 */
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_SMB_FLAG){
				conf_data = CS5536_SMB_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_SMB_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_SMB), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_1:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_GPIO_FLAG){
				conf_data = CS5536_GPIO_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_GPIO_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_GPIO), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_2:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_MFGPT_FLAG){
				conf_data = CS5536_MFGPT_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_MFGPT_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			} else {
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_MFGPT), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_3:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if (lo & SOFT_BAR_IRQ_FLAG) {
				conf_data = CS5536_IRQ_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_IRQ_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_IRQ), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;	
		case PCI_BASE_ADDRESS_4:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_PMS_FLAG){
				conf_data = CS5536_PMS_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_PMS_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_PMS), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_5:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_ACPI_FLAG){
				conf_data = CS5536_ACPI_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_ACPI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(DIVIL_MSR_REG(DIVIL_LBAR_ACPI), &hi, &lo);
				conf_data = lo & 0x0000ffff;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_ISA_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS :
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				(0 << 8) | 0x00;
			break;
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

/*
 * ide_write : ide write transfering
 */
static void pci_ide_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	
	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MASTER){
				_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
				lo |= (0x03 << 4);
				_wrmsr(GLIU_MSR_REG(GLIU_PAE), hi, lo);			
			}else{
				_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
				lo &= ~(0x03 << 4);
				_wrmsr(GLIU_MSR_REG(GLIU_PAE), hi, lo);	
			}
			// STATUS
			if((value >> 16) & PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_CACHE_LINE_SIZE:
			value &= 0x0000ff00;
			_rdmsr(SB_MSR_REG(SB_CTRL), &hi, &lo);
			hi &= 0xffffff00;
			hi |= (value >> 8);
			_wrmsr(SB_MSR_REG(SB_CTRL), hi, lo);
			break;
		case PCI_BASE_ADDRESS_4:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_IDE_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if(value & 0x01){
				hi = 0x00000000;
				//lo = ((value & 0x0fffffff) << 4) | 0x001;
				lo = (value & 0xfffffff0) | 0x1;
				_wrmsr(IDE_MSR_REG(IDE_IO_BAR), hi, lo);
#ifdef SB_RX				
				hi = 0x60000000 | ((value & 0x000ff000) >> 12);
				lo = 0x000ffff0 | ((value & 0x00000ffc) << 20);
				_wrmsr(GLIU_MSR_REG(GLIU_IOD_BM2), hi, lo);
#endif				
			}
			break;
		case PCI_IDE_CFG_REG :
			if(value == CS5536_IDE_FLASH_SIGNATURE){
				_rdmsr(DIVIL_MSR_REG(DIVIL_BALL_OPTS), &hi, &lo);
				lo |= 0x01;
				_wrmsr(DIVIL_MSR_REG(DIVIL_BALL_OPTS), hi, lo);
			}else{
				hi = 0;
				lo = value;
				_wrmsr(IDE_MSR_REG(IDE_CFG), hi, lo);			
			}
			break;
		case PCI_IDE_DTC_REG :
			hi = 0;
			lo = value;
			_wrmsr(IDE_MSR_REG(IDE_DTC), hi, lo);
			break;
		case PCI_IDE_CAST_REG :
			hi = 0;
			lo = value;
			_wrmsr(IDE_MSR_REG(IDE_CAST), hi, lo);
			break;
		case PCI_IDE_ETC_REG :
			hi = 0;
			lo = value;
			_wrmsr(IDE_MSR_REG(IDE_ETC), hi, lo);
			break;
		case PCI_IDE_PM_REG :
			hi = 0;
			lo = value;
			_wrmsr(IDE_MSR_REG(IDE_INTERNAL_PM), hi, lo);
			break;
		default :
			break;			
	}
	
	return;
}

/*
 * ide_read : ide read tranfering.
 */
static u32 pci_ide_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = 0;
			_rdmsr(IDE_MSR_REG(IDE_CAP), &hi, &lo);
			if( (lo != 0x0) && (lo != 0xffffffff) ){
				conf_data = (CS5536_IDE_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			}
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND
			_rdmsr(IDE_MSR_REG(IDE_IO_BAR), &hi, &lo);
			if(lo & 0xfffffff0)
				conf_data |= PCI_COMMAND_IO;
			_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
			if( (lo & 0x30) == 0x30 )
				conf_data |= PCI_COMMAND_MASTER;
			/* conf_data |= PCI_COMMAND_BACKTOBACK_ENABLE??? HOW TO GET..*/
			//STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= PCI_STATUS_DEVSEL_MEDIUM;
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(IDE_MSR_REG(IDE_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_IDE_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			_rdmsr(SB_MSR_REG(SB_CTRL), &hi, &lo);
			hi &= 0x000000f8;
			conf_data = ( 0 << 24) | (PCI_HEADER_TYPE_NORMAL << 16) |
				(hi << 8) | PCI_NORMAL_CACHE_LINE_SIZE;
			break;
		case PCI_BASE_ADDRESS_0:
			conf_data = 0x00000000;
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x00000000;
			break;
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x00000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x00000000;
			break;
		case PCI_BASE_ADDRESS_4:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_IDE_FLAG){
				conf_data = CS5536_IDE_RANGE | PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_IDE_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(IDE_MSR_REG(IDE_IO_BAR), &hi, &lo);
				//conf_data = lo >> 4;
				conf_data = lo & 0xfffffff0;
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x00000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_IDE_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS:
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				( 0 << 8) | 0x00;
			break;
		case PCI_IDE_CFG_REG :
			_rdmsr(IDE_MSR_REG(IDE_CFG), &hi, &lo);
			conf_data = lo;
			break;
		case PCI_IDE_DTC_REG :
			_rdmsr(IDE_MSR_REG(IDE_DTC), &hi, &lo);
			conf_data = lo;
			break;
		case PCI_IDE_CAST_REG :
			_rdmsr(IDE_MSR_REG(IDE_CAST), &hi, &lo);
			conf_data = lo;
			break;
		case PCI_IDE_ETC_REG :
			_rdmsr(IDE_MSR_REG(IDE_ETC), &hi, &lo);
			conf_data = lo;
		case PCI_IDE_PM_REG :
			_rdmsr(IDE_MSR_REG(IDE_INTERNAL_PM), &hi, &lo);
			conf_data = lo;
			break;
			
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

static void pci_acc_write_reg(int reg, u32 value)
{
	u32 hi, lo;

	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MASTER){
				_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
				lo |= (0x03 << 8);
				_wrmsr(GLIU_MSR_REG(GLIU_PAE), hi, lo);			
			}else{
				_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
				lo &= ~(0x03 << 8);
				_wrmsr(GLIU_MSR_REG(GLIU_PAE), hi, lo);	
			}
			// STATUS
			if((value >> 16) & PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_BASE_ADDRESS_0:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_ACC_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if( value & 0x01 ){
				value &= 0xfffffffc;
				hi = 0xA0000000 | ((value & 0x000ff000) >> 12);
				lo = 0x000fff80 | ((value & 0x00000fff) << 20);
				_wrmsr(GLIU_MSR_REG(GLIU_IOD_BM1), hi, lo);
			}
			break;
		default :
			break;			
	}

	return;
}

static u32 pci_acc_read_reg(int reg)
{
	u32 hi, lo;
	u32 conf_data;

	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = (CS5536_ACC_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			break;
		case PCI_COMMAND:
			
			conf_data = 0;
			// COMMAND
			_rdmsr(GLIU_MSR_REG(GLIU_IOD_BM1), &hi, &lo);
			if( ( (lo & 0xfff00000) || (hi & 0x000000ff) ) 
					&& ((hi & 0xf0000000) == 0xa0000000) )
				conf_data |= PCI_COMMAND_IO;
			_rdmsr(GLIU_MSR_REG(GLIU_PAE), &hi, &lo);
			if( (lo & 0x300) == 0x300 )
				conf_data |= PCI_COMMAND_MASTER;
			/* conf_data |= PCI_COMMAND_BACKTOBACK_ENABLE??? HOW TO GET..*/
			//STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= PCI_STATUS_DEVSEL_MEDIUM;
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(ACC_MSR_REG(ACC_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_ACC_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			conf_data = ( 0 << 24) | ( 0 << 16) |
				( 0x40 << 8) | 0x08;
			break;
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_ACC_FLAG){
				conf_data = CS5536_ACC_RANGE |PCI_BASE_ADDRESS_SPACE_IO;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_ACC_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(GLIU_MSR_REG(GLIU_IOD_BM1), &hi, &lo);
				conf_data = (hi & 0x000000ff) << 12;
				conf_data |= (lo & 0xfff00000) >> 20; 
				conf_data |= 0x01;
				conf_data &= ~0x02;
			}
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x000000;
			break;		
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_4:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_ACC_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS:
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_USB_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				(0x01 << 8) | (0x00);

			break;
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

/*
 * ohci_write : ohci write tranfering.
 */
static void pci_ohci_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	
	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MASTER){
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				hi |= (1 << 2);
				_wrmsr(USB_MSR_REG(USB_OHCI), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				hi &= ~(1 << 2);
				_wrmsr(USB_MSR_REG(USB_OHCI), hi, lo);
			}
			if (value & PCI_COMMAND_MEMORY) {
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				hi |= (1 << 1);
				_wrmsr(USB_MSR_REG(USB_OHCI), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				hi &= ~(1 << 1);
				_wrmsr(USB_MSR_REG(USB_OHCI), hi, lo);				
			}
			// STATUS
			if((value >> 16)& PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_BASE_ADDRESS_0:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_OHCI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if( (value & 0x01) == 0x00 ){
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				//lo = (value & 0xffffff00) << 8;
				lo = value;
				_wrmsr(USB_MSR_REG(USB_OHCI), hi, lo);
				
				hi = 0x40000000 | (value&0xff000000) >> 24;
				lo = 0x000fffff | (value&0x00fff000) << 8;
				_wrmsr(GLIU_MSR_REG(GLIU_P2D_BM3), hi, lo);
			}
			break;
		case PCI_INTERRUPT_LINE:
			value &= 0x000000ff;
			break;
		case 0x40:
			break;
		default :
			break;			
	}
	
	return;
}

/*
 * ohci_read : ohci read transfering.
 */
static u32 pci_ohci_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = 0;
			_rdmsr(USB_MSR_REG(USB_CAP), &hi, &lo);
			if( (lo != 0x0) && (lo != 0xffffffff) ){
				conf_data = (CS5536_OHCI_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			}
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND
			_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
			if(hi & 0x04)
				conf_data |= PCI_COMMAND_MASTER;
			if(hi & 0x02)
				conf_data |= PCI_COMMAND_MEMORY;
			// STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= (PCI_STATUS_DEVSEL_MEDIUM << 16);
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(USB_MSR_REG(USB_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_OHCI_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			conf_data = ( 0 << 24) | (PCI_HEADER_TYPE_NORMAL<< 16) |
				(0x00 << 8) |  0x8;
			break;
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_OHCI_FLAG){
				conf_data = CS5536_OHCI_RANGE | PCI_BASE_ADDRESS_SPACE_MEMORY;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_OHCI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
				//conf_data = lo >> 8;
				conf_data = lo & 0xffffff00;
				conf_data &= ~0x0000000f; // 32bit mem
			}
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x000000;
			break;		
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_4:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_OHCI_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS:
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_USB_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				( 4 << 8) | 0x00;
			break;
		case 0x40:
			conf_data = 0;
			break;
		case 0x50 :
			_rdmsr(GLIU_MSR_REG(GLIU_P2D_BM1), &hi, &lo);
			//printf("p2d bm1 : hi = 0x%x, lo = 0x%x\n", hi, lo);
			_rdmsr(USB_MSR_REG(USB_OHCI), &hi, &lo);
			//printf("usb ohci : hi = 0x%x, lo = 0x%x\n", hi, lo);
			conf_data = 0;
			break;		
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

#ifdef	TEST_CS5536_USE_EHCI
static void pci_ehci_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	
	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MASTER){
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				hi |= (1 << 2);
				_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				hi &= ~(1 << 2);
				_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
			}
			if(value & PCI_COMMAND_MEMORY){
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				hi |= (1 << 1);
				_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				hi &= ~(1 << 1);
				_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);				
			}
			// STATUS
			if((value >> 16) & PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_BASE_ADDRESS_0:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_EHCI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if( (value & 0x01) == 0x00 ){
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				lo = value;
				_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
				
				value &= 0xfffffff0;
				hi = 0x40000000 | ((value & 0xff000000) >> 24);
				lo = 0x000fffff | ((value & 0x00fff000) << 8);
				_wrmsr(GLIU_MSR_REG(GLIU_P2D_BM4), hi, lo);
			}
			break;
		case PCI_EHCI_LEGSMIEN_REG :
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			hi &= 0x003f0000;
			hi |= (value & 0x3f) << 16;
			_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
			break;
		case PCI_EHCI_FLADJ_REG :
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			hi &= ~0x00003f00;
			hi |= value & 0x00003f00;
			_wrmsr(USB_MSR_REG(USB_EHCI), hi, lo);
			break;
		default :
			break;			
	}
	
	return;
}

static u32 pci_ehci_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = (CS5536_EHCI_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			if(hi & 0x04)
				conf_data |= PCI_COMMAND_MASTER;
			if(hi & 0x02)
				conf_data |= PCI_COMMAND_MEMORY;
			// STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= (PCI_STATUS_DEVSEL_MEDIUM << 16);
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(USB_MSR_REG(USB_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_EHCI_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			conf_data = ( 0 << 24) | (PCI_HEADER_TYPE_NORMAL << 16) |
				(0x40 << 8) | 0x08;
			break;
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_EHCI_FLAG){
				conf_data = CS5536_EHCI_RANGE | PCI_BASE_ADDRESS_SPACE_MEMORY;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_EHCI_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
				conf_data = lo & 0xffffff00;
				conf_data &= ~0x0000000f; // 32bit mem
			}
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x000000;
			break;		
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_4:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_EHCI_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS :
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_USB_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				(0x01 << 8) | 0x00;
			break;
		case PCI_EHCI_LEGSMIEN_REG :
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			conf_data = (hi & 0x003f0000) >> 16;
			break;
		case PCI_EHCI_LEGSMISTS_REG :
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			conf_data = (hi & 0x3f000000) >> 24;
			break;
		case PCI_EHCI_FLADJ_REG :
			_rdmsr(USB_MSR_REG(USB_EHCI), &hi, &lo);
			conf_data = hi & 0x00003f00;
			break;
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}
#else
static void pci_ehci_write_reg(int reg, u32 value)
{
	return;
}

static u32 pci_ehci_read_reg(int reg)
{
	return  0xffffffff;
}


#endif

#ifdef	TEST_CS5536_USE_UDC
static void pci_udc_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	
	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MASTER){
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				hi |= (1 << 2);
				_wrmsr(USB_MSR_REG(USB_UDC), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				hi &= ~(1 << 2);
				_wrmsr(USB_MSR_REG(USB_UDC), hi, lo);
			}
			if(value & PCI_COMMAND_MEMORY){
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				hi |= (1 << 1);
				_wrmsr(USB_MSR_REG(USB_UDC), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				hi &= ~(1 << 1);
				_wrmsr(USB_MSR_REG(USB_UDC), hi, lo);				
			}
			// STATUS
			if((value >> 16) & PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_BASE_ADDRESS_0:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_UDC_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if( (value & 0x01) == 0x00 ){
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				lo = value;
				_wrmsr(USB_MSR_REG(USB_UDC), hi, lo);
				
				value &= 0xfffffff0;
				hi = 0x40000000 | ((value & 0xff000000) >> 24);
				lo = 0x000fffff | ((value & 0x00fff000) << 8);
				_wrmsr(GLIU_MSR_REG(GLIU_P2D_BM0), hi, lo);
			}
			break;
		default :
			break;			
	}
	
	return;
}

static u32 pci_udc_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = (CS5536_UDC_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND
			_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
			if(hi & 0x04)
				conf_data |= PCI_COMMAND_MASTER;
			if(hi & 0x02)
				conf_data |= PCI_COMMAND_MEMORY;
			// STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= (PCI_STATUS_DEVSEL_MEDIUM << 16);
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(USB_MSR_REG(USB_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_UDC_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			conf_data = (0 << 24) | (PCI_NORMAL_HEADER_TYPE << 16) |
				(0x00 << 8) | 0x08;
			break;
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_UDC_FLAG){
				conf_data = CS5536_UDC_RANGE | PCI_BASE_ADDRESS_SPACE_MEMORY;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_UDC_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_UDC), &hi, &lo);
				conf_data = lo & 0xfffff000;
				conf_data &= ~0x0000000f; // 32bit mem
			}
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x000000;
			break;		
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_4:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_UDC_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS :
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_USB_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				(0x01 << 8) | 0x00;
			break;
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

#else	/* TEST_CS5536_USE_UDC */

static void pci_udc_write_reg(int reg, u32 value)
{
	return;
}

static u32 pci_udc_read_reg(int reg)
{
	return  0xffffffff;
}

#endif	/* TEST_CS5536_USE_UDC */


#ifdef	TEST_CS5536_USE_OTG
static void pci_otg_write_reg(int reg, u32 value)
{
	u32 hi, lo;
	
	switch(reg){
		case PCI_COMMAND:
			// COMMAND
			if(value & PCI_COMMAND_MEMORY){
				_rdmsr(USB_MSR_REG(USB_OTG), &hi, &lo);
				hi |= (1 << 1);
				_wrmsr(USB_MSR_REG(USB_OTG), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_OTG), &hi, &lo);
				hi &= ~(1 << 1);
				_wrmsr(USB_MSR_REG(USB_OTG), hi, lo);				
			}
			// STATUS
			if((value >> 16) & PCI_STATUS_PARITY){
				_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
				if(lo & SB_PARE_ERR_FLAG){
					lo = (lo & 0x0000ffff) | SB_PARE_ERR_FLAG;
					_wrmsr(SB_MSR_REG(SB_ERROR), hi, lo);
				}				
			}
			break;
		case PCI_BASE_ADDRESS_0:
			if(value == PCI_BAR_RANGE_MASK){
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo |= SOFT_BAR_OTG_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else if( (value & 0x01) == 0x00 ){
				_rdmsr(USB_MSR_REG(USB_OTG), &hi, &lo);
				lo = value & 0xffffff00;
				_wrmsr(USB_MSR_REG(USB_OTG), hi, lo);
				
				value &= 0xfffffff0;
				hi = 0x40000000 | ((value & 0xff000000) >> 24);
				lo = 0x000fffff | ((value & 0x00fff000) << 8);
				_wrmsr(GLIU_MSR_REG(GLIU_P2D_BM1), hi, lo);
			}
			break;
		default :
			break;			
	}
	
	return;
}

static u32 pci_otg_read_reg(int reg)
{
	u32 conf_data;
	u32 hi, lo;
	
	switch(reg){
		case PCI_VENDOR_ID:
			conf_data = (CS5536_OTG_DEVICE_ID << 16 | CS5536_VENDOR_ID);
			break;
		case PCI_COMMAND:
			conf_data = 0;
			// COMMAND
			_rdmsr(USB_MSR_REG(USB_OTG), &hi, &lo);
			if(hi & 0x02)
				conf_data |= PCI_COMMAND_MEMORY;
			// STATUS
			conf_data |= (PCI_STATUS_66MHZ << 16);
			conf_data |= (PCI_STATUS_FAST_BACK << 16);
			_rdmsr(SB_MSR_REG(SB_ERROR), &hi, &lo);
			if(lo & SB_PARE_ERR_FLAG)
				conf_data |= (PCI_STATUS_PARITY << 16);
			conf_data |= (PCI_STATUS_DEVSEL_MEDIUM << 16);
			break;
		case PCI_CLASS_REVISION:
			_rdmsr(USB_MSR_REG(USB_CAP), &hi, &lo);
			conf_data = lo & 0x000000ff;
			conf_data |= (CS5536_OTG_CLASS_CODE << 8);
			break;
		case PCI_CACHE_LINE_SIZE:
			conf_data = (0 << 24) | (PCI_HEADER_TYPE_NORMAL << 16) |
				(0x00 << 8) | 0x08;
			break;
		case PCI_BASE_ADDRESS_0:
			_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
			if(lo & SOFT_BAR_OTG_FLAG){
				conf_data = CS5536_OTG_RANGE | PCI_BASE_ADDRESS_SPACE_MEMORY;
				_rdmsr(GLCP_MSR_REG(GLCP_SOFT_COM), &hi, &lo);
				lo &= ~SOFT_BAR_OTG_FLAG;
				_wrmsr(GLCP_MSR_REG(GLCP_SOFT_COM), hi, lo);
			}else{
				_rdmsr(USB_MSR_REG(USB_OTG), &hi, &lo);
				conf_data = lo & 0xffffff00;
				conf_data &= ~0x0000000f;
			}
			break;
		case PCI_BASE_ADDRESS_1:
			conf_data = 0x000000;
			break;		
		case PCI_BASE_ADDRESS_2:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_3:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_4:
			conf_data = 0x000000;
			break;
		case PCI_BASE_ADDRESS_5:
			conf_data = 0x000000;
			break;
		case PCI_CARDBUS_CIS:
			conf_data = PCI_CARDBUS_CIS_POINTER;
			break;
		case PCI_SUBSYSTEM_VENDOR_ID:
			conf_data = (CS5536_OTG_SUB_ID << 16) | CS5536_SUB_VENDOR_ID;
			break;
		case PCI_ROM_ADDRESS :
			conf_data = PCI_EXPANSION_ROM_BAR;
			break;
		case PCI_CAPABILITY_LIST:
			conf_data = PCI_CAPLIST_USB_POINTER;
			break;
		case PCI_INTERRUPT_LINE:
			conf_data = (PCI_MAX_LAT << 24) | (PCI_MIN_GNT << 16) | 
				(0x01 << 8) | 0x00;
			break;
		default :
			conf_data = 0;
			break;
	}

	return conf_data;
}

#else	/* TEST_CS5536_USE_OTG */

static void pci_otg_write_reg(int reg, u32 value)
{
	return;
}

static u32 pci_otg_read_reg(int reg)
{
	return 0xffffffff;
}

#endif	/* TEST_CS5536_USE_OTG */

/*******************************************************************************/

/*
 * writen : write to PCI config space and transfer it to MSR write.
 */
int cs5536_pci_conf_writen(int function, int reg, int width, u32 value)
{
	unsigned short temp_reg;

	/* some basic checking. */
	if( (function < CS5536_FUNC_START) || (function > CS5536_FUNC_END) ){
		printf("cs5536 pci conf read : function range error.\n");
		return (-1);
	}
	if( (reg < 0) || (reg > 0x100) ){
		printf("cs5536 pci conf read : register range error.\n");
		return (-1);
	}
	if( width != 4 ){
		printf("cs5536 pci conf read : width error.\n");
		return (-1);
	}
	
	temp_reg = reg;
	reg = temp_reg & 0xfc;
	switch(function){
		case CS5536_ISA_FUNC :
			pci_isa_write_reg(reg, value);		
			break;

		case CS5536_FLASH_FUNC :
			pci_flash_write_reg(reg, value);
			break;
		
		case CS5536_IDE_FUNC :
			pci_ide_write_reg(reg, value);
			break;

		case CS5536_ACC_FUNC :
			pci_acc_write_reg(reg, value);
			break;

		case CS5536_OHCI_FUNC :
			pci_ohci_write_reg(reg, value);
			break;

		case CS5536_EHCI_FUNC :
			pci_ehci_write_reg(reg, value);
			break;

		case CS5536_UDC_FUNC :
			pci_udc_write_reg(reg, value);
			break;

		case CS5536_OTG_FUNC :
			pci_otg_write_reg(reg, value);
			break;
		
		default :
			printf("cs5536 not supported function.\n");
			break;
	
	}
	
	return 0;
}


/*
 * readn : read PCI config space and transfer it to MSR access.
 */
u32 cs5536_pci_conf_readn(int function, int reg, int width)
{
	u32 data;
	int temp_reg;

	/* some basic checking. */
	if( (function < CS5536_FUNC_START) || (function > CS5536_FUNC_END) ){
		printf("cs5536 pci conf read : function range error.\n");
		return (-1);
	}
	if( (reg < 0) || (reg > 0x100) ){
		printf("cs5536 pci conf read : register range error.\n");
		return (-1);
	}
	if( (width != 1) && (width != 2) && (width != 4) ){
		printf("cs5536 pci conf read : width error.\n");
		return (-1);
	}
	
	temp_reg = reg;
	reg = temp_reg & 0xfc;
	switch(function){
		case CS5536_ISA_FUNC :
			data = pci_isa_read_reg(reg);		
			break;

		case CS5536_FLASH_FUNC :
			data = pci_flash_read_reg(reg);
			break;
		
		case CS5536_IDE_FUNC :
			data = pci_ide_read_reg(reg);
			break;

		case CS5536_ACC_FUNC :
			data = pci_acc_read_reg(reg);
			break;

		case CS5536_OHCI_FUNC :
			data = pci_ohci_read_reg(reg);
			break;

		case CS5536_EHCI_FUNC :
			data = pci_ehci_read_reg(reg);
			break;

		case CS5536_UDC_FUNC :
			data = pci_udc_read_reg(reg);
			break;

		case CS5536_OTG_FUNC :
			data = pci_otg_read_reg(reg);
			break;
		
		default :
			printf("cs5536 not supported function.\n");
			break;
	
	}
	
	data =  data >> ( (temp_reg & 0x03) << 3 );
	return data;
}

void cs5536_pci_conf_write4(int function, int reg, u32 value)
{
	cs5536_pci_conf_writen(function, reg, 4, value);
	return;
}

u32 cs5536_pci_conf_read4(int function, int reg)
{
	return cs5536_pci_conf_readn(function, reg, 4);
}


