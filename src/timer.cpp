#include "timer.h"
#include "cpu.h"
#include "addressDef.h"





//should be called for every cycle that passes
void updateTimer(CPUState* S) {
    TimerState* T = S->Ti;
    if (T->cycles > 0) {
        T->cycles--;
    } else {
        // Decrement the actual memory register
        S->memory[INTIM]--;
        
        // Check if we just wrapped from 00 to FF
        if (S->memory[INTIM] == 0xFF) {
            T->hyperMode = true;
            S->memory[INSTAT] |= 0b11000000; // Set Underflow flags
        }

        // Set the reload value for T->cycles
        if (T->hyperMode) {
            T->cycles = 0; // Tick every cycle
        } else {
            T->cycles = T->interval - 1; // Tick every N cycles
            // T->cycles = 7;
        }
    }
}

//should set interval based on specific address, and INTIM from the value written
//IDEA: INTIM will be set through normal procedures, and rest will be handled in this function.
void setTimer(CPUState* S, uint16_t address){
    TimerState* T = S->Ti;
    if(address == TIM1T)       T->interval = 1;
    else if(address == TIM8T)  T->interval = 8;
    else if(address == TIM64T) T->interval = 64;
    else if(address == T1024T) T->interval = 1024;
    else { printf("bad timer addr: 0x%X\n", address); exit(1); }

    T->hyperMode = false;
    T->cycles = T->interval;  // ← was 0; first decrement now happens after N cycles, not immediately
    S->memory[INSTAT] &= 0b01111111;
}