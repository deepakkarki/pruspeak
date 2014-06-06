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

#define INCOMING		1 << 21

__far volatile char C4[0x100] __attribute__((cregister("C4", near)));	/* PRUCFG */

#define PRUCFG(_reg) \
	(*(volatile u32 *)((char *)&C4 + (_reg)))

#define PRUCFG_SYSCFG		PRUCFG(0x0004)
#define  SYSCFG_STANDBY_INIT	(1 << 4)

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4));


static int handle_downcall(u32 id, u32 arg0, u32 arg1, u32 arg2,
		u32 arg3, u32 arg4)
{
	if(id == 0){
		__R30 = 0x00000000;
	}
	else if (id == 1){	
		__R30 = 0x0000FFFF;
	}
	else if (id == 2){
		//__R30 = 0x0000FFFF;
		PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT);
		u32 *base = (u32 *)arg0;
		int val = *base;
		*base = 24;
		__R30 = 0x0000FFFF;
	
	}
	return 123;
}

void check_event(void)
{
	if( PINTC_SRSR0 & INCOMING ){
		PINTC_SICR = 21; //clear the system event
		sc_downcall(handle_downcall);
	}
}

int main()
{
	__R30 = 0x00000000; //set all pins to high

	while(1)
	{
		check_event();
	}
	return 0;
}
