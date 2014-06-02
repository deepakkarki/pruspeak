//volatile char shared_buf[100] __attribute__((cregister("EXT_MEM")));
//int local_data;
volatile register unsigned int __R30;
volatile register unsigned int __R31;

void wait(int time)
{
	int i;
	for(i=0; i < time; i++)
	{
	}
}

int main()
{
	__R30 = 0xFFFFFFFF; //set all pins to high

	for (;;)
	{        
		wait(0x0AFFFFFF);

		__R30 = 0x00; //set all to low
		
		wait(0x0AFFFFFF);

		__R30 = 0xFFFFFFFF; //set all to high

	}
}
