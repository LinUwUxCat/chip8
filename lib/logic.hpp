#ifndef _LOGIC_CHIP8
#define _LOGIC_CHIP8
#include <stdint.h>
#include "graphics.hpp"
////////////////////////////////
/////////CHIP-8 LOGIC///////////
////////////////////////////////
bool fileLoaded = false;
uint16_t filesize;
uint16_t curInst = 0x00E0;
uint16_t PC = 0x200; //Program Counter. Counts how much instructions we are in
uint16_t I = 0; //Index register. Used to point at instructions in memory
uint16_t stack[32] = { 0 };
uint8_t V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF; //registers
uint8_t mem[4096] = { //we fill memory with the font. 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
bool B1, B2, B3, B4, B5, B6, B7, B8, B9, B0, BA, BB, BC, BD, BE, BF;

/**
 * @brief Fetch, Decode, Execute
 * 
 */
void FDE(){
    /////////////////////
    ////////FETCH////////
    /////////////////////
    if (PC > filesize){
        printf("Warning : Program Counter above program size! Stopping.\n");
        fileLoaded = false;
        return;
    }
    curInst = ((uint16_t)mem[PC+1] << 8) | mem[PC];
    PC+=2;

    /////////////////////
    ////////DECODE///////
    /////////////////////
    switch (curInst & 0xF000){
        case 0x0000:
            if (curInst == 0x00E0){
                memset(screen, 0, sizeof(screen));
                break;
            }
            
        
    }
}
#endif //_LOGIC_CHIP8