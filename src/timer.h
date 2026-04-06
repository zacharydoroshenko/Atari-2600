#pragma once
#include <cstdint>

struct CPUState;

// typedef void (*TimerFunc)(struct TimerState* S);

typedef struct TimerState{
    int interval;
    int cycles;
    bool hyperMode;
    bool instatBit6reset;
} TimerState;

void updateTimer(CPUState* S);

void setTimer(CPUState* S, uint16_t address);