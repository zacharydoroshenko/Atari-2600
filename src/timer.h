#include "cpu.h"
#define INTIM 0x284
#define INSTAT 0x285
#define TIM1T 0x294
#define TIM8T 0x295
#define TIM64T 0x296
#define T1024T 0x297

typedef void (*FunctionPtr)(struct TimerState* S);

typedef struct TimerState{
    int interval;
    int cycles;
} TimerState;

