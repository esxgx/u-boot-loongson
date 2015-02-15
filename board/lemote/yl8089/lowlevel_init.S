/*
 *  Memory sub-system initialization code for Loongson-2F board YL8089.
 *
 *  Copyright (c) 2009  yanhua <yanh@lemote.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 */

#include <asm/asm.h>

#include <config.h>
#include <asm/regdef.h>

#include "bonito.h"
#include "ns16550.h"

#include "cs5536.h"

#ifdef CONFIG_CPU_UART
#define PRINTSTR(x) \
    .rdata;98: .asciz x; .text; la a0, 98b; bal stringserial; nop
#else
#define PRINTSTR(x)
#endif

// These defines should moved to CPU specific files
#define UNCACHED_MEMORY_ADDR 0xa0000000
#define CACHED_MEMORY_ADDR   0x80000000

#define CACHED_TO_PHYS(x)   ((x) & 0x1fffffff)
#define PHYS_TO_CACHED(x)   ((x) | CACHED_MEMORY_ADDR)
#define UNCACHED_TO_PHYS(x)     ((x) & 0x1fffffff)
#define PHYS_TO_UNCACHED(x)     ((x) | UNCACHED_MEMORY_ADDR)
#define VA_TO_CINDEX(x)     ((x) & 0xffffff | CACHED_MEMORY_ADDR)
#define CACHED_TO_UNCACHED(x)   (PHYS_TO_UNCACHED(CACHED_TO_PHYS(x)))
#define CFGADDR(idsel,function,reg) ((1<<(11+(idsel)))+((function)<<8)+(reg))

#define PCI_CFG_SPACE BONITO_PCICFG_BASE

// s4 is used as bonito base address
#define bonito s4

#define PCICONF_WRITEW(dev,func,reg,data) \
    li      a0,CFGADDR(dev,func,reg); \
	li      a1,PHYS_TO_UNCACHED(PCI_CFG_SPACE); \
	and     a2,a0,0xffff; \
	or      a1,a2; \
	srl     a0,16; \
	li      a2,BONITO_BASE+BONITO_PCIMAP_CFG; \
	sw      a0,BONITO_PCIMAP_CFG(bonito); \
    lw      zero,BONITO_PCIMAP_CFG(bonito); \
    or		a0,zero,data; \
	sw      a0,(a1);

#define	PCICONF_READW(dev, func, reg) \
	li		a0, CFGADDR(dev, func, reg); \
	li		a1, PHYS_TO_UNCACHED(PCI_CFG_SPACE); \
	and		a2, a0, 0xffff; \
	or		a1, a2; \
	srl		a0, 16; \
	li		a2, BONITO_BASE+BONITO_PCIMAP_CFG; \
	sw		a0, BONITO_PCIMAP_CFG(bonito); \
	lw		zero, BONITO_PCIMAP_CFG(bonito); \
	lw		a0, (a1);

#define	CS5536_MSR_WRITE(reg, lo, hi) \
	PCICONF_WRITEW(PCI_IDSEL_CS5536, 0, 0xF4, reg); \
	PCICONF_WRITEW(PCI_IDSEL_CS5536, 0, 0xF8, lo); \
	PCICONF_WRITEW(PCI_IDSEL_CS5536, 0, 0xFC, hi);

#define	CS5536_MSR_READ(reg) \
	PCICONF_WRITEW(PCI_IDSEL_CS5536, 0, 0xF4, reg); \
	PCICONF_READW(PCI_IDSEL_CS5536, 0, 0xF8); \
	PCICONF_READW(PCI_IDSEL_CS5536, 0, 0xFC);

#define	GPIO_HI_BIT(bit, reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	lw	v0, 0(v1);  \
	li	a0, 0x0001; \
	sll	a0, bit;    \
	or	v0, a0;     \
	sll	a0, 16;     \
	not	a0;         \
	and	v0, a0;     \
	sw	v0, 0(v1);

#define	GPIO_LO_BIT(bit, reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	lw	v0, 0(v1);  \
	li	a0, 0x0001; \
	sll	a0, (bit + 16); \
	or	v0, a0;     \
	srl	a0, 16;     \
	not	a0;         \
	and	v0, a0;     \
	sw	v0, 0(v1);

/*
 * io space byte access
 */
#define	IO_READ_BYTE(reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	lbu	v0, 0(v1);

#define	IO_WRITE_BYTE(reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	sb	v0, 0(v1);

#define	IO_READ_DWORD(reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	lw	v0, 0(v1);

#define	IO_WRITE_DWORD(reg) \
	lui	v1, 0xbfd0; \
	ori	v1, reg;    \
	sw	v0, 0(v1);

#define PCI_IDSEL_CS5536			14

#ifdef CONFIG_SB_CS5536

#define	DIVIL_BASE_ADDR		0xB000
#define	SMB_BASE_ADDR		(DIVIL_BASE_ADDR | 0x320)
#define	GPIO_BASE_ADDR		(DIVIL_BASE_ADDR | 0x000)
#define	MFGPT_BASE_ADDR		(DIVIL_BASE_ADDR | 0x280)
#define	PMS_BASE_ADDR		(DIVIL_BASE_ADDR | 0x200)
#define	ACPI_BASE_ADDR		(DIVIL_BASE_ADDR | 0x2c0)

#define	HW_POWER_OFF_TIME	0x18000		// Hardware power off delay time is 3s
#define	UART1_TX	8
#define	UART1_RX	9
#define	UART2_TX	4
#define	UART2_RX	3
#define	I8259_IRQ	12
#define	MIDE_IRQ	2
#define	GPIO_5		5
#define	SMB_CLK		14
#define	SMB_DATA	15
#define	PWR_BUT		(28 - 16)	// WE USE HIGH GPIO BANK FOR 28

.macro	cs5536_init
	.set push
	.set noreorder
	/* Set id select to default 0xe */
	li		v0, 0xbfd00000
	li		v1, 0x02000000
	sw		v1, (v0)

2:
	/* Read device id and vendor id, 0x208f1022 is expected */
	PCICONF_READW(PCI_IDSEL_CS5536, 0, 0x00)
	li		a1, 0x208f1022
	beq	a0, a1, 1f
	nop

	b		2b
	nop

1:
	// Enable MSR access
	PCICONF_WRITEW(PCI_IDSEL_CS5536, 0, 0xf0, 0x01);

	//
	// MSR ACCESS
	//

	// Activate all ports
	CS5536_MSR_WRITE((CS5536_GLIU_MSR_BASE | 0x81), 0x0000ffff, 0x0);

	// Global control setting
	CS5536_MSR_WRITE((CS5536_SB_MSR_BASE | 0x10), 0x00000003, 0x44000030);

	// DD LBAR function checking
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x0b), SMB_BASE_ADDR, 0xf001);
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x0c), GPIO_BASE_ADDR, 0xf001);
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x0f), PMS_BASE_ADDR, 0xf001);

	//
	// IO SPACE ACCESS
	//

	// Enable com1 and place legacy IO to 0x3f8
	// hi : bit+16 = 0 and bit = 1
	// lo : bit+16 = 1 and bit = 0
#define USE_CS5536_UART1

#if defined(USE_CS5536_UART2) || defined(USE_CS5536_UART1)
	/* config uart1 as 0x3f8, uart2 as 0x2f8 */
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x14), 0x04570003, 0x0);

	/* config uart1 legacy configuration */
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x3a), 0x2, 0x0);

	/* config uart1 gpio */
	GPIO_HI_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_HI_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);
	GPIO_HI_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_PU_EN);
	GPIO_LO_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_LO_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);
	GPIO_LO_BIT(UART1_TX, GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);

	GPIO_HI_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_PU_EN);
	GPIO_HI_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_HI_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);
	GPIO_LO_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_LO_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);
	GPIO_LO_BIT(UART1_RX, GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);

	/* disable the uart2 pin as input for nas board debug. */
	GPIO_HI_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_HI_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_IN_EN);
#if 0
	/* config uart2 as 0x3f8 */
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x14), 0x04700003, 0x0);
#endif
	/* config uart2 legacy configuration */
	CS5536_MSR_WRITE((CS5536_DIVIL_MSR_BASE | 0x3e), 0x2, 0x0);

	/* config uart2 gpio */
	GPIO_HI_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_OUT_OD_EN);
	GPIO_HI_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);
	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);
//	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_PU_EN);
//	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_PD_EN);
	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_LO_BIT(UART2_TX, GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);

//	GPIO_HI_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_PU_EN);
	GPIO_HI_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_HI_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);
	GPIO_LO_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_LO_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);
	GPIO_LO_BIT(UART2_RX, GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);

#endif

	/* fixup the ide and i8259 irq line. */
	/* IDE irq init */
	GPIO_HI_BIT(MIDE_IRQ,	GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_HI_BIT(MIDE_IRQ,	GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);
	GPIO_LO_BIT(MIDE_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_LO_BIT(MIDE_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);
	GPIO_LO_BIT(MIDE_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);

	/* I8259 irq init*/
	GPIO_HI_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_EN);
	GPIO_HI_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_AUX1_SEL);
	GPIO_LO_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_OD_EN);
	GPIO_LO_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_OUT_AUX2_SEL);
	GPIO_LO_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_IN_EN);
	GPIO_LO_BIT(I8259_IRQ,	GPIO_BASE_ADDR | GPIOL_IN_AUX1_SEL);

	/* PWN_BUT gpio init */
	GPIO_HI_BIT(PWR_BUT,	GPIO_BASE_ADDR | GPIOH_IN_EN);
	GPIO_HI_BIT(PWR_BUT,	GPIO_BASE_ADDR | GPIOH_IN_AUX1_SEL);
	GPIO_LO_BIT(PWR_BUT,	GPIO_BASE_ADDR | GPIOH_OUT_EN);
	GPIO_LO_BIT(PWR_BUT,	GPIO_BASE_ADDR | GPIOH_OUT_AUX1_SEL);
	GPIO_LO_BIT(PWR_BUT,	GPIO_BASE_ADDR | GPIOH_OUT_AUX2_SEL);

	.set	pop
	.endm
#endif


LEAF(lowlevel_init)
	.set	push
	.set	noreorder
	li		bonito, 0xbfe00000
	move	s0, ra

#ifdef CONFIG_CPU_UART
	bal		initserial
	nop
#endif

	PRINTSTR("Initializing...\r\n");

#ifdef CONFIG_SB_CS5536
	// Init CS5536
	cs5536_init
	nop
#endif

	PRINTSTR("DDR2 init...\r\n");

// DDR2 controller config registers address
#define DDR2_CONF_CTL_REG_BASE	0xaffffe00

	// Enable DDR2 config registers
	li 		t2, 0xbfe00180
	lw		t1, (t2)
	and	t1, 0x4ff			/* disable ddr_buffer_cpu according to beijing suggestion. */
	#and	t1, t1, 0x6ff			/* this is the default setting. */
        #or	t1, t1, 0x100		/* disable ddr2 config, just for debugging. */
	sw		t1, (t2)

	// DDR2 configuation begins
	li		t0, DDR2_CONF_CTL_REG_BASE
	li		t1, 29
	PTR_LA	t2, ddr2_reg_data

mem_reg_write:
	ld		t3, (t2)
	sd		t3, (t0)
	add	t2, t2, 8
	add	t0, t0, 0x10
	sub	t1, t1, 1
	bgtz	t1, mem_reg_write
	nop

	/* Start to config */
	li			t0, DDR2_CONF_CTL_REG_BASE
	PTR_LA		t1, DDR2_CTL_start_DATA_LO
#if 0
	lw  t1, 0x4(t1)
	sw  t1, 0x34(t0)
#else
	.word 0xdd290000
	.word 0xfd090030
#endif
	// DDR2 configuration ends

	// read ddr2 registers for debugging
	// Wait DLL locked
        li	t0, DDR2_CONF_CTL_REG_BASE

not_locked:
        ld	t1, 0x10(t0)
        andi    t1, 0x01
        beqz    t1, not_locked
        nop
	PRINTSTR("DDR2 DLL locked\r\n");
	
	// Disable DDR2 config registers
	li	t0, 0xbfe00180
        lw	t1, (t0)
        or	t1, t1,0x100
        sw	t1, (t0)

	// For rom access speed update.
	li		t2, 0xbfe00108
	lw		t1, (t2)
	li		a0, 0xffffff83
	and	t1, a0
	or		t1, 0x0c
	sw		t1, (t2)

	li		t2, 0xbfe00108
	lw		t1, (t2)
	li		a0, 0xffffe0ff
	and	t1, a0
	ori		t1, 0x0f00
	sw		t1, (t2)

#if 0
	/* Add memory test code here */
#endif

	bal		__asm_invalidate_tlb_all
	nop

	move ra, s0
	jr   ra
	nop
	.set	pop
END(lowlevel_init)



initserial:
	la 	v0, COMMON_COM_BASE_ADDR
1:
	li 	v1, FIFO_ENABLE|FIFO_RCV_RST|FIFO_XMT_RST|FIFO_TRIGGER_1
	sb  v1, NSREG(NS16550_FIFO)(v0)
	li  v1, CFCR_DLAB                  #DLAB
	sb  v1, NSREG(NS16550_CFCR)(v0)
	li  v1, NS16550HZ/(16*CONS_BAUD)   #set BRDL
	sb  v1, NSREG(NS16550_DATA)(v0)
	srl v1, 8
	sb  v1, NSREG(NS16550_IER)(v0)     #set BRDH
	li  v1, CFCR_8BITS                 #8bit
	sb  v1, NSREG(NS16550_CFCR)(v0)
	li  v1, MCR_DTR|MCR_RTS
	sb      v1, NSREG(NS16550_MCR)(v0)
	li      v1, 0x0
	sb      v1, NSREG(NS16550_IER)(v0)

	move    v1, v0
	la      v0, COMMON_COM_BASE_ADDR
	bne     v0, v1, 1b
	nop

	j       ra
	nop

stringserial:
	.set noreorder
	move	a2, ra
	move 	a1, a0
	lbu	a0, 0(a1)
1:
	beqz	a0, 2f
	nop
	bal	tgt_putchar
	addiu	a1, 1
	b	1b
	lbu	a0, 0(a1)

2:
	j	a2
	nop
	.set reorder

LEAF(hexserial)
	.set noreorder
	move	a2, ra
	move	a1, a0
	li	a3, 7
1:
	rol	a0, a1, 4
	move	a1, a0
	and	a0, 0xf
	la	v0, hexchar
	addu	v0, a0
	bal	tgt_putchar
	lbu	a0, 0(v0)

	bnez	a3, 1b
	addu	a3, -1

	j	a2
	nop
	.set reorder
END(hexserial)

tgt_putchar:
	la	v0, COMMON_COM_BASE_ADDR
1:
	lbu	v1, NSREG(NS16550_LSR)(v0)
	and	v1, LSR_TXRDY
	beqz	v1, 1b
	nop

	sb	a0, NSREG(NS16550_DATA)(v0)
	move v1,v0
	la	v0, COMMON_COM_BASE_ADDR
	bne v0,v1,1b
	nop
	j	ra
	nop

	.data
hexchar:	.ascii  "0123456789abcdef"

/* expanation of "old" is the reserved configuration data for 2f motherboard. */
	.align 5
ddr2_reg_data:
	DDR2_CTL_00_DATA_LO: .word 0x00000101
	DDR2_CTL_00_DATA_HI: .word 0x01000100	 #no_concurrentap
	DDR2_CTL_01_DATA_LO: .word 0x00010000
	DDR2_CTL_01_DATA_HI: .word 0x00000001
	DDR2_CTL_02_DATA_LO: .word 0x00000000
	DDR2_CTL_02_DATA_HI: .word 0x01000101
	DDR2_CTL_03_DATA_LO: .word 0x01000000
	DDR2_CTL_03_DATA_HI: .word 0x01010000
	//Felix-2008-1020
	DDR2_CTL_04_DATA_LO: .word 0x01010101
	DDR2_CTL_04_DATA_HI: .word 0x01000202
	DDR2_CTL_05_DATA_LO: .word 0x04050102
	DDR2_CTL_05_DATA_HI: .word 0x00000000
	DDR2_CTL_06_DATA_LO: .word 0x03050203 #800
	DDR2_CTL_06_DATA_HI: .word 0x0a040306 #800
	DDR2_CTL_07_DATA_LO: .word 0x00010a0b
	DDR2_CTL_07_DATA_HI: .word 0x00000400 #800
	DDR2_CTL_08_DATA_LO: .word 0x00000102
	DDR2_CTL_08_DATA_HI: .word 0x00000102
	DDR2_CTL_09_DATA_LO: .word 0x00000000
	DDR2_CTL_09_DATA_HI: .word 0x0000060c #800
	DDR2_CTL_10_DATA_LO: .word 0x3f1f0200 #800
	DDR2_CTL_10_DATA_HI: .word 0x2323233f
	DDR2_CTL_11_DATA_LO: .word 0x23232323
	DDR2_CTL_11_DATA_HI: .word 0x5f7f2323
	DDR2_CTL_12_DATA_LO: .word 0x15000000 #800
	DDR2_CTL_12_DATA_HI: .word 0x002a3c06 #800
	DDR2_CTL_13_DATA_LO: .word 0x002a002a
	DDR2_CTL_13_DATA_HI: .word 0x002a002a
	DDR2_CTL_14_DATA_LO: .word 0x002a002a
	DDR2_CTL_14_DATA_HI: .word 0x002a002a
	DDR2_CTL_15_DATA_LO: .word 0x00000004
	DDR2_CTL_15_DATA_HI: .word 0x00b40020
	DDR2_CTL_16_DATA_LO: .word 0x00000087
	DDR2_CTL_16_DATA_HI: .word 0x000007ff #no_interrupt
	DDR2_CTL_17_DATA_LO: .word 0x0016101f
	DDR2_CTL_17_DATA_HI: .word 0x00000000
	DDR2_CTL_18_DATA_LO: .word 0x00000000
	DDR2_CTL_18_DATA_HI: .word 0x001c0000
	DDR2_CTL_19_DATA_LO: .word 0x00c8006b
	DDR2_CTL_19_DATA_HI: .word 0x28e10002 #800
	DDR2_CTL_20_DATA_LO: .word 0x00c8002f #800
	DDR2_CTL_20_DATA_HI: .word 0x00000000
	DDR2_CTL_21_DATA_LO: .word 0x00030d40 #real
	DDR2_CTL_21_DATA_HI: .word 0x00000000
	DDR2_CTL_22_DATA_LO: .word 0x00000000
	DDR2_CTL_22_DATA_HI: .word 0x00000000
	DDR2_CTL_23_DATA_LO: .word 0x00000000
	DDR2_CTL_23_DATA_HI: .word 0x00000000
	DDR2_CTL_24_DATA_LO: .word 0x00000000
	DDR2_CTL_24_DATA_HI: .word 0x00000000
	DDR2_CTL_25_DATA_LO: .word 0x00000000
	DDR2_CTL_25_DATA_HI: .word 0x00000000
	DDR2_CTL_26_DATA_LO: .word 0x00000000
	DDR2_CTL_26_DATA_HI: .word 0x00000000
	DDR2_CTL_27_DATA_LO: .word 0x00000000
	DDR2_CTL_27_DATA_HI: .word 0x00000000
	DDR2_CTL_28_DATA_LO: .word 0x00000000
	DDR2_CTL_28_DATA_HI: .word 0x00000000
	DDR2_CTL_start_DATA_LO: .word 0x01000000
	DDR2_CTL_start_DATA_HI: .word 0x01010100
