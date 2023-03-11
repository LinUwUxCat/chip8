#ifndef _GRAPHICS_CHIP8
#define _GRAPHICS_CHIP8
#include <chrono>
#include <3ds.h>
#include <citro2d.h>

// Some variables
// How much to upscale the rendering. Original width and height is 64x32 (scale 1). default scale is 10
extern const int scale;
// The screen array.
extern bool screen[2048];
// Pixel colors. ON is by default white, OFF is by default black.
extern u32 on_color;
extern u32 off_color;


/// THESE ARE DEFINED HERE BECAUSE THEY DECREMENT AT THE SAME SPEED AS THE RENDER ///
extern uint8_t delayT; //Delay timer. decremented 60 times per second until it reaches 0.
extern uint8_t soundT; //Sound timer. Works like the delay timer, but makes sound until it reaches 0.

//FPS and LPS stuff
extern std::chrono::_V2::system_clock::time_point LPSLastTime;
extern uint32_t LPSCurrent;
extern uint32_t LPSLoops;
extern std::chrono::_V2::system_clock::time_point FPSLastTime;
extern uint32_t FPSCurrent;
extern uint32_t FPSFrames;
extern std::chrono::_V2::system_clock::time_point FPSLastRefreshAt;
extern uint32_t IPSTarget;
extern std::chrono::_V2::system_clock::time_point IPSLastTime;
extern uint32_t IPSCurrent;
extern uint32_t IPSInstructions;
extern std::chrono::_V2::system_clock::time_point IPSLastRefreshAt;

void render();
int flipPixel(uint8_t x, uint8_t y);
void FileOpener();
#endif //_GRAPHICS_CHIP8