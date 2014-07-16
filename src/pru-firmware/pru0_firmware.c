#include "pru_firmware.h"
#include "pru0_firmware.h"

int var_loc[128];
void wait(int);

/*instruction handlers*/

void dio_handler(int opcode, u32 inst)
{
	int val1, val2;
	if(opcode == SET_DIO_a){
	/* SET DIO[c/v], c/v */

		val1 = GET_BIT(inst, 23) ? var_loc[GET_BYTE(inst, 1)]: GET_BYTE(inst, 1);
		val2 = GET_BIT(inst, 22) ? var_loc[GET_BYTE(inst, 0)]: GET_BYTE(inst, 0);
	}

	else if (opcode == SET_DIO_b){
	/* SET DIO[c], arr[v] */
		val1 = GET_BYTE(inst, 2);
		int addr = GET_BYTE(inst, 1) + GET_BYTE(inst, 0);
		val2 = var_loc[addr];
	}

	else {
	/* SET DIO[v], arr[v] */
		val1 = var_loc[GET_BYTE(inst,2)];
		int addr = GET_BYTE(inst, 1) + GET_BYTE(inst, 0);
		val2 = var_loc[addr];
	}

	/* set hi*/
	if(val2 && (val1 < MAX_DIO)){ 
        	__R30 = __R30 | ( 1 << val1);
        }

	/* set low*/
        else{ 
        	__R30 = __R30 & ~( 1 << val1);
        }

}

void set_handler(int opcode, u32 inst)
{
	int val1, val2;

	if(opcode == SET_32_a){
	/* SET V, C */
		val1 = GET_BYTE(inst,2);
		val2 = inst & 0xffff; //16 bit constant
		var_loc[val1] = val2;
	}

	else if(opcode == SET_32_b){
	/* SET V, V */
		val1 = GET_BYTE(inst,2);
		val2 = GET_BYTE(inst,0);
		var_loc[val1] = var_loc[val2];
	}

	else{
	/* SET V/Arr[v], C/V/Arr[v] */
		int op = GET_BYTE(inst, 2) >> 6;

		//if op == 1, operand1 is a variable; else it is a Arr[v] type
		val1 = (op == 1) ? GET_BYTE(inst, 0): (GET_BYTE(inst,1) + GET_BYTE(inst, 0));
		u32 old_inst = inst;

		//get the next 32 bits of the inst
		PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
                inst = *(shm_base + inst_pointer);
                PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
                inst_pointer++;
		
		op = ((GET_BYTE(old_inst, 2) >> 4) & 3); //get bits 4 and 5 of byte2

		if (op == 0){
		//operand2 is of type C
			val2 = inst & 0xFF;
			var_loc[val1] = val2;
		}

		if (op == 1){
		//operand2 is of type V
			val2 = GET_BYTE(inst,0);
			var_loc[val1] = var_loc[val2];
		}

		else{
		//second operand is of type Arr[v]
			val2 = GET_BYTE(inst, 1) + GET_BYTE(inst, 0);
			var_loc[val1] = var_loc[val2];
		}
	}
}

void wait_handler(int opcode, u32 inst)
{
	int op = (GET_BYTE(inst, 2) >> 6);
	int val;
	if(op == 0){
	/* WAIT c*/
		val = inst & 0xFFFF;
	}

	else if (op == 1){
	/* WAIT v*/
		val = var_loc[GET_BYTE(inst, 0)];
	}

	else{
	/* WAIT Arr[v] */
		val = var_loc[GET_BYTE(inst,1) + GET_BYTE(inst, 0)];
	}

	wait(val);
}

//this can actually be merged in wait_handler 
//with a simple if stmnt in the fnc
void goto_handler(int opcode, u32 inst)
{
        int op = (GET_BYTE(inst, 2) >> 6);
        int val;
        if(op == 0){
        /* GOTO c*/
                val = inst & 0xFFFF;
        }

        else if (op == 1){
        /* GOTO v*/
                val = var_loc[GET_BYTE(inst, 0)];
        }

        else{
        /* GOTO Arr[v] */
                val = var_loc[GET_BYTE(inst,1) + GET_BYTE(inst, 0)];
        }

        inst_pointer = val;
}

void if_handler(int opcode, u32 inst)
{
	/* IF (a <cond> b) GOTO c 
		a => C/V/Arr[V] (byte1, byte0)
		b => C/V/Arr[V] (byte5, byte4)
		c => C/V/Arr[V] (byte7, byte6)
	*/
	int val1, val2, val3;
	int op = GET_BYTE(inst, 2) >> 6;
	int rel_op, res;
	
	rel_op = opcode & 0x0F;

	if(op == 0){
	/* operand1 is of type C */
		val1 = GET_BYTE(inst, 0);
	}

	else if(op == 1){
	/* operand1 is of type V */
		val1 = var_loc[GET_BYTE(inst, 0)];
	}

	else{
	/* operand1 is of type Arr[v] */
		val1 = var_loc[GET_BYTE(inst, 1) + GET_BYTE(inst, 0)];
	}

	u32 old_inst = inst; //old_inst is the 1st 32 bits of the inst.
	
	//get the next 32 bits of the inst
	PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
	inst = *(shm_base + inst_pointer);
	PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
	inst_pointer++;
	
	/*** GET val2 from second part of the inst ***/

        op = (GET_BYTE(old_inst, 2) >> 4) & 3;

        if(op == 0){
        /* operand2 is of type C */
                val2 = GET_BYTE(inst, 0);
        }

        else if(op == 1){
        /* operand2 is of type V */
                val2 = var_loc[GET_BYTE(inst, 0)];
        }

        else{
        /* operand2 is of type Arr[v] */
                val2 = var_loc[GET_BYTE(inst, 1) + GET_BYTE(inst, 0)];
        }

        /*** GET val3 from second part of the inst ***/

        op = (GET_BYTE(old_inst, 2) >> 2) & 3;

        if(op == 0){
        /* operand3 is of type C */
                val3 = GET_BYTE(inst, 2);
        }

        else if(op == 1){
        /* operand3 is of type V */
                val3 = var_loc[GET_BYTE(inst, 2)];
        }

        else{
        /* operand3 is of type Arr[v] */
                val3 = var_loc[GET_BYTE(inst, 3) + GET_BYTE(inst, 2)];
        }
	
	/* calculate the result based on rel_op 
		==	0
		!=	1
		>=	2
		<=	3
		>	4
		<	5
	*/
	switch(rel_op){
		case 0:
			res = (val1 == val2);
		break;

                case 1:
                        res = (val1 != val2);
                break;

                case 2:
                        res = (val1 >= val2);
                break;

                case 3:
                        res = (val1 <= val2);
                break;

                case 4:
                        res = (val1 > val2);
                break;

                case 5:
                        res = (val1 < val2);
                break;

		default:
			res = 0;
		break;
	}

	if(res){
		inst_pointer = val3;
	}

}

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
			is_waiting = false;
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
		
		case SYS_INST_32:
		/* an single 32 bit instruction has been recieved for execution */
			single_command = (u32)arg0;
		break;
		
		default:
			return 0; /* error case - unknown instruction in memory */
		break;
	}

	return 1;
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

	int opcode = inst >> 24; //most significant byte contains the cmd 

	switch(opcode){

		case SET_DIO_a:
		case SET_DIO_b:
		case SET_DIO_c:
			dio_handler(opcode, inst);
		break;

		case SET_32_a:
		case SET_32_b:
		case SET_64:
			set_handler(opcode, inst);
		break;
		
		case WAIT:
			wait_handler(opcode, inst);
		break;
		
		case GOTO:
			goto_handler(opcode, inst);
		break;
		
		case IF_EQ:
		case IF_NEQ:
		case IF_GTE:
		case IF_LTE:
		case IF_GT:
		case IF_LT:
			if_handler(opcode, inst);
		break;
		
		default:
		/*no op*/
		break;
		
/*
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
*/
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
