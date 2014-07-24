#define PRU			1
//pru0 -> pru1
#define EVENT_BIT               17 //should change this name later; diff b/w various kinds of events
#define IF_EVENT                PINTC_SRSR0 & (1 << EVENT_BIT)
#define CLEAR_EVENT             (PINTC_SICR0 = EVENT_BIT)

#define MAX_PWM			8

