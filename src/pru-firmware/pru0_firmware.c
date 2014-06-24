#include "pru_firmware.h"

#define EVENT_BIT		21
#define IF_EVENT		PINTC_SRSR0 & (1 << EVENT_BIT)
#define CLEAR_EVENT		(PINTC_SRSR0 = EVENT_BIT)

/* sys calls ids*/
#define SYS_DEBUG	0
#define SYS_INIT	1
#define SYS_EXEC	2
#define SYS_ABRT	3
#define SYS_STAT	4
#define SYS_INST	5

#define false	0
#define true	1

/* base address pointer of the instruction stream */
u32 *shm_base = 0;

/* the compiled 32 bit instruction */
u32 single_command = 0;

/* pointer to current instruction in terms of offset */
int inst_pointer = 0;

/* boolean value, true if there is a botspeak script under execution */
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
			/*
				arg0 == 0 : pause
				arg0 == 1 : start/continue
			*/
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

		case SYS_ABRT:
			is_executing = false;
			inst_pointer = 0;
		break;

		case SYS_STAT:
		/* returns the status of execution i.e. executing or not */
			return is_executing;
		break;
		
		case SYS_INST:
		/* an single instruction has been recieved for execution */
			single_command = (u32)arg0;
		break;
		
		default:
			return 1; /* error case - unknown instruction in memory */
		break;
	}

	return 123;
}

/* make this an inline function? */
void check_event(void)
{
	if( IF_EVENT ){
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
	u32 inst;
	if(!single_command){
		PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
		inst = *(shm_base + inst_pointer);
		PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
		inst_pointer++;
	}

	else {
		inst = single_command;
		single_command = 0;
	}

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
		break; 
	}

}

int main()
{
	__R30 = 0x00000000; //set all pins to low
	
	PRUCFG_ENABLE_GLOBAL; //enable global access

	/* configure timer */

	/* activate the timer, set default inc to 1, set compensation inc to 1 (though this is never used)*/
	PIEP_GLOBAL_CFG = GLOBAL_CFG_CNT_ENABLE	| GLOBAL_CFG_DEFAULT_INC(1) | GLOBAL_CFG_CMP_INC(1);

	/* clear the counter status bit for CMP0 register, i.e. PIEP_CMP_STATUS[0] */
	PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0); /* clear the interrupt */
	
	/* Compare registers enable, enables signal from CMP0*/
	PIEP_CMP_CFG |= CMP_CFG_CMP_EN(0);

	/* done configuring timer*/
	
	/* TEST the timer */	
	
	//set 4 sec timeout 
	PIEP_CMP_CMP0 = 0x2faf0800;
	
	while(!(PIEP_CMP_STATUS & 1))
	{
		//wait till counter reaches 0x2faf0800 (or 4 sec)
	}
	
	__R30 = 0xFFFFFFFF; //set all pins to high

	while (1) {}

/*
	while(1)
	{
		check_event();
		if (is_executing) //or if single_cmd
			execute_instruction();
	}

	return 0;
*/
}
