/*
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2002, 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2015 Hsiang Kao, Hfut University of Technology <0xe0e1e@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <pci.h>

#include "bonito.h"
#include "cs5536_pci.h"

enum {
	PCI_ACCESS_READ,
	PCI_ACCESS_WRITE
};

static int
bonito_pci_config_access(u8 access_type, pci_dev_t dev, int reg, int width, u32 *data)
{
	int	bus = PCI_BUS(dev),
		device = PCI_DEV(dev),
		function = PCI_FUNC(dev);
	int addr;

	if (!bus) {
		extern u32 cs5536_pci_conf_read4(int, int);
		extern void cs5536_pci_conf_write4(int, int, u32);

		// Type 0 configuration on onboard PCI bus
		if ((device == PCI_IDSEL_CS5536) && (reg < 0xf0)) {
			switch(access_type) {
				case PCI_ACCESS_READ :
					*data = cs5536_pci_conf_read4(function, reg);
					break;
				case PCI_ACCESS_WRITE :
					cs5536_pci_conf_write4(function, reg, *data);
					break;
			}
			return 0;
		}
		addr = (1 << (device+11)) | (function << 8) | reg;
	} else {
		// Type 1 configuration on offboard PCI bus
		addr = (bus << 16) | (device << 11) | (function << 8) | reg;
	}
	BONITO_PCICMD |= BONITO_PCICMD_MABORT_CLR | BONITO_PCICMD_MTABORT_CLR;	// clear aborts
	BONITO_PCIMAP_CFG = ((bus!=0)<<16) | (addr >> 16);	// PCIMAP_CFG

	switch(access_type) {
		case PCI_ACCESS_READ:
			*data = (*(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc)))			// get data
					 >> ((addr & 3) << 3);		// and shift bits correctly
			break;
		case PCI_ACCESS_WRITE: {
				// write the shifted data
				u32 ori, mask;
				ori = *(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc));
				if (width & 3) {
					if (width == 2) mask = ~0UL & ~(0xffff << ((addr & 3)) << 4);
					else if (width == 1) mask = ~0UL & ~(0xff << ((addr & 3) << 3));
					else return -1;
					*(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc))  = (ori & mask) | (*data << ((addr & 3) << 3));
				} else *(volatile u32*)((u32)BONITO_PCICFG | (addr & 0xfffc))  = *data << ((addr & 3) << 3);
		}	break;
	}
	return 0;
}

static void
bonito_pci_hwinit(void)
{
	BONITO_PCICMD = BONITO_PCICMD_MTABORT_CLR| BONITO_PCICMD_SERR_CLR|
			BONITO_PCICMD_PERR_CLR| BONITO_PCICMD_TABORT_CLR| BONITO_PCICMD_MABORT_CLR|
			BONITO_PCICMD_MPERR_CLR| BONITO_PCICMD_MEMEN| BONITO_PCICMD_MSTREN;
//	BONITO(0x150) = 0x8000000c;
//	BONITO(0x154) = 0xffffffff;

	// set PCIMap register; first windows for 0-0x4000000 pci mem
	BONITO_PCIMAP =
			BONITO_PCIMAP_WIN(0, /*PCI_MEM_SPACE_PCI_BASE+*/0x00000000) |
			BONITO_PCIMAP_WIN(1, PCI_MEM_SPACE_PCI_BASE+0x04000000) |
			BONITO_PCIMAP_WIN(2, PCI_MEM_SPACE_PCI_BASE+0x08000000) |
			BONITO_PCIMAP_PCIMAP_2;
//	BONITO_PCIBASE0 = 0x8000000c;
//	BONITO_PCIBASE1 = 0x0;
//	BONITO_PCIBASE2 = 0x0;

#if	1	// for 2f
	BONITO_PCIBASE0 = 0x80000000;
	BONITO_PCIBASE1 = 0x0;

	BONITO_PCIBASE2 = 0x70000000;
	BONITO_PCI_REG(0x40) = 0x80000000;
	BONITO_PCI_REG(0x44) =  0xf0000000;
#endif

}

static int bonito_pci_read_config_byte(struct pci_controller *pci, pci_dev_t dev, int reg, u8 *val)
{
	u32 data;
	int ret = bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, 1, &data);
	if (!ret) *val = data;
	return ret;
}

static int bonito_pci_read_config_word(struct pci_controller *pci, pci_dev_t dev, int reg, u16 *val)
{
	if (reg & 1) return -1;
	u32 data;
	int ret = bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, 2, &data);
	if (!ret) *val = data;
	return ret;
}

static int bonito_pci_read_config_dword(struct pci_controller *pci, pci_dev_t dev, int reg, u32 *val)
{
	if (reg & 3) return -1;
	return bonito_pci_config_access(PCI_ACCESS_READ, dev, reg, 4, val);
}

static int bonito_pci_write_config_byte(struct pci_controller *pci, pci_dev_t dev, int reg, u8 val)
{
	u32 data = val;
	return bonito_pci_config_access(PCI_ACCESS_WRITE, dev, reg, 1, &data);
}

static int bonito_pci_write_config_word(struct pci_controller *pci, pci_dev_t dev, int reg, u16 val)
{
	if (reg & 1) return -1;
	u32 data = val;
	return bonito_pci_config_access(PCI_ACCESS_WRITE, dev, reg, 2, &data);
}

static int bonito_pci_write_config_dword(struct pci_controller *pci, pci_dev_t dev, int reg, u32 val)
{
	if (reg & 3) return -1;
	return bonito_pci_config_access(PCI_ACCESS_WRITE, dev, reg, 4, &val);
}

void
bonito_pci_init (struct pci_controller *hose)
{
	bonito_pci_hwinit();

	hose->first_busno = 0;
	hose->last_busno = 0xff;

	// take care that U-Boot maintainers replace phys_addr by virt_addr
	// for the MIPS, so we deal with it as follows.

	// PCI memory space #0 is reserved

	// PCI memory space #1
	pci_set_region(hose->regions + 0,
		BONITO_PCILO1_BASE, BONITO_PCILO1_BASE_VA, BONITO_PCILO1_SIZE, PCI_REGION_MEM);

	// PCI memory space #2
	pci_set_region(hose->regions + 1,
		BONITO_PCILO2_BASE, BONITO_PCILO2_BASE_VA, BONITO_PCILO2_SIZE, PCI_REGION_MEM);

	// PCI I/O space 
	pci_set_region(hose->regions + 2,
		BONITO_PCIIO_BASE + 0xb000, BONITO_PCIIO_BASE_VA + 0xb000, BONITO_PCIIO_SIZE - 0xb000, PCI_REGION_IO);

	// System memory space(simply used for PCIDMA)
	pci_set_region(hose->regions + 3,
		0x80000000, 0, 0x80000000, PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 4;

	pci_set_ops (hose,
		bonito_pci_read_config_byte,
		bonito_pci_read_config_word,
		bonito_pci_read_config_dword,
		bonito_pci_write_config_byte,
		bonito_pci_write_config_word,
		bonito_pci_write_config_dword
	);
	pci_register_hose (hose);
	hose->last_busno = pci_hose_scan(hose);
}
