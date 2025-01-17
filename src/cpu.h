//Helpful links
//forums.atariage.com/topic/27190-session-5-memory-architecture/
//nesdev.org/obelisk-6502-guide/reference.html

//Assumptions made:
//decrement is done by I--
//increment is done by I++
//to push to stack you do S->SP++; S->memory[S->SP] = S->A;
//to pull from stack you do S->A = S->memory[S->SP]; S->SP--;
//offset can be calculated with S->PC += offset;

//look into interrupts
//find out what to put into 0xFFFE/F when BRK is run
//find out where stack pointer starts

//Don't worry about interrupts for now ie
//BRK RTI

#include <iostream>
#include <cstdint>

#define AA 1 //always 2
#define REL 2 //always 2 (+1 if branch succeeds +2 if to a new page)
#define IMM 3 //always 2 
#define IND 4 //only for JMP and always uses 5 cycles
#define INDY 5 //always 5 (+1 if page crossed)
#define INDX 6 //always 6
#define ZPG 7 //usually 3 (ASL = 5)(DEC = 5)(INC = 5)(LSR = 5)(ROL = 5)(ROR = 5)
#define ZPGX 8 //usually 4 (ASL = 6)(DEC = 6)(INC = 6)(LSR = 6)(ROL = 6)(ROR = 6)
#define ZPGY 9 //always 4
#define ABS 10 //usually 4 (ASL = 6)(DEC = 6)(INC = 6)(LSR = 6)(ROL = 6)(ROR = 6)     (JSR = 6)(JMP = 3)
#define ABSX 11 //usually 4 (+1 if page crossed)     (ASL = 7)(DEC = 7)(INC = 7)(LSR = 7)(ROL = 7)(ROR = 7) (STA = 5)
#define ABSY 12 //usually 4 (+1 if page crossed)     (STA = 5)
#define NA 13 //usually 2 (PHA = 3)(PHP = 3)(PLA = 4)(PLP = 4)(RTI = 6)(RTS = 6)(BRK = 7)


using namespace std;


typedef void (*FunctionPtr)(struct CPUState* S);

typedef struct CPUState{
    //registers
    uint8_t A, X, Y, SP;
    uint16_t PC;

    //processor status
    bool C;
    bool Z;
    bool I;
    bool D;
    bool B;
    bool V;
    bool N;

    FunctionPtr Instr[16][16];
    uint8_t addressingMode[16][16];

    bool halt;
    int cycleDif;
    uint8_t memory[65536];


} CPUState;

//initializes cpustate variables to starting values
void initialize(CPUState* S);

//To run an instruction based on its oppcode
void Run(CPUState* S);

//Add Memory to Accumulator with Carry
void ADC(CPUState* S);

//And Memory with Accumulator
void AND(CPUState* S);

//Shift left one bit (memory or accumulator)
void ASL(CPUState* S);

//Branch on Carry clear
void BCC(CPUState* S);

//Branch on Carry set
void BCS(CPUState* S);

//Branch on result zero
void BEQ(CPUState* S);

//Test Bits in memory with accumulator
void BIT(CPUState* S);

//Branch on result minus
void BMI(CPUState* S);

//branch on result not zero
void BNE(CPUState* S);

//branch on result plus
void BPL(CPUState* S);

//force break
void BRK(CPUState* S);

//Branch on Overflow Clear
void BVC(CPUState* S);

//Branch on Overflow Set
void BVS(CPUState* S);

//Clear Carry Flag
void CLC(CPUState* S);

//Clear Decimal Mode
void CLD(CPUState* S);

//Clear Interrupt Disable Bit
void CLI(CPUState* S);

//Clear Overflow Flag
void CLV(CPUState* S);

//Compare Memory and Accumulator
void CMP(CPUState* S);

//Compare Memory and Index X
void CPX(CPUState* S);

//Compare Memory and Index Y
void CPY(CPUState* S);

//Decrement Memory by One
void DEC(CPUState* S);

//Decrement Index X by One
void DEX(CPUState* S);

//Decrement Index Y by One
void DEY(CPUState* S);

//"Exclusive-or" Memory with Accumulator
void EOR(CPUState* S);

//Increment Memory by One
void INC(CPUState* S);

//Increment Index X by One
void INX(CPUState* S);

//Increment Index Y by one
void INY(CPUState* S);

//Jump to New Location
void JMP(CPUState* S);

//Jump to new Location Saving Return Address
void JSR(CPUState* S);

//Load Accumulator with Memory
void LDA(CPUState* S);

//Load Index x with Memory
void LDX(CPUState* S);

//Load Index Y with Memory
void LDY(CPUState* S);

//Shift One Bit Right (Memory or Accumulator)
void LSR(CPUState* S);

//No Operation
void NOP(CPUState* S);

//"OR" Memory with Accumulator
void ORA(CPUState* S);

//Push Accumulator on Stack
void PHA(CPUState* S);

//Push Processor Status on Stack
void PHP(CPUState* S);

//Pull Accumulator from Stack
void PLA(CPUState* S);

//Pull Processor Status from Stack
void PLP(CPUState* S);

//Rotate One Bit Left (Memory or Accumulator)
void ROL(CPUState* S);

//Rotate One Bit Right (Memory or Accumulator)
void ROR(CPUState* S);

//Return from Interrupt
void RTI(CPUState* S);

//Return from Subroutine
void RTS(CPUState* S);

//Subtract Memory from Accumulator
void SBC(CPUState* S);

//Set Carry Flag
void SEC(CPUState* S);

//Set Decimal Mode
void SED(CPUState* S);

//Set Interrupt Disable Status
void SEI(CPUState* S);

//Store Accumulator in Memory
void STA(CPUState* S);

//Store Index X in Memory
void STX(CPUState* S);

//Store Index Y in Memory
void STY(CPUState* S);

//Transfer Accumulator to Index X
void TAX(CPUState* S);

//Transfer Accumulator to Index Y
void TAY(CPUState* S);

//Transfer Stack Pointer to Index X
void TSX(CPUState* S);

//Transfer index X to Accumulator 
void TXA(CPUState* S);

//Transfer Index X to Stack pointer
void TXS(CPUState* S);

//Transfer Index Y to Accumulator
void TYA(CPUState* S);
