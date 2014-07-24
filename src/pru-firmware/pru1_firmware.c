#include "pru_firmware.h"
#include "pru1_firmware.h"

static int info;

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
	__R30 = 0x00;
        while(1)
        {
                if (check_event()){
   	        	if (info)
        			__R30 = 0xFFFF;
			else
				__R30 = 0x0000;
		}
		
        }

        return 0;
}

