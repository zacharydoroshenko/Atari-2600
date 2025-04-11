#include "cpu.h"
//do store functions for X, Y, A next
//do jump next
//do compare next


//initializes cpustate variables to starting values
void initialize(CPUState* S){
    //initialize variables 
    S->PC = 0x1000;
    S->SP = 0xFF;
}

uint16_t MemmoryMirror(uint16_t s, uint8_t mod){
    // keep first 13 bits (% 0x2000)
    uint16_t result = s % 0x2000;
    if(((result >> 12) & 1) == 0){ //A12 is 0
        if(((result >> 7) & 1) == 0){ //A7 is 0
            //TIA
            if(mod == R){
                //keep first 4 bits
                result = result % 0x10;
            } else if(mod == W){ //write
                //keep first 6 bits
                result = result % 0x40;
            } else {
                printf("not using the Write or Read mode of TIA\n");
                exit(1);
            }
        } else {//A7 is 1
            //RIOT
            if(((result >> 9) & 1) == 0){ //A9 is 0
                //RAM
                // keep first 8 bits
                result = result % 0x100;
            } else { //A9 is 1
                //xxx0 xx1x 1xxx xxxx
                if(((result >> 2) & 1) == 0){ //A2 is 0
                    //IO
                    // 0000 0010 1000 00??
                    result = 0b0000001010000000 + (result & 0b11);
                } else {
                    //xxx0 xx1x 1xxx x1xx
                    if(mod == R){
                        if((result & 1) == 0){ //A0 is 0
                            //read timer
                            // 0000 0010 1000 ?100
                            result = 0b0000001010000100 + (result & 0b1000);
                        } else {
                            //read timer interrupt flag
                            // 0000 0010 1000 0101
                            result = 0b0000001010000101;
                        } 
                    } else if(mod == W) { //write
                        if(((result >> 4) & 1) == 0){ //A4 is 0
                            //write timer interrupt flag
                            // 0000 0010 1000 01??
                            result = 0b0000001010000100 + (result & 0b11);
                        } else { //A4 is 1
                            //write timer
                            // 0000 0010 1001 ?1??
                            result = 0b0000001010010100 + (result & 0b1011);
                        } 
                    } else {
                        printf("not using the Write or Read mode of TIA\n");
                        exit(1);
                    }
                }
            }
        }
    }

    return result;
}

//To run an instruction based on its oppcode
void Run(CPUState* S){
    static const FunctionPtr Instr[16][16] = {
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

    static const uint8_t addressingMode[16][16] = {
        {IMP, INDX, 0, 0, 0, ZPG, ZPG, 0, IMP, IMM, AA, 0, 0, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0},
        {ABS, INDX, 0, 0, ZPG, ZPG, ZPG, 0, IMP, IMM, AA, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0},
        {IMP, INDX, 0, 0, 0, ZPG, ZPG, 0, IMP, IMM, AA, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0},
        {IMP, INDX, 0, 0, 0, ZPG, ZPG, 0, IMP, IMM, AA, 0, IND, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0},
        {0, INDX, 0, 0, ZPG, ZPG, ZPG, 0, IMP, 0, IMP, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, ZPGX, ZPGX, ZPGY, 0, IMP, ABSY, IMP, 0, 0, ABSX, 0, 0},
        {IMM, INDX, IMM, 0, ZPG, ZPG, ZPG, 0, IMP, IMM, IMP, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, ZPGX, ZPGX, ZPGY, 0, IMP, ABSY, IMP, 0, ABSX, ABSX, ABSX, 0},
        {IMM, INDX, 0, 0, ZPG, ZPG, ZPG, 0, IMP, IMM, IMP, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0},
        {IMM, INDX, 0, 0, ZPG, ZPG, ZPG, 0, IMP, IMM, IMP, 0, ABS, ABS, ABS, 0},
        {REL, INDY, 0, 0, 0, ZPGX, ZPGX, 0, IMP, ABSY, 0, 0, 0, ABSX, ABSX, 0}
    };


    static const uint16_t modeM[16][16] = {
        {0, R, 0, 0, 0, R, WR, 0, 0, R, WR, 0, 0, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0},
        {0, R, 0, 0, R, R, WR, 0, 0, R, WR, 0, R, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, WR, 0, 0, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, WR, 0, 0, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0},
        {0, W, 0, 0, W, W, W, 0, 0, 0, 0, 0, W, W, W, 0},
        {0, W, 0, 0, W, W, W, 0, 0, W, 0, 0, 0, W, 0, 0},
        {R, R, R, 0, R, R, R, 0, 0, R, 0, 0, R, R, R, 0},
        {0, R, 0, 0, R, R, R, 0, 0, R, 0, 0, R, R, R, 0},
        {R, R, 0, 0, R, R, WR, 0, 0, R, 0, 0, R, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0},
        {R, R, 0, 0, R, R, WR, 0, 0, R, 0, 0, R, R, WR, 0},
        {0, R, 0, 0, 0, R, WR, 0, 0, R, 0, 0, 0, R, WR, 0}
    };

    uint8_t oppcode = S->memory[S->PC]; 
    uint8_t aMode = addressingMode[oppcode >> 4][oppcode & 0b1111];
    uint16_t mode = modeM[oppcode >> 4][oppcode & 0b1111];

    //based on addressing mode extract data and update cycles/ProgramCounter/target
    if(aMode == IMP){
        //Implied addressing
        S->cycleDif = 2;
        S->PC++;
    } else if(aMode == AA){
        //Accumulator 
        S->target = &(S->A);

        S->cycleDif = 2;
        S->PC++;
    } else if(aMode == REL){
        //Relative addressing
        S->target = S->memory + S->PC + 1;

        S->cycleDif = 2;
        S->PC += 2;
    }  else if(aMode == IMM){
        //Immediate
        S->target = S->memory + S->PC + 1;

        S->cycleDif = 2;
        S->PC += 2;

    } else if(aMode == IND){
        //Indirect addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1];
        location = MemmoryMirror(location, mode);
        uint16_t actualLocation = (S->memory[location + 1] << 8) + S->memory[location];
        actualLocation = MemmoryMirror(actualLocation, mode);
        S->target = S->memory + actualLocation;

        S->cycleDif = 5;
        S->PC += 3;

    } else if(aMode == INDY){
        //Indirect Y
        uint8_t location = S->memory[S->PC + 1];
        location = MemmoryMirror(location, mode);
        uint16_t oldLocation = (S->memory[location + 1] << 8) + S->memory[location];
        uint16_t actualLocation = oldLocation + S->Y;
        actualLocation = MemmoryMirror(actualLocation, mode);
        S->target = S->memory + actualLocation;

        S->cycleDif = (oldLocation >> 8 == actualLocation >> 8) ? 5 : 6;
        S->PC += 2;
    } else if(aMode == INDX){
        //Indirect X
        uint8_t location = S->memory[S->PC + 1] + S->X;
        // printf("0x%X\n", location);
        location = MemmoryMirror(location, mode);
        uint16_t actualLocation = (S->memory[location + 1] << 8) + S->memory[location];
        actualLocation = MemmoryMirror(actualLocation, mode);
        S->target = S->memory + actualLocation;

        S->cycleDif = 6;
        S->PC += 2;
        
    } else if(aMode == ZPG){
        //Zero page addressing
        uint8_t location = S->memory[S->PC + 1];
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;
        

        S->cycleDif = 3;
        S->PC += 2;

    } else if(aMode == ZPGX){
        //Zero page X addressing
        uint8_t location = S->memory[S->PC + 1] + S->X;
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 2;

    } else if(aMode == ZPGY){
        //Zero page Y addressing
        uint8_t location = S->memory[S->PC + 1] + S->Y;
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 2;

    } else if(aMode == ABS){
        //Absolute addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1];
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;
        S->cycleDif = 4;
        S->PC += 3;

    } else if(aMode == ABSX){
        //Absolute X addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1] + S->X;
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 3;

    } else if(aMode == ABSY){
        //Absolute Y addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1] + S->Y;
        location = MemmoryMirror(location, mode);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 3;

    } else {
        printf("addressing mode isn't valid\n");
        exit(1);
    }

    //run the instruction
    Instr[oppcode >> 4][oppcode & 0b1111](S);
}

//Add Memory to Accumulator with Carry
void ADC(CPUState* S){
    uint8_t oldA = S->A;
    
    S->A = S->A + *(S->target) + S->C;
    
    
    uint8_t oldASign = (oldA >> 7) & 1;
    uint8_t targetSign = (*(S->target) >> 7) & 1;
    uint8_t resultSign = (S->A >> 7) & 1;
    //check if overflow
    //if target and oldA have the same sign and result has opposite sign
    S->V = (oldASign == targetSign && resultSign != oldASign) ? 1 : 0;

    //check if carry
    S->C = (S->A < oldA || S->A < *(S->target)) ? 1 : 0;
    
    S->Z = (S->A == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//And Memory with Accumulator
void AND(CPUState* S){
    S->A = S->A & *(S->target);

    //zero
    S->Z = (S->A == 0) ? 1 : 0;

    //negative
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Shift left one bit (memory or accumulator)
void ASL(CPUState* S){
    S->cycleDif += 2;

    //update carry
    S->C = (*(S->target) >> 7) & 1;
    //perform rotate
    *(S->target) = *(S->target) << 1;

    //zero
    S->Z = (*(S->target) == 0) ? 1 : 0;

    //negative
    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;
}

//Branch on Carry clear
void BCC(CPUState* S){
    if(S->C == 0){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }

}

//Branch on Carry set
void BCS(CPUState* S){
    //extract the argument

    if(S->C == 1){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Branch on result zero
void BEQ(CPUState* S){
    if(S->Z == 1){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Test Bits in memory with accumulator
void BIT(CPUState* S){
    uint8_t result = S->A & *(S->target);
    S->Z = result == 0;
    S->V = (*(S->target) >> 6) & 1;
    S->N = (*(S->target) >> 7) & 1;
}

//Branch on result minus
void BMI(CPUState* S){
    if(S->N == 1){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//branch on result not zero
void BNE(CPUState* S){
    if(S->Z == 0){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//branch on result plus
void BPL(CPUState* S){
    if(S->N == 0){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//force break (force interrupt) program counter then status
void BRK(CPUState* S){
    S->A++;
    S->A--;
    // S->B = 1;

    

    // //push program counter
    // S->PC++;
    // S->SP++;
    // // S->memory[S->SP] = S->PC; THATS WRONG PC IS 16 bits

    // //push status
    // S->SP++;
    // uint8_t P = 0b00100000;
    // if(S->C == true) P += 0b00000001;
    // if(S->Z == true) P += 0b00000010;
    // if(S->I == true) P += 0b00000100;
    // if(S->D == true) P += 0b00001000;
    // if(S->B == true) P += 0b00010000;
    // if(S->V == true) P += 0b01000000;
    // if(S->N == true) P += 0b10000000;
    // S->memory[S->SP] = P;

    // //IRQ interrupt vector at $FFFE/F is loaded into the PC
    // // S->PC = 0xFFFE; THIS is wrong since its just an address that is located at FFFE/FFFF


    // //break flag in the status set to one.
    // S->B = 1;


    // S->cycleDif = 7;
    
}

//Branch on Overflow Clear
void BVC(CPUState* S){
    if(S->V == 0){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Branch on Overflow Set
void BVS(CPUState* S){
    if(S->V == 1){
        //branch taken
        int8_t offset = *(S->target);
        S->PC += offset;

        //put correct cycleDif based on if it crossed a page
        S->cycleDif = ((S->PC - *(S->target)) >> 8 == S->PC >> 8) ? 3 : 4;
    }
}

//Clear Carry Flag
void CLC(CPUState* S){
    S->C = 0;
}

//Clear Decimal Mode
void CLD(CPUState* S){
    S->D = 0;
}

//Clear Interrupt Disable Bit
void CLI(CPUState* S){
    S->I = 0;
}

//Clear Overflow Flag
void CLV(CPUState* S){
    S->V = 0;
}

//Compare Memory and Accumulator
void CMP(CPUState* S){
    uint8_t result = S->A - *(S->target);
    S->C = S->A >= *(S->target);
    S->Z = S->A == *(S->target);
    S->N = (result >> 7) & 1;
}

//Compare Memory and Index X
void CPX(CPUState* S){
    uint8_t result = S->X - *(S->target);
    S->C = S->X >= *(S->target);
    S->Z = S->X == *(S->target);
    S->N = (result >> 7) & 1;
}

//Compare Memory and Index Y
void CPY(CPUState* S){
    uint8_t result = S->Y - *(S->target);
    S->C = S->Y >= *(S->target);
    S->Z = S->Y == *(S->target);
    S->N = (result >> 7) & 1;
}

//Decrement Memory by One
void DEC(CPUState* S){
    *(S->target) -= 1;

    S->Z = (*(S->target) == 0) ? 1 : 0;

    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;

    S->cycleDif += 2;
}

//Decrement Index X by One
void DEX(CPUState* S){
    S->X--;
    
    S->Z = (S->X == 0) ? 1 : 0;

    S->N = ((S->X >> 7) & 1) ? 1 : 0;
}

//Decrement Index Y by One
void DEY(CPUState* S){
    S->Y--;
    
    S->Z = (S->Y == 0) ? 1 : 0;
    //extracted bit
    S->N = ((S->Y >> 7) & 1) ? 1 : 0;
}

//"Exclusive-or" Memory with Accumulator
void EOR(CPUState* S){
    S->A = S->A ^ *(S->target);

    //zero
    S->Z = (S->A == 0) ? 1 : 0;

    //negative
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Increment Memory by One
void INC(CPUState* S){
    *(S->target) += 1;

    S->Z = (*(S->target) == 0) ? 1 : 0;

    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;

    S->cycleDif += 2;
}

//Increment Index X by One
void INX(CPUState* S){
    S->X++;
    
    S->Z = (S->X == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->X >> 7) & 1) ? 1 : 0;
}

//Increment Index Y by one
void INY(CPUState* S){
    S->Y++;
    
    S->Z = (S->Y == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->Y >> 7) & 1) ? 1 : 0;
}

//Jump to New Location
void JMP(CPUState* S){
    //check if absolute
    if(S->memory[S->PC - 3] == 0x4C) S->cycleDif = 3;

    S->PC = S->target - S->memory;
}

//Jump to new Location Saving Return Address
void JSR(CPUState* S){
    //push PC to stack
    uint16_t toPush = S->PC - 1;
    S->memory[S->SP] = toPush >> 8;
    S->SP--;
    S->memory[S->SP] = toPush & 0xFF;
    S->SP--;
    
    //jump to target address
    S->PC = S->target - S->memory;

    S->cycleDif += 2;
}

//Load Accumulator with Memory
void LDA(CPUState* S){
    S->A = *(S->target);

    //zero
    S->Z = (S->A == 0) ? 1 : 0;

    //negative
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Load Index x with Memory
void LDX(CPUState* S){
    S->X = *(S->target);

    //zero
    S->Z = (S->X == 0) ? 1 : 0;

    //negative
    S->N = ((S->X >> 7) & 1) ? 1 : 0;
}

//Load Index Y with Memory
void LDY(CPUState* S){
    S->Y = *(S->target);

    //zero
    S->Z = (S->Y == 0) ? 1 : 0;

    //negative
    S->N = ((S->Y >> 7) & 1) ? 1 : 0;
}

//Shift One Bit Right (Memory or Accumulator)
void LSR(CPUState* S){
    S->cycleDif += 2;

    //update carry
    S->C = *(S->target) & 1;
    //perform rotate
    *(S->target) = *(S->target) >> 1;

    //zero
    S->Z = (*(S->target) == 0) ? 1 : 0;

    //negative
    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;
}

//No Operation
void NOP(CPUState* S){
    //its here to avoid errors
    S->A++;
    S->A--;
}

//"OR" Memory with Accumulator
void ORA(CPUState* S){
    
    S->A = S->A | *(S->target);

    //zero
    S->Z = (S->A == 0) ? 1 : 0;

    //negative
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Push Accumulator on Stack
void PHA(CPUState* S){
    S->memory[S->SP] = S->A;
    S->SP--;

    S->cycleDif += 1;
}

//Push Processor Status on Stack
void PHP(CPUState* S){
    uint8_t P = 0b00100000;
    if(S->C == true) P += 0b00000001;
    if(S->Z == true) P += 0b00000010;
    if(S->I == true) P += 0b00000100;
    if(S->D == true) P += 0b00001000;
    if(S->B == true) P += 0b00010000;
    if(S->V == true) P += 0b01000000;
    if(S->N == true) P += 0b10000000;

    S->memory[S->SP] = P;
    S->SP--;

    S->cycleDif += 1;
}

//Pull Accumulator from Stack
void PLA(CPUState* S){
    S->SP++;
    S->A = S->memory[S->SP];

    S->Z = (S->A == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->A >> 7) & 1) ? 1 : 0;

    S->cycleDif += 2;
}

//Pull Processor Status from Stack
void PLP(CPUState* S){
    S->SP++;
    uint8_t temp = S->memory[S->SP];

    S->C = (((temp >> 0) & 1) == 0) ? 0 : 1;
    S->Z = (((temp >> 1) & 1) == 0) ? 0 : 1;
    S->I = (((temp >> 2) & 1) == 0) ? 0 : 1;
    S->D = (((temp >> 3) & 1) == 0) ? 0 : 1;
    S->B = (((temp >> 4) & 1) == 0) ? 0 : 1;
    S->V = (((temp >> 6) & 1) == 0) ? 0 : 1;
    S->N = (((temp >> 7) & 1) == 0) ? 0 : 1;

    S->cycleDif += 2;
}

//Rotate One Bit Left (Memory or Accumulator)
void ROL(CPUState* S){
    S->cycleDif += 2;

    //save 7th bit
    uint8_t temp = (*(S->target) >> 7) & 1;
    //perform rotate
    *(S->target) = *(S->target) << 1;
    //update 0th bit to equal carry
    *(S->target) = *(S->target) | S->C;
    //update carry
    S->C = temp;

    //zero
    S->Z = (*(S->target) == 0) ? 1 : 0;

    //negative
    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;

}

//Rotate One Bit Right (Memory or Accumulator)
void ROR(CPUState* S){
    S->cycleDif += 2;

    //save 0th bit
    uint8_t temp = *(S->target) & 1;
    //perform rotate
    *(S->target) = *(S->target) >> 1;
    //set 7th bit to equal carry
    *(S->target) = (*(S->target) & 0x7F) | (S->C << 7);
    //update carry
    S->C = temp;

    //zero
    S->Z = (*(S->target) == 0) ? 1 : 0;

    //negative
    S->N = ((*(S->target) >> 7) & 1) ? 1 : 0;

}

//Return from Interrupt
void RTI(CPUState* S){

    //pull processor Status from Stack
    S->SP++;
    uint8_t temp = S->memory[S->SP];
    
    S->C = (((temp >> 0) & 1) == 0) ? 0 : 1;
    S->Z = (((temp >> 1) & 1) == 0) ? 0 : 1;
    S->I = (((temp >> 2) & 1) == 0) ? 0 : 1;
    S->D = (((temp >> 3) & 1) == 0) ? 0 : 1;
    S->B = (((temp >> 4) & 1) == 0) ? 0 : 1;
    S->V = (((temp >> 6) & 1) == 0) ? 0 : 1;
    S->N = (((temp >> 7) & 1) == 0) ? 0 : 1;

    //pull program counter
    S->SP += 2;
    S->PC = (S->memory[S->SP] << 8) + S->memory[S->SP - 1]; 


    S->cycleDif += 4;
}

//Return from Subroutine
void RTS(CPUState* S){
    //pull program counter
    S->SP += 2;
    S->PC = (S->memory[S->SP] << 8) + S->memory[S->SP - 1] + 1;

    S->cycleDif += 4;

}

//Subtract Memory from Accumulator
void SBC(CPUState* S){
    uint8_t oldA = S->A;
    S->A = S->A - *(S->target) - (1 - S->C);
    
    
    
    uint8_t oldASign = (oldA >> 7) & 1;
    uint8_t targetSign = (*(S->target) >> 7) & 1;
    uint8_t resultSign = (S->A >> 7) & 1;
    //check if overflow
    //if target and oldA have the same sign and result has opposite sign
    S->V = (oldASign != targetSign && resultSign != oldASign) ? 1 : 0;

    //check if carry
    // S->C = (S->A < oldA || S->A < *(S->target)) ? 0 : 1;
    S->C = (oldA >= *(S->target) + (-1 * S->C)) ? 1 : 0;
    
    S->Z = (S->A == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Set Carry Flag
void SEC(CPUState* S){
    S->C = 1;
}

//Set Decimal Mode
void SED(CPUState* S){
    S->D = 1;
}

//Set Interrupt Disable Status
void SEI(CPUState* S){
    S->I = 1;
}

//Store Accumulator in Memory
void STA(CPUState* S){
    *(S->target) = S->A;
}

//Store Index X in Memory
void STX(CPUState* S){
    *(S->target) = S->X;
}

//Store Index Y in Memory
void STY(CPUState* S){
    *(S->target) = S->Y;
}

//Transfer Accumulator to Index X
void TAX(CPUState* S){
    S->X = S->A;
    
    S->Z = (S->X == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->X >> 7) & 1) ? 1 : 0;
}

//Transfer Accumulator to Index Y
void TAY(CPUState* S){
    S->Y = S->A;
    
    S->Z = (S->Y == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->Y >> 7) & 1) ? 1 : 0;
}

//Transfer Stack Pointer to Index X
void TSX(CPUState* S){
    S->X = S->SP;
    
    S->Z = (S->X == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->X >> 7) & 1) ? 1 : 0;
}

//Transfer index X to Accumulator 
void TXA(CPUState* S){
    S->A = S->X;
    
    S->Z = (S->A == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}

//Transfer Index X to Stack pointer
void TXS(CPUState* S){
    S->SP = S->X;
}

//Transfer Index Y to Accumulator
void TYA(CPUState* S){
    S->A = S->Y;
    
    S->Z = (S->A == 0) ? 1 : 0;

    //extracted bit
    S->N = ((S->A >> 7) & 1) ? 1 : 0;
}
