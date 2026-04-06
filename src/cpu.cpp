#include "cpu.h"
#include "addressDef.h"
//do store functions for X, Y, A next
//do jump next
//do compare next

uint8_t applyHmove(uint8_t mem, uint8_t pos){
    int8_t adjust = (int8_t)mem >> 4;
    int newPos = (int)pos - adjust;
    if (newPos < 0) newPos += 160;
    if (newPos >= 160) newPos -= 160;
    return newPos;
}

//initializes cpustate variables to starting values
void initialize(CPUState* S, int size, TIAState* T, TimerState* Ti) {

    S->T = T;
    S->Ti = Ti;
    T->swchb=0b11001011;

    // 1. Zero out all memory first to ensure a clean state
    for (int i = 0; i < 0xFFFF; i++) {
        S->memory[i] = 0x00;
    }

    // 2. Set CPU Registers
    // uint16_t lo = MemoryMirror(0xFFFC, R, S, Ti);
    // uint16_t hi = MemoryMirror(0xFFFD, R, S, Ti);
    // S->PC = (S->memory[hi] << 8) | S->memory[lo];
    S->PC = 0x1000;
    S->SP = 0xFF;
    S->fourKilo = size > 2048;

    // 3. Hardware Defaults (Active Low / High bits)
    S->memory[SWCHA] = 0xFF; // Joysticks idle
    // S->memory[SWCHB] = 0xFF; // Console switches idle
    S->memory[INPT4] = 0x80; // P0 Button up (bit 7 is 1)
    S->memory[INPT5] = 0x80; // P1 Button up (bit 7 is 1)
    // printf("SWCHB: 0x%X\n", S->memory[SWCHB]);

    //debug
    if(false){
        S->frameCounter = true;
        S->memoryGrid = true;
        S->instructions = true;
        S->frameStep = true;    
    } else {
        S->frameCounter = false;
        S->memoryGrid = false;
        S->instructions = false;
        S->frameStep = false;
    }

}

uint16_t MemoryMirror(uint16_t s, uint8_t mod, CPUState* S){
    // keep first 13 bits (% 0x2000)
    uint16_t result = s % 0x2000;
    if(((result >> 12) & 1) == 0){ //A12 is 0
            
        if(((result >> 7) & 1) == 0){ //A7 is 0
            //TIA
            if(mod == R){
                //keep first 4 bits
                result = result & 0x3F;
            } else if(mod == W){ //write
                //keep first 6 bits
                result = result % 0x40;
                //strobe and tia effects
                switch (result){
                case WSYNC:
                    S->halt = true;
                    S->cycleDif = 0;
                    break;
                case GRP0:
                    S->T->updatePlayerBit0=true;
                    if(S->memory[VDELP0] & 1) return GRP0BUFF;
                    S->grp0 = true;
                    break;
                case GRP1:
                    S->T->updatePlayerBit1=true;
                    if(S->memory[VDELP1] & 1) return GRP1BUFF;
                    S->grp1 = true;
                    break;
                case NUSIZ0:
                    S->T->updatePlayerBit0=true;
                    break;
                case NUSIZ1:
                    S->T->updatePlayerBit1=true;
                    break;
                case RESP0:
                    S->T->updatePlayerBit0=true;
                    S->resp0=true;
                    break;
                case RESP1:
                    S->T->updatePlayerBit1=true;
                    S->resp1=true;
                    break;
                case RESM0:
                    S->resm0=true;
                    break;
                case RESM1:
                    S->resm1=true;
                    break;
                case RESBL:
                    S->resbl=true;
                    break;
                case HMP0:
                    S->T->updatePlayerBit0=true;
                    break;
                case HMP1:
                    S->T->updatePlayerBit1=true;
                    break;
                case REFP0:
                    S->T->updatePlayerBit0=true;
                    break;
                case REFP1:
                    S->T->updatePlayerBit1=true;
                    break;
                case CXCLR:
                    S->cxclr=true;
                    break;
                case HMOVE:
                    S->T->updatePlayerBit0=true;
                    S->T->updatePlayerBit1=true;
                    S->hmove=true;
                    break;
                case HMCLR:
                    S->hmclr=true;
                    break;
                default:
                    break;
                }

            } else {
                // printf("not using the Write or Read mode of TIA\n");
                // exit(1);
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
                    if(result == SWCHB && mod == R){
                        // printf("WE MAKE THE CORRECT SWCHB if it wants to be read\n");
                        // WE MAKE THE CORRECT SWCHB if it wants to be read
                        S->memory[SWCHBREAD] = (S->memory[SWCHB] & 0b00110100) | (S->T->swchb & 0b11001011);
                        return  SWCHBREAD;
                    }
                } else {
                    //xxx0 xx1x 1xxx x1xx
                    if(mod == R){
                        if((result & 1) == 0){ //A0 is 0
                            //read timer
                            // 0000 0010 1000 ?100
                            result = 0b0000001010000100 + (result & 0b1000);
                            if(result == INTIM) S->Ti->hyperMode = false;
                        } else {
                            //read timer interrupt flag
                            // 0000 0010 1000 0101
                            result = 0b0000001010000101;
                            //OK SO BASICALLY I gotta rest bit 6 in INSTAT but only after it is retrieved
                            S->Ti->instatBit6reset=true;
                        } 
                    } else if(mod == W) { //write
                        if(((result >> 4) & 1) == 0){ //A4 is 0
                            //write timer interrupt flag
                            // 0000 0010 1000 01??
                            result = 0b0000001010000100 + (result & 0b11);
                        } else { //A4 is 1
                            //write timer
                            // 0000 0010 1001 ?1??
                            // result = 0b0000001010010100 + (result & 0b1011);
                            uint16_t timerAddr = 0b0000001010010100 + (result & 0b1011);
                            setTimer(S, timerAddr);
                            result = INTIM;  // ← route write to INTIM so initial value (43) lands there
                        } 
                    } else {
                        printf("not using the Write or Read mode of TIA\n");
                        exit(1);
                    }
                }
            }
        }
    } else {
        //A12 is 1 you are in cartridge

        //2k cart::::
        if(S->fourKilo == false){
            result = result % 0x800;
            result += 0x1000;
        }
        return result;
    }

    return result;
}

//To run an instruction based on its oppcode
void Run(CPUState* S){

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
        location = MemoryMirror(location, mode, S);
        uint16_t actualLocation = (S->memory[location + 1] << 8) + S->memory[location];
        actualLocation = MemoryMirror(actualLocation, mode, S);
        S->target = S->memory + actualLocation;

        S->cycleDif = 5;
        S->PC += 3;

    } else if(aMode == INDY){
        //Indirect Y
        uint8_t location = S->memory[S->PC + 1];
        location = MemoryMirror(location, mode, S);
        uint16_t oldLocation = (S->memory[location + 1] << 8) + S->memory[location];
        uint16_t actualLocation = oldLocation + S->Y;
        actualLocation = MemoryMirror(actualLocation, mode, S);
        S->target = S->memory + actualLocation;

        S->cycleDif = (oldLocation >> 8 == actualLocation >> 8) ? 5 : 6;
        S->PC += 2;
    } else if(aMode == INDX){
        //Indirect X
        uint8_t location = S->memory[S->PC + 1] + S->X;
        // printf("0x%X\n", location);
        location = MemoryMirror(location, mode, S);
        uint16_t actualLocation = (S->memory[location + 1] << 8) + S->memory[location];
        actualLocation = MemoryMirror(actualLocation, mode, S);
        S->target = S->memory + actualLocation;

        S->cycleDif = 6;
        S->PC += 2;
        
    } else if(aMode == ZPG){
        //Zero page addressing
        uint8_t location = S->memory[S->PC + 1];
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;
        

        S->cycleDif = 3;
        S->PC += 2;

    } else if(aMode == ZPGX){
        //Zero page X addressing
        uint8_t location = S->memory[S->PC + 1] + S->X;
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 2;

    } else if(aMode == ZPGY){
        //Zero page Y addressing
        uint8_t location = S->memory[S->PC + 1] + S->Y;
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 2;

    } else if(aMode == ABS){
        //Absolute addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1];
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;
        S->cycleDif = 4;
        S->PC += 3;

    } else if(aMode == ABSX){
        //Absolute X addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1] + S->X;
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 3;

    } else if(aMode == ABSY){
        //Absolute Y addressing
        uint16_t location = (S->memory[S->PC + 2] << 8) + S->memory[S->PC + 1] + S->Y;
        location = MemoryMirror(location, mode, S);
        S->target = S->memory + location;

        S->cycleDif = 4;
        S->PC += 3;

    } else {
        printf("addressing mode isn't valid\n");
        exit(1);
    }

    //run the instruction
    if(mode == W){
        S->writeQueueOpp = oppcode;
    } else {
        Instr[oppcode >> 4][oppcode & 0b1111](S);
    }

    //check for bit 6 INSTAT reset (since its not R dont have to worry about instruction running after this)
    if(S->Ti->instatBit6reset){
        S->memory[INSTAT] &= 0b10111111;
        S->Ti->instatBit6reset = false;
    } 

}

void HandleStrobe(CPUState* S){
    TIAState* T= S->T;
    if(S->resp0){
        T->player0H = T->hpos - 68 + 5;

        //if bits 0-2 of NUSIZ0 is 5 or 7 then add 1 
        uint8_t nusiz = S->memory[NUSIZ0] & 0b00000111;
        if(nusiz == 5 || nusiz == 7) T->player0H++;
        S->resp0=false;
    }
    if(S->resp1){
        T->player1H = T->hpos - 68 + 5;
        // printf("Resp1 activated\n");

        //if bits 0-2 of NUSIZ0 is 5 or 7 then add 1 
        uint8_t nusiz = S->memory[NUSIZ1] & 0b00000111;
        if(nusiz == 5 || nusiz == 7) T->player1H++;
        S->resp1=false;
    }
    if(S->resm0){
        T->missile0H = T->hpos - 68 + 2;
        if((S->memory[RESMP0] >> 1) & 1) S->T->missile0H = S->T->player0H + 3;
        S->resm0=false;
    }
    if(S->resm1){
        T->missile1H = T->hpos - 68 + 2;
        if((S->memory[RESMP1] >> 1) & 1) S->T->missile1H = S->T->player1H + 3;
        S->resm1=false;
    }
    if(S->resbl){
        T->ballH = T->hpos - 68 + 2;
        S->resbl=false;
    }
    if(S->grp0){
        if(S->memory[VDELP1] & 1) S->memory[GRP1] = S->memory[GRP1BUFF];
        S->grp0=false;
    }
    if(S->grp1){
        if(S->memory[VDELP0] & 1) S->memory[GRP0] = S->memory[GRP0BUFF];
        S->grp1=false;
    }
    if(S->cxclr){
        // printf("CXCLR fired at hpos:%d vpos:%d\n", S->T->hpos, S->T->vpos);
        for(int i = 0x30; i <= 0x37; i++) {
            if(S->memory[i]); //printf("  clearing 0x%X = 0x%X\n", i, S->memory[i]);
            S->memory[i] = 0;
        }
        S->cxclr = false;
    }
    if(S->hmove){
        int8_t adj0  = (int8_t)(S->memory[HMP0]) >> 4;
        int8_t adj1  = (int8_t)(S->memory[HMP1]) >> 4;
        int8_t adjm0 = (int8_t)(S->memory[HMM0]) >> 4;
        int8_t adjm1 = (int8_t)(S->memory[HMM1]) >> 4;
        int8_t adjb  = (int8_t)(S->memory[HMBL]) >> 4;
        S->T->player0H  = ((S->T->player0H  - adj0 ) % 160 + 160) % 160;
        S->T->player1H  = ((S->T->player1H  - adj1 ) % 160 + 160) % 160;
        S->T->missile0H = ((S->T->missile0H - adjm0) % 160 + 160) % 160;
        S->T->missile1H = ((S->T->missile1H - adjm1) % 160 + 160) % 160;
        S->T->ballH     = ((S->T->ballH     - adjb ) % 160 + 160) % 160;

        //check if missles locked on player
        if((S->memory[RESMP0] >> 1) & 1) S->T->missile0H = S->T->player0H + 3;
        if((S->memory[RESMP1] >> 1) & 1) S->T->missile1H = S->T->player1H + 3;


        S->hmove=false;
    }
    if(S->hmclr){
        S->memory[HMP0] = 0;
        S->memory[HMP1] = 0;
        S->memory[HMM0] = 0;
        S->memory[HMM1] = 0;
        S->memory[HMBL] = 0;

        S->hmclr=false;
    }
    // if(S->cxclr){
    //     S->memory[CXM0P]  = 0;
    //     S->memory[CXM1P]  = 0;
    //     S->memory[CXP0FB] = 0;
    //     S->memory[CXP1FB] = 0;
    //     S->memory[CXM0FB] = 0;
    //     S->memory[CXM1FB] = 0;
    //     S->memory[CXBLPF] = 0;
    //     S->memory[CXPPMM] = 0;
    //     S->cxclr=false;
    // }

    if(T->updatePlayerBit0){
        updatePlayerLine(S, 0);
        T->updatePlayerBit0=false;
    }
    if(T->updatePlayerBit1){
        updatePlayerLine(S, 1);
        T->updatePlayerBit1=false;
    }
}

//Add Memory to Accumulator with Carry
void ADC(CPUState* S){
    uint16_t result = (uint16_t)S->A + (uint16_t)*(S->target) + (uint16_t)S->C;
    
    if(S->D){
        // BCD correction
        if((result & 0x0F) > 9) result += 0x06;  // fix low nibble
        if((result & 0xF0) > 0x90) result += 0x60; // fix high nibble
    }
    
    uint8_t oldA = S->A;
    S->A = (uint8_t)result;
    S->C = (result > 0xFF) ? 1 : 0;
    S->V = ((oldA ^ S->A) & (*(S->target) ^ S->A) & 0x80) ? 1 : 0;
    S->Z = (S->A == 0) ? 1 : 0;
    S->N = (S->A >> 7) & 1;
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
    if(S->target != &S->A) S->cycleDif += 2;

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
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
    }

}

//Branch on Carry set
void BCS(CPUState* S){
    //extract the argument

    if(S->C == 1){
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
    }
}

//Branch on result zero
void BEQ(CPUState* S){
    if(S->Z == 1){
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
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
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
    }
}

//branch on result not zero
void BNE(CPUState* S){
    if(S->Z == 0){
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
    }
}

//branch on result plus
void BPL(CPUState* S){
    if(S->N == 0){
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
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
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
    }
}

//Branch on Overflow Set
void BVS(CPUState* S){
    if(S->V == 1){
        //branch taken
        int8_t offset = *(S->target);
        uint16_t prevPC = S->PC;   // PC is already at PC+2 here
        S->PC += offset;
        S->cycleDif = ((prevPC >> 8) == (S->PC >> 8)) ? 3 : 4;
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

//Shift One Bit Right (Memory or Accumulator)F
void LSR(CPUState* S){
    if(S->target != &S->A) S->cycleDif += 2;

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
    if(S->target != &S->A) S->cycleDif += 2;

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
    if(S->target != &S->A) S->cycleDif += 2;

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
    uint8_t borrow = 1 - S->C;
    uint16_t result = (uint16_t)S->A - (uint16_t)*(S->target) - borrow;
    
    if(S->D){
        if((result & 0x0F) > 9) result -= 0x06;
        if((result & 0xF0) > 0x90) result -= 0x60;
    }
    
    uint8_t oldA = S->A;
    S->A = (uint8_t)result;
    S->C = (result <= 0xFF) ? 1 : 0;
    S->V = ((oldA ^ *(S->target)) & (oldA ^ S->A) & 0x80) ? 1 : 0;
    S->Z = (S->A == 0) ? 1 : 0;
    S->N = (S->A >> 7) & 1;
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
