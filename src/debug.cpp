#include "debug.h"



void printCode(CPUState* S){
    uint16_t oldPC = S->PC;
    S->PC = 0x1000;
    uint16_t end = 0x1800;
    if(S->fourKilo){
        end=0x2000;
    }
    bool bytesTime = false;
    
    while(S->PC < end){
        if(bytesTime){
            printf("0x%X: $%X\n", S->PC, S->memory[S->PC]);
            S->PC++;
            continue;
        }
        uint8_t oppcode = S->memory[S->PC]; 

        //Print
        printf("0x%X: %s", S->PC, InstrStr[oppcode >> 4][oppcode & 0b1111]);
        //Update PC
        uint8_t aMode = addressingMode[oppcode >> 4][oppcode & 0b1111];
        if(aMode == IMP){
            S->PC++;
        } else if(aMode == AA){
            S->PC++;
        } else if(aMode == REL){
            S->PC += 2;
        }  else if(aMode == IMM){
            S->PC += 2;
        } else if(aMode == IND){
            S->PC += 3;
        } else if(aMode == INDY){
            S->PC += 2;
        } else if(aMode == INDX){
            S->PC += 2;
        } else if(aMode == ZPG){
            S->PC += 2;
        } else if(aMode == ZPGX){
            S->PC += 2;
        } else if(aMode == ZPGY){
            S->PC += 2;
        } else if(aMode == ABS){
            S->PC += 3;
        } else if(aMode == ABSX){
            S->PC += 3;
        } else if(aMode == ABSY){
            S->PC += 3;
        } else {
            printf("$%X // BYTETIME START", S->memory[S->PC]);
            // exit(1);
            bytesTime = true;
            S->PC++;
        }
        printf("\n");
    }
    S->PC = oldPC;
}