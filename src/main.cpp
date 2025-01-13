#include "cpu.h"
#include <fstream>
typedef void (*FunctionPtr)(CPUState* S);

int main(){
    //initialize instruction matrix
    FunctionPtr InstMatrix[2][2] = {
        {nullptr, NOP},
        {SEC, SED}
    };

    //initialize CPUState
    CPUState* state = (CPUState*) calloc(sizeof(CPUState), 1);
    state->P = 0b00100000;
    state->PC = 0x1000;

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
    while(i < 1){
        
        InstMatrix[0][1];
        printf("%d\n", state->cycleDif);
        i++;
    }



    


}