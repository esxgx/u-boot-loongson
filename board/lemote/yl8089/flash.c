/*
 * (C) Masami Komiya <mkomiya@sonare.it> 2004
 *
 * (C) Copyright 2009
 * Yanhua, Lemote Software Engineering, yanh@lemote.com
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/processor.h>

flash_info_t flash_info[CONFIG_SYS_MAX_FLASH_BANKS];	/* info for FLASH chips        */


#define FLASH_WORD_SIZE unsigned char
#define	FLASH_ID_MASK	0xFF

/*-----------------------------------------------------------------------
 * Functions
 */
/* stolen from esteem192e/flash.c */
ulong flash_get_size (volatile FLASH_WORD_SIZE * addr, flash_info_t * info);

static int write_byte (flash_info_t * info, ulong dest, ulong data);
static void flash_get_offsets (ulong base, flash_info_t * info);

/*-----------------------------------------------------------------------
 */

unsigned long flash_init (void)
{
	unsigned long size_b0, size_b1;
	int i;

	/* Init: no FLASHes known */
	for (i = 0; i < CONFIG_SYS_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	/* Static FLASH Bank configuration here - FIXME XXX */

	size_b0 =
		flash_get_size ((volatile FLASH_WORD_SIZE *) CONFIG_SYS_FLASH_BASE,
				&flash_info[0]);

	if (flash_info[0].flash_id == FLASH_UNKNOWN) {
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n", size_b0, size_b0 << 20);
	}

	/* Only one bank */
	if (CONFIG_SYS_MAX_FLASH_BANKS == 1) {
		/* Setup offsets */
		flash_get_offsets (CONFIG_SYS_FLASH_BASE, &flash_info[0]);

		/* Monitor protection ON by default */
#if 0				/* sand: */
		(void) flash_protect (FLAG_PROTECT_SET,
				      FLASH_BASE0_PRELIM - monitor_flash_len +
				      size_b0,
				      FLASH_BASE0_PRELIM - 1 + size_b0,
				      &flash_info[0]);
#else
		(void) flash_protect (FLAG_PROTECT_SET,
				      CONFIG_SYS_MONITOR_BASE,
				      CONFIG_SYS_MONITOR_BASE + monitor_flash_len -
				      1, &flash_info[0]);
#endif
		size_b1 = 0;
		flash_info[0].size = size_b0;
	}

	return (size_b0 + size_b1);
}


/*-----------------------------------------------------------------------
 */

static void flash_get_offsets (ulong base, flash_info_t * info)
{
	int i;

	/* set up sector start adress table */
	switch ((info->flash_id & FLASH_VENDMASK)) {
	/* FIXME Add any other vendors */	
	default:
	case FLASH_MAN_SST:
	case FLASH_MAN_MX:
		for (i=0; i<info->sector_count; i++)	
			info->start[i] = base + i * (info->size / info->sector_count);
	}

}

/*-----------------------------------------------------------------------
 */

void flash_print_info (flash_info_t * info)
{
	int i;
	uchar *boottype;
	uchar botboot[] = ", bottom boot sect)\n";
	uchar topboot[] = ", top boot sector)\n";

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:
		printf ("AMD ");
		break;
	case FLASH_MAN_FUJ:
		printf ("FUJITSU ");
		break;
	case FLASH_MAN_SST:
		printf ("SST ");
		break;
	case FLASH_MAN_STM:
		printf ("STM ");
		break;
	case FLASH_MAN_INTEL:
		printf ("INTEL ");
		break;
	default:
		printf ("Unknown Vendor ");
		break;
	}

	if (info->flash_id & 0x0001) {
		boottype = botboot;
	} else {
		boottype = topboot;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM400B:
		printf ("AM29LV400B (4 Mbit%s", boottype);
		break;
	case FLASH_AM400T:
		printf ("AM29LV400T (4 Mbit%s", boottype);
		break;
	case FLASH_AM040:
		printf("AM29X040 (4 Mbit%s", boottype);
		break;
	case FLASH_AM800B:
		printf ("AM29LV800B (8 Mbit%s", boottype);
		break;
	case FLASH_AM800T:
		printf ("AM29LV800T (8 Mbit%s", boottype);
		break;
	case FLASH_AM160B:
		printf ("AM29LV160B (16 Mbit%s", boottype);
		break;
	case FLASH_AM160T:
		printf ("AM29LV160T (16 Mbit%s", boottype);
		break;
	case FLASH_AM320B:
		printf ("AM29LV320B (32 Mbit%s", boottype);
		break;
	case FLASH_AM320T:
		printf ("AM29LV320T (32 Mbit%s", boottype);
		break;
	case FLASH_INTEL800B:
		printf ("INTEL28F800B (8 Mbit%s", boottype);
		break;
	case FLASH_INTEL800T:
		printf ("INTEL28F800T (8 Mbit%s", boottype);
		break;
	case FLASH_INTEL160B:
		printf ("INTEL28F160B (16 Mbit%s", boottype);
		break;
	case FLASH_INTEL160T:
		printf ("INTEL28F160T (16 Mbit%s", boottype);
		break;
	case FLASH_INTEL320B:
		printf ("INTEL28F320B (32 Mbit%s", boottype);
		break;
	case FLASH_INTEL320T:
		printf ("INTEL28F320T (32 Mbit%s", boottype);
		break;

#if 0				/* enable when devices are available */

	case FLASH_INTEL640B:
		printf ("INTEL28F640B (64 Mbit%s", boottype);
		break;
	case FLASH_INTEL640T:
		printf ("INTEL28F640T (64 Mbit%s", boottype);
		break;
#endif
	case FLASH_28F320J3A:
		printf ("INTEL28F320J3A (32 Mbit%s", boottype);
		break;
	case FLASH_28F640J3A:
		printf ("INTEL28F640J3A (64 Mbit%s", boottype);
		break;
	case FLASH_28F128J3A:
		printf ("INTEL28F128J3A (128 Mbit%s", boottype);
		break;
	case FLASH_SST040:
		printf ("SST39VF040 (4 Mbit%s", boottype);
		break;

	default:
		printf ("Unknown Chip Type\n");
		break;
	}

	printf ("  Size: %ld MB in %d Sectors\n",
		info->size >> 20, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i = 0; i < info->sector_count; ++i) {
		if ((i % 5) == 0)
			printf ("\n   ");
		printf (" %08lX%s",
			info->start[i], info->protect[i] ? " (RO)" : "     ");
	}
	printf ("\n");
	return;
}


/*-----------------------------------------------------------------------
 */


/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */
ulong flash_get_size (volatile FLASH_WORD_SIZE * addr, flash_info_t * info)
{
	short i;
	ulong base = (ulong) addr;
	FLASH_WORD_SIZE value;

	/* Write auto select command: read Manufacturer ID */


	/* 8bit */
	addr[0x0555] = 0xAA;
	addr[0x02AA] = 0x55;
	addr[0x0555] = 0x90;
	addr[0x5555] = 0xAA; //SST, and Winbond
	addr[0x2AAA] = 0x55;
	addr[0x5555] = 0x90;

	value = addr[0];

	switch (value) {
	case (AMD_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_AMD;
		break;
	case (FUJ_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_FUJ;
		break;
	case (STM_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_STM;
		break;
	case (SST_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_SST;
		break;
	case (INTEL_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_INTEL;
		break;
	case (MX_MANUFACT & FLASH_ID_MASK):
		info->flash_id = FLASH_MAN_MX;
		break;
	default:
		info->flash_id = FLASH_UNKNOWN;
		info->sector_count = 0;
		info->size = 0;
		return (0);	/* no or unknown flash  */

	}

	value = addr[1];	/* device ID            */

	switch (value) {
	case (AMD_ID_LV400T & FLASH_ID_MASK):
		info->flash_id += FLASH_AM400T;
		info->sector_count = 11;
		info->size = 0x00100000;
		break;		/* => 1 MB              */

	case (AMD_ID_LV400B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM400B;
		info->sector_count = 11;
		info->size = 0x00100000;
		break;		/* => 1 MB              */

	case (AMD_ID_LV800T & FLASH_ID_MASK):
		info->flash_id += FLASH_AM800T;
		info->sector_count = 19;
		info->size = 0x00200000;
		break;		/* => 2 MB              */

	case (AMD_ID_LV800B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM800B;
		info->sector_count = 19;
		info->size = 0x00200000;
		break;		/* => 2 MB              */

	case (AMD_ID_LV160T & FLASH_ID_MASK):
		info->flash_id += FLASH_AM160T;
		info->sector_count = 35;
		info->size = 0x00400000;
		break;		/* => 4 MB              */

	case (AMD_ID_LV160B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM160B;
		info->sector_count = 35;
		info->size = 0x00400000;
		break;		/* => 4 MB              */
	case (AMD_ID_LV040B	& FLASH_ID_MASK):
		info->flash_id += FLASH_AM040;
		info->sector_count = 8;
		info->size = 0x00080000;
		break;
#if 0				/* enable when device IDs are available */
	case (AMD_ID_LV320T & FLASH_ID_MASK):
		info->flash_id += FLASH_AM320T;
		info->sector_count = 67;
		info->size = 0x00800000;
		break;		/* => 8 MB              */

	case (AMD_ID_LV320B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM320B;
		info->sector_count = 67;
		info->size = 0x00800000;
		break;		/* => 8 MB              */
#endif

	case (INTEL_ID_28F800B3T & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL800T;
		info->sector_count = 23;
		info->size = 0x00200000;
		break;		/* => 2 MB              */

	case (INTEL_ID_28F800B3B & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL800B;
		info->sector_count = 23;
		info->size = 0x00200000;
		break;		/* => 2 MB              */

	case (INTEL_ID_28F160B3T & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL160T;
		info->sector_count = 39;
		info->size = 0x00400000;
		break;		/* => 4 MB              */

	case (INTEL_ID_28F160B3B & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL160B;
		info->sector_count = 39;
		info->size = 0x00400000;
		break;		/* => 4 MB              */

	case (INTEL_ID_28F320B3T & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL320T;
		info->sector_count = 71;
		info->size = 0x00800000;
		break;		/* => 8 MB              */

	case (INTEL_ID_28F320B3B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM320B;
		info->sector_count = 71;
		info->size = 0x00800000;
		break;		/* => 8 MB              */

#if 0				/* enable when devices are available */
	case (INTEL_ID_28F320B3T & FLASH_ID_MASK):
		info->flash_id += FLASH_INTEL320T;
		info->sector_count = 135;
		info->size = 0x01000000;
		break;		/* => 16 MB             */

	case (INTEL_ID_28F320B3B & FLASH_ID_MASK):
		info->flash_id += FLASH_AM320B;
		info->sector_count = 135;
		info->size = 0x01000000;
		break;		/* => 16 MB             */
#endif
	case (INTEL_ID_28F320J3A & FLASH_ID_MASK):
		info->flash_id += FLASH_28F320J3A;
		info->sector_count = 32;
		info->size = 0x00400000;
		break;		/* => 32 MBit   */
	case (INTEL_ID_28F640J3A & FLASH_ID_MASK):
		info->flash_id += FLASH_28F640J3A;
		info->sector_count = 64;
		info->size = 0x00800000;
		break;		/* => 64 MBit   */
	case (INTEL_ID_28F128J3A & FLASH_ID_MASK):
		info->flash_id += FLASH_28F128J3A;
		info->sector_count = 128;
		info->size = 0x01000000;
		break;		/* => 128 MBit          */
	case (SST_ID_xF040 & FLASH_ID_MASK):		 
		info->flash_id += FLASH_SST040;
		info->sector_count = 128;
		info->size = 0x00080000;
		break;

	default:
		/* FIXME */
		info->flash_id = FLASH_UNKNOWN;
		return (0);	/* => no or unknown flash */
	}

	flash_get_offsets (base, info);

	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		/* read sector protection at sector address, (A7 .. A0) = 0x02 */
		/* D0 = 1 if protected */
		addr = (volatile FLASH_WORD_SIZE *) (info->start[i]);
		info->protect[i] = addr[2] & 1;
	}

	/*
	 * Prevent writes to uninitialized FLASH.
	 */
	if (info->flash_id != FLASH_UNKNOWN) {
		addr = (volatile FLASH_WORD_SIZE *) info->start[0];
		*addr = (0xF0 & FLASH_ID_MASK);	/* reset bank */
	}

	return (info->size);
}


/*-----------------------------------------------------------------------
 */

int flash_erase (flash_info_t * info, int s_first, int s_last)
{

	volatile FLASH_WORD_SIZE *addr =
		(volatile FLASH_WORD_SIZE *) (info->start[0]);
	int flag, prot, sect, l_sect, barf;
	int seq[5];
	int rcode = 0;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;
	}

	if ((info->flash_id == FLASH_UNKNOWN) ||
	    ((info->flash_id > FLASH_AMD_COMP) &&
	     ((info->flash_id & FLASH_VENDMASK) != FLASH_MAN_SST) &&
		 ((info->flash_id & FLASH_VENDMASK) != FLASH_MAN_MX))) {
		printf ("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect = s_first; sect <= s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n", prot);
	} else {
		printf ("\n");
	}

	l_sect = -1;

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();
	if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
		seq[0] = 0x5555;
		seq[1] = 0x2AAA;
		seq[2] = 0x5555;
		seq[3] = 0x5555;
		seq[4] = 0x2AAA;
	} else {	
		seq[0] = 0x0555;
		seq[1] = 0x02AA;
		seq[2] = 0x0555;
		seq[3] = 0x0555;
		seq[4] = 0x02AA;
	}

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect <= s_last; sect++) {
		if (info->protect[sect] == 0) {	/* not protected */
			addr = (volatile FLASH_WORD_SIZE *) (info->start[0]);
			addr[seq[0]]=0xAA;
			addr[seq[1]]=0x55;
			addr[seq[2]]=0x80;
			addr[seq[3]]=0xAA;
			addr[seq[4]]=0x55;
			addr = (volatile FLASH_WORD_SIZE *) (info->
					     start
					     [sect]);
			addr[0] = (0x30 & FLASH_ID_MASK);
			while (1) {
				int toggle1, toggle2;
				toggle1 = addr[0];

				if ((toggle1 & 0x80) == 0x00)
					continue;
				toggle1 = addr[0];
				toggle2 = addr[1];
				if ((toggle1 ^ toggle2) == 0x40)
					continue;
				break;
			}
			l_sect = sect;
			/* Reset Chip */
			addr = (volatile FLASH_WORD_SIZE *) info->start[0];
			addr[0] = (0xF0 & FLASH_ID_MASK);	/* reset bank */
		}
	}

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts ();

#if 0
	/* wait at least 80us - let's wait 1 ms */
	udelay (1000);

	/*
	 * We wait for the last triggered sector
	 */
	if (l_sect < 0)
			goto DONE;

	start = get_timer (0);
	last = start;
	addr = (volatile FLASH_WORD_SIZE *) (info->start[l_sect]);
	while ((addr[0] & (0x80 & FLASH_ID_MASK)) !=
				(0x80 & FLASH_ID_MASK)) {
		if ((now = get_timer (start)) > CONFIG_SYS_FLASH_ERASE_TOUT) {
				printf ("Timeout\n");
				return 1;
		}
		/* show that we're waiting */
		if ((now - last) > 1000000) {	/* every second */
				serial_putc ('.');
				last = now;
		}
	}

DONE:
		/* reset to read mode */
	addr = (volatile FLASH_WORD_SIZE *) info->start[0];
	addr[0] = (0xF0 & FLASH_ID_MASK);	/* reset bank */
 	printf (" done\n");
#endif
	return rcode;
}

/*-----------------------------------------------------------------------
 */

/*flash_info_t *addr2info (ulong addr)
{
	flash_info_t *info;
	int i;

	for (i=0, info=&flash_info[0]; i<CONFIG_SYS_MAX_FLASH_BANKS; ++i, ++info) {
		if ((addr >= info->start[0]) &&
		    (addr < (info->start[0] + info->size)) ) {
			return (info);
		}
	}

	return (NULL);
}
*/
/*-----------------------------------------------------------------------
 * Copy memory to flash.
 * Make sure all target addresses are within Flash bounds,
 * and no protected sectors are hit.
 * Returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 * 4 - target range includes protected sectors
 * 8 - target address not in Flash memory
 */

/*int flash_write (uchar *src, ulong addr, ulong cnt)
{
	int i;
	ulong         end        = addr + cnt - 1;
	flash_info_t *info_first = addr2info (addr);
	flash_info_t *info_last  = addr2info (end );
	flash_info_t *info;

	if (cnt == 0) {
		return (0);
	}

	if (!info_first || !info_last) {
		return (8);
	}

	for (info = info_first; info <= info_last; ++info) {
										ulong b_end = info->start[0] + info->size;*/ /* bank end addr */
/*		short s_end = info->sector_count - 1;
		for (i=0; i<info->sector_count; ++i) {
			ulong e_addr = (i == s_end) ? b_end : info->start[i + 1];

			if ((end >= info->start[i]) && (addr < e_addr) &&
			    (info->protect[i] != 0) ) {
				return (4);
			}
		}
	}

*/ /* finally write data to flash */
/*	for (info = info_first; info <= info_last && cnt>0; ++info) {
		ulong len;

		len = info->start[0] + info->size - addr;
		if (len > cnt)
			len = cnt;
		if ((i = write_buff(info, src, addr, len)) != 0) {
			return (i);
		}
		cnt  -= len;
		addr += len;
		src  += len;
	}
	return (0);
}
*/
/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */

int write_buff (flash_info_t * info, uchar * src, ulong addr, ulong cnt)
{
	int i, rc;
	uchar *start = (volatile FLASH_WORD_SIZE *) info->start[0];

	if (cnt == 0) {
		return (0);
	}

	for (i=0; i < cnt; ++i) {
		if ((rc = write_byte(info, addr++, *src++))!= 0)
			return rc;
	}
	
	start[0] = 0xF0;
	return rc;

}

/*-----------------------------------------------------------------------
 * Write a word to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */
static int write_byte (flash_info_t * info, ulong dest, ulong data)
{
	volatile char *addr = (vu_long *) (info->start[0]);
	ulong start, barf;
	int flag;

	/* Check if Flash is (sufficiently) erased */
	if (*((uchar *) dest) != 0xff ) {
		return (2);
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts ();

	if ((info->flash_id < FLASH_AMD_COMP) || 
		 (info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX) {
		/* AMD stuff */
		addr[0x0555] = 0xAA;
		addr[0x02AA] = 0x55;
		addr[0x0555] = 0xA0;
	} else if ((info->flash_id & FLASH_VENDMASK ) == FLASH_MAN_SST ) {
		addr[0x5555] = 0xAA;
		addr[0x2AAA] = 0x55;
		addr[0x5555] = 0xA0;
	} else {
		/* intel stuff */
		*addr = 0x00400040;
	}

	*((volatile char *) dest) = (char )data;

	/* re-enable interrupts if necessary */
	if (flag)
		enable_interrupts ();

	/* data polling for D7 */
	start = get_timer (0);

	if ((info->flash_id < FLASH_AMD_COMP) || 
		((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) ||
		((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_MX)) {

		while ((*((volatile char *) dest) & 0x80) !=
		       (data & 0x80) || *(volatile char *)dest ^ *(volatile char *)dest) {
			if (get_timer (start) > CONFIG_SYS_FLASH_WRITE_TOUT) {
				printf ("timeout\n");
				return (1);
			}
		}
	} 

	return (0);
}


