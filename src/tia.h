#pragma once
#include "cpu.h"
#include "addressDef.h"
struct CPUState;

// typedef void (*FunctionPtr)(struct TIAState* S);

typedef struct TIAState{
    int hpos;
    int vpos;
    uint32_t frame[WINDOWHEIGHT * WINDOWWIDTH];
    bool wasInVsync;
    // uint8_t frameBuffer[192][160][3];

    uint8_t swchb;
    
    uint8_t player1H;
    uint8_t player1Bits[160];
    uint8_t player0H;
    uint8_t player0Bits[160];

    uint8_t missile1H;
    uint8_t missile0H;

    uint8_t ballH;

    bool updatePlayerBit0;
    bool updatePlayerBit1;

} TIAState;

bool playFieldBit(CPUState* S);
bool moveBeam(CPUState* S);

bool playerBit(CPUState* S, uint8_t playernum);
void updatePlayerLine(CPUState* S, uint8_t playernum);
void stampPlayer(uint8_t* arr, uint8_t graphic, int position, uint8_t stretch);
uint8_t reflect_8(uint8_t b);

void drawPixel(CPUState* S);
// void updateFrameBuffer(TIAState* T);
