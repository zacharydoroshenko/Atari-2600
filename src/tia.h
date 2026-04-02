#include "cpu.h"

typedef void (*FunctionPtr)(struct TIAState* S);

typedef struct TIAState{
    int hpos;
    int vpos;
    uint8_t frame[192][160];
    uint8_t frameBuffer[192][160][3];
} TIAState;

