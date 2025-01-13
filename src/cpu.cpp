#include "cpu.h"

//No operation
void NOP(CPUState* S){
    S->cycleDif = 2;
    S->PC++;
}

//Set Carry Flag
void SEC(CPUState* S){
    //set bit 0
    S->P = S->P | (1 << 0);
    S->PC++;
    S->cycleDif = 2;
}

//Set Decimal mode
void SED(CPUState* S){
    //set bit 3
    S->P = S->P | (1 << 3);
    S->PC++;
    S->cycleDif = 2;
}

