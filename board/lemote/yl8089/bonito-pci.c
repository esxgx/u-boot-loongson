/*
 * Loongson 2F cpu's bonito likely PCI controller 
 *
 * (C) Copyright 2009 
 * Yanhua,  yanh@.lemote.com 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#include <common.h>
#include <pci.h>
#include <asm/addrspace.h>

#include <asm/ls2f/ls2f.h>
#include "bonito.h"

volatile unsigned int *pciconfigaddr;
volatile unsigned int *pciconfigdata;

#define PCI_ACCESS_READ  0
#define PCI_ACCESS_WRITE 1

/*
 *	Access PCI Configuration Register for VR4131
 */

static int bonito_pci_config_access(u8 access_type, pci_dev_t dev, u32 reg,
				     u32 * data)
{
	extern u32 cs5536_pci_conf_read4(int, int);
	extern void cs5536_pci_conf_write4(int, int, u32);
	u32 bus;
	u32 device;
	u32 function;
	u32 addr;

	bus = ((dev & 0xff0000) >> 16);
	device = ((dev & 0xf800) >> 11);
	function = ((dev & 0x700) >> 8);

	if (bus == 0) {
		/* Type 0 Configuration */
		addr = (u32) ((1UL << (device + 11)) | (function << 8)| (reg & 0xfc));
	} else {
		/* Type 1 Configuration */
		addr  = (u32) (dev | ((reg / 4) << 2) | 1);
	}

	/* CS5536 config access */
	if ((bus == 0) && (device == 0xE) && (reg < 0xF0)) {
		switch(access_type){
		case PCI_ACCESS_READ :
			*data = cs5536_pci_conf_read4(function, reg);
			break;
		case PCI_ACCESS_WRITE :
			cs5536_pci_conf_write4(function, reg, *data);
			break;
		}
		return 0;
	}

	/* clear aborts */
	BONITO_PCICMD |= BONITO_PCICMD_MABORT_CLR | BONITO_PCICMD_MTABORT_CLR;

	BONITO_PCIMAP_CFG = (addr >> 16) | (addr & 1);
	bflush();

	if (access_type == PCI_ACCESS_WRITE) {
		*(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc))  = *data;
	} else {
		*data = *(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc));
	}

	return (0);
}

static int bonito_pci_read_config_byte(struct pci_controller *pci, pci_dev_t dev, int reg, u8 *val)
{
	u32 data;

	if (bonito_pci_config_access(PCI_ACCESS_READ, dev, (reg & ~3), &data))
		return -1;

	*val = (data >> ((reg & 0x3) <<3)) & 0xff;

	return 0;
}


static int bonito_pci_read_config_word(struct pci_controller *pci, pci_dev_t dev, int reg, u16 *val)
{
	u32 data;

	if (reg & 1)
		return -1;

	if (bonito_pci_config_access(PCI_ACCESS_READ, dev, (reg & ~3), &data))
		return -1;

	*val = (data >> ((reg & 0x2) <<3)) & 0xffff;

	return 0;
}


static int bonito_pci_read_config_dword(struct pci_controller *pci, pci_dev_t dev, int reg, u32 *val)
{
	u32 data = 0;

	if (reg & 3)
		return -1;

	if (bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, &data))
		return -1;

	*val = data;

	return (0);
}

static int bonito_pci_write_config_byte(struct pci_controller *pci, pci_dev_t dev, int reg, u8 val)
{
	u32 data = 0;

	if (bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, &data))
		return -1;

	data = (data & ~(0xff << ((reg & 3) << 3))) | (val <<
						       ((reg & 3) << 3));

	if (bonito_pci_config_access(PCI_ACCESS_WRITE, dev, (reg & ~3), &data))
		return -1;

	return 0;
}


static int bonito_pci_write_config_word(struct pci_controller *pci, pci_dev_t dev, int reg, u16 val)
{
	u32 data = 0;

	if (reg & 1)
		return -1;

	if (bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, &data))
		return -1;

	data = (data & ~(0xffff << ((reg & 3) << 3))) | (val <<
							 ((reg & 3) << 3));

	if (bonito_pci_config_access(PCI_ACCESS_WRITE, dev, reg, &data))
		return -1;

	return 0;
}

static int bonito_pci_write_config_dword(struct pci_controller *pci, pci_dev_t dev, int reg, u32 val)
{
	u32 data;

	if (reg & 3) {
		return -1;
	}

	data = val;

	if (bonito_pci_config_access(PCI_ACCESS_WRITE, dev, reg, &data))
		return -1;

	return (0);
}


/*
 *	Initialize Bonito pcib
 */

void bonito_pci_init(void)
{
	BONITO_PCICMD = BONITO_PCICMD_MTABORT_CLR| BONITO_PCICMD_SERR_CLR| 
			BONITO_PCICMD_PERR_CLR| BONITO_PCICMD_TABORT_CLR| BONITO_PCICMD_MABORT_CLR|
			BONITO_PCICMD_MPERR_CLR| BONITO_PCICMD_MEMEN| BONITO_PCICMD_MSTREN;
	BONITO(0x150) = 0x8000000c;
	BONITO(0x154) = 0xffffffff;
	BONITO_PCIMAP =
			BONITO_PCIMAP_WIN(0, /*PCI_MEM_SPACE_PCI_BASE+*/0x00000000) |
			BONITO_PCIMAP_WIN(1, PCI_MEM_SPACE_PCI_BASE+0x04000000) |
			BONITO_PCIMAP_WIN(2, PCI_MEM_SPACE_PCI_BASE+0x08000000) |
			BONITO_PCIMAP_PCIMAP_2;
	BONITO_PCIBASE0 = 0x8000000c;
	BONITO_PCIBASE1 = 0x0;
	BONITO_PCIBASE2 = 0x0;
}

/*
 *	Initialize Module
 */

void init_bonito_pci(struct pci_controller *hose)
{
	hose->first_busno = 0;
	hose->last_busno = 0xff;

	bonito_pci_init();	/* Initialize Loongson2F PCIU */

	/* PCI memory space #1 */
	pci_set_region(hose->regions + 0,
			0x14000000, 0x14000000, 0x04000000, PCI_REGION_MEM);

	/* PCI memory space #2 */
	pci_set_region(hose->regions + 1,
			0x18000000, 0x18000000, 0x04000000, PCI_REGION_MEM);

	/* 
	 * PCI I/O space 
	 * FIXME, I don't like the urgly hacking
	 */
	pci_set_region(hose->regions + 2,
			0x0000b000, 0xbfd00000+0xb000, 0x00040000-0xb000, PCI_REGION_IO);

	/* 
	 * System memory space 
	 * This is also a hacking, although I don't like it
	 * In fact the phys is virt address
	 */
	pci_set_region(hose->regions + 3,
			0x80000000,
			0x80000000,
			0x10000000, PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 4;

	hose->read_byte = bonito_pci_read_config_byte;
	hose->read_word = bonito_pci_read_config_word;
	hose->read_dword = bonito_pci_read_config_dword;
	hose->write_byte = bonito_pci_write_config_byte;
	hose->write_word = bonito_pci_write_config_word;
	hose->write_dword = bonito_pci_write_config_dword;

	pci_register_hose (hose);

	hose->last_busno = pci_hose_scan (hose);

	return;
}
