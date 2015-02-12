/*
 *  Cache-handling routined for MIPS CPUs
 *
 *  Copyright (c) 2015	Hsiang Kao <0xe0e1e@gmail.com>
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
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

#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>

#define Index_Store_Tag_D			0x05
#define Index_Invalidate_I			0x00
#define Index_Writeback_Inv_D			0x01
#define Index_Store_Tag_S			0x0b
#define Index_Writeback_Inv_S			0x03

/*******************************************************************************
*
* mips_cache_reset - low level initialisation of the caches
*
* This routine initialises the caches to ensure that they
* have good parity.  It must be called by the ROM before any cached locations
* are used to prevent the possibility of data with bad parity being written to
* memory.
* To initialise the instruction cache it is essential that a source of data
* with good parity is available. This routine
* will initialise an area of memory starting at location zero to be used as
* a source of parity.
*
* RETURNS: N/A
*
*/
LEAF(mips_cache_reset)
	.set push
	.set noreorder
####part 2####
cache_detect_2way:
	mfc0	t4, CP0_CONFIG
	andi	t5, t4, 0x0e00
	srl	t5, t5, 9
	andi	t6, t4, 0x01c0
	srl	t6, t6, 6
	addiu	t6, t6, 11
	addiu	t5, t5, 11
	addiu	t4, $0, 1
	sllv	t6, t4, t6
	srl	t6,1
	sllv	t5, t4, t5
	srl	t5,1
	addiu	t7, $0, 2
####part 3####
	lui	a0, 0x8000
	addu	a1, $0, t5
	addu	a2, $0, t6
cache_init_d2way:
#a0=0x80000000, a1=icache_size, a2=dcache_size
#a3, v0 and v1 used as local registers
	mtc0	$0, CP0_TAGHI
	addu	v0, $0, a0
	addu	v1, a0, a2
1:	slt	a3, v0, v1
	beq	a3, $0, 1f
	nop
	mtc0	$0, CP0_TAGLO
	cache	Index_Store_Tag_D, 0x0(v0)
	mtc0	$0, CP0_TAGLO
	cache	Index_Store_Tag_D, 0x1(v0)
	mtc0	$0, CP0_TAGLO
	cache   Index_Store_Tag_D, 0x2(v0)
	mtc0	$0, CP0_TAGLO
	cache   Index_Store_Tag_D, 0x3(v0)
	beq	$0, $0, 1b
	addiu	v0, v0, 0x20

#if 1
1:
cache_init_l24way:
        mtc0    $0, CP0_TAGHI
        addu    v0, $0, a0
        addu    v1, a0, 128*1024
1:      slt     a3, v0, v1
        beq     a3, $0, 1f
        nop
        mtc0    $0, CP0_TAGLO
        cache   Index_Store_Tag_S, 0x0(v0)
        mtc0    $0, CP0_TAGLO
        cache   Index_Store_Tag_S, 0x1(v0)
        mtc0    $0, CP0_TAGLO
        cache   Index_Store_Tag_S, 0x2(v0)
        mtc0    $0, CP0_TAGLO
        cache   Index_Store_Tag_S, 0x3(v0)
        beq     $0, $0, 1b
        addiu   v0, v0, 0x20


1:
cache_flush_4way:
	addu	v0, $0, a0
	addu	v1, a0, 128*1024
1:	slt	a3, v0, v1
	beq	a3, $0, 1f
	nop
	cache	Index_Writeback_Inv_S, 0x0(v0)
	cache	Index_Writeback_Inv_S, 0x1(v0)
	cache	Index_Writeback_Inv_S, 0x2(v0)
	cache	Index_Writeback_Inv_S, 0x3(v0)
	beq	$0, $0, 1b
	addiu	v0, v0, 0x20
# endif

1:
cache_flush_i2way:
	addu	v0, $0, a0
	addu	v1, a0, a1
1:	slt	a3, v0, v1
	beq	a3, $0, 1f
	nop
	cache	Index_Invalidate_I, 0x0(v0)
#	cache	Index_Invalidate_I, 0x1(v0)
#	cache	Index_Invalidate_I, 0x2(v0)
#	cache	Index_Invalidate_I, 0x3(v0)
	beq	$0, $0, 1b
	addiu	v0, v0, 0x20
1:
cache_flush_d2way:
	addu	v0, $0, a0
	addu	v1, a0, a2
1:	slt	a3, v0, v1
	beq	a3, $0, 1f
	nop
	cache	Index_Writeback_Inv_D, 0x0(v0)
	cache	Index_Writeback_Inv_D, 0x1(v0)
	cache	Index_Writeback_Inv_D, 0x2(v0)
	cache	Index_Writeback_Inv_D, 0x3(v0)
	beq	$0, $0, 1b
	addiu	v0, v0, 0x20
1:
cache_init_finish:
	nop
	j	ra
	nop
	.set	pop
END(mips_cache_reset)

#define COP_0_TLB_PG_MASK	$5
#define COP_0_TLB_HI			$10
#define COP_0_TLB_LO0		$2
#define COP_0_TLB_LO1		$3
#define COP_0_TLB_INDEX		$0
#define PG_SIZE_16K	0x00006000

LEAF(__asm_invalidate_tlb_all)
	li	a3, 0			# First TLB index.

	mtc0 $0, $6   #Clear Wired Register
	li  a2, PG_SIZE_16K
	MTC0   a2, COP_0_TLB_PG_MASK   # Whatever...
	lui  a0, 0x8000

1:	sll    t0, a3 ,15
	or     t0, a0
	MTC0   t0, COP_0_TLB_HI		# Clear entry high
	MTC0   zero, COP_0_TLB_LO0	# Clear entry low0.
	MTC0   zero, COP_0_TLB_LO1	# Clear entry low1.

	mtc0    a3, COP_0_TLB_INDEX	# Set the index.
	addiu	a3, 1
	li	a2, 64
	nop
	nop
	tlbwi						# Write the TLB

	bne	a3, a2, 1b
	nop

	j	ra
	nop
END(__asm_invalidate_tlb_all)
