typedef int u32;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

__far volatile char C0[0x300] __attribute__((cregister("C0", far)));
#define PINTC(_reg) \
	(*(volatile u32 *)((char *)&C0[_reg]))

#define PINTC_SISR		PINTC(0x0020)
#define PINTC_SICR		PINTC(0x0024)
#define PINTC_SRSR0		PINTC(0x0200)
#define PINTC_SRSR1		PINTC(0x0204)
#define PINTC_SECR0		PINTC(0x0280)
#define PINTC_SECR1		PINTC(0x0284)

__far volatile char C4[0x100] __attribute__((cregister("C4", near)));	/* PRUCFG */

#define PRUCFG(_reg) \
	(*(volatile u32 *)((char *)&C4 + (_reg)))

#define PRUCFG_SYSCFG		PRUCFG(0x0004)
#define SYSCFG_STANDBY_INIT	(1 << 4)
#define PRUCFG_ENABLE_GLOBAL	PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT)
#define PRUCFG_DISABLE_GLOBAL	PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT

#define false	0
#define true	1

