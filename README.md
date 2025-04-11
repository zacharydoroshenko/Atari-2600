# Atari-2600
tasks:
cpu:
    6 determine if a cartridge is 2k and make 11th bit not matter in that case
    3 igure out how to implement memory locations that are different when reading and writing (must edit memory mirroring function)

tia:
    4 start tia programming

OpenGL:
    7 start frame buffer work

main:
    5 include input support
    1 create a way to print instructions being executed
    2 create a way to print machine code as readable assembly





Memory Mirroring

Cartridge
    4k:     xxx1 ???? ???? ????
    2k:     xxx1 x??? ???? ????

TIA
    write:  xxx0 xxxx 0x?? ????
    read:   xxx0 xxxx 0xxx ????

RIOT
    RAM:    xxx0 xx0x 1??? ????
    IO:     xxx0 xx1x 1xxx x0??
    timer
     write: xxx0 xx1x 1xx1 ?1??
     read:  xxx0 xx1x 1xxx ?1x0
    timer interrupt flag/ pa7 timer interrupt flag
     read:  xxx0 xx1x 1xxx x1x1
     write: xxx0 xx1x 1xx0 x1??

    