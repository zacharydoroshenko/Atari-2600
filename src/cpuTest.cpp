#include "cpu.h"
#include <fstream>
#include <assert.h>
// printf("0x%x\n", S->PC);
//RTI RTS BRK

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

    // S->PC = 0x1000;
    // S->memory[0x1000] = 0x00;
    // S->B = 0;




    // Run(S);
    // assert(S->cycleDif = 7);
    // assert(S->B = 1);
    // assert(S->PC = 0xFFFE);


    // S->PC = 0x1000;
    // S->memory[0x1000] = 0x1;

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

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->C = 1;
    S->memory[S->PC] = 0x18;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->C == 0);

    //CLD --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->D = 1;
    S->memory[S->PC] = 0xD8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->D == 0);

    //CLI --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->I = 1;
    S->memory[S->PC] = 0x58;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->I == 0);

    //CLV --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->V = 1;
    S->memory[S->PC] = 0xB8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->V == 0);

    //DEX --------------------------------------------------------------------------------------------------------------------------------------


    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xCA;
    S->X = 0x01;
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
    S->memory[S->PC] = 0xCA;
    S->X = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFF);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //DEY --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0x88;
    S->Y = 0x01;
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
    S->memory[S->PC] = 0x88;
    S->Y = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xFF);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //INX --------------------------------------------------------------------------------------------------------------------------------------
    
    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xE8;
    S->X = 0xFF;
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
    S->memory[S->PC] = 0xE8;
    S->X = 0xEF;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xF0);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //INY --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xC8;
    S->Y = 0xFF;
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
    S->memory[S->PC] = 0xC8;
    S->Y = 0xEF;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xF0);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //NOP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->memory[S->PC] = 0xEA;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);

    //PHA --------------------------------------------------------------------------------------------------------------------------------------
    
    // 03 07 05 08

    
    S->PC = 0x1000;
    S->memory[0x1000] = 0x48; // push 3
    S->memory[0x1001] = 0x48; // push 7
    S->memory[0x1002] = 0x48; // push 5
    S->memory[0x1003] = 0x68; // pull 5
    S->memory[0x1004] = 0x48; // push 8
    S->memory[0x1005] = 0x68; // pull 8
    S->memory[0x1006] = 0x68; // pull 7
    S->memory[0x1007] = 0x68; // pull 3

    S->A = 3;
    Run(S);
    S->A = 7;
    Run(S);
    S->A = 5;
    Run(S);
    Run(S);
    assert(S->A == 05);
    S->A = 8;
    Run(S);
    Run(S);
    assert(S->A == 8);
    Run(S);
    assert(S->A == 7);
    Run(S);
    assert(S->A == 3);

    

    //PHP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x08;
    S->memory[0x1001] = 0x28;
    S->N = 1;
    S->V = 0;
    //1
    S->B = 0;
    S->D = 1;
    S->I = 0;
    S->Z = 1;
    S->C = 0;

    Run(S);

    assert(S->memory[S->SP + 1] == 0b10101010);
    assert(S->cycleDif == 3);

    //pull
    S->N = 0;
    S->V = 1;
    //1
    S->B = 1;
    S->D = 0;
    S->I = 1;
    S->Z = 0;
    S->C = 1;
    Run(S);
    assert(S->N == 1);
    assert(S->V == 0);
    assert(S->B == 0);
    assert(S->D == 1);
    assert(S->I == 0);
    assert(S->Z == 1);
    assert(S->C == 0);
    assert(S->cycleDif == 4);
    

    

    //PLA --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x48; // push 3
    S->memory[0x1001] = 0x48; // push 7
    S->memory[0x1002] = 0x48; // push 5
    S->memory[0x1003] = 0x68; // pull 5
    S->memory[0x1004] = 0x48; // push 8
    S->memory[0x1005] = 0x68; // pull 8
    S->memory[0x1006] = 0x68; // pull 7
    S->memory[0x1007] = 0x68; // pull 3

    S->A = 3;
    Run(S);
    assert(S->cycleDif == 3);
    S->A = 7;
    Run(S);
    S->A = 5;
    Run(S);
    Run(S);
    assert(S->A == 05);
    S->A = 8;
    Run(S);
    Run(S);
    assert(S->A == 8);
    Run(S);
    assert(S->A == 7);
    Run(S);
    assert(S->A == 3);

    assert(S->cycleDif == 4);

    //PLP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x08;
    S->memory[0x1001] = 0x28;
    S->N = 1;
    S->V = 0;
    //1
    S->B = 0;
    S->D = 1;
    S->I = 0;
    S->Z = 1;
    S->C = 0;

    Run(S);
    assert(S->memory[S->SP + 1] == 0b10101010);
    assert(S->cycleDif == 3);

    //pull
    S->N = 0;
    S->V = 1;
    //1
    S->B = 1;
    S->D = 0;
    S->I = 1;
    S->Z = 0;
    S->C = 1;
    Run(S);
    assert(S->N == 1);
    assert(S->V == 0);
    assert(S->B == 0);
    assert(S->D == 1);
    assert(S->I == 0);
    assert(S->Z == 1);
    assert(S->C == 0);
    assert(S->cycleDif == 4);

    //RTI --------------------------------------------------------------------------------------------------------------------------------------
    //Don't worry about it for now since interrupts are rarely used



    //RTS --------------------------------------------------------------------------------------------------------------------------------------
    S->PC = 0x1000;
    S->memory[S->SP] = 0x10;
    S->memory[S->SP - 1] = 0x12;
    S->memory[0x1000] = 0x60;



    //SEC --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->C = 0;
    S->memory[S->PC] = 0x38;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->C = 1);

    //SED --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->D = 0;
    S->memory[S->PC] = 0xF8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->D = 1);

    //SEI --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->I = 0;
    S->memory[S->PC] = 0x78;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->I = 1);

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

