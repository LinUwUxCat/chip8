#ifndef _LOGIC_CHIP8
#define _LOGIC_CHIP8
#include <stdint.h>
#include <vector>
#include "graphics.hpp"
////////////////////////////////
/////////CHIP-8 LOGIC///////////
////////////////////////////////
bool fileLoaded = false;
uint16_t filesize;
uint16_t curInst = 0x00E0;
uint16_t PC = 0x200; //Program Counter. Counts how much instructions we are in
uint16_t I = 0; //Index register. Used to point at instructions in memory
std::vector<uint16_t> stack;
uint8_t V[15] = {0}; //registers
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
    curInst = ((uint16_t)mem[PC] << 8) | mem[PC+1];
    PC+=2;
    /////////////////////
    ////////DECODE///////
    /////////////////////
    switch (curInst & 0xF000){
        case 0x0000:
            if (curInst == 0x00E0){
                memset(screen, 0, sizeof(screen));break;
            } else if (curInst == 0x00EE){
                PC = stack.back();
                stack.pop_back();break;
            } else {
                printf("Instruction %x is not known!\n", curInst);break;
            }
            break;
        case 0x1000:
            PC = 0x0FFF & curInst;break;
        case 0x2000:
            stack.push_back(PC);
            PC = 0x0FFF & curInst;printf("PC is now %X\n", PC);break;
        case 0x3000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0x4000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0x5000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0x6000:
            V[(curInst & 0x0F00) >> 8] = curInst & 0x00FF;break;
        case 0x7000:
            V[(curInst & 0x0F00) >> 8] += curInst & 0x00FF;break;
        case 0x8000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0x9000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0xA000:
            I = curInst & 0x0FFF;break;
        case 0xB000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0xC000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0xD000: //Drawing sprites
            {
                uint16_t xpos = V[(curInst & 0x0F00) >> 8] % 64; // Starting position of sprites wrap
                uint16_t ypos = V[(curInst & 0x00F0) >> 4] % 32; // But not the sprite itself
                uint8_t curByte;
                V[0xF] = 0;
                for (int i = 0; i < (curInst & 0x000F); i++){
                    curByte = mem[I+i];
                    for (int b = 7; b>=0; b--){
                        if (xpos >= 64)break;
                        if ((curByte >> b)&1){
                            if (flipPixel(xpos, ypos)){
                                V[0xF] = 1;
                            }
                        }
                        xpos++;
                    }
                    ypos++;
                    if (ypos>=32)break;
                    xpos = V[(curInst & 0x0F00) >> 8] % 64;
                }
                break;
            }
        case 0xE000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        case 0xF000:
            printf("Instruction %x is not implemented!\n", curInst);break;
        default:
            printf("What is your system?!\n");
    }
}
#endif //_LOGIC_CHIP8