//Helpful links
//forums.atariage.com/topic/27190-session-5-memory-architecture/
//nesdev.org/obelisk-6502-guide/reference.html

//Assumptions made:
//decrement is done by I--
//increment is done by I++
//to push to stack you do S->SP++; S->memory[S->SP] = S->A;
//to pull from stack you do S->A = S->memory[S->SP]; S->SP--;
//offset can be calculated with S->PC += offset;

#include <iostream>
#include <cstdint>

using namespace std;

typedef struct {
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


    bool halt;
    int cycleDif;
    uint8_t memory[65536];

    uint8_t oppcode;

} CPUState;


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
