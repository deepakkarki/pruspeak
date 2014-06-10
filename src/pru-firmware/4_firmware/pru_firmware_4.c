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

#define SYS_DEBUG	0
#define SYS_INIT	1
#define SYS_EXEC	2

#define false	0
#define true	1

u32 *shm_base = 0;
int inst_pointer = 0;
int is_executing = false;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4));


static int handle_downcall(u32 id, u32 arg0, u32 arg1, u32 arg2,
		u32 arg3, u32 arg4)
{
	switch(id)
	{
		case SYS_DEBUG:
			if(arg0){
				__R30 = 0x0000FFFF;
			}
			else{
				__R30 = 0x00000000;
			}
		break;

		case SYS_INIT:
			is_executing = 0;
			PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
			shm_base = (u32 *)arg0;
			*shm_base = arg1 * arg1; /*for test purposes - checked by kernel*/
			PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
		break;
		
		case SYS_EXEC:
			if(arg0){
				if (!inst_pointer)
					inst_pointer = 0; // incase it was not executing before, start
							  // else all we want to do is continue
				is_executing = true;
			}
			else{
				is_executing = false;
			}
		break;

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

void wait(int time)
{
        int i;
        for(i=0; i < time; i++)
        {
        }
}

void execute_instruction()
{
	PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
	u32 inst = *(shm_base + inst_pointer);
	PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
	
	inst_pointer++;

	int cmd = inst >> 24; //most significant byte contains the cmd
	int pin; 

	switch(cmd){

		case 1:	//DIO
			pin = inst & 0x7F; //ls 7 bits
			if(inst & 0x80){ //high
				__R30 = __R30 | ( 1 << pin);
			}
			else{ //low
				__R30 = __R30 & ~( 1 << pin);
			}
		break;

		case 2:
			//sleep - 1 sec as of now - default. figure out other way later.
			__delay_cycles(0xbebc200);
			//wait(0x2bfffff);
		break;

		case 3:
			//jump
			inst_pointer = inst & 0xFFFF;
			//max 16 bits of address space for any PRU script
			//directly jump to ls 16 bits.
		break;

		default:
			//do nothing 
	}

}

int main()
{
	__R30 = 0x00000000; //set all pins to low

	while(1)
	{
		check_event();
		if (is_executing)
			execute_instruction();
	}
	return 0;
}
