#include "pru_firmware.h"
#include "pru0_firmware.h"

int var_loc[256];
void wait(int);
int pwm_val = 0;

static void send_ret_value(int val)
{
	//TODO : handle multiple params?
	//TODO : change to sysfs
	/*enable gloabl access*/
	PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); 

	/* write val to the loc pointed to by current ret_pointer */
	*(shm_ret + ret_pointer) = val;

	/*inc the ret_pointer to point to the next empty loc*/
	ret_pointer = (ret_pointer % 1023) + 1; 
	//max buffer space is 4096 => 1023 (free)words.

	/*inc the num_ret_value*/
	*shm_ret = ret_pointer; //ret_pointer is like a stack pointer, 
		//userspace will read this value to figure out till where the buffer has been filled
		//e.g. if the ret_pointer = 10; ARM will read 9 slots uptill 10

	PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;

}

/* to fetch 2nd part of 64 bit instruction */
static u32 get_second_word()
{
	u32 inst;
	
	/* when the 64 bit inst is part of the script */
	if(!single_command){
		PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
		inst = *(shm_code + inst_pointer);
		PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
		inst_pointer++;
	}

	/* when the 64 bit inst is a single_inst */
	else {
		inst = single_command_2;
	}
	
	return inst;
}

/* used to get value of a variable*/
int get_var_val(int addr)
{
	if (addr > MAX_DATA){
		if(addr < AIO_OFF){
		//case DIO
		//240 to 255 -> DIO[0] to DIO[15]
			
			addr -= 1; 
			//this is there since if the code reaces here it is Arr[c/v]
			//so either the userspace compiler or calling pru func here
			//would have added 1. this should be taken care of.

			addr -= DIO_OFF;
			return GET_BIT(__R31, addr);
		}
	}

	else{
		return var_loc[addr];
	}

	return 0;
}

/*instruction handlers*/

void dio_handler(int opcode, u32 inst)
{
	int val1, val2;
	if(opcode == SET_DIO_a){
	/* SET DIO[c/v], c/v */
		
		val1 = GET_BIT(inst, 23) ? var_loc[GET_BYTE(inst, 1)]: GET_BYTE(inst, 1);
		val2 = GET_BIT(inst, 22) ? var_loc[GET_BYTE(inst, 0)]: GET_BYTE(inst, 0);
	}
	
	else{	
		// "SET DIO[c], arr[v]"  orelse "SET DIO[v] , arr[v]"
		val1 = (opcode == SET_DIO_b) ? GET_BYTE(inst, 2) : var_loc[GET_BYTE(inst,2)];
		
		//array size check -- this case same for both case
		int index = var_loc[GET_BYTE(inst, 0)];
		if (var_loc[GET_BYTE(inst,1)] <= index ){
			//error
			if (single_command)
				send_ret_value(0);
			return;
		}
		//if everything okay
		int addr = GET_BYTE(inst, 1) + index + 1;
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
	
	if(single_command)
		send_ret_value(val2 ? 1 : 0);
}

void pwm_handler(int opcode, u32 inst)
{
	//takes in only PWM[c], c. This will be merged with dio later
        int val1, val2;
        if(opcode == SET_PWM_a){
        /* SET PWM[c/v], c/v */

                val1 = GET_BIT(inst, 23) ? var_loc[GET_BYTE(inst, 1)]: GET_BYTE(inst, 1);
                val2 = GET_BIT(inst, 22) ? var_loc[GET_BYTE(inst, 0)]: GET_BYTE(inst, 0);
        }

        else{
                // "SET PWM[c], arr[v]"  orelse "SET PWM[v] , arr[v]"
                val1 = (opcode == SET_PWM_b) ? GET_BYTE(inst, 2) : var_loc[GET_BYTE(inst,2)];

                //array size check -- this case same for both case
                int index = var_loc[GET_BYTE(inst, 0)];
                if (var_loc[GET_BYTE(inst,1)] <= index ){
                        //error
                        if (single_command)
                                send_ret_value(0);
                        return;
                }
                //if everything okay
                int addr = GET_BYTE(inst, 1) + index + 1;
                val2 = var_loc[addr];
        }

        /* set pwm value */
	data_sock->info[PRU1][val1] = val2;
	SIGNAL_EVENT(EV_PRU0_PRU1);
	send_ret_value(val2);
}

void set_handler(int opcode, u32 inst)
{
	int val2, addr1, addr2;

	if(opcode == SET_32_a){
	/* SET V, C */
		addr1 = GET_BYTE(inst,2);
		val2 = inst & 0xffff; //16 bit constant
		var_loc[addr1] = val2;
	        if(single_command)
        	        send_ret_value(val2);

	}

	else if(opcode == SET_32_b){
	/* SET V, V */
		addr1 = GET_BYTE(inst,2);
		addr2 = GET_BYTE(inst,0);
		var_loc[addr1] = get_var_val(addr2);
	        if(single_command)
	                send_ret_value(var_loc[addr1]);

	}

	else{
	/* SET V/Arr[v], C/V/Arr[v] */
		int op = GET_BYTE(inst, 2) >> 6;

		//if op == 1, operand1 is a variable; else it is a Arr[v] type
		if (op == 1){ 
			addr1 = GET_BYTE(inst, 0); 
		}

		//Arr[v]
		else{
			//array size check
	                int index = var_loc[GET_BYTE(inst, 0)];
                	if (var_loc[GET_BYTE(inst,1)] <= index ){
                        	//error 
                        	if (single_command)
                                	send_ret_value(0);
                        	return;
                	} 
			addr1 = GET_BYTE(inst,1) + index + 1;
		}

		u32 old_inst = inst;

		//get the next 32 bits of the inst
		inst = get_second_word();
		
		op = ((GET_BYTE(old_inst, 2) >> 4) & 3); //get bits 4 and 5 of byte2
		
		if (op == 0){
		//operand2 is of type C
			val2 = inst & 0xFF;
			var_loc[addr1] = val2;
		        if(single_command)
		                send_ret_value(val2);

		}

		if (op == 1){
		//operand2 is of type V
			addr2 = GET_BYTE(inst,0);
			var_loc[addr1] = get_var_val(addr2);
		        if(single_command)
		                send_ret_value(var_loc[addr1]);

		}

		else{
		//second operand is of type Arr[v]
			//array size check
			int index = var_loc[GET_BYTE(inst, 0)];
                        if (var_loc[GET_BYTE(inst,1)] <= index ){
                                //error 
                                if (single_command)
                                        send_ret_value(0);
                                return;
                        }
                        addr2 = GET_BYTE(inst,1) + index + 1;
			var_loc[addr1] = get_var_val(addr2);
                        if(single_command)
                                send_ret_value(var_loc[addr1]);
		}

	}
}

void array_dec_handler(int opcode, u32 inst)
{
	int arr = GET_BYTE(inst, 1);
	int size = GET_BIT(inst, 23) ? var_loc[GET_BYTE(inst, 0)]: GET_BYTE(inst, 0);
	var_loc[arr] = size;
	send_ret_value(size);
}

//three in one :)
void wait_goto_get_handler(int opcode, u32 inst)
{
	int op = (GET_BYTE(inst, 2) >> 6);
	int val;
	if(op == 0){
	/* WAIT c*/
		val = inst & 0xFFFF;
	}

	else if (op == 1){
	/* WAIT v*/
		val = get_var_val(GET_BYTE(inst, 0));
	}

	else{
	/* WAIT Arr[v] */
                //array size check
                int index = var_loc[GET_BYTE(inst, 0)];
                if (var_loc[GET_BYTE(inst,1)] <= index ){
                    //error 
                        if (single_command)
				send_ret_value(0);
                        return;
                }

		val = get_var_val(GET_BYTE(inst,1) + index + 1);
	}
	
	if(opcode == WAIT){
		wait(val);
	}

	else if (opcode == GOTO)
		inst_pointer = val;

	else if((opcode == GET) && single_command)
		send_ret_value(val);	
}

#if 1
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
		val1 = get_var_val(GET_BYTE(inst, 0));
	}

	else{
	/* operand1 is of type Arr[v] */

	        //array size check
	        int index = var_loc[GET_BYTE(inst, 0)];
	        if (var_loc[GET_BYTE(inst,1)] <= index ){
	        	//error 
	        	if (single_command)
	                	send_ret_value(0);
	        	return;
	        }

                val1 = get_var_val(GET_BYTE(inst,1) + index + 1);
	}

	u32 old_inst = inst; //old_inst is the 1st 32 bits of the inst.
	
	//get the next 32 bits of the inst
	inst = get_second_word();
	
	/*** GET val2 from second part of the inst ***/

        op = (GET_BYTE(old_inst, 2) >> 4) & 3;

        if(op == 0){
        /* operand2 is of type C */
                val2 = GET_BYTE(inst, 0);
        }

        else if(op == 1){
        /* operand2 is of type V */
                val2 = get_var_val(GET_BYTE(inst, 0));
        }

        else{
        /* operand2 is of type Arr[v] */

                //array size check
                int index = var_loc[GET_BYTE(inst, 0)];
                if (var_loc[GET_BYTE(inst,1)] <= index ){
                        //error 
                        if (single_command)
                                send_ret_value(0);
                        return;
                }

                val2 = get_var_val(GET_BYTE(inst,1) + index + 1);
        }

        /*** GET val3 from second part of the inst ***/

        op = (GET_BYTE(old_inst, 2) >> 2) & 3;

        if(op == 0){
        /* operand3 is of type C */
                val3 = GET_BYTE(inst, 2);
        }

        else if(op == 1){
        /* operand3 is of type V */
                val3 = get_var_val(GET_BYTE(inst, 2));
        }

        else{
        /* operand3 is of type Arr[v] */

                //array size check
                int index = var_loc[GET_BYTE(inst, 2)];
                if (var_loc[GET_BYTE(inst,3)] <= index ){
                        //error 
                        if (single_command)
                                send_ret_value(0);
                        return;
                }

                val3 = get_var_val(GET_BYTE(inst,3) + index + 1);
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

/* handler for all inst like ADD, SUB, AND, OR*/
void math_handler(int opcode, u32 inst)
{
	int val1, val2, res;
	int addr; //address of var to modify
	if(opcode % 2 == 0){
	/* 32 bit inst */
		//val1 = GET_BIT(inst, 23) ? var_loc[GET_BYTE(inst, 1)]: GET_BYTE(inst, 1);
		addr = GET_BYTE(inst,1);
		val1 = var_loc[addr]; //val1 can't be const
		val2 = GET_BIT(inst, 22) ? var_loc[GET_BYTE(inst, 0)]: GET_BYTE(inst, 0);	
	}
	
	else{
	/* 64 bit inst */
		//TODO the following should be abstracted away into a fn 
		//since all 64 bit inst have the same operator placement, common fn to extract all values

		int op = GET_BYTE(inst, 2) >> 6;

		if(op == 0){//XXX : this case is not possible 
        	/* operand1 is of type C */
                	val1 = GET_BYTE(inst, 0);
        	}
	
	        else if(op == 1){
	        /* operand1 is of type V */
			addr = GET_BYTE(inst,0);
	                val1 = get_var_val(addr);
	        }
	
	        else{
	        /* operand1 is of type Arr[v] */

	                //array size check
	                int index = var_loc[GET_BYTE(inst, 0)];
	                if (var_loc[GET_BYTE(inst,1)] <= index ){
	                        //error 
	                        if (single_command)
	                                send_ret_value(0);
	                        return;
	                }

        	        val1 = get_var_val(GET_BYTE(inst,1) + index + 1);
	        }	
		
		u32 old_inst = inst; //old_inst is the 1st 32 bits of the inst.

	        //get the next 32 bits of the inst
	        inst = get_second_word();
	
	        /*** GET val2 from second part of the inst ***/
	
	        op = (GET_BYTE(old_inst, 2) >> 4) & 3;
	
       		if(op == 0){
	        /* operand2 is of type C */
	                val2 = GET_BYTE(inst, 0);
	        }
	
	        else if(op == 1){
	        /* operand2 is of type V */
	                val2 = get_var_val(GET_BYTE(inst, 0));
	        }
	
	        else{
	        /* operand2 is of type Arr[v] */

	                //array size check
	                int index = var_loc[GET_BYTE(inst, 0)];
	                if (var_loc[GET_BYTE(inst,1)] <= index ){
	                        //error 
	                        if (single_command)
	                                send_ret_value(0);
	                        return;
	                }

	                val2 = get_var_val(GET_BYTE(inst,1) + index + 1);
	        }
	}
	
	switch(opcode){
		case ADD_32:
		case ADD_64:
			res = val1 + val2;
		break;

		case SUB_32:
		case SUB_64:
			res = val1 - val2;
		break;
		
		case AND_32:
		case AND_64:
			res = val1 & val2;
		break;

		case OR_32:
		case OR_64:
			res = val1 | val2;
		break;

		case NOT_32:
		case NOT_64:
			res = ~val2;
		break;


		default:
			res = val1;
		break;
	}

	var_loc[addr] = res;

	if(single_command)
		send_ret_value(res);

}
#endif

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
			var_loc[DIO_OFF] = 0xFFFFFF; //no index out of bounds for DIO, AIO etc.
			PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
			shm_code = (u32 *)arg0;
			shm_ret = (u32 *)arg1;
			*shm_code = 25; /*for test purposes - checked by kernel*/
			*shm_ret = 1;
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
			ret_pointer = 1;
			PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT);
			*shm_ret = 1;
			PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
		break;

		case SYS_STAT:
		/* returns the status of execution i.e. executing or not */
			return (is_executing | is_waiting);
		break;
		
		case SYS_INST_32:
		/* an single 32 bit instruction has been recieved for execution */
			single_command = (u32)arg0;
		break;
		
		case SYS_INST_64:
		/* an single 32 bit instruction has been recieved for execution */
			single_command = (u32)arg0;
			single_command_2 = (u32)arg1; //2nd word for 64 bit inst
			//bad naming convention - alexanderhiam is not going to be happy
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
		//PIEP_GLOBAL_CFG &= ~(GLOBAL_CFG_CNT_ENABLE);

		/* reset the timer to 0 (timer is a clear to write reg) */
		//PIEP_COUNT = 0XFFFFFFFF;

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
	//PIEP_CMP_CMP0 =  MS * ms;

	/* change execution state of the PRU */
	is_executing = false;
	is_waiting = true;

	/* enable events from CMP0*/
	PIEP_CMP_CFG |= CMP_CFG_CMP_EN(0);

        /* set the CMP0 reg value */
        PIEP_CMP_CMP0 =  ((MS * ms) + PIEP_COUNT);// % 0xFFFFFFFF;//is the % op needed at all?

	/* clear the timer event for CMP0 incase it has been already set by mistake */
	PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0);

        /* start the timer */
        //PIEP_GLOBAL_CFG |= GLOBAL_CFG_CNT_ENABLE;	
}

void execute_instruction()
{
	u32 inst;
	if(!single_command){
		PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT); /*enable gloabl access*/
		inst = *(shm_code + inst_pointer);
		PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT;
		inst_pointer++;
	}
	else {
		inst = single_command;
	}
	
	int opcode = inst >> 24; //most significant byte contains the cmd 

	switch(opcode){

		case SET_DIO_a:
		case SET_DIO_b:
		case SET_DIO_c:
			dio_handler(opcode, inst);
		break;
	
		case SET_PWM_a:
			pwm_handler(opcode, inst);
		break;

		case SET_32_a:
		case SET_32_b:
		case SET_64:
			set_handler(opcode, inst);
		break;
		
		case SET_ARR_DEC:
			array_dec_handler(opcode, inst);
		break;
	
		case WAIT:
		case GOTO:
		case GET:
			wait_goto_get_handler(opcode, inst);
		break;

		case IF_EQ:
		case IF_NEQ:
		case IF_GTE:
		case IF_LTE:
		case IF_GT:
		case IF_LT:
			if_handler(opcode, inst);
		break;
		
		case ADD_32:
		case ADD_64:
		case SUB_32:
		case SUB_64:
		case AND_32:
		case AND_64:
		case OR_32:
		case OR_64:
			math_handler(opcode, inst);
		break;
		
		case HALT:
			is_waiting = 0;
			is_executing = 0;
			inst_pointer = 0;
		break;

		default:
		/*no op*/
		break;

	}
	
	single_command = 0; /*incase there was a single command executed this time, set to zero*/
}

void timer_init()
{
        /* set default inc to 1, set compensation inc to 1 (though this is never used)*/
        PIEP_GLOBAL_CFG = GLOBAL_CFG_DEFAULT_INC(1) | GLOBAL_CFG_CMP_INC(1);

        /* Compare registers enable, enables signal from CMP0, on CMP0 --> counter is reset */
        //PIEP_CMP_CFG |=  RESET_ON_CMP0_EVENT; -- cant afford to do this when timer is shared
	
	/* clear the counter status bit for CMP0 register, i.e. PIEP_CMP_STATUS[0] */
        PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(0); /* clear the interrupt */

        /* start the timer */
	PIEP_GLOBAL_CFG |= GLOBAL_CFG_CNT_ENABLE;
}

int main()
{
	
	timer_init();
	//send_ret_value(1000);
	while(1)
	{
		check_event();
		
		if (is_executing || single_command){ //or if single_cmd
			execute_instruction();
			//send_ret_value(is_executing);
			//send_ret_value(single_command);
		}
		
	}

	return 0;
}
