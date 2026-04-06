# Atari-2600
https://problemkaputt.de/2k6specs.htm

time to go up:
mine: 3.09 actual: 5.37
time to go right:
mine: 2.68 actual: 4.27


FRAME 1: GREAT
FRAME 2: 
AE: 01, 00
AF: 01, 3c
D5: 00, bc
D6: 0E, a7
D7: 00, 61
D8: 0E, a7
D9: 00, 61
DA: 04, df
DB: 08, 09

8d: 00, f0
8e: 00, f0
98: 09, 08
AE: 01, 00
AF: 01, 3c
BE: 1C, 00
                               Actual     Mine    Diff
When going to scanline 0x21:     52        56      4
When going to scanline 0x30:     67        71      4
When going to scanline 0x50:     99        103     4
When going to scanline 0x70:     131       135     4
When going to scanline 0x80:     147       151     4
When going to scanline 0x82:     149       153     4
When going to scanline 0x83:     150    
When going to scanline 0x84:     151      
When going to scanline 0x88:     155     
When going to scanline 0x90:     163      
When going to scanline 0xE0:     243       247     4
When going to scanline 0xEC:     255       259     4


tasks:
cpu:
tia: 
    make the offset for missiles on stretched player different
Audio:
    start audio



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
     write: xxx0 xx1x 1xx0 x1??
     read:  xxx0 xx1x 1xxx x1x1

TIA notes
Resolution: Wide: 160 visible 68 invisible 228 total, Tall: 192 visible, 40 invisible on top 232 total

Scan speed:
    3 pixels per cycle
    1 line is 76 cycles
    22 2/3 cycles for h-blank (68 pixels)
    playfield is updated every 4 pixels

TIA Registers:
    Other:
    hmove (strobe) applies all hm registers to sprites (note hm registers will not apply until hmove is activated)

    Sync: (note vblank and vsync are updated at the start of the line where its not vsync/vblank)
    wsync (strobe) pauses cpu until next scanline 
    vsync (------0-) has to be 1 for first 3 scan lines and then put back to 0
    vblank (------0-) should be set to 1 after vsync is done for 37 lines

    playfield:
    Pf0 4bit (left 4 used)
    Pf1 8bit
    Pf2 8bit

    player:
    GRP0 the player 0 values ✅
    Nusiz0 (-----PPP) PPP: ✅
        000: normal 
        001: two spaced by 16 
        010: two spaced by 32 
        011: three spaced by 16 
        100: 2 spaced by 64 
        101: stretched by x2 
        110: 3 spaced by 32 
        111: stretched by x4
    resp0 (strobe) (5 pixels after store is done for no stretch, 6 pixels for x2) (keeps horizontal position even after line is done)✅
    hmp0 (xxxx----) 4 bit signed number of pixels to move to left ✅
    colup0 color of player 2 ✅
    refp0 if bit 3 is 1, player is mirrored, otherwise normal ✅
    vdelp0 if bit 0 is 1, grp0 is only changed if grp1 is written to ✅
    if 1 grp0buff address is returned and that location is modified. then when grp1 is triggered for strobe, the grp0buffer is applied to actual grp0 ✅

    first implement sprite check (normal)
    implement repeating modes
    implement mirror
    implement structure for resp0 and hmp0 for horizontal offset setting
    implement vdelp0 delay


    GRP1 the player 1 values
    Nusiz1 (-----PPP) PPP: 000: normal 001: two spaced by 16 010: two spaced by 32 011: three spaced by 16 100: 2 spaced by 64 101: stretched by x2 110: 3 spaced by 32 111: stretched by x4
    resp1 (strobe) (5 pixels after store is done for no stretch, 6 pixels for x2) (keeps horizontal position even after line is done)
    hmp1 (xxxx----) 4 bit signed number of pixels to move to left
    colup1 color of player 1
    refp1 if bit 3 is 1, player is mirrored, otherwise normal
    vdelp1 if bit 0 is 1, grp1 is only changed if grp0 is written to

    missle:
    Enam0 1bit (------1-)
    Enam1 1bit (------1-)
    Nusiz0 (--SS----) SS: 00: 1 long 01: 2 long 10: 4 11: 8 long
    Nusiz1 (--SS----) SS: 00: 1 long 01: 2 long 10: 4 11: 8 long
    resm0 (strobe) (5 pixels after store is done for no stretch, 6 pixels for x2) (keeps horizontal position even after line is done)
    resm1 (strobe) (5 pixels after store is done for no stretch, 6 pixels for x2) (keeps horizontal position even after line is done)
    hmm0 (xxxx----) 4 bit signed number of pixels to move to left
    hmm1 (xxxx----) 4 bit signed number of pixels to move to left

    ball:
    Enabl 1bit (bit 1)
    Resbl (strobe) (5 pixels after store is done for no stretch, 6 pixels for x2) (keeps horizontal position even after line is done)
    hmbl (xxxx----) 4 bit signed number of pixels to move to left

PlayField:
    order of drawing (byte is 76543210):
        pf0: 4567 pf1: 76543210 pf2 1234567
        repeated or mirrored
    Garbage is written if pf register is written to while it is being used to draw to screen

    there are 160 pixels across, at nth pixel we need to find if its zero or 1 

Players:
    only way to disable player sprites is to set them to all zeros

Missles:

Ball:
