/*
 * Board initialize code for Lemote YL8089.
 *
 * (C) Yanhua <yanh@lemote.com> 2009
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/reboot.h>
#include <pci.h>
#include <netdev.h>

#if defined(CONFIG_SHOW_BOOT_PROGRESS)
void show_boot_progress(int progress)
{
	printf("Boot reached stage %d\n", progress);
}
#endif

void _machine_restart(void)
{
	void (*f)(void) = (void *) 0xbfc00000;

#if 0
	/* dark the lcd */
	outb(0xfe, 0xbfd00381);
	outb(0x01, 0xbfd00382);
	outb(0x00, 0xbfd00383);
#endif
	/* Tell EC to reset the whole system */
	outb(0xf4, 0xbfd00381);
	outb(0xec, 0xbfd00382);
	outb(0x01, 0xbfd00383);

	while (1);
	/* Not reach here normally */
	f();
}

#if defined(CONFIG_PCI)
static struct pci_controller hose;

void pci_init_board (void)
{
	extern void cs5536_init(void);
	extern void init_bonito_pci(struct pci_controller *);
	cs5536_init();

	bonito_pci_init(&hose);
}
#endif

phys_size_t initdram(int board_type)
{
	return get_ram_size ((void *)CONFIG_SYS_SDRAM_BASE, 0x8000000);
}

int board_early_init_r(void)
{
	set_io_port_base(0x0);
	return 0;
}

int checkboard (void)
{
#define ___(a,b)		a#b
	printf(___("Board: Lemote YL8089 (CPU Speed ",CONFIG_CPU_CLK_MHZ) " MHz)\n");
	return 0;
}

int board_eth_init(bd_t *bis)
{
	pci_eth_init(bis);
	return 0;
}
