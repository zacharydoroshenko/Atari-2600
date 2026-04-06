#include "cpu.h"
#include "debug.h"
#include <fstream>
#include <SDL2/SDL.h>
#include <vector>
#include <assert.h>
#include "addressDef.h"
typedef void (*FunctionPtr)(CPUState* S);

int main(int argc, char *argv[]){
    assert(argc == 2);
    //sdl window initialization
    // 1. Define your internal resolution
    const int NATIVE_WIDTH = WINDOWWIDTH;
    const int NATIVE_HEIGHT = WINDOWHEIGHT; // Typical visible area

    // 2. Define a 4:3 window size (e.g., 640x480 or 800x600)
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480; 

    SDL_Window* window = SDL_CreateWindow("Atari 2600 Emulator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // 3. This is the magic line:
    // It tells SDL to treat the window as 160x192 and stretch it to fit the 4:3 window.
    // SDL_RenderSetLogicalSize(renderer, NATIVE_WIDTH, NATIVE_HEIGHT);
        // Set to "0" for pixelated (nearest neighbor) 
    // Set to "1" for linear filtering (blurry/smooth)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"); 

    SDL_Texture* texture = SDL_CreateTexture(renderer, 
    SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 
    NATIVE_WIDTH, NATIVE_HEIGHT);
    SDL_Event event;

    //Open ROM 
    // ifstream file("Q-bert.a26", ios::binary);
    // ifstream file("Combat.a26", ios::binary);
    // ifstream file("Adventure.a26", ios::binary);
    // ifstream file("SpaceInvaders.a26", ios::binary);
    // ifstream file("Asteroids.a26", ios::binary);
    // ifstream file("DonkeyKong.a26", ios::binary);
    // ifstream file("Frogger.a26", ios::binary);
    // ifstream file("Pitfall.a26", ios::binary);
    // ifstream file("PacMan.a26", ios::binary);
    ifstream file(argv[1], ios::binary);
    

    if(!file){
        std::cout << "Attempting to open: " << argv[1] << std::endl;
        cerr << "Failed to open the file!" << std::endl;
        return 1;
    }
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg); // Move back to start



    //initialize CPUState
    CPUState* S = (CPUState*) calloc(sizeof(CPUState), 1);
    TimerState* Ti = (TimerState*) calloc(sizeof(TimerState), 1);
    TIAState* T = (TIAState*) calloc(sizeof(TIAState), 1);
    initialize(S, size, T, Ti);

    //initialize TIAState

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
    // while(((S->memory[VSYNC] & 2) == 0)){
    //     uint8_t oppcode = S->memory[S->PC]; 
    //     printf("cycle: %d, 0x%X: %s\n", cycle, S->PC, InstrStr[oppcode >> 4][oppcode & 0b1111]);
    //     Run(S, Ti);
    //     cycle+=S->cycleDif;
    // }

    //main loop
    bool running = true;
    bool next = true;
    int frame=0;
    uint64_t freq = SDL_GetPerformanceFrequency();
    double targetFrameTime = 1.0 / 60.0;
    S->memory[INPT4] |= 0x80;
    S->memory[INPT5] |= 0x80;
    while (running) {
        uint64_t start = SDL_GetPerformanceCounter();
        // handle input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_SPACE) next = true;

                // --- Console Switches (SWCHB) ---
                if (event.key.keysym.sym == SDLK_1) T->swchb &= ~0x01; // Reset pressed (bit 0)
                if (event.key.keysym.sym == SDLK_2) T->swchb &= ~0x02; // Select pressed (bit 1)

                // --- Player 0 Joystick (SWCHA bits 4-7) ---
                if (event.key.keysym.sym == SDLK_UP)    S->memory[SWCHA] &= ~0x10;
                if (event.key.keysym.sym == SDLK_DOWN)  S->memory[SWCHA] &= ~0x20;
                if (event.key.keysym.sym == SDLK_LEFT)  S->memory[SWCHA] &= ~0x40;
                if (event.key.keysym.sym == SDLK_RIGHT) S->memory[SWCHA] &= ~0x80;

                // --- Player 0 Fire (INPT4 bit 7) ---
                if (event.key.keysym.sym == SDLK_z || event.key.keysym.sym == SDLK_LCTRL) {
                    S->memory[INPT4] &= ~0x80;
                }

                if (event.key.keysym.sym == SDLK_w)    S->memory[SWCHA] &= ~0x01;
                if (event.key.keysym.sym == SDLK_s)  S->memory[SWCHA] &= ~0x02;
                if (event.key.keysym.sym == SDLK_a)  S->memory[SWCHA] &= ~0x04;
                if (event.key.keysym.sym == SDLK_d) S->memory[SWCHA] &= ~0x08;

                if (event.key.keysym.sym == SDLK_e || event.key.keysym.sym == SDLK_LCTRL) {
                    S->memory[INPT5] &= ~0x80;
                }
            }

            if (event.type == SDL_KEYUP) {
                // --- Release Console Switches ---
                if (event.key.keysym.sym == SDLK_1) T->swchb |= 0x01;
                if (event.key.keysym.sym == SDLK_2) T->swchb |= 0x02;

                // --- Release Joystick ---
                if (event.key.keysym.sym == SDLK_UP)    S->memory[SWCHA] |= 0x10;
                if (event.key.keysym.sym == SDLK_DOWN)  S->memory[SWCHA] |= 0x20;
                if (event.key.keysym.sym == SDLK_LEFT)  S->memory[SWCHA] |= 0x40;
                if (event.key.keysym.sym == SDLK_RIGHT) S->memory[SWCHA] |= 0x80;

                // --- Release Fire ---
                if (event.key.keysym.sym == SDLK_z || event.key.keysym.sym == SDLK_LCTRL) {
                    S->memory[INPT4] |= 0x80;
                }

                if (event.key.keysym.sym == SDLK_w)  S->memory[SWCHA] |= 0x01;
                if (event.key.keysym.sym == SDLK_s)  S->memory[SWCHA] |= 0x02;
                if (event.key.keysym.sym == SDLK_a)  S->memory[SWCHA] |= 0x04;
                if (event.key.keysym.sym == SDLK_d)  S->memory[SWCHA] |= 0x08;

                if (event.key.keysym.sym == SDLK_e || event.key.keysym.sym == SDLK_LCTRL) {
                    S->memory[INPT5] |= 0x80;
                }

            }

            //apply swchb to actual mem
            // S->memory[SWCHB] &= 0b00110100;
            // T->swchb &= 0b11001011;
            // S->memory[SWCHB] |= T->swchb;
        }

        // plot pixels
        // int cycle = 0;
        if(next || !(S->frameStep)){
            for(int i=0; i < NATIVE_WIDTH * NATIVE_HEIGHT;i++) T->frame[i] = 0xFF000000;

            while(true){
                
                // printf("0x%X: %s\n", S->PC, InstrStr[oppcode >> 4][S->memory[S->PC] & 0b1111]);
                // Run(S, Ti);
                if(moveBeam(S)) {
                    // printf("Background: 0x%X\n", S->memory[COLUBK]);
                    // printf("PlayField: 0x%X\n", S->memory[COLUPF]);
                    // if (((S->memory[VSYNC] & 2) != 0) && T->vpos > 5) break;

                    //print out the memory
                    if(S->frameCounter) printf("FRAME %d\n", frame);
                    frame++;
                    if(S->memoryGrid){
                        printf("   ");
                        for(int j=0x0; j <= 0xf; j++){
                            printf(" %X ", j);
                        }
                        printf("\n");
                        for(int i=0x8; i <= 0xf; i++){
                            printf("%X: ", i);
                            for(int j=0x0; j <= 0xf; j++){
                                printf("%02X ", S->memory[(i << 4)+j]);
                            }
                            printf("\n");
                            
                        }
                    }
                    // printf("Collision with m1 p0: 0x%X\n", (S->memory[CXM1P] >> 7) & 1);
                    // printf("Frame end — CXM0P:0x%X CXM1P:0x%X CXPPMM:0x%X StirTimer:0x%X\n",S->memory[CXM0P], S->memory[CXM1P], S->memory[CXPPMM], S->memory[0x8A]);

                    break;
                }
            }
            
        }
        next=false;

        //render
        // Ensure WIDTH is 160 and HEIGHT is 192
        SDL_UpdateTexture(texture, NULL, T->frame, NATIVE_WIDTH * sizeof(uint32_t));
        // memset(T->frame, 0, sizeof(T->frame));
        // 

        SDL_RenderClear(renderer);
        // Because of RenderSetLogicalSize, NULL as the destination rect 
        // will automatically stretch 160x192 to fill the 4:3 window.
        SDL_RenderCopy(renderer, texture, NULL, NULL); 
        SDL_RenderPresent(renderer);
        uint64_t end = SDL_GetPerformanceCounter();
        double elapsed = (double)(end - start) / freq;

        if (elapsed < targetFrameTime) {
            // Calculate milliseconds to sleep
            uint32_t sleepTime = (uint32_t)((targetFrameTime - elapsed) * 1000.0);
            if (sleepTime > 0) {
                SDL_Delay(sleepTime);
            }
            
            // Optional: High-precision "Busy Wait" for the final tiny fraction
            // while ((double)(SDL_GetPerformanceCounter() - start) / freq < targetFrameTime) {
            //    // Do nothing, just wait out the last few microseconds
            // }
        }
        
    }




}