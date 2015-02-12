#ifndef __LS2F_H
#define __LS2F_H

#include <asm/addrspace.h>

#define uncached(x) KSEG1ADDR(x)
#define tobus(x)    (((unsigned long)(x)&0x1fffffff) | 0x80000000)

#endif
