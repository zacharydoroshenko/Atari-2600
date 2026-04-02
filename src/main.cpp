#include "cpu.h"
#include "debug.h"
#include <fstream>
typedef void (*FunctionPtr)(CPUState* S);

int main(){
    

    //Open ROM 
    ifstream file("Combat.A26", ios::binary);

    if(!file){
        cerr << "Failed to open the file!" << std::endl;
        return 1;
    }
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg); // Move back to start



    //initialize CPUState
    CPUState* S = (CPUState*) calloc(sizeof(CPUState), 1);
    initialize(S, size);

    //import ROM data into memory
    int memPos = 0x1000;
    char byte;
    while(file.read(&byte, 1)){
        S->memory[memPos] = (uint8_t) byte;
        memPos++;
    }

    file.close();

    //initialize frame buffer

    // printCode(S);


    printf("START\n");
    //run until vsync is 1
    while(((S->memory[0x00] & 2) == 0)){
        uint8_t oppcode = S->memory[S->PC]; 
        printf("0x%X: %s\n", S->PC, InstrStr[oppcode >> 4][oppcode & 0b1111]);
        Run(S);
    }



}