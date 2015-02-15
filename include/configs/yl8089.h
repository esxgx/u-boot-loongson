/*
 * Copyright (C) 2015 Hsiang Kao <0xe0e1e@gmail.com>
 * Copyright (C) 2009 www.lemote.com
 *
 * Configuration settings for the lemote yl8089 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __CONFIG_YL8089_H
#define __CONFIG_YL8089_H

#define CONFIG_YL8089			1
#define CONFIG_CPU_LOONGSON2

#define CONFIG_CPU_CLK_MHZ		800				/* clock for the MIPS core */
#define CONFIG_SYS_MHZ_CLOCK	(CONFIG_CPU_CLK_MHZ / 2)

#define CONFIG_SYS_SDRAM_BASE		0x80000000

// #define CONFIG_SYS_TEXT_BASE		0xBFC00000	// ROM version
#define CONFIG_SYS_TEXT_BASE		0x80100000	// RAM version


/* chipset selection */
#define CONFIG_SB_CS5536

/*-----------------------------------------------------------------------
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_PXE_CLIENTARCH		0x100


/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */

/* Size of malloc() pool after relocation */
#define CONFIG_SYS_MALLOC_LEN		128*1024

#define CONFIG_SYS_LONGHELP				/* undef to save memory	     */
#define CONFIG_SYS_PROMPT		"# "		/* Monitor Command Prompt    */

#define CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args */

#define CONFIG_SYS_LOAD_ADDR		0x80400000	/* default load address */
#define CONFIG_TIMESTAMP		/* Print image info with timestamp */

/*-----------------------------------------------------------------------
 * Board initialization
 */
#define CONFIG_SYS_GENERIC_BOARD
//#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_BOARD_EARLY_INIT_R

/*-----------------------------------------------------------------------
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PING
#define CONFIG_CMD_PCI
#define CONFIG_CMD_ELF

#define CONFIG_CMD_IDE

#define CONFIG_RTC_MC146818
#define CONFIG_CMD_DATE

#define CONFIG_CMD_USB

#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_FAT_WRITE
#define CONFIG_CMD_FS_GENERIC

#define CONFIG_CMD_PART
#define CONFIG_PARTITION_UUIDS

#include <config_distro_defaults.h>


/*-----------------------------------------------------------------------
 * Memory test configuration
 */
#define CONFIG_SYS_MEMTEST_START		(CONFIG_SYS_SDRAM_BASE + 0x3000000)
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x1000000)
#define CONFIG_SYS_MEMTEST_SCRATCH  (CONFIG_SYS_MEMTEST_END + 0x1000)
#define CONFIG_SYS_ALT_MEMTEST

/*-----------------------------------------------------------------------
 * Timer configration
 */
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_MIPS_TIMER_FREQ	(CONFIG_SYS_MHZ_CLOCK*1000000)


/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xbfc00000 /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_MONITOR_LEN		(192 << 10)

#define CONFIG_SYS_INIT_SP_OFFSET	0x600000

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(20 * CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Write */

#define CONFIG_ENV_IS_IN_FLASH	1
//#define ENV_IS_EMBEDDED

/* Address and size of Primary Environment Sector	*/
#define CONFIG_ENV_ADDR		0xBFC50000
#define CONFIG_ENV_SIZE		0x10000

#define CONFIG_SYS_DIRECT_FLASH_TFTP

#define CONFIG_NR_DRAM_BANKS	1



/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_SIZE		64*1024
#define CONFIG_SYS_ICACHE_SIZE		64*1024
#define CONFIG_SYS_CACHELINE_SIZE	32

/*-----------------------------------------------------------------------
 * Serial Configuration
 */
#define CONFIG_CPU_UART
#define CONFIG_CONS_INDEX		1			/* Use UART0			*/
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	 1
#define CONFIG_SYS_NS16550_CLK	 3686400
#define CONFIG_SYS_NS16550_COM1	 0xbff003f8

#define CONFIG_BAUDRATE		115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


/*-----------------------------------------------------------------------
 * Keyboard Configuration
 */
#define CONFIG_I8042_KBD

/*-----------------------------------------------------------------------
 * PCI stuff
 */
#define CONFIG_PCI
#define CONFIG_PCI_PNP
#define CONFIG_NET_MULTI
#define CONFIG_SYS_RX_ETH_BUFFER	8		/* use 8 rx buffer on eepro100	*/

#define CONFIG_RTL8139



/*-----------------------------------------------------------------------
 * PCI OHCI stuff
 */
#define CONFIG_USB_OHCI_NEW	1
#define CONFIG_PCI_OHCI		1
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS 15
#define CONFIG_SYS_USB_OHCI_SLOT_NAME	"ohci_pci"
#define CONFIG_USB_STORAGE	1
#define CONFIG_SYS_USB_EVENT_POLL 1
#define CONFIG_USB_KEYBOARD 1
//#define CONFIG_SYS_USB_OHCI_BOARD_INIT 1




/*-----------------------------------------------------------------------
 * IDE/ATA stuff (Supports IDE harddisk)
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_8xx_DIRECT		/* Direct IDE    not supported	*/
#undef	CONFIG_IDE_LED			/* LED   for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CONFIG_SYS_IDE_MAXBUS		1	/* max. 1 IDE bus		*/
#define CONFIG_SYS_IDE_MAXDEVICE	1	/* max. 1 drive per IDE bus	*/

#define CONFIG_SYS_ATA_IDE0_OFFSET	0x0000

#define CONFIG_SYS_ATA_BASE_ADDR	0xbfd001f0

/* Offset for data I/O			*/
#define CONFIG_SYS_ATA_DATA_OFFSET	0x0000

/* Offset for normal register accesses	*/
#define CONFIG_SYS_ATA_REG_OFFSET	0x0000

/* Offset for alternate registers	*/
#define CONFIG_SYS_ATA_ALT_OFFSET	0x0200

#define CONFIG_SYS_ISA_IO_BASE_ADDRESS 0xbfd00000
#define CONFIG_SYS_ISA_IO	CONFIG_SYS_ISA_IO_BASE_ADDRESS

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/* Graphics display support */
#define CONFIG_CFB_CONSOLE
#define CONFIG_VIDEO_SM712

#ifdef CONFIG_VIDEO_SM712
#define VIDEO_FB_16BPP_PIXEL_SWAP
#define VIDEO_HW_RECTFILL
#define VIDEO_HW_BITBLT
#define VIDEO_HW_RECTFILL
#endif

/*
#ifdef CONFIG_CFB_CONSOLE
#define VIDEO_KBD_INIT_FCT (1)
#define VIDEO_TSTC_FCT     _serial_tstc
#define VIDEO_GETC_FCT     _serial_getc
#endif
*/


#define CONFIG_PREBOOT	"echo;" \
	"echo Type \\\"boot\\\" for the network boot using DHCP, TFTP and NFS;" \
	"echo Type \\\"run netboot_initrd\\\" for the network boot with initrd;" \
	"echo Type \\\"run flash_nfs\\\" to mount root filesystem over NFS;" \
	"echo Type \\\"run flash_local\\\" to mount local root filesystem;" \
	"echo"

#undef	CONFIG_BOOTARGS

#if 0
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"netboot=dhcp;tftp;run netargs; bootm\0"			\
	"nfsargs=setenv bootargs root=/dev/nfs ip=dhcp\0"		\
	"localargs=setenv bootargs root=1F02 ip=dhcp\0"			\
	"addmisc=setenv bootargs ${bootargs} "				\
		"console=ttyS0,${baudrate} "				\
		"read-only=readonly\0"					\
	"netargs=run nfsargs addmisc\0"					\
	"flash_nfs=run nfsargs addmisc;"				\
		"bootm ${kernel_addr}\0"				\
	"flash_local=run localargs addmisc;"				\
		"bootm ${kernel_addr}\0"				\
	"busclock=66666666\0"	\
	"cpuclock=800000000\0"	\
	"memsize=256\0"							\
	"highmemsize=768\0"				\
	"netboot_initrd=dhcp;tftp;tftp 80600000 initrd;"		\
		"setenv bootargs root=/dev/ram ramdisk_size=8192 ip=dhcp;"\
		"run addmisc;"						\
		"bootm 80400000 80600000\0"				\
	"rootpath=/export/miniroot-mipsel\0"				\
	"autoload=no\0"							\
	"kernel_addr=BFC60000\0"					\
	"ramdisk_addr=81000000\0"					\
	"u-boot=u-boot.bin\0"						\
	"bootfile=uImage\0"						\
	"load=dhcp;tftp 80400000 ${u-boot}\0"				\
	"load_kernel=dhcp;tftp 80400000 ${bootfile}\0"			\
	"update_uboot=run load;"					\
		"protect off BFC00000 BFC3FFFF;"			\
		"erase BFC00000 BFC3FFFF;"				\
		"cp.b 80400000 BFC00000 ${filesize}\0"			\
	"update_kernel=run load_kernel;"				\
		"erase BFC60000 BFD5FFFF;"				\
		"cp.b 80400000 BFC60000 ${filesize}\0"			\
	"initenv=erase bfc50000 bfc5ffff\0"				\
	"ub=tftp 80400000 u-boot.bin;"					\
		"protect off BFC00000 BFC4FFFF;"			\
		"erase BFC00000 BFC4FFFF;"				\
		"cp.b 80400000 BFC00000 ${filesize}\0"			\
	"stdin=vga,serial\0"	\
	"stdout=vga,serial\0"	\
	"stderr=vga,serial\0"	\
	""
#else

#define CONFIG_EXTRA_ENV_SETTINGS					\
	"stdin=vga,serial\0"	\
	"stdout=vga,serial\0"	\
	"stderr=vga,serial\0"	\
	"busclock=66666666\0"	\
	"cpuclock=800000000\0"	\
	"memsize=256\0"							\
	"highmemsize=768\0"				\
	"ipaddr=192.168.10.1\0"			\
	"serverip=192.168.10.2\0"			\
	""
#endif

/*#define CONFIG_BOOTCOMMAND	"run flash_local" */
/*#define CONFIG_BOOTCOMMAND	"run netboot" */

#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/

#define CONFIG_CONSOLE_MUX

#define CONFIG_SHOW_BOOT_PROGRESS
//#define DEBUG
//#define ET_DEBUG

#define CONFIG_SYS_LDSCRIPT    "arch/mips/cpu/u-boot.lds"

#endif	/* __CONFIG_H */
