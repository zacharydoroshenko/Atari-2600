#include <stdio.h>
#include <cstdint>

int main(void){

    // keep first 13 bits (% 0x2000)
    // if A12 is 0
    //     if A7 is 0
    //         //TIA
    //         if read
    //             keep first 4 bits
    //         else //write
    //             keep first 6 bits
    //     else //A7 is 1
    //         //RIOT
    //         if A9 is 0
    //             //RAM
    //             keep first 8 bits
    //         else  //A9 is 1
    //             //xxx0 xx1x 1xxx xxxx
    //             if A2 is 0
    //                 //IO
    //                 0000 0010 1000 00??
    //             else
    //                 //xxx0 xx1x 1xxx x1xx
    //                 if read
    //                     if A0 is 0
    //                         //read timer
    //                         0000 0010 1000 ?100
    //                     else
    //                         //read timer interrupt flag
    //                         0000 0010 1000 0101
    //                 else //write
    //                     if A4 is 0
    //                         //write timer interrupt flag
    //                         0000 0010 1000 01??
    //                     else //A4 is 1
    //                         //write timer
    //                         0000 0010 1001 ?1??

    
    uint16_t s = 0xFF45 % 0x100;

    printf("0x%x\n", s);
}