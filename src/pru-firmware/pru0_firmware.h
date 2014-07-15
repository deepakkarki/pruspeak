#define EVENT_BIT               21
#define IF_EVENT                PINTC_SRSR0 & (1 << EVENT_BIT)
#define CLEAR_EVENT             (PINTC_SRSR0 = EVENT_BIT)

#define MS                      0x30d40 //number of clock cycles it takes for 1ms

/* 
	sys calls ids
	Different syscall values, aliases and their meaning
*/

//DEBUG - Sets all output pins to high
#define SYS_DEBUG       0

//INIT - initializes the shm address, 
//writes into it so kernel knows that pru can talk via shm
#define SYS_INIT        1

//EXEC - start or pause script execution
#define SYS_EXEC        2

//ABORT - aborts the running script, resets env
#define SYS_ABRT        3

//returns the status of whether a script is executing
#define SYS_STAT        4

//a single inst cmd, to be executed next cycle.
#define SYS_INST        5

/* base address pointer of the instruction stream */
u32 *shm_base = 0;

/* the compiled 32 bit instruction */
u32 single_command = 0;

/* pointer to current instruction in terms of offset */
int inst_pointer = 0;

/* boolean value, true if there is a botspeak script under execution */
int is_executing = false;

/* boolean value, true if wait is being executed */
int is_waiting = false;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4));


