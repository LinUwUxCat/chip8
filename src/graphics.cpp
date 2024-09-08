#include "graphics.hpp"
int scale = 10;
bool screen[2048] = {false};
SDL_Color on_color = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color off_color = {0x00, 0x00, 0x00, 0xFF};

/// THESE ARE DEFINED HERE BECAUSE THEY DECREMENT AT THE SAME SPEED AS THE RENDER ///
uint8_t delayT = 0; //Delay timer. decremented 60 times per second until it reaches 0.
uint8_t soundT = 0; //Sound timer. Works like the delay timer, but makes sound until it reaches 0.

//Counters
Uint32 LPSLastTime=0;
Uint32 LPSCurrent=0;
Uint32 LPSLoops=0;
Uint32 FPSLastTime=0;
Uint32 FPSCurrent=0;
Uint32 FPSFrames=0;
Uint32 FPSLastRefreshAt=0;
Uint32 IPSTarget=700;
Uint32 IPSLastTime=0;
Uint32 IPSCurrent=0;
Uint32 IPSInstructions=0;
std::chrono::_V2::system_clock::time_point IPSLastRefreshAt=std::chrono::high_resolution_clock::now();

void flip(uint16_t x, uint16_t y){
    x = x%64;
    y = y%32;
    uint16_t pos = y*64 + x; 
    screen[pos] = !screen[pos];
}

void render(SDL_Renderer* renderer){
    for(int i = 0; i < 2048 ; i++){
        SDL_FRect rect = {i%64 * scale,i/64 * scale + 19,scale,scale};
        SDL_Color *used = screen[i]?&on_color:&off_color;
        SDL_SetRenderDrawColor(renderer, used->r, used->g, used->b, used->a);
        SDL_RenderFillRect(renderer, &rect);
    }
    if (delayT) delayT--;
    if (soundT) soundT--;
}

/**
 * @brief Flips a pixel on screen.
 * 
 * @param x X of the pixel to flip.
 * @param y Y of the pixel to flip.
 * @return int 1 if the pixel was previously ON and switched to OFF, else 0
 */
int flipPixel(uint8_t x, uint8_t y){
    return !(screen[x%64 + (y%32)*64] = !screen[x%64 + (y%32)*64]);
}

void FileOpener(){};