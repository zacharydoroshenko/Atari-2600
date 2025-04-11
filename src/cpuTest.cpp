#include "cpu.h"
#include <fstream>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
// printf("0x%x\n", S->PC);
//RTI RTS BRK

//WR instructions: ASL DEC INC LSR

// AA 1 
// REL 2 
// IMM 3
// IND 4 
// INDY 5 
// INDX 6 
// ZPG 7
// ZPGX 8 
// ZPGY 9 
// ABS 10 
// ABSX 11 
// ABSY 12 
// IMP 13 

void setVal(string set, string val, uint8_t mode, CPUState *S){
    //convert val to a hex
    int value = stoi(val, nullptr, 16);
    uint16_t val_hex = static_cast<uint8_t>(value);


    //set the value in the appropriate place
    if(set == "A"){
        S->A = val_hex;
    } else if(set == "X"){
        S->X = val_hex;
    } else if(set == "Y"){
        S->Y = val_hex;
    } else if(set == "M"){
        if(mode == AA){
            //val_hex will be the value of accumulator
            S->A = val_hex;
        } else if(mode == REL){
            //val_hex will be the relative offset
            S->memory[0x1001] = val_hex;
        } else if(mode == IMM){
            //val_hex will be the immediate value
            S->memory[0x1001] = val_hex;
            // printf("IMM: 0x%X\n", S->memory[0x1001]);
        } else if(mode == IND){
            //only for jump
            //val_hex will be the address to jump to
            S->memory[0x1001] = 0x20;
            S->memory[0x1002] = 0x10;
            S->memory[0x1020] = 0x42;
            S->memory[0x1021] = 0x10;
            S->memory[0x1042] = val_hex;
        } else if(mode == INDY){
            //val_hex will be the value at the end of indirect Y (values to reach it are defined automatically)
            
            S->memory[0x1001] = 0x80;

            S->memory[0x80] = 0x40;
            S->memory[0x81] = 0x10;
            S->Y = 0x3;
            S->memory[0x1043] = val_hex; 
        } else if(mode == INDX){
            //val_hex will be the value at the end of indirect X (values to reach it are defined automatically)
            S->X = 0x81;
            S->memory[0x1001] = 0xFF;
            uint8_t temp = S->X + 0xFF; //0x80
            S->memory[temp] = 0x40;
            S->memory[temp + 1] = 0x10;
            S->memory[0x1040] = val_hex;
            // printf("INDX: 0x%X\n", S->memory[0x1040]);
        } else if(mode == ZPG){
            //val_hex will be the value at the automatically assigned zero page address
            S->memory[0x1001] = 0x80;
            S->memory[0x80] = val_hex;
            // printf("ZPG: 0x%X\n", S->memory[0x20]);
        } else if(mode == ZPGX){
            //val_hex will be the value at the automatically assigned zero page address (+X also assigned)
            S->memory[0x1001] = 0xFF;
            S->X = 0x83;
            S->memory[0x82] = val_hex;
            // printf("ZPGX: 0x%X\n", S->memory[0x2]);
        } else if(mode == ZPGY){
            //val_hex will be the value at the automatically assigned zero page address (+Y also assigned)
            S->memory[0x1001] = 0xFF;
            S->Y = 0x81;
            S->memory[0x80] = val_hex;
        } else if(mode == ABS){
            //val_hex will be the value at the automatically assigned address
            S->memory[0x1001] = 0x42;
            S->memory[0x1002] = 0x10;
            S->memory[0x1042] = val_hex;
            // printf("ABS: 0x%X\n", S->memory[0x1042]);
        } else if(mode == ABSX){
            //val_hex will be at automatically assigned address + automatically assigned X
            S->memory[0x1001] = 0x02;
            S->memory[0x1002] = 0x10;
            S->X = 0x40;
            S->memory[0x1042] = val_hex;
            // printf("ABSX: 0x%X\n", S->memory[0x1042]);
        } else if(mode == ABSY){
            //val_hex will be at automatically assigned address + automatically assigned Y
            S->memory[0x1001] = 0x02;
            S->memory[0x1002] = 0x10;
            S->Y = 0x40;
            S->memory[0x1042] = val_hex;
            // printf("ABSY: 0x%X\n", S->memory[0x1042]);
        } else if(mode == IMP){
            cout << "trying to set M with IMP mode\n";
            exit(1);
        } else {
            cout << "invalid addressing mode\n";
            exit(1);
        }
    } else if(set == "PC"){
        S->PC = val_hex;
    } else if(set == "C"){
        S->C = val_hex;
    } else if(set == "Z"){
        S->Z = val_hex;
    } else if(set == "I"){
        S->I = val_hex;
    } else if(set == "D"){
        S->D = val_hex;
    } else if(set == "B"){
        S->B = val_hex;
    } else if(set == "V"){
        S->V = val_hex;
    } else if(set == "N"){
        S->N = val_hex;
    } else if(set == "cycleDif"){
        S->cycleDif = val_hex;
    } else if(set == "SP"){
        S->SP = val_hex;
    } else {
        cout << "unrecognized check for setVal\n";
        exit(1);
    }
}

bool checkVal(string check, string val, uint8_t mode, CPUState *S){

    //convert check string to uint8_t hex from inside the cpustate
    uint16_t check_hex;
    if(check == "A"){
        check_hex = S->A;
        // printf("A: 0x%X\n", S->A);
    } else if(check == "X"){
        check_hex = S->X;
    } else if(check == "Y"){
        check_hex = S->Y;
    } else if(check == "M"){
        //DEC, INC, STA, STX, STY, 
        //ZPG, ZPGX, ZPGY, ABS, ABSX, ABSY, INDX, INDY
        //same memory location as where it was written (for STA STX STY it was never written)
        if(mode == AA){
            // cout << "doesnt make sense to check memory of AA";
            // exit(1);
            check_hex = S->A;
        } else if(mode == REL){
            cout << "doesnt make sense to check memory of REL";
            exit(1);
        } else if(mode == IMM){
            cout << "doesnt make sense to check memory of IMM";
            exit(1);
        } else if(mode == IND){
            cout << "doesnt make sense to check memory of IND";
            exit(1);
        } else if(mode == INDY){
            //val_hex will be the value at the end of indirect Y (values to reach it are defined automatically)
            // S->memory[0x1001] = 0x40;
            // S->memory[0x40] = 0x40;
            // S->memory[0x41] = 0x10;
            // S->Y = 0x3;
            // S->memory[0x1040 + S->Y] = val_hex; 
            uint8_t locationOf16 = S->memory[0x1001];
            uint8_t lower = S->memory[locationOf16];
            uint16_t upper = S->memory[locationOf16 + 1];
            check_hex = S->memory[(upper << 8)+(lower)+S->Y];
        } else if(mode == INDX){
            //val_hex will be the value at the end of indirect X (values to reach it are defined automatically)
            // S->X = 0x9;
            // S->memory[0x1001] = 0xFF;
            // uint8_t temp = S->X + 0xFF; //0x8
            // S->memory[temp] = 0x40;
            // S->memory[temp + 1] = 0x10;
            // S->memory[0x1040] = val_hex;
            uint8_t temp = S->X + S->memory[0x1001];
            uint8_t lower = S->memory[temp];
            uint16_t upper = S->memory[temp+1];
            check_hex = S->memory[(upper << 8)+(lower)];

        } else if(mode == ZPG){
            //val_hex will be the value at the automatically assigned zero page address
            // S->memory[0x1001] = 0x20;
            // S->memory[0x20] = val_hex;
            uint8_t temp = S->memory[0x1001];
            check_hex = S->memory[temp];
        } else if(mode == ZPGX){
            //val_hex will be the value at the automatically assigned zero page address (+X also assigned)
            // S->memory[0x1001] = 0xFF;
            // S->X = 0x3;
            // S->memory[0x2] = val_hex;
            uint8_t temp = S->memory[0x1001];
            uint8_t actual = temp + S->X;
            check_hex = S->memory[actual];
        } else if(mode == ZPGY){
            //val_hex will be the value at the automatically assigned zero page address (+Y also assigned)
            // S->memory[0x1001] = 0xFF;
            // S->Y = 0x1;
            // S->memory[0x100] = val_hex;
            uint8_t temp = S->memory[0x1001];
            uint8_t actual = temp + S->Y;
            check_hex = S->memory[actual];
        } else if(mode == ABS){
            //val_hex will be the value at the automatically assigned address
            // S->memory[0x1001] = 0x42;
            // S->memory[0x1002] = 0x10;
            // S->memory[0x1042] = val_hex;
            uint8_t lower = S->memory[0x1001];
            uint16_t upper = S->memory[0x1002];
            check_hex = S->memory[(upper << 8)+(lower)];
        } else if(mode == ABSX){
            //val_hex will be at automatically assigned address + automatically assigned X
            // S->memory[0x1001] = 0x02;
            // S->memory[0x1002] = 0x10;
            // S->X = 0x40;
            // S->memory[0x1042] = val_hex;
            uint8_t lower = S->memory[0x1001];
            uint16_t upper = S->memory[0x1002];
            check_hex = S->memory[(upper << 8)+(lower)+S->X];
        } else if(mode == ABSY){
            //val_hex will be at automatically assigned address + automatically assigned Y
            // S->memory[0x1001] = 0x02;
            // S->memory[0x1002] = 0x10;
            // S->Y = 0x40;
            // S->memory[0x1042] = val_hex;
            uint8_t lower = S->memory[0x1001];
            uint16_t upper = S->memory[0x1002];
            check_hex = S->memory[(upper << 8)+(lower)+S->Y];
        } else if(mode == IMP){
            cout << "trying to check M with IMP mode\n";
            exit(1);
        } else {
            cout << "invalid addressing mode\n";
            exit(1);
        }
    } else if(check == "PC"){
        check_hex = S->PC;
    } else if(check == "C"){
        check_hex = S->C;
    } else if(check == "Z"){
        check_hex = S->Z;
    } else if(check == "I"){
        check_hex = S->I;
    } else if(check == "D"){
        check_hex = S->D;
    } else if(check == "B"){
        check_hex = S->B;
    } else if(check == "V"){
        check_hex = S->V;
    } else if(check == "N"){
        check_hex = S->N;
    } else if(check == "cycleDif"){
        check_hex = S->cycleDif;
    } else if(check == "SP"){
        check_hex = S->SP;
    } else {
        cout << "unrecognized check for checkVal\n";
        exit(1);
    }


    //convert val string to uint15_t hex
    int value = stoi(val, nullptr, 16);
    uint16_t val_hex = static_cast<uint16_t>(value);
    (void) val_hex;

    //compare
    
    if(val_hex == check_hex){
        return true;
    } else {
        printf("MISTMATCH with 0x%x\n", S->memory[0x1000]);
        cout << "Expected: " << check << " = " << val << "\n";
        cout << "     Result: " << check << " = ";
        printf("0x%X\n", check_hex);
        // printf("%X\n", check_hex);
        return false;
    }
}

bool Tester(CPUState *S, string set_input, string check_input, string type_input){
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
    // for each mode
    bool result = true;
    std::istringstream iss_type(type_input);
    std::string token;

    while (iss_type >> token) {
        uint8_t type;
        int temp;
        std::stringstream ss;
        ss << std::hex << token;
        ss >> temp;
        type = static_cast<uint8_t>(temp);
        // printf("type: 0x%X\n", type);

        // for each mode

        // set standard values (ie PC)
        S->PC = 0x1000;
        S->memory[0x1000] = type;
        uint8_t mode = addressingMode[type >> 4][type & 0b1111];
        
        

        // for each setvalue
        std::istringstream iss_set(set_input);
        std::string val;
        std::string set;
        while(iss_set >> val){
            iss_set >> set;

            //set value
            setVal(val, set, mode, S);
        }
        // run instruction
        Run(S);
        // for each checkval
        std::istringstream iss_check(check_input);
        std::string check;
        while(iss_check >> val){
            iss_check >> check;
        //     if checkval != expected value return false
            // cout << check << "\n";
            // cout << val << "\n";
            if(!checkVal(val, check, mode, S)){
                result = false;
            }
        }


    }
    return result;
}

int main(){


    //initialize CPUState
    CPUState* S = (CPUState*) calloc(sizeof(CPUState), 1);
    initialize(S);

    //ADC --------------------------------------------------------------------------------------------------------------------------------------
    assert(Tester(S, "A 0x01 M 0x09 C 0", "A 0x0A Z 0 C 0 N 0 V 0", "0x69 0x65 0x75 0x6D 0x7D 0x79 0x61 0x71"));
    assert(Tester(S, "A 0xFF M 0x81 C 0", "A 0x80 Z 0 C 1 N 1 V 0", "0x69 0x65 0x75 0x6D 0x7D 0x79 0x61 0x71"));
    assert(Tester(S, "A 0xFE M 0x01 C 1", "A 0x00 Z 1 C 1 N 0 V 0", "0x69 0x65 0x75 0x6D 0x7D 0x79 0x61 0x71"));
    assert(Tester(S, "A 0x70 M 0x70 C 1", "A 0xE1 Z 0 C 0 N 1 V 1", "0x69 0x65 0x75 0x6D 0x7D 0x79 0x61 0x71"));
    assert(Tester(S, "A 0x80 M 0x80 C 0", "A 0x00 Z 1 C 1 N 0 V 1", "0x69 0x65 0x75 0x6D 0x7D 0x79 0x61 0x71"));
    
    //AND --------------------------------------------------------------------------------------------------------------------------------------
    assert(Tester(S, "A 0xAA M 0x55", "A 0x00 Z 1 N 0", "0x29 0x25 0x35 0x2D 0x3D 0x39 0x21 0x31"));
    assert(Tester(S, "A 0x80 M 0x8F", "A 0x80 Z 0 N 1", "0x29 0x25 0x35 0x2D 0x3D 0x39 0x21 0x31"));
    assert(Tester(S, "A 0xFA M 0x8F", "A 0x8A Z 0 N 1", "0x29 0x25 0x35 0x2D 0x3D 0x39 0x21 0x31"));

    //ASL --------------------------------------------------------------------------------------------------------------------------------------
    assert(Tester(S, "M 0x80","A 0x00 Z 1 C 1 N 0","0x0A 0x06 0x16 0x0E 0x1E"));
    assert(Tester(S, "M 0x47","A 0x8E Z 0 C 0 N 1","0x0A 0x06 0x16 0x0E 0x1E"));


    // BCC --------------------------------------------------------------------------------------------------------------------------------------
    // branch on carry clear

    // test branch
    S->C = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    
    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->C = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x90;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BCS --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->C = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->C = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xB0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);



    //BEQ --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->Z = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->Z = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xF0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BIT --------------------------------------------------------------------------------------------------------------------------------------
    assert(Tester(S, "A 0xAA M 0x55","Z 1 V 1 N 0","0x24 0x2C"));
    assert(Tester(S, "A 0x80 M 0x80","Z 0 V 0 N 1","0x24 0x2C"));

    //BMI --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->N = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->N = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x30;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BNE --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->Z = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->Z = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0xD0;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BPL --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->N = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->N = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x10;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BRK --------------------------------------------------------------------------------------------------------------------------------------

    // S->PC = 0x1000;
    // S->memory[0x1000] = 0x00;
    // S->B = 0;




    // Run(S);
    // assert(S->cycleDif = 7);
    // assert(S->B = 1);
    // assert(S->PC = 0xFFFE);


    // S->PC = 0x1000;
    // S->memory[0x1000] = 0x1;

    //BVC --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->V = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->V = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x50;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //BVS --------------------------------------------------------------------------------------------------------------------------------------

    //test branch
    S->V = 1;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1000 + 4 + 2);
    assert(S->cycleDif == 3);

    S->PC = 0x1FFD;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);

    assert(S->PC == 0x1FFD + 4 + 2);
    assert(S->cycleDif == 4);

    //test negative travel
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0xFC; 
    Run(S);
    assert(S->PC == 0x1000 - 4 + 2 );

    //test branch not taken
    S->V = 0;
    S->PC = 0x1000;
    S->memory[S->PC] = 0x70;
    S->memory[S->PC + 1] = 0x04; 
    Run(S);
    assert(S->PC == 0x1002);
    assert(S->cycleDif == 2);

    //CLC --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->C = 1;
    S->memory[S->PC] = 0x18;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->C == 0);

    //CLD --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->D = 1;
    S->memory[S->PC] = 0xD8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->D == 0);

    //CLI --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->I = 1;
    S->memory[S->PC] = 0x58;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->I == 0);

    //CLV --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->V = 1;
    S->memory[S->PC] = 0xB8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->V == 0);

    //CMP --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "A 0xA9 M 0xA9","Z 1 C 1 N 0","0xC9 0xC5 0xD5 0xCD 0xDD 0xD9 0xC1 0xD1"));
    assert(Tester(S, "A 0xFF M 0x00","Z 0 C 1 N 1","0xC9 0xC5 0xD5 0xCD 0xDD 0xD9 0xC1 0xD1"));
    assert(Tester(S, "A 0x00 M 0xFF","Z 0 C 0 N 0","0xC9 0xC5 0xD5 0xCD 0xDD 0xD9 0xC1 0xD1"));

    //CPX --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "X 0xA9 M 0xA9","Z 1 C 1 N 0","0xE0 0xE4 0xEC"));
    assert(Tester(S, "X 0xFF M 0x00","Z 0 C 1 N 1","0xE0 0xE4 0xEC"));
    assert(Tester(S, "X 0x00 M 0xFF","Z 0 C 0 N 0","0xE0 0xE4 0xEC"));

    //CPY --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "Y 0xA9 M 0xA9","Z 1 C 1 N 0","0xC0 0xC4 0xCC"));
    assert(Tester(S, "Y 0xFF M 0x00","Z 0 C 1 N 1","0xC0 0xC4 0xCC"));
    assert(Tester(S, "Y 0x00 M 0xFF","Z 0 C 0 N 0","0xC0 0xC4 0xCC"));

    //DEC --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x1","M 0x0 Z 1 N 0","0xC6 0xD6 0xCE 0xDE"));
    assert(Tester(S, "M 0x0","M 0xFF Z 0 N 1","0xC6 0xD6 0xCE 0xDE"));

    //DEX --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xCA;
    S->X = 0x01;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xCA;
    S->X = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFF);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //DEY --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0x88;
    S->Y = 0x01;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->Y == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x88;
    S->Y = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xFF);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //EOR --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "A 0xAA M 0x55", "A 0xFF Z 0 N 1", "0x49 0x45 0x55 0x4D 0x5D 0x59 0x41 0x51"));
    assert(Tester(S, "A 0xA7 M 0xA7", "A 0x00 Z 1 N 0", "0x49 0x45 0x55 0x4D 0x5D 0x59 0x41 0x51"));

    //INC --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0xFF", "M 0x00 Z 1 N 0", "0xE6 0xF6 0xEE 0xFE"));
    assert(Tester(S, "M 0x7F", "M 0x80 Z 0 N 1", "0xE6 0xF6 0xEE 0xFE"));

    //INX --------------------------------------------------------------------------------------------------------------------------------------
    
    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xE8;
    S->X = 0xFF;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xE8;
    S->X = 0xEF;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xF0);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //INY --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag
    S->PC = 0x1000;
    S->memory[S->PC] = 0xC8;
    S->Y = 0xFF;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->Y == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xC8;
    S->Y = 0xEF;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xF0);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //JMP --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x20", "PC 0x1042", "0x4C 0x6C"));

    //JSR --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x20;
    S->memory[0x1001] = 0x80;
    S->memory[0x1002] = 0x10;

    S->SP = 0xFF;

    Run(S);
    //will save 0x1002 in stack

    assert(S->PC == 0x1080);
    // printf("FF: 0x%X FE: 0x%X FD: 0x%X FC: 0x%X\n", S->memory[0xFF], S->memory[0xFE], S->memory[0xFD], S->memory[0xFC]);
    assert(S->memory[0xFE] == 0x02);
    assert(S->memory[0xFF] == 0x10);

    //LDA --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x00", "A 0x00 Z 1 N 0", "0xA9 0xA5 0xB5 0xAD 0xBD 0xB9 0xA1 0xB1"));
    assert(Tester(S, "M 0x80", "A 0x80 Z 0 N 1", "0xA9 0xA5 0xB5 0xAD 0xBD 0xB9 0xA1 0xB1"));
    assert(Tester(S, "M 0x56", "A 0x56 Z 0 N 0", "0xA9 0xA5 0xB5 0xAD 0xBD 0xB9 0xA1 0xB1"));

    //LDX --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x00", "X 0x00 Z 1 N 0", "0xA2 0xA6 0xB6 0xAE 0xBE"));
    assert(Tester(S, "M 0x80", "X 0x80 Z 0 N 1", "0xA2 0xA6 0xB6 0xAE 0xBE"));
    assert(Tester(S, "M 0x56", "X 0x56 Z 0 N 0", "0xA2 0xA6 0xB6 0xAE 0xBE"));

    //LDY --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x00", "Y 0x00 Z 1 N 0", "0xA0 0xA4 0xB4 0xAC 0xBC"));
    assert(Tester(S, "M 0x80", "Y 0x80 Z 0 N 1", "0xA0 0xA4 0xB4 0xAC 0xBC"));
    assert(Tester(S, "M 0x56", "Y 0x56 Z 0 N 0", "0xA0 0xA4 0xB4 0xAC 0xBC"));

    //LSR --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0x01", "M 0x00 C 1 Z 1 N 0", "0x4A 0x46 0x56 0x4E 0x5E"));
    assert(Tester(S, "M 0x80", "M 0x40 C 0 Z 0 N 0", "0x4A 0x46 0x56 0x4E 0x5E"));


    //NOP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->memory[S->PC] = 0xEA;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);

    //ORA --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "M 0xAA A 0x55", "A 0xFF Z 0 N 1", "0x09 0x05 0x15 0x0D 0x1D 0x19 0x01 0x11"));
    assert(Tester(S, "M 0x80 A 0x00", "A 0x80 Z 0 N 1", "0x09 0x05 0x15 0x0D 0x1D 0x19 0x01 0x11"));
    assert(Tester(S, "M 0x00 A 0x00", "A 0x00 Z 1 N 0", "0x09 0x05 0x15 0x0D 0x1D 0x19 0x01 0x11"));

    //PHA --------------------------------------------------------------------------------------------------------------------------------------
    
    // 03 07 05 08

    
    S->PC = 0x1000;
    S->memory[0x1000] = 0x48; // push 3
    S->memory[0x1001] = 0x48; // push 7
    S->memory[0x1002] = 0x48; // push 5
    S->memory[0x1003] = 0x68; // pull 5
    S->memory[0x1004] = 0x48; // push 8
    S->memory[0x1005] = 0x68; // pull 8
    S->memory[0x1006] = 0x68; // pull 7
    S->memory[0x1007] = 0x68; // pull 3

    S->A = 3;
    Run(S);
    S->A = 7;
    Run(S);
    S->A = 5;
    Run(S);
    Run(S);
    assert(S->A == 05);
    S->A = 8;
    Run(S);
    Run(S);
    assert(S->A == 8);
    Run(S);
    assert(S->A == 7);
    Run(S);
    assert(S->A == 3);

    

    //PHP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x08;
    S->memory[0x1001] = 0x28;
    S->N = 1;
    S->V = 0;
    //1
    S->B = 0;
    S->D = 1;
    S->I = 0;
    S->Z = 1;
    S->C = 0;

    Run(S);

    assert(S->memory[S->SP + 1] == 0b10101010);
    assert(S->cycleDif == 3);

    //pull
    S->N = 0;
    S->V = 1;
    //1
    S->B = 1;
    S->D = 0;
    S->I = 1;
    S->Z = 0;
    S->C = 1;
    Run(S);
    assert(S->N == 1);
    assert(S->V == 0);
    assert(S->B == 0);
    assert(S->D == 1);
    assert(S->I == 0);
    assert(S->Z == 1);
    assert(S->C == 0);
    assert(S->cycleDif == 4);
    

    

    //PLA --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x48; // push 3
    S->memory[0x1001] = 0x48; // push 7
    S->memory[0x1002] = 0x48; // push 5
    S->memory[0x1003] = 0x68; // pull 5
    S->memory[0x1004] = 0x48; // push 8
    S->memory[0x1005] = 0x68; // pull 8
    S->memory[0x1006] = 0x68; // pull 7
    S->memory[0x1007] = 0x68; // pull 3

    S->A = 3;
    Run(S);
    assert(S->cycleDif == 3);
    S->A = 7;
    Run(S);
    S->A = 5;
    Run(S);
    Run(S);
    assert(S->A == 05);
    S->A = 8;
    Run(S);
    Run(S);
    assert(S->A == 8);
    Run(S);
    assert(S->A == 7);
    Run(S);
    assert(S->A == 3);

    assert(S->cycleDif == 4);

    //PLP --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[0x1000] = 0x08;
    S->memory[0x1001] = 0x28;
    S->N = 1;
    S->V = 0;
    //1
    S->B = 0;
    S->D = 1;
    S->I = 0;
    S->Z = 1;
    S->C = 0;

    Run(S);
    assert(S->memory[S->SP + 1] == 0b10101010);
    assert(S->cycleDif == 3);

    //pull
    S->N = 0;
    S->V = 1;
    //1
    S->B = 1;
    S->D = 0;
    S->I = 1;
    S->Z = 0;
    S->C = 1;
    Run(S);
    assert(S->N == 1);
    assert(S->V == 0);
    assert(S->B == 0);
    assert(S->D == 1);
    assert(S->I == 0);
    assert(S->Z == 1);
    assert(S->C == 0);
    assert(S->cycleDif == 4);

    //ROL --------------------------------------------------------------------------------------------------------------------------------------
    
    assert(Tester(S, "M 0x80 C 0", "M 0x00 C 1 Z 1 N 0", "0x2A 0x26 0x36 0x2E 0x3E"));
    assert(Tester(S, "M 0x79 C 1", "M 0xF3 C 0 Z 0 N 1", "0x2A 0x26 0x36 0x2E 0x3E"));

    //ROR --------------------------------------------------------------------------------------------------------------------------------------
    
    assert(Tester(S, "M 0x01 C 0", "M 0x00 C 1 Z 1 N 0", "0x6A 0x66 0x76 0x6E 0x7E"));
    assert(Tester(S, "M 0x56 C 1", "M 0xAB C 0 Z 0 N 1", "0x6A 0x66 0x76 0x6E 0x7E"));

    //RTI --------------------------------------------------------------------------------------------------------------------------------------
    //Don't worry about it for now since interrupts are rarely used



    //RTS --------------------------------------------------------------------------------------------------------------------------------------
    S->PC = 0x1000;
    S->memory[0x1000] = 0x60;
    S->memory[0xFF] = 0x10;
    S->memory[0xFE] = 0x42;

    Run(S);
    assert(S->PC == 0x1043);


    //SBC --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "A 0x02 M 0x01 C 0", "A 0x00 C 1 Z 1 V 0 N 0", "0xE9 0xE5 0xF5 0xED 0xFD 0xF9 0xE1 0xF1"));
    assert(Tester(S, "A 0x80 M 0x01 C 1", "A 0x7F C 1 Z 0 V 1 N 0", "0xE9 0xE5 0xF5 0xED 0xFD 0xF9 0xE1 0xF1"));
    assert(Tester(S, "A 0x7F M 0xFF C 1", "A 0x80 C 0 Z 0 V 1 N 1", "0xE9 0xE5 0xF5 0xED 0xFD 0xF9 0xE1 0xF1"));

    //SEC --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->C = 0;
    S->memory[S->PC] = 0x38;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->C = 1);

    //SED --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->D = 0;
    S->memory[S->PC] = 0xF8;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->D = 1);

    //SEI --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->cycleDif = 0;
    S->I = 0;
    S->memory[S->PC] = 0x78;
    Run(S);

    assert(S->PC == 0x1001);
    assert(S->cycleDif == 2);
    assert(S->I = 1);

    //STA --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "A 0x7F M 0x2", "M 0x7F", "0x85 0x95 0x8D 0x9D 0x99 0x81 0x91"));
    assert(Tester(S, "A 0x59 M 0x2", "M 0x59", "0x85 0x95 0x8D 0x9D 0x99 0x81 0x91"));

    //STX --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "X 0x7F M 0x2", "M 0x7F", "0x86 0x96 0x8E"));
    assert(Tester(S, "X 0x59 M 0x2", "M 0x59", "0x86 0x96 0x8E"));

    //STY --------------------------------------------------------------------------------------------------------------------------------------

    assert(Tester(S, "Y 0x7F M 0x2", "M 0x7F", "0x84 0x94 0x8C"));
    assert(Tester(S, "Y 0x59 M 0x2", "M 0x59", "0x84 0x94 0x8C"));


    //TAX --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xAA;
    S->X = 0x34;
    S->A = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xAA;
    S->X = 0x35;
    S->A = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TAY --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xA8;
    S->Y = 0x34;
    S->A = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->Y == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xA8;
    S->Y = 0x35;
    S->A = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->Y == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TSX --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0xBA;
    S->X = 0x34;
    S->SP = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->X == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0xBA;
    S->X = 0x35;
    S->SP = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->X == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TXA --------------------------------------------------------------------------------------------------------------------------------------

    S->PC = 0x1000;
    S->memory[S->PC] = 0x8A;
    S->A = 0x34;
    S->X = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->A == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x8A;
    S->A = 0x35;
    S->X = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->A == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TXS --------------------------------------------------------------------------------------------------------------------------------------

    //test zero flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x9A;
    S->SP = 0x34;
    S->X = 0x00;
    S->Z = 1;
    S->N = 0;
    Run(S);

    assert(S->SP == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x9A;
    S->SP = 0x35;
    S->X = 0xFE;
    S->Z = 0;
    S->N = 1;
    Run(S);
    assert(S->SP == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    //TYA --------------------------------------------------------------------------------------------------------------------------------------
    
    //test zero flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x98;
    S->A = 0x34;
    S->Y = 0x00;
    S->Z = 0;
    S->N = 1;
    Run(S);

    assert(S->A == 0x00);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 1);
    assert(S->N == 0);

    //test negative flag

    S->PC = 0x1000;
    S->memory[S->PC] = 0x98;
    S->A = 0x35;
    S->Y = 0xFE;
    S->Z = 1;
    S->N = 0;
    Run(S);
    assert(S->A == 0xFE);
    assert(S->cycleDif == 2);
    assert(S->PC == 0x1001);
    assert(S->Z == 0);
    assert(S->N == 1);

    printf("Everything works! :)\n");
}

