#include "cpu.h"
#include <fstream>
typedef void (*FunctionPtr)(CPUState* S);

int main(){
    //initialize CPUState
    CPUState* state = (CPUState*) calloc(sizeof(CPUState), 1);
    initialize(state);

    //Open ROM and put into memory
    ifstream file("Combat.A26", ios::binary);

    if(!file){
        cerr << "Failed to open the file!" << std::endl;
        return 1;
    }

    int memPos = 0x1000;
    char byte;
    while(file.read(&byte, 1)){
        state->memory[memPos] = (uint8_t) byte;
        memPos++;
    }

    file.close();

    //initialize frame buffer

    //TODO make while loop good
    //While loop to create frame buffer
    int i = 0;
    while(i < 550){
        printf("%X, 0x%X\n", state->PC, state->memory[state->PC]);
        Run(state);
        // printf("%d\n", state->cycleDif);
        

        
        i++;
    }
    //0x78 SEI
    //0xD8 CLD
    //0xA2 LDX
    //0x9A TXS
    //0xA2 LDX
    //0x20 JSR
    

    //ClearMem
    //0xA9 LDA
    //0xE8 INX
    //0x95 STA
    //0xD0 BNE

    //0xE8 INX
    //0x95 STA
    //0xD0 BNE

    //0xE8 INX
    //0x95 STA
    //0xD0 BNE

    //0x60 RTS

    //0xA9 LDA
    //0x8D STA 
    //0x85 STA
    //0x20 JSR

    //ClrGam
    //
    



    


}