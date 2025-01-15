#include "cpu.h"

//initializes cpustate variables to starting values
void initialize(CPUState* S){
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

    //Copy M into S->M
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 16; j++){
            S->M[i][j] = M[i][j];
        }
    }

    //initialize variables 
    S->PC = 0x1000;
    S->SP = 4;
}

//To run an instruction based on its oppcode
void Run(CPUState* S){
    uint8_t oppcode = S->memory[S->PC]; 
    S->M[oppcode >> 4][oppcode & 0b1111](S);
}

//Add Memory to Accumulator with Carry
void ADC(CPUState* S){}

//And Memory with Accumulator
void AND(CPUState* S){}

//Shift left one bit (memory or accumulator)
void ASL(CPUState* S){}

//Branch on Carry clear
void BCC(CPUState* S){
    //extract the argument
    uint16_t oldPC = S->PC;
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->C == 1){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }

}

//Branch on Carry set
void BCS(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->C == 0){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Branch on result zero
void BEQ(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->Z == 0){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Test Bits in memory with accumulator
void BIT(CPUState* S){}

//Branch on result minus
void BMI(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->N == 0){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//branch on result not zero
void BNE(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->Z == 1){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//branch on result plus
void BPL(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->N == 1){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//force break (force interrupt) program counter then status
void BRK(CPUState* S){
    S->B = 1;

    

    //push program counter
    S->PC++;
    S->SP++;
    S->memory[S->SP] = S->PC;

    //push status
    S->SP++;
    uint8_t P = 0b00100000;
    if(S->C == true) P += 0b00000001;
    if(S->Z == true) P += 0b00000010;
    if(S->I == true) P += 0b00000100;
    if(S->D == true) P += 0b00001000;
    if(S->B == true) P += 0b00010000;
    if(S->V == true) P += 0b01000000;
    if(S->N == true) P += 0b10000000;
    S->memory[S->SP] = P;

    //IRQ interrupt vector at $FFFE/F is loaded into the PC
    S->PC = 0xFFFE;


    //break flag in the status set to one.
    S->B = 1;


    S->cycleDif = 7;
}

//Branch on Overflow Clear
void BVC(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->V == 1){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Branch on Overflow Set
void BVS(CPUState* S){
    //extract the argument
    S->PC++;
    int8_t offset = S->memory[S->PC];
    S->PC++;

    if(S->V == 0){
        //branch not taken
        S->cycleDif = 2;
    } else {
        //branch taken
        uint16_t oldPC = S->PC;
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = (oldPC >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Clear Carry Flag
void CLC(CPUState* S){
    S->C = 0;
    S->PC++;
    S->cycleDif = 2;
}

//Clear Decimal Mode
void CLD(CPUState* S){
    S->D = 0;
    S->PC++;
    S->cycleDif = 2;
}

//Clear Interrupt Disable Bit
void CLI(CPUState* S){
    S->I = 0;
    S->PC++;
    S->cycleDif = 2;
}

//Clear Overflow Flag
void CLV(CPUState* S){
    S->V = 0;
    S->PC++;
    S->cycleDif = 2;
}

//Compare Memory and Accumulator
void CMP(CPUState* S){}

//Compare Memory and Index X
void CPX(CPUState* S){}

//Compare Memory and Index Y
void CPY(CPUState* S){}

//Decrement Memory by One
void DEC(CPUState* S){}

//Decrement Index X by One
void DEX(CPUState* S){
    S->X--;
    
    if(S->X == 0) S->Z = 1;

    //extracted bit
    if(((S->X >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Decrement Index Y by One
void DEY(CPUState* S){
    S->Y--;
    
    if(S->Y == 0) S->Z = 1;

    //extracted bit
    if(((S->Y >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//"Exclusive-or" Memory with Accumulator
void EOR(CPUState* S){}

//Increment Memory by One
void INC(CPUState* S){}

//Increment Index X by One
void INX(CPUState* S){
    S->X++;
    
    if(S->X == 0) S->Z = 1;

    //extracted bit
    if(((S->X >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Increment Index Y by one
void INY(CPUState* S){
    S->Y++;
    
    if(S->Y == 0) S->Z = 1;

    //extracted bit
    if(((S->Y >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Jump to New Location
void JMP(CPUState* S){}

//Jump to new Location Saving Return Address
void JSR(CPUState* S){}

//Load Accumulator with Memory
void LDA(CPUState* S){}

//Load Index x with Memory
void LDX(CPUState* S){}

//Load Index Y with Memory
void LDY(CPUState* S){}

//Shift One Bit Right (Memory or Accumulator)
void LSR(CPUState* S){}

//No Operation
void NOP(CPUState* S){
    S->cycleDif = 2;
    S->PC++;
}

//"OR" Memory with Accumulator
void ORA(CPUState* S){}

//Push Accumulator on Stack
void PHA(CPUState* S){
    S->SP++;
    S->memory[S->SP] = S->A;

    S->cycleDif = 3;
    S->PC++;
}

//Push Processor Status on Stack
void PHP(CPUState* S){
    S->SP++;
    uint8_t P = 0b00100000;
    if(S->C == true) P += 0b00000001;
    if(S->Z == true) P += 0b00000010;
    if(S->I == true) P += 0b00000100;
    if(S->D == true) P += 0b00001000;
    if(S->B == true) P += 0b00010000;
    if(S->V == true) P += 0b01000000;
    if(S->N == true) P += 0b10000000;
    S->memory[S->SP] = P;

    S->cycleDif = 3;
    S->PC++;
}

//Pull Accumulator from Stack
void PLA(CPUState* S){
    S->A = S->memory[S->SP];
    S->SP--;

    if(S->A == 0) S->Z = 1;

    //extracted bit
    if(((S->A >> 7) & 1) == 1) S->N = 1;

    S->cycleDif = 4;
    S->PC++;
}

//Pull Processor Status from Stack
void PLP(CPUState* S){
    uint8_t temp = S->memory[S->SP];
    S->SP--;
    S->C = (((temp >> 0) & 1) == 0) ? 0 : 1;
    S->Z = (((temp >> 1) & 1) == 0) ? 0 : 1;
    S->I = (((temp >> 2) & 1) == 0) ? 0 : 1;
    S->D = (((temp >> 3) & 1) == 0) ? 0 : 1;
    S->B = (((temp >> 4) & 1) == 0) ? 0 : 1;
    S->V = (((temp >> 6) & 1) == 0) ? 0 : 1;
    S->N = (((temp >> 7) & 1) == 0) ? 0 : 1;

    S->cycleDif = 4;
    S->PC++;
}

//Rotate One Bit Left (Memory or Accumulator)
void ROL(CPUState* S){}

//Rotate One Bit Right (Memory or Accumulator)
void ROR(CPUState* S){}

//Return from Interrupt
void RTI(CPUState* S){

    //pull processor Status from Stack
    uint8_t temp = S->memory[S->SP];
    S->SP--;
    S->C = (((temp >> 0) & 1) == 0) ? 0 : 1;
    S->Z = (((temp >> 1) & 1) == 0) ? 0 : 1;
    S->I = (((temp >> 2) & 1) == 0) ? 0 : 1;
    S->D = (((temp >> 3) & 1) == 0) ? 0 : 1;
    S->B = (((temp >> 4) & 1) == 0) ? 0 : 1;
    S->V = (((temp >> 6) & 1) == 0) ? 0 : 1;
    S->N = (((temp >> 7) & 1) == 0) ? 0 : 1;

    //pull program counter
    S->PC = S->memory[S->SP]; 
    S->SP--;

    S->cycleDif = 6;
}

//Return from Subroutine
void RTS(CPUState* S){
    S->PC = S->memory[S->SP]; 
    S->SP--;


    S->cycleDif = 6;
}

//Subtract Memory from Accumulator
void SBC(CPUState* S){}

//Set Carry Flag
void SEC(CPUState* S){
    S->C = 1;
    S->PC++;
    S->cycleDif = 2;
}

//Set Decimal Mode
void SED(CPUState* S){
    S->D = 1;
    S->PC++;
    S->cycleDif = 2;
}

//Set Interrupt Disable Status
void SEI(CPUState* S){
    S->I = 1;
    S->PC++;
    S->cycleDif = 2;
}

//Store Accumulator in Memory
void STA(CPUState* S){}

//Store Index X in Memory
void STX(CPUState* S){}

//Store Index Y in Memory
void STY(CPUState* S){}

//Transfer Accumulator to Index X
void TAX(CPUState* S){
    S->X = S->A;
    
    if(S->X == 0) S->Z = 1;

    //extracted bit
    if(((S->X >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Transfer Accumulator to Index Y
void TAY(CPUState* S){
    S->Y = S->A;
    
    if(S->Y == 0) S->Z = 1;

    //extracted bit
    if(((S->Y >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Transfer Stack Pointer to Index X
void TSX(CPUState* S){
    S->X = S->SP;
    
    if(S->X == 0) S->Z = 1;

    //extracted bit
    if(((S->X >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Transfer index X to Accumulator 
void TXA(CPUState* S){
    S->A = S->X;
    
    if(S->A == 0) S->Z = 1;

    //extracted bit
    if(((S->A >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}

//Transfer Index X to Stack pointer
void TXS(CPUState* S){
    S->SP = S->X;

    S->PC++;
    S->cycleDif = 2;
}

//Transfer Index Y to Accumulator
void TYA(CPUState* S){
    S->A = S->Y;
    
    if(S->A == 0) S->Z = 1;

    //extracted bit
    if(((S->A >> 7) & 1) == 1) S->N = 1;

    S->PC++;
    S->cycleDif = 2;
}
