typedef unsigned int u32;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

/* The number of PRU's that exist */
#define NUM_PRU		2
#define PRU0		0
#define PRU1		1

/* Events list */
#define EV_PRU0_PRU1	17
#define EV_PRU1_PRU0	18
#define EV_PRU0_ARM	19
#define EV_PRU1_ARM	20
#define EV_ARM_PRU0	21
#define EV_ARM_PRU1	22


__far volatile char C0[0x300] __attribute__((cregister("C0", far)));

#define PINTC(_reg) \
	(*(volatile u32 *)((char *)&C0[_reg]))

#define PINTC_SISR		PINTC(0x0020)
#define PINTC_SICR		PINTC(0x0024)
#define PINTC_SRSR0		PINTC(0x0200)
#define PINTC_SRSR1		PINTC(0x0204)
#define PINTC_SECR0		PINTC(0x0280)
#define PINTC_SECR1		PINTC(0x0284)

/* Macro to set a system event */
#define SIGNAL_EVENT(x) \
	do { \
		__R31 = (1 << 5) | ((x) - 16); \
	} while(0);

/* Macro to check a system event */
#define CHECK_EVENT(x) \
	( PINTC_SRSR0 & ( 1 << (x) ) )

/* Macro to clear a system event */
#define CLEAR_EVENT(x) \
	do { \
		PINTC_SICR0 = (x); \
	} while(0);


__far volatile char C4[0x100] __attribute__((cregister("C4", near)));	/* PRUCFG */

#define PRUCFG(_reg) \
	(*(volatile u32 *)((char *)&C4 + (_reg)))

#define PRUCFG_SYSCFG		PRUCFG(0x0004)
#define SYSCFG_STANDBY_INIT	(1 << 4)
#define PRUCFG_ENABLE_GLOBAL	PRUCFG_SYSCFG = PRUCFG_SYSCFG & (~SYSCFG_STANDBY_INIT)
#define PRUCFG_DISABLE_GLOBAL	PRUCFG_SYSCFG = PRUCFG_SYSCFG | SYSCFG_STANDBY_INIT


__far volatile char C26[0x100] __attribute__((cregister("C26", near)));	/* PRUIEP */
#define PIEP(_reg) \
	(*(volatile u32 *)((char *)&C26 + (_reg)))

#define PIEP_GLOBAL_CFG		PIEP(0x0000)
#define  GLOBAL_CFG_CNT_ENABLE		(1 << 0)
#define  GLOBAL_CFG_DEFAULT_INC_S	4
#define  GLOBAL_CFG_DEFAULT_INC_W	4
#define  GLOBAL_CFG_DEFAULT_INC_M	((GLOBAL_CFG_DEFAULT_INC_W - 1) << GLOBAL_CFG_DEFAULT_INC_S)
#define  GLOBAL_CFG_DEFAULT_INC(x)	(((x) << GLOBAL_CFG_DEFAULT_INC_S) & GLOBAL_CFG_DEFAULT_INC_M)
#define  GLOBAL_CFG_CMP_INC_S		8
#define  GLOBAL_CFG_CMP_INC_W		12
#define  GLOBAL_CFG_CMP_INC_M		((GLOBAL_CFG_CMP_INC_W - 1) << GLOBAL_CFG_CMP_INC_S)
#define  GLOBAL_CFG_CMP_INC(x)		(((x) << GLOBAL_CFG_CMP_INC_S) & GLOBAL_CFG_CMP_INC_M)

#define PIEP_GLOBAL_STATUS	PIEP(0x0004)
#define  GLOBAL_STATUS_CNT_OVF		(1 << 0)

#define PIEP_COMPEN		PIEP(0x0008)
#define PIEP_COUNT		PIEP(0x000C)
#define RESET_COUNTER		PIEP_COUNT = 0XFFFFFFFF

#define PIEP_CMP_CFG		PIEP(0x0040)
#define  CMP_CFG_CMP0_RST_CNT_EN	(1 << 0)
#define  CMP_CFG_CMP_EN_S		1
#define  CMP_CFG_CMP_EN_W		8
#define  CMP_CFG_CMP_EN_M		((CMP_CFG_CMP_EN_W - 1) << CMP_CFG_CMP_EN_S)
#define  CMP_CFG_CMP_EN(x)		((1 << ((x) + CMP_CFG_CMP_EN_S)) & CMP_CFG_CMP_EN_M)

#define PIEP_CMP_STATUS		PIEP(0x0044)
#define  CMD_STATUS_CMP_HIT_S		0
#define  CMD_STATUS_CMP_HIT_W		8
#define  CMD_STATUS_CMP_HIT_M		((CMD_STATUS_CMP_HIT_W - 1) << CMD_STATUS_CMP_HIT_S)
#define  CMD_STATUS_CMP_HIT(x)		((1 << ((x) + CMD_STATUS_CMP_HIT_S)) & CMD_STATUS_CMP_HIT_M)

#define PIEP_CMP_CMP0		PIEP(0x0048)
#define PIEP_CMP_CMP1		PIEP(0x004C)
#define PIEP_CMP_CMP2		PIEP(0x0050)
#define PIEP_CMP_CMP3		PIEP(0x0054)
#define PIEP_CMP_CMP4		PIEP(0x0058)
#define PIEP_CMP_CMP5		PIEP(0x005C)
#define PIEP_CMP_CMP6		PIEP(0x0060)
#define PIEP_CMP_CMP7		PIEP(0x0064)
#define PIEP_CMP_CMP(x)		PIEP(0x0048 + ((x) << 2))
#define RESET_ON_CMP0_EVENT	1

#define false	0
#define true	1

#define DPRAM_SHARED	0x00010000
#define DATA_SLOTS	16
/* structure used to pass information pru0 <--> pru1 
   status[i] => pru<i> has information for it to read
   info[i] => the data pru<i> has received
*/
struct shared_data{
	int status[NUM_PRU];	/* valid info waiting for PRU x */
	int info[NUM_PRU][DATA_SLOTS];	/* each PRU has DATA_SLOTS loc reserved for it */
};

typedef struct shared_data SD;

SD *data_sock = (SD *)((void*)DPRAM_SHARED);

/*extract a byte #pos from value */
#define GET_BYTE(val, pos) \
        (((val) >> ((pos)*8)) & 0xFF)

/* extract a bit #pos from a value */
#define GET_BIT(val, pos) \
        (((val) >> (pos)) & 1)

