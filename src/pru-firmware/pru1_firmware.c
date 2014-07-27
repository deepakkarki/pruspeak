#include "pru_firmware.h"
#include "pru1_firmware.h"

#if 0
int check_event(void)
{
        if( (IF_EVENT) || data_sock->status[PRU] ){ //change this to just sysevent later
                /* clear the system event */
                PINTC_SICR = 17;

                /* call the handler to perform the downcall*/
                info = data_sock->info[PRU];
		data_sock->status[PRU] = 0;
		return 1;
        }
	
	return 0;
}
#endif

int main()
{
        //init the structs in the array
	int i,j,k;
	for(i = 0; i < CHANNELS; i++ ){
		pwm_sys[i].pin = i;
		pwm_sys[i].hi_time = 0;
	}

	//timer initializations

	while(1){
		//	_								_
		//	|	update pins						|
			i = (pwm_sys[0].hi_time > units_elapsed)
			__R30 = i << pwm_sys[0].pin;
		
			i = (pwm_sys[1].hi_time > units_elapsed)
			__R30 = i << pwm_sys[1].pin;

			i = (pwm_sys[2].hi_time > units_elapsed)
			__R30 = i << pwm_sys[2].pin;

			i = (pwm_sys[3].hi_time > units_elapsed)
			__R30 = i << pwm_sys[3].pin;

			i = (pwm_sys[4].hi_time > units_elapsed)
			__R30 = i << pwm_sys[4].pin;

			i = (pwm_sys[5].hi_time > units_elapsed)
			__R30 = i << pwm_sys[5].pin;

			i = (pwm_sys[6].hi_time > units_elapsed)
			__R30 = i << pwm_sys[6].pin;

			i = (pwm_sys[7].hi_time > units_elapsed)
			__R30 = i << pwm_sys[7].pin;

		//	|	inc the counter						|
			units_elapsed++;

		//	|	if (EV_PRU0_PRU1 && count == 10) : update pru_sys[]	|
			if(units_elapsed == RESOLUTION){
				units_elapsed = 0;
				if (CHECK_EVENT(EV_PRU0_PRU1)){
					//update pru_sys
				}

			}
		//	_ 								_
			
			//***till here no more than 2K inst***
			
			while(!( PIEP_CMP_STATUS & (2) )){ //0b10
				//the timer has gone off
				
				//now restart
			}
		//	loop along till timer time is up
		//	restart timer
			
	}

	return 0;
}

