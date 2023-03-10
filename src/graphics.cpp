#include "graphics.hpp"
int scale = 10;
bool screen[2048] = {false};
u32 on_color = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
u32 off_color = C2D_Color32(0x00, 0x00, 0x00, 0xFF);

/// THESE ARE DEFINED HERE BECAUSE THEY DECREMENT AT THE SAME SPEED AS THE RENDER ///
uint8_t delayT = 0; //Delay timer. decremented 60 times per second until it reaches 0.
uint8_t soundT = 0; //Sound timer. Works like the delay timer, but makes sound until it reaches 0.

//Counters
uint32_t LPSLastTime=0;
uint32_t LPSCurrent=0;
uint32_t LPSLoops=0;
uint32_t FPSLastTime=0;
uint32_t FPSCurrent=0;
uint32_t FPSFrames=0;
uint32_t FPSLastRefreshAt=0;
uint32_t IPSTarget=700;
uint32_t IPSLastTime=0;
uint32_t IPSCurrent=0;
uint32_t IPSInstructions=0;
std::chrono::_V2::system_clock::time_point IPSLastRefreshAt=std::chrono::high_resolution_clock::now();

void flip(uint16_t x, uint16_t y){
    x = x%64;
    y = y%32;
    uint16_t pos = y*64 + x; 
    screen[pos] = !screen[pos];
}

void render(){
    for(int i = 0; i < 2048 ; i++){
        u32 used = screen[i]?on_color:off_color;
        C2D_DrawRectSolid(i%64 * scale,i/64 * scale + 19,1,scale,scale,used);
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