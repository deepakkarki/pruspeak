#include "pru_firmware.h"
#include "pru0_firmware.h"


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
			is_waiting = false;
			inst_pointer = 0;
		break;

		case SYS_STAT:
		/* returns the status of execution i.e. executing or not */
			return (is_executing | is_waiting);
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
		/* clear the system event */
		PINTC_SICR = 21; 

		/* call the handler to perform the downcall*/
		sc_downcall(handle_downcall);
	}
	

	if(PIEP_CMP_STATUS & 1){

		/* disable the timer */
		PIEP_GLOBAL_CFG &= ~(GLOBAL_CFG_CNT_ENABLE);

		/* reset the timer to 0 (timer is a clear to write reg) */
		PIEP_COUNT = 0XFFFFFFFF;

		/* set CMP0 value back to zero */
		PIEP_CMP_CMP0 = 0;
		
		/* disable events from CMP0*/
       		PIEP_CMP_CFG &= ~(CMP_CFG_CMP_EN(0));

		/* clear the timer event for CMP0 incase it has been set by mistake */
                PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0);

		/* change the current state of the PRU */
		if (is_waiting){
			//there could have been an abort. in case there wasn't :
			is_executing = true;
			is_waiting = false;
		}
	}

}

void wait(int ms)
{	
	/* set the CMP0 reg value */
	PIEP_CMP_CMP0 =  MS * ms;

	/* change execution state of the PRU */
	is_executing = false;
	is_waiting = true;

	/* enable events from CMP0*/
	PIEP_CMP_CFG |= CMP_CFG_CMP_EN(0);

	/* clear the timer event for CMP0 incase it has been already set by mistake */
	PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0);

        /* start the timer */
        PIEP_GLOBAL_CFG |= GLOBAL_CFG_CNT_ENABLE;	
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
			wait(inst & 0x00FFFFFF);
			//__delay_cycles(0xbebc200);
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

void timer_init()
{
        /* set default inc to 1, set compensation inc to 1 (though this is never used)*/
        PIEP_GLOBAL_CFG = GLOBAL_CFG_DEFAULT_INC(1) | GLOBAL_CFG_CMP_INC(1);

        /* Compare registers enable, enables signal from CMP0, on CMP0 --> counter is reset */
        PIEP_CMP_CFG |=  RESET_ON_CMP0_EVENT;
	
	/* clear the counter status bit for CMP0 register, i.e. PIEP_CMP_STATUS[0] */
        PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0); /* clear the interrupt */
}

int main()
{
	
	timer_init();

	while(1)
	{
		check_event();
		if (is_executing || single_command) //or if single_cmd
			execute_instruction();
	}

	return 0;
}
