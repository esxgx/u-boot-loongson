/*
 * (C) Copyright 2009
 * Yanhua lemote Software Engineering, <yanh@lemote.com>
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
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
#include <command.h>
#include <netdev.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>
#include <asm/io.h>

int	in8(int addr) { return inb(addr); }
void out8(unsigned int addr, u8 val) { return outb(val, addr); }

#if 0
#define MAX_ENVS 10
#define MAX_ENVSIZE 0x400 

void init_env(char *env[], char *envs)
{
	char *p, *value;

	p = envs;
	if ((value = getenv("busclock"))) {
		strcat(p, "busclock=");
		strcat(p, value);
		env[0] = p;
		p += strlen(p) + 1;
	}

	if ((value = getenv("cpuclock"))) {
		strcat(p, "cpuclock=");
		strcat(p, value);
		env[1] = p;
		p += strlen(p) + 1;
	}

	if ((value = getenv("memsize"))) {
		strcat(p, "memsize=");
		strcat(p, value);
		env[2] = p;
		p += strlen(p) + 1;
	}
	
	if ((value = getenv("highmemsize"))) {
		strcat(p, "highmemsize=");
		strcat(p, value);
		env[3] = p;
		p += strlen(p) + 1;
	}

}

unsigned long do_go_exec (ulong (*entry)(int, char *[], char*[]), int argc, char *argv[])
{
	char *env[MAX_ENVS], envs[MAX_ENVSIZE];
	
	memset(env, 0, sizeof(env));
	memset(envs, 0, sizeof(envs));

	init_env(env, envs);

	return entry(argc, argv, env);
}

unsigned long do_bootelf_exec (ulong (*entry)(int, char *[], char*[]), int argc, char *argv[]) 
{
	char *env[MAX_ENVS], envs[MAX_ENVSIZE];

	memset(env, 0, sizeof(env));
	memset(envs, 0, sizeof(envs));

	init_env(env, envs);

	return entry(argc, argv, env);
}
#endif

#if 0
void __attribute__((weak)) _machine_restart(void)
{
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	_machine_restart();

	fprintf(stderr, "*** reset failed ***\n");
	return 0;
}
#endif

void flush_cache(ulong start_addr, ulong size)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (start_addr + size + lsize -1 ) & ~(lsize - 1);

	addr = start_addr & ~(lsize - 1);
	while (1) {
		mips_cache(INDEX_WRITEBACK_INV_SD, (void *)addr);
		mips_cache(INDEX_WRITEBACK_INV_SD, (void *)(addr|1));
		mips_cache(INDEX_WRITEBACK_INV_SD, (void *)(addr|2));
		mips_cache(INDEX_WRITEBACK_INV_SD, (void *)(addr|3));
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void flush_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	const void *addr = (const void *)(start_addr & ~(lsize - 1));
	const void *aend = (const void *)((stop - 1) & ~(lsize - 1));

	while (1) {
		mips_cache(HIT_WRITEBACK_INV_SD, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

void invalidate_dcache_range(ulong start_addr, ulong stop)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	const void *addr = (const void *)(start_addr & ~(lsize - 1));
	const void *aend = (const void *)((stop - 1) & ~(lsize - 1));

	while (1) {
		mips_cache(HIT_INVALIDATE_SD, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

#if 0
void write_one_tlb(int index, u32 pagemask, u32 hi, u32 low0, u32 low1)
{
	write_c0_entrylo0(low0);
	write_c0_pagemask(pagemask);
	write_c0_entrylo1(low1);
	write_c0_entryhi(hi);
	write_c0_index(index);
	tlb_write_indexed();
}
#endif

int cpu_eth_init(bd_t *bis)
{
	return 0;
}
