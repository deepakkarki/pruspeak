#include "pru_firmware.h"
#include "pru1_firmware.h"


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

int main()
{
        //init the structs in the array

	while(1){
		//	_								_
		//	|	update pins						|
		//	|	inc the counter						|
		//	|	if (EV_PRU0_PRU1 && count == 10) : update pru_sys[]	|
		//	_ 								_
			
			//***till here no more than 2K inst***
			
		//	loop along till timer time is up
		//	restart timer
			
	}
	return 0;
}

