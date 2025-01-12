//ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CMP CPX XPY DEC
//EOR INC JMP JSR LDA LDX LDY LSR ORA PHA PHP PLA PLP ROL ROR RTI RTS SBC 
//STA STX STY


#include <cstdint>
#include <iostream>
using namespace std;

uint8_t A = 0;
uint8_t X = 0;
uint8_t Y = 0;
uint16_t PC = 0;
uint8_t SP = 0;
uint8_t SR = 0b00100000; //32
int cycles = 0;

//Add memory to accumulator with Carry
void ADC(uint8_t M){


    PC++;
}

//"AND" Memory with Accumulator
void AND(uint8_t Memory){
    Memory = A && Memory;

    PC++;
}

//Shift left One Bit (Memory or Accumulator)
void ASL(){
    PC++;
}

//Branch on Carry Clear
void BCC(uint16_t address){
    //Extract bit 0
    int extractedBit = (SR >> 0) & 1;

    if(extractedBit == 0){
        PC = address;
    } else {
        PC++;
    }
}

//Branch on Carry Set
void BCS(uint16_t address){
    //Extract bit 0
    int extractedBit = (SR >> 0) & 1;

    if(extractedBit == 1){
        PC = address;
    } else {
        PC++;
    }
}

//Branch on Result Zero
void BEQ(uint16_t address){
    //Extract bit 1
    int extractedBit = (SR >> 1) & 1;

    if(extractedBit == 1){
        PC = address;
    } else {
        PC++;
    }
}

//Test Bits In Memory with Accumulator
void BIT(){
    PC++;
}

//Branch on Result Minus
void BMI(uint16_t address){
    //Extract bit 7
    int extractedBit = (SR >> 7) & 1;

    if(extractedBit == 1){
        PC = address;
    } else {
        PC++;
    }
}

//Branch on Result not Zero
void BPL(uint16_t address){
    //Extract bit 1
    int extractedBit = (SR >> 1) & 1;

    if(extractedBit == 0){
        PC = address;
    } else {
        PC++;
    }
}

//Force Break
void BNK(){
    
}

//Branch on Overflow Clear
void BVC(uint16_t address){
    //Extract bit 6
    int extractedBit = (SR >> 6) & 1;

    if(extractedBit == 0){
        PC = address;
    } else {
        PC++;
    }
}

//Branch on Overflow Set
void BVS(uint16_t address){
    //Extract bit 6
    int extractedBit = (SR >> 6) & 1;

    if(extractedBit == 1){
        PC = address;
    } else {
        PC++;
    }
    
}

//Clear Carry Flag
void CLC(){
    //clear bit 0
    SR = SR & ~(1 << 0);

    PC++;
    cycles += 2;
}

//Clear Decimal Mode
void CLD(){
    //clear bit 3
    SR = SR & ~(1 << 3);

    PC++;
    cycles += 2;
}

//Clear Interrupt Disable Bit
void CLI(){
    //clear bit 2
    SR = SR & ~(1 << 2);

    PC++;
    cycles += 2;
}

//Clear Overflow Flag
void CLV(){
    //clear bit 6
    SR = SR & ~(1 << 6);

    PC++;
    cycles += 2;
}

//Compare Memory and Accumulator
void CMP(){
    PC++;
}

//Compare Memory and Index X
void CPX(){
    PC++;
}

//Compare Memory and Index Y
void CPY(){
    PC++;
}

//Decrement Memory by One
void DEC(){
    PC++;
}

//Decrement Index X by One
void DEX(){
    X--;
    PC++;
    cycles += 2;
}

//Decrement Index Y by One
void DEY(){
    Y--;
    PC++;
    cycles += 2;
}

//Exclusive-OR Memory with Accumulator
void EOR(){
    PC++;
}

//Increment Memory by One
void INC(){
    PC++;
}

//increment X by 1
void INX(){
    X++;
    PC++;
    cycles += 2;
}

//increment Y by 1
void INY(){
    Y++;
    PC++;
    cycles += 2;
}

//Jump to New Location
void JMP(){

}

//Jump to New Location Saving Return Address
void JSR(){

}

//Load Accumulator with Memory
void LDA(uint8_t firstByte){
    A = firstByte;
    PC++;
}

//Load Index X with Memory
void LDX(uint8_t firstByte){
    X = firstByte;
    PC++;
}

//Load Index Y with Memory
void LDY(uint8_t firstByte){
    Y = firstByte;
    PC++;
}

//Shift One Bit Right (Memory or Accumulator)
void LSR(){
    
    PC++;
    
}

//No Operation
void NOP(){
    PC++;
    cycles += 2;
}

//"OR" Memory with Accumulator
void ORA(){
    PC++;
}

//Push Accumulator on Stack
void PHA(){
    PC++;
}

//Push Processor Status on Stack
void PHP(){
    PC++;
}

//Pull Accumulator from Stack
void PLA(){
    PC++;
}

//Pull Processor Status from Stack
void PLP(){
    PC++;
}

//Rotate One Bit Left (Memory or Accumulator)
void ROL(){
    PC++;
}

//Rotate One Right (Memory or Accumulator)
void ROR(){
    PC++;
}

//Return from Interrupt
void RTI(){
    PC++;
}

//Return from Subroutine
void RTS(){
    PC++;
}

//Subtract Memory from Accumulator with Borrow
void SBC(){
    PC++;
}

//Set Carry Flag
void SEC(){
    //set bit 0
    SR = SR | (1 << 0);
    PC++;
    cycles += 2;
}

//Set Decimal Mode
void SED(){
    //set bit 3
    SR = SR | (1 << 3);

    PC++;
    cycles += 2;
}

//Set Interrupt Disable Status
void SEI(){
    //set bit 2
    SR = SR | (1 << 2);

    PC++;
    cycles += 2;
}

//Store Accumulator in Memory
void STA(){
    PC++;
}

//Store Index X in Memory
void STX(){
    PC++;
}

//Store Index Y in Memory
void STY(){
    PC++;
}

//Transfer Accumulator to Index X
void TAX(){
    X = A;
    PC++;
    cycles +=2;
}

//Transfer Accumulator to Index Y 
void TAY(){
    Y = A;
    PC++;
    cycles += 2;
}

//Transfer Stack Pointer to Index X
void TSX(){
    X = SP;
    PC++;
    cycles +=2;
}

//Transfer Index X to Accumulator
void TXA(){
    A = X;
    PC++;
    cycles += 2;
}

//Transfer Index X to Stack Pointer
void TXS(){
    SP = X;
    PC++;
    cycles += 2;
}

//Transfer Index Y to Accumulator
void TYA(){
    A = Y;
    PC++;
    cycles += 2;
}

