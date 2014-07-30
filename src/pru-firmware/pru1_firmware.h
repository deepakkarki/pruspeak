#define PRU		1

//pru0 -> pru1
#define IF_EVENT                (CHECK_EVENT(EV_PRU0_PRU1)) // || CHECK_EVENT(EV_ARM_PRU0)) --not reqd as of now

/* The number of clock cycles it takes for 1 micro sec*/
#define US		0xc8 //dec val = 200
#define MS		0x30d40

/* The number of PWM channels */
#define CHANNELS        8

/* The time unit in terms of micro sec */
#define TIME_UNIT       5 //equivalent to 1K instructions

/* The number of time units in one wave/time-period */
#define RESOLUTION      100 //should be atleast 20 :(

/* time period of pwm o/p in micro seconds */
#define TIME_PERIOD     ((TIME_UNIT) * (RESOLUTION))
//freq of PWM = (10^6) / (TIME_PERIOD) ; 10^6 since it is micro sec

/* basic pwm unit structure */
struct pwm_unit{
        int pin; //output pin
        int hi_time; //number of TIME_UNITs to stay high a TIME_PERIOD
};

typedef struct pwm_unit PWM;

/* array that represents all the soft PWM ports*/
PWM pwm_sys[CHANNELS];


//######################ALGORITHM############################
#if 0

|_ _ _ _ _ _ _ _ _ _ | ==> one waveform (time period)

PWM : 0.3
|- - - _ _ _ _ _ _ _ | ==> up three time units 

each time unit == either 5us or 10us

8 PWM channels in PWM pwm_arr[8]; array of pwm structs

PWM => {
        int channel;
        int hi_time; //number of time units high in waveform
}

every 100us PRU0->PRU1 flag is checked, if it is set, hi_time is updated in all struct.
global int count is maintained to count till 10. count++ every time unit.


#endif

