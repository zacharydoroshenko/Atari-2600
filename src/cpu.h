#include <iostream>
#include <cstdint>

using namespace std;

typedef struct {
    //registers
    uint8_t A, X, Y, SP, P;
    uint16_t PC;


    bool halt;
    int cycleDif;
    uint8_t memory[65536];

} CPUState;

void NOP(CPUState* S);
void SEC(CPUState* S);
void SED(CPUState* S);