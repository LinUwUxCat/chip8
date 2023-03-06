#include "graphics.hpp"
int scale = 10;
bool screen[2048] = {false};
SDL_Color on_color = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color off_color = {0x00, 0x00, 0x00, 0xFF};

/// THESE ARE DEFINED HERE BECAUSE THEY DECREMENT AT THE SAME SPEED AS THE RENDER ///
uint8_t delayT = 0; //Delay timer. decremented 60 times per second until it reaches 0.
uint8_t soundT = 0; //Sound timer. Works like the delay timer, but makes sound until it reaches 0.

Uint32 LPSLastTime=0;
Uint32 LPSCurrent=0;
Uint32 LPSFrames=0;
Uint32 FPSLastTime=0;
Uint32 FPSCurrent=0;
Uint32 FPSFrames=0;
Uint32 LastRefreshAt=0;
Uint32 FPSLimit=60;

void flip(uint16_t x, uint16_t y){
    x = x%64;
    y = y%32;
    uint16_t pos = y*64 + x; 
    screen[pos] = !screen[pos];
}

void render(SDL_Renderer* renderer){
    for(int i = 0; i < 2048 ; i++){
        SDL_Rect rect = {i%64 * scale,i/64 * scale + 19,scale,scale};
        SDL_Color *used = screen[i]?&on_color:&off_color;
        SDL_SetRenderDrawColor(renderer, used->r, used->g, used->b, used->a);
        SDL_RenderFillRect(renderer, &rect);
    }
    if (delayT) delayT--;
    if (soundT) soundT--;
}

void FileOpener(){};