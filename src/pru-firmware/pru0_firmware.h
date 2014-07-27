//ARM -> pru0
#define EVENT_BIT               21 //keep for now, later replace with 'EV_ARM_PRU0'
#define IF_EVENT                PINTC_SRSR0 & (1 << EVENT_BIT)

#define MS                      0x30d40 //number of clock cycles it takes for 1ms

/*max number of DIO pins*/
#define MAX_DIO			12

/* max data area (in terms of index for var_loc[])*/
#define MAX_DATA		239

/* start of DIO, AIO */
#define DIO_OFF			240
#define AIO_OFF			256

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

//a single inst cmd, 32 bit, to be executed next cycle.
#define SYS_INST_32	5

//a single inst cmd, 64 bit, to be executed next cycle.
#define SYS_INST_64	6

/* base address pointer of the instruction stream */
u32 *shm_code = 0;

/* base address pointer of the return value stream */
u32 *shm_ret = 0;

/* offset (from shm_ret) to current return value loc*/
int ret_pointer = 1;

/* the compiled 32 bit instruction */
u32 single_command = 0;

/* 2nd word of the compiled 64 bit instruction */
u32 single_command_2 = 0;

/* pointer to current instruction in terms of offset */
int inst_pointer = 0;

/* boolean value, true if there is a botspeak script under execution */
int is_executing = false;

/* boolean value, true if wait is being executed */
int is_waiting = false;

extern void sc_downcall(int (*handler)(u32 nr, u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4));

/*instruction set encoding */
#define NOP		0

/*SET RES[x], y - IO operations*/
#define SET_DIO_a       1
#define SET_DIO_b       2
#define SET_DIO_c       3
#define SET_PWM_a       4
#define SET_PWM_b       5
#define SET_PWM_c       6
#define SET_AIO_a       7
#define SET_AIO_b       8
#define SET_AIO_c       9

/*Variable set operations*/
#define SET_32_a	16
#define SET_32_b	17
#define SET_64		18
#define SET_ARR_DEC	19
/*If conditions*/
#define IF_EQ		32
#define IF_NEQ		33
#define IF_GTE		34
#define IF_LTE		35
#define IF_GT		36
#define IF_LT		37

/*single operand instructions*/
#define WAIT		20
#define GOTO		21
#define GET		22

/*arithmetic instructions*/
#define ADD_32		48
#define ADD_64		49
#define SUB_32		50
#define SUB_64		51
#define MUL_32		52
#define MUL_64		53
#define DIV_32		54
#define DIV_64		55
#define MOD_32		56
#define MOD_64		57

/*bit wise operations*/
#define BSL_32		64
#define BSL_64		65
#define BSR_32		66
#define BSR_64		67
#define AND_32		68
#define AND_64		69
#define OR_32		70
#define OR_64		71
#define NOT_32		72
#define NOT_64		73

#define HALT		127


