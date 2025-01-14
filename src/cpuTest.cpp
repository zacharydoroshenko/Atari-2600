#include "cpu.h"
#include <fstream>
typedef void (*FunctionPtr)(CPUState* S);

int main(){
    //initialize instruction matrix
    FunctionPtr M[16][16] = {
        {BRK, ORA, nullptr, nullptr, nullptr, ORA, ASL, nullptr, PHP, ORA, ASL, nullptr, nullptr, ORA, ASL, nullptr},
        {BPL, ORA, nullptr, nullptr, nullptr, ORA, ASL, nullptr, CLC, ORA, nullptr, nullptr, nullptr, ORA, ASL, nullptr},
        {JSR, AND, nullptr, nullptr, BIT, AND, ROL, nullptr, PLP, AND, ROL, nullptr, BIT, AND, ROL, nullptr},
        {BMI, AND, nullptr, nullptr, nullptr, AND, ROL, nullptr, SEC, AND, nullptr, nullptr, nullptr, AND, ROL, nullptr},
        {RTI, EOR, nullptr, nullptr, nullptr, EOR, LSR, nullptr, PHA, EOR, LSR, nullptr, JMP, EOR, LSR, nullptr},
        {BVC, EOR, nullptr, nullptr, nullptr, EOR, LSR, nullptr, CLI, EOR, nullptr, nullptr, nullptr, EOR, LSR, nullptr},
        {RTS, ADC, nullptr, nullptr, nullptr, ADC, ROR, nullptr, PLA, ADC, ROR, nullptr, JMP, ADC, ROR, nullptr},
        {BVS, ADC, nullptr, nullptr, nullptr, ADC, ROR, nullptr, SEI, ADC, nullptr, nullptr, nullptr, ADC, ROR, nullptr},
        {nullptr, STA, nullptr, nullptr, STY, STA, STX, nullptr, DEY, nullptr, TXA, nullptr, STY, STA, STX, nullptr},
        {BCC, STA, nullptr, nullptr, STY, STA, STX, nullptr, TYA, STA, TXS, nullptr, nullptr, STA, nullptr, nullptr},
        {LDY, LDA, LDX, nullptr, LDY, LDA, LDX, nullptr, TAY, LDA, TAX, nullptr, LDY, LDA, LDX, nullptr},
        {BCS, LDA, nullptr, nullptr, LDY, LDA, LDX, nullptr, CLV, LDA, TSX, nullptr, LDY, LDA, LDX, nullptr},
        {CPY, CMP, nullptr, nullptr, CPY, CMP, DEC, nullptr, INY, CMP, DEX, nullptr, CPY, CMP, DEC, nullptr},
        {BNE, CMP, nullptr, nullptr, nullptr, CMP, DEC, nullptr, CLD, CMP, nullptr, nullptr, nullptr, CMP, DEC, nullptr},
        {CPX, SBC, nullptr, nullptr, CPX, SBC, INC, nullptr, INX, SBC, NOP, nullptr, CPX, SBC, INC, nullptr},
        {BEQ, SBC, nullptr, nullptr, nullptr, SBC, INC, nullptr, SED, SBC, nullptr, nullptr, nullptr, SBC, INC, nullptr}

    };


    //initialize CPUState
    CPUState* s = (CPUState*) calloc(sizeof(CPUState), 1);
    s->PC = 0x1000;

    //Test ADC Immediate
    s->oppcode = 0x69;
    s->memory[0x1000] = 0x69;
    s->memory[0x1001] = 0x01;
    M[6][9](s);


    //Test ADC Zero Page


    //Test ADC Zero Page, X


    //Test ADC Absolute


    //Test ADC Absolute, X


    //Test ADC Absolute Indirect X

    //Test ADC Indirect Y
}

