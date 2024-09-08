#ifndef _GRAPHICS_CHIP8
#define _GRAPHICS_CHIP8
#include <SDL3/SDL.h>
#include <chrono>
// Some variables
// How much to upscale the rendering. Original width and height is 64x32 (scale 1). default scale is 10
extern int scale;
// The screen array.
extern bool screen[2048];
// Pixel colors. ON is by default white, OFF is by default black.
extern SDL_Color on_color;
extern SDL_Color off_color;


/// THESE ARE DEFINED HERE BECAUSE THEY DECREMENT AT THE SAME SPEED AS THE RENDER ///
extern uint8_t delayT; //Delay timer. decremented 60 times per second until it reaches 0.
extern uint8_t soundT; //Sound timer. Works like the delay timer, but makes sound until it reaches 0.

//FPS and LPS stuff
extern Uint32 LPSLastTime;
extern Uint32 LPSCurrent;
extern Uint32 LPSLoops;
extern Uint32 FPSLastTime;
extern Uint32 FPSCurrent;
extern Uint32 FPSFrames;
extern Uint32 FPSLastRefreshAt;
extern Uint32 IPSTarget;
extern Uint32 IPSLastTime;
extern Uint32 IPSCurrent;
extern Uint32 IPSInstructions;
extern std::chrono::_V2::system_clock::time_point IPSLastRefreshAt;

void render(SDL_Renderer* renderer);
int flipPixel(uint8_t x, uint8_t y);
void FileOpener();
#endif //_GRAPHICS_CHIP8