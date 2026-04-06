#include "tia.h"
#include "addressDef.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <SDL2/SDL.h>
#include <assert.h>
#include "debug.h"
// 1. draw background
// 2. draw playfield

static const int TIAtoRGB[16][8] = {
    {0x000000, 0x1A1A1A, 0x393939, 0x5B5B5B, 0x7E7E7E, 0xA2A2A2, 0xC7C7C7, 0xEDEDED},
    {0x160301, 0x362006, 0x594211, 0x7D651E, 0xA2892B, 0xC7AE39, 0xEDD34B, 0xFDFA66},
    {0x320402, 0x561207, 0x792E10, 0x9D4F1C, 0xC27128, 0xE7943B, 0xF3B753, 0xF9E080},
    {0x400704, 0x69110A, 0x8B2312, 0xAF3E25, 0xD35D40, 0xED7E60, 0xF0A187, 0xF5C8BD},
    {0x3E060B, 0x651021, 0x891C40, 0xAC3362, 0xD05184, 0xEC71A8, 0xEF92D3, 0xF3BAF2},
    {0x280347, 0x4F0A63, 0x721687, 0x942EAB, 0xB84DD0, 0xDD6DF7, 0xEF90F0, 0xF3B8F3},
    {0x0B007D, 0x2E049B, 0x4E15C1, 0x6F32E7, 0x9252F5, 0xB876F6, 0xDF9CF8, 0xF5C2F6},
    {0x00008B, 0x0905B5, 0x2722DB, 0x4742F4, 0x6A64F5, 0x8F8AF7, 0xB6B0F8, 0xDED8FB},
    {0x00006D, 0x071BA4, 0x173BCE, 0x325DF4, 0x507FF6, 0x73A4F7, 0x98CCFA, 0xBFF4FC},
    {0x031038, 0x11306A, 0x22549D, 0x3577C2, 0x4D9BE7, 0x6ABFF8, 0x8BE6FB, 0xAAFBFD},
    {0x081E05, 0x194229, 0x2C6758, 0x3D8B7B, 0x53AE9E, 0x6ED4C3, 0x8DFBE9, 0xA4FBFD},
    {0x0A2307, 0x1D4913, 0x2F6E21, 0x43933B, 0x5BB75A, 0x78DD7B, 0x95FB9D, 0xAFFBBB},
    {0x092005, 0x1B4511, 0x2F691B, 0x498E27, 0x67B336, 0x88D84C, 0xAAFB68, 0xC9FC7E},
    {0x041503, 0x1B3509, 0x3A5815, 0x5C7D23, 0x80A230, 0xA3C73D, 0xC8ED51, 0xECFD65},
    {0x170301, 0x372006, 0x5A4211, 0x7E651E, 0xA3892B, 0xC9AE39, 0xEED34B, 0xFDFA66},
    {0x330402, 0x571207, 0x7A2E11, 0x9E4F1C, 0xC37028, 0xE8943B, 0xF3B753, 0xF9E080}
};

uint32_t RGBconvert(uint8_t col){
    int firstOne = (0xF0 & col) >> 4;
    int secondOne = (0b00001110 & col) >> 1; 
    return 0xFF000000 + TIAtoRGB[firstOne][secondOne];
}


bool moveBeam(CPUState* S) {
    static bool collision=false;
    // TimerState* Ti = S->Ti;
    TIAState* T = S->T;
    T->hpos += 1;
    bool frameFinished = false;

    // 1. Handle Horizontal Wrap and WSYNC
    if (T->hpos >= 228) {
        T->hpos = 0;
        T->vpos += 1;
        if (S->halt) {
            S->halt = false; 
            S->cycleDif = 0; 
        }
    }

    // 2. Handle Vertical Sync (Edge Detection)
    bool vsyncNow = (S->memory[VSYNC] & 0x02) != 0;

    if (vsyncNow) {
        // While VSYNC is high, we keep the beam at the top
        T->vpos = 0;
        T->wasInVsync = true; 
    } else {
        // If VSYNC just transitioned from High to Low
        if (T->wasInVsync) {
            T->vpos = 3;         // Per your requirement: vpos is 3 when VSYNC finishes
            T->wasInVsync = false;
            frameFinished = true; // Signal that the next frame context has started
        }
    }

    // 3. Draw Pixel (only in visible area)
    // hpos 68-227 (160 pixels), vpos 40-231 (192 lines)
    if (T->hpos >= 68 && T->vpos >= 40 && T->vpos < WINDOWHEIGHT + 40) {
        drawPixel(S);
        if((S->memory[VBLANK] >> 1) & 1) T->frame[(160 * (T->vpos - 40))+(T->hpos - 68)] = RGBconvert(0x88);
        
    }
    // if((S->memory[VBLANK] >> 1) & 1) drawPixel(T,S);

    // 4. CPU and Timer Updates (3 TIA cycles = 1 CPU cycle)
    if (T->hpos % 3 == 0) {
        if (!S->halt) {
            if (S->cycleDif <= 0) {
                // Run() sets cycleDif to the instruction cost
                if(S->instructions)printf("0x%X: %s, vpos: %d, pixel pos: %d, scanLine: 0x%X\n", S->PC, InstrStr[S->memory[S->PC] >> 4][S->memory[S->PC] & 0b1111], T->vpos,T->hpos - 68, S->memory[0xB4]); 
                if(S->PC == HMP0) printf("HMP0 here\n");
                // assert(S->PC != 0x1454);
                // assert(S->memory[CXM0P] == 0);
                // assert(S->memory[CXM1P] == 0);
                // assert(S->memory[CXP0FB] == 0);
                // assert(S->memory[CXP1FB] == 0);
                // assert(S->memory[CXPPMM] == 0);

                // assert(S->memory[] == 0);
                // assert(S->memory[0x9E] == 0);
                // assert(S->memory[0x93] == 0);
                // assert(S->memory[0x8D] == 0);

                //write queue handle
                uint8_t oppcode = S->writeQueueOpp;
                if(oppcode != 0x0){
                    Instr[oppcode >> 4][oppcode & 0b1111](S);
                    HandleStrobe(S);
                    S->writeQueueOpp = 0x0;
                }

                Run(S); 
                if((S->memory[CXM0P] >> 7) & 1) collision=true;
                // static uint8_t prevStirTimer = 0;
                // uint8_t cur = S->memory[0x8A];
                // if(cur != prevStirTimer){
                //     printf("StirTimer 0x%X → 0x%X at PC=0x%X, instr=%s\n",
                //         prevStirTimer, cur, S->PC,
                //         InstrStr[S->memory[S->PC-1] >> 4][S->memory[S->PC-1] & 0xF]);
                //     prevStirTimer = cur;
                // }
                // if(S->memory[SWCHBREAD] != 0xFF) exit(1);
                // printf("CXM0P 0x%X\n", S->memory[CXM0P]);
            }
            S->cycleDif--;
        }
        updateTimer(S);
    }
    // if(frameFinished && collision){printf("Collision with m1 p0"); collision=false;}

    return frameFinished;
}

bool playFieldBit(CPUState* S){
    //translate into playfield space
    TIAState* T = S->T;
    uint8_t h = (T->hpos - 68) / 4;
    //apply repeating or mirroring
    if(h > 19){
        if(S->memory[CTRLPF] & 1) {
            //mirror
            h = 39 - h;
        } else {
            //repeat
            h -= 20;
        }
    }

    if(h < 4){
        return (S->memory[PF0] >> (h+4)) & 1;
    } else if(h < 12){
        return (S->memory[PF1] >> (11-h)) & 1;
    } else {
        return (S->memory[PF2] >> (h-12)) & 1;
    }
}

bool playerBit(CPUState* S, uint8_t playernum){
    uint8_t h = S->T->hpos - 68;
    return playernum ? S->T->player1Bits[h] : S->T->player0Bits[h];

}

void updatePlayerLine(CPUState* S, uint8_t playernum){
    uint8_t* arr = playernum ? S->T->player1Bits : S->T->player0Bits;
    memset(arr, 0, 160);
    uint8_t graphic = playernum ? S->memory[GRP1] : S->memory[GRP0];

    //Hadjust
    // int8_t adjust = playernum ? S->memory[HMP1] : S->memory[HMP0];
    // adjust = (adjust >> 4);

    int position = playernum ? S->T->player1H : S->T->player0H;
    uint8_t numsize = playernum ? S->memory[NUSIZ1] : S->memory[NUSIZ0];
    numsize &= 0b00000111;

    //reflection
    uint8_t reflection = playernum ? S->memory[REFP1] : S->memory[REFP0];
    reflection = (reflection >> 3) & 1;
    if(reflection) graphic = reflect_8(graphic);

    switch (numsize){
    case 0:
        stampPlayer(arr, graphic, position, 1);
        break;
    case 1:
        stampPlayer(arr, graphic, position, 1);
        stampPlayer(arr, graphic, position+16, 1);
        break;
    case 2:
        stampPlayer(arr, graphic, position, 1);
        stampPlayer(arr, graphic, position+32, 1);
        break;
    case 3:
        stampPlayer(arr, graphic, position, 1);
        stampPlayer(arr, graphic, position+16, 1);
        stampPlayer(arr, graphic, position+32, 1);
        break;
    case 4:
        stampPlayer(arr, graphic, position, 1);
        stampPlayer(arr, graphic, position+64, 1);
        break;
    case 5:
        stampPlayer(arr, graphic, position, 2);
        break;
    case 6:
        stampPlayer(arr, graphic, position, 1);
        stampPlayer(arr, graphic, position+32, 1);
        stampPlayer(arr, graphic, position+64, 1);
        break;
    case 7:
        stampPlayer(arr, graphic, position, 4);
        break;
    default:
        printf("INCORRECT NUMSIZE\n");
        exit(1);
        break;
    }
}

void stampPlayer(uint8_t* arr, uint8_t graphic, int position, uint8_t stretch){
    for(int i=0; i < 8*stretch; i++){
        arr[((i + position) % 160 + 160) % 160] = (graphic >> (7 - i/stretch)) & 1; //bit i << stretch of graphics
    }
}

uint8_t reflect_8(uint8_t b) {
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

bool stampMissile(uint8_t h, uint8_t missileH, uint8_t size){
    uint8_t hh = (h % 160 + 160) % 160;
    return (hh >= missileH && hh < missileH + size);
}

bool missileBit(CPUState* S, uint8_t missilenum){
    if( !( (S->memory[ENAM0 + missilenum] >> 1) & 1 ) ) return false;
    if((S->memory[RESMP0 + missilenum] >> 1) & 1) return false;
    uint8_t h = S->T->hpos - 68;

    uint8_t size = 1 << ((S->memory[NUSIZ0 + missilenum] & 0b00110000) >> 4);
    uint8_t missileH = missilenum ? S->T->missile1H : S->T->missile0H;


    uint8_t numsize = missilenum ? S->memory[NUSIZ1] : S->memory[NUSIZ0];
    numsize &= 0b00000111;

    switch (numsize){
    case 0:
        return stampMissile(h, missileH, size);
    case 1:
        return stampMissile(h, missileH, size) || stampMissile(h-16, missileH, size);
    case 2:
        return stampMissile(h, missileH, size) || stampMissile(h-32, missileH, size);
    case 3:
        return stampMissile(h, missileH, size) || stampMissile(h-16, missileH, size) || stampMissile(h-32, missileH, size);
    case 4:
        return stampMissile(h, missileH, size) || stampMissile(h-64, missileH, size);
    case 5:
        return stampMissile(h, missileH, size);
    case 6:
        return stampMissile(h, missileH, size) || stampMissile(h-32, missileH, size) || stampMissile(h-64, missileH, size);
    case 7:
        return stampMissile(h, missileH, size);
    default:
        printf("INCORRECT NUMSIZE\n");
        exit(1);
        break;
    }


}

bool ballBit(CPUState* S){
    if( !( (S->memory[ENABL] >> 1) & 1 ) ) return false;
    uint8_t h = S->T->hpos - 68;

    uint8_t size = 1 << ((S->memory[CTRLPF] & 0b00110000) >> 4);
    uint8_t ballH = S->T->ballH;

    return (h >= ballH && h < ballH + size);
}

void drawPixel(CPUState* S){

    
     TIAState* T = S->T;

    //score mode for PF
    uint16_t pfaddress;
    if((S->memory[CTRLPF] >> 1) & 1){
        pfaddress = (T->hpos - 68) < 80 ? COLUP0 : COLUP1;
    } else {
        pfaddress = COLUPF;
    }

    bool P0=playerBit(S, 0);
    bool P1=playerBit(S, 1);
    bool M0=missileBit(S,0);
    bool M1=missileBit(S,1);
    bool BL=ballBit(S);
    bool PF=playFieldBit(S);

    //apply according to priority
    uint8_t color = S->memory[COLUBK];
    if(PF || BL) color = S->memory[pfaddress];
    if(P1 || M1) color = S->memory[COLUP1];
    if(P0 || M0) color = S->memory[COLUP0];
    if((PF || BL) && (S->memory[CTRLPF] >> 2) & 1) color = S->memory[pfaddress];

    //render color to frame
    T->frame[(160 * (T->vpos - 40))+(T->hpos - 68)] = RGBconvert(color);

    //handle collision 
    if(M0 && P1){ S->memory[CXM0P] |= 0x80;}  //printf("M0P1 collision at hpos:%d vpos:%d — writing to addr 0x%X\n", S->T->hpos, S->T->vpos, (int)(S->memory + CXM0P - S->memory));}
    if(M0 && P0) S->memory[CXM0P] |= 0x40;
    if(M1 && P0) S->memory[CXM1P] |= 0x80;
    if(M1 && P1) S->memory[CXM1P] |= 0x40;

    if(P0 && PF) S->memory[CXP0FB] |= 0x80;
    if(P0 && BL) S->memory[CXP0FB] |= 0x40;
    if(P1 && PF) S->memory[CXP1FB] |= 0x80;
    if(P1 && BL) S->memory[CXP1FB] |= 0x40;

    if(M0 && PF) S->memory[CXM0FB] |= 0x80;
    if(M0 && BL) S->memory[CXM0FB] |= 0x40;
    if(M1 && PF) S->memory[CXM1FB] |= 0x80;
    if(M1 && BL) S->memory[CXM1FB] |= 0x40;

    if(BL && PF) S->memory[CXBLPF] |= 0x80;

    if(P0 && P1){ S->memory[CXPPMM] |= 0x80;} //printf("COLLISION WITH PLAYER AND PLAYER\n");}
    if(M0 && M1) S->memory[CXPPMM] |= 0x40;

}

