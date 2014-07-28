#include "pru_firmware.h"
#include "pru1_firmware.h"

/* The number of TIME_UNIT elapsed in the current TIME_PERIOD */
int units_elapsed = 0;

void timer_init()
{
        /* set default inc to 1, set compensation inc to 1 (though this is never used)*/
        PIEP_GLOBAL_CFG = GLOBAL_CFG_DEFAULT_INC(1) | GLOBAL_CFG_CMP_INC(1);

	/* Enable interrupts from cmp1 */
        PIEP_CMP_CFG |= CMP_CFG_CMP_EN(1);

        /* set the value of cmp1 = (time_unit in us) * (no. of clk cycle for 1 us) */
        PIEP_CMP_CMP1 =  ((US * TIME_UNIT) + PIEP_COUNT);

	/* clear the interrupt from cmp1 in case it has been set by mistake */
        PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(1); 

        /* start the timer */
        PIEP_GLOBAL_CFG |= GLOBAL_CFG_CNT_ENABLE;
}


int main()
{
        //init the structs in the array
	int i;
	for(i = 0; i < CHANNELS; i++ ){
		pwm_sys[i].pin = i;
		pwm_sys[i].hi_time = 0;
	}

	//timer initializations
	timer_init();

	while(1){
		//	_								_
		//	|	update pins						|
			i = (pwm_sys[0].hi_time > units_elapsed)? 1 : 0;
			__R30 = i << pwm_sys[0].pin;
		
			i = (pwm_sys[1].hi_time > units_elapsed);
			__R30 = i << pwm_sys[1].pin;

			i = (pwm_sys[2].hi_time > units_elapsed);
			__R30 = i << pwm_sys[2].pin;

			i = (pwm_sys[3].hi_time > units_elapsed);
			__R30 = i << pwm_sys[3].pin;

			i = (pwm_sys[4].hi_time > units_elapsed);
			__R30 = i << pwm_sys[4].pin;

			i = (pwm_sys[5].hi_time > units_elapsed);
			__R30 = i << pwm_sys[5].pin;

			i = (pwm_sys[6].hi_time > units_elapsed);
			__R30 = i << pwm_sys[6].pin;

			i = (pwm_sys[7].hi_time > units_elapsed);
			__R30 = i << pwm_sys[7].pin;

		//	|	inc the counter						|
			units_elapsed++;

		//	|	if (EV_PRU0_PRU1 && count == 10) : update pru_sys[]	|
			if(units_elapsed == RESOLUTION){
				units_elapsed = 0;

				if (CHECK_EVENT(EV_PRU0_PRU1)){

					/* clear the system event */
                			PINTC_SICR = EV_PRU0_PRU1; //17

					/* update the pru_sys array */
					for(i=0; i<CHANNELS; i++)
               					pwm_sys[i].hi_time = data_sock->info[PRU][i];

					__R30 = 0xff;
					while(1){}
				}

			}
		//	_ 								_
			
			//***till here no more than 2K inst***

			// loop along till timer is up	
			while(!( PIEP_CMP_STATUS & (2) )){ //0b10
				//the timer has gone off
					//disable the event	|
					//clear it		| --> are these 3 reqd.?
					//enable		|

					//reassign value cmp1
					PIEP_CMP_CMP1 =  ((US * TIME_UNIT) + PIEP_COUNT);

					/* clear the interrupt from cmp1  */
					PIEP_CMP_STATUS = CMD_STATUS_CMP_HIT(1);
			}
			
	}

	return 0;
}

