#include "timer.h"

void updateTimer(CPUState* S, TimerState* T){
    if(T->cycles == 1){
        T->cycles = T->interval;
        S->memory[INTIM]--;
        if(S->memory[INTIM] == 0xFF){
            T->interval = 1;
            T->cycles = 1;
        }
    } else {
        T->cycles--;
    }
    
}