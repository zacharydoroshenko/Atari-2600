#include "cpu.h"
#include <fstream>
#include <assert.h>
// printf("0x%x\n", S->PC);


int main(){


    //initialize CPUState
    CPUState* S = (CPUState*) calloc(sizeof(CPUState), 1);
    initialize(S);

    //BCC --------------------------------------------------------------------------------------------------------------------------------------
    //branch on carry clear

    //test branch
    S->C = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    
    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->C = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BCS --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->C = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->C = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);



    //BEQ --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->Z = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->Z = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BMI --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->N = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->N = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BNE --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->Z = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->Z = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BPL --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->N = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->N = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BRK --------------------------------------------------------------------------------------------------------------------------------------

    //BVC --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->V = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->V = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BVS --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->V = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->V = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //CLC --------------------------------------------------------------------------------------------------------------------------------------

    //CLD --------------------------------------------------------------------------------------------------------------------------------------

    //CLI --------------------------------------------------------------------------------------------------------------------------------------

    //CLV --------------------------------------------------------------------------------------------------------------------------------------

    //DEX --------------------------------------------------------------------------------------------------------------------------------------

    //DEY --------------------------------------------------------------------------------------------------------------------------------------

    //INX --------------------------------------------------------------------------------------------------------------------------------------
    
    //INY --------------------------------------------------------------------------------------------------------------------------------------

    //NOP --------------------------------------------------------------------------------------------------------------------------------------

    //PHA --------------------------------------------------------------------------------------------------------------------------------------

    //PHP --------------------------------------------------------------------------------------------------------------------------------------

    //PLA --------------------------------------------------------------------------------------------------------------------------------------

    //PLP --------------------------------------------------------------------------------------------------------------------------------------

    //RTI --------------------------------------------------------------------------------------------------------------------------------------

    //RTS --------------------------------------------------------------------------------------------------------------------------------------

    //SEC --------------------------------------------------------------------------------------------------------------------------------------

    //SED --------------------------------------------------------------------------------------------------------------------------------------

    //SEI --------------------------------------------------------------------------------------------------------------------------------------

    //TAX --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xAA;
    S->X = 0x34;
    S->A = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xAA;
    S->X = 0x35;
    S->A = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TAY --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xA8;
    S->Y = 0x34;
    S->A = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->Y == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xA8;
    S->Y = 0x35;
    S->A = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TSX --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xBA;
    S->X = 0x34;
    S->SP = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xBA;
    S->X = 0x35;
    S->SP = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TXA --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0x8A;
    S->A = 0x34;
    S->X = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->A == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x8A;
    S->A = 0x35;
    S->X = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->A == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TXS --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x9A;
    S->SP = 0x34;
    S->X = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);

    assert(S->SP == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x9A;
    S->SP = 0x35;
    S->X = 0xFE;
    S->Z = 0;
    S->N = 1;
    Run(S);
    assert(S->SP == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TYA --------------------------------------------------------------------------------------------------------------------------------------
    
    //test zero flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x98;
    S->A = 0x34;
    S->Y = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->A == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x98;
    S->A = 0x35;
    S->Y = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->A == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    printf("Everything works! :)\n");
}

