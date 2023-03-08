#ifndef _LOGIC_CHIP8
#define _LOGIC_CHIP8
#define X ((curInst & 0x0F00) >> 8)
#define Y ((curInst & 0x00F0) >> 4)
#include <stdint.h>
#include <vector>
#include <bitset>
#include "graphics.hpp"
////////////////////////////////
/////////CHIP-8 LOGIC///////////
////////////////////////////////

// True if a rom is being executed. That doesn't mean there is nothing in memory!!
bool fileLoaded = false;
// Size of the rom. Useful to check whether instructions request data from the correct place
uint16_t filesize;
// Current instruction. Default is Clear Screen but it shouldn't matter
uint16_t curInst = 0x00E0;
//Program Counter. Counts how much instructions we are in
uint16_t PC = 0x200; 
//Index register. Used to point at instructions in memory
uint16_t I = 0; 
//The stack.
std::vector<uint16_t> stack;
//registers. Go from 0 to F, VF being the carry register
uint8_t V[16] = {0}; 
//Memory. Contains the font as its first 80 bytes, and the rom is inserted after byte 512, so there's some free space?
uint8_t mem[4096] = { 0 };
//The font.
uint8_t font[80] = {
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
// Buttons. used for inputting data.
std::bitset<16> B = {0};

/* The following variables are quirks that may change how a program works and are due to the introduction of CHIP-48 and SUPER-CHIP.
   They are set to true if their behavior follows the SUPER-CHIP instructions, and false if only following the original CHIP-8.      */

// True if 0x8XY6 and 0x8XYE shift in place, false if they copy VY to VX then shift in place.
bool super8 = false;
// False if doing 0xBNNN, true if doing 0xBXNN. See case 0xB000 for more info
bool superB = false;
// False if I is incremented when storing/loading memory, true if it isn't.
bool superF = true;
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
            if (curInst == 0x00E0){ //Clear screen
                memset(screen, 0, sizeof(screen));break;
            } else if (curInst == 0x00EE){ //Return from subroutine (see 0x2000)
                PC = stack.back();
                stack.pop_back();break;
            } else {
                printf("Instruction %x is not known!\n", curInst);break;
            }
            break;
        case 0x1000:    //Jump to NNN
            PC = 0x0FFF & curInst;break; 
        case 0x2000:    //Call subroutine (jump to NNN, but save PC location in the stack first.)
            stack.push_back(PC);            
            PC = 0x0FFF & curInst;break;    
        case 0x3000:    //Skip (PC+=2) if VX == NN
            if (V[X] == (curInst & 0x00FF))PC+=2;break;
        case 0x4000:    //Skip (PC+=2) if VX != NN
            if (V[X] != (curInst & 0x00FF))PC+=2;break;
        case 0x5000:    //Skip (PC+=2) if VX == VY
            if (V[X] == V[Y])PC+=2;break;
        case 0x6000:    //Set VX to NN
            V[X] = curInst & 0x00FF;break;
        case 0x7000:    //Add NN to VX. Does not affect the carry flag.
            V[X] += curInst & 0x00FF;break;
        case 0x8000:    //Logic and arithmetic 
            switch (curInst & 0x000F){
                case 0x0000: //Set VX to VY
                    V[X] = V[Y];break;
                case 0x0001: //Set VX to VX | VY (Bitwise OR)
                    V[X] = V[X] | V[Y];break;
                case 0x0002: //Set VX to VX & VY (Bitwise AND)
                    V[X] = V[X] & V[Y];break;
                case 0x0003: //Set VX to VX ^ VY (Bitwise XOR)
                    V[X] = V[X] ^ V[Y];break;
                case 0x0004: //Set VX to VX + VY. Unlike 0x7000, this does affect the carry flag.
                    if ((uint16_t)V[X] + (uint16_t)V[Y] > 0xFF)V[0xF]=1;else V[0xF]=0;
                    V[X]+=V[Y];break;
                case 0x0005: //Set VX to VX - VY. The carry flag is set to 1 if the operation doesn't underflow, and 0 if it does.
                    V[0xF]=V[X]>=V[Y];
                    V[X]-=V[Y];break;
                case 0x0006: //If not CHIP-48 or SUPER-CHIP, set VX to VY, then shift VX one bit to the right and set VF to the shifted bit.
                    if(!super8)V[X]=V[Y];
                    V[0xF] = V[X]&1;
                    V[X]>>=1;break;
                case 0x0007: //Set VX to VY - VX. The carry flag is set to 1 if the operation doesn't underflow, and 0 if it does.
                    V[0xF]=V[Y]>=V[X];
                    V[Y]-=V[X];break;
                case 0x000E: //If not CHIP-48 or SUPER-CHIP, set VX to VY, then shift VX one bit to the left and set VF to the shifted bit.
                    if(!super8)V[X]=V[Y];
                    V[0xF] = V[X]&8;
                    V[X]<<=1;break;
                default:
                    printf("Instruction %X is not known!\n", curInst);break;
            }break;
        case 0x9000:    //Skip (PC+=2) if VX != VY
            if (V[X] != V[Y])PC+=2;break;
        case 0xA000:    //Set I to NNN
            I = curInst & 0x0FFF;break;
        case 0xB000:    //Jump to NNN + V0 if CHIP-8, jump to XNN + VX if CHIP-48 or SUPER-CHIP
            PC = (0x0FFF & curInst) + V[superB?X:0];break;
        case 0xC000:    //Generates a random number, & NN and put it in VX
            V[X] = (rand() % 256) & (curInst & 0x00FF);break;
        case 0xD000: //Draw a sprite at position VX, VY, N tall.
            {
                uint16_t xpos = V[X] % 64; // Starting position of sprites wrap
                uint16_t ypos = V[Y] % 32; // But not the sprite itself
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
                    xpos = V[X] % 64;
                }
                break;
            }
        case 0xE000:    //Skip if key
            if ((curInst & 0x00FF) == 0x9E){ //PC+=2 if the button corresponding to the value in VX is pressed.
                if (B[V[X]])PC+=2;break;
            } else if ((curInst & 0x00FF) == 0xA1){ //PC+=2 if the button corresponding to the value in VX is NOT pressed.
                if (!B[V[X]])PC+=2;break;
            } else {
                printf("Instruction %X is not known!\n", curInst);break;
            }
        case 0xF000:    //A few other instructions
            switch (curInst & 0x00FF){
                case 0x07: //Set VX to the current value of the delay timer
                    V[X] = delayT;break;
                case 0x15: //Set the delay timer to VX
                    delayT = V[X];break;
                case 0x18: //Set the sound timer to VX
                    soundT = V[X];break;
                case 0x1E: //Add VX to I. Will set VF to 1 if overflows. This was not the case on COSMAC VIP but it is the case on Amiga and 1 game relies on it.
                    {
                        uint16_t add = V[X] + I;
                        if (add >= 0x1000)V[0xF]=1;
                        I+=V[X];I&=0x0FFF;break; // Here we decide to loop back I to avoid any out-of bounds access. I will check later in Spacefight 2091! if everything works correctly
                    }
                case 0x0A: //Loop until button is pressed
                    if (!B.any())PC-=2;else V[X] = B._Find_first(); break;
                case 0x29: //Set I to character of font contained in VX. Since the font is saved from 0 to 80, we just do VX * 5.
                    I=5*(V[X] & 0x0F);break;
                case 0x33: //Set mem[I] to the first character of VX, mem[I+1] to the 2nd and mem[I+2] to the 3rd.
                    mem[I]=V[X]/100;
                    mem[I+1]=(V[X]/10)%10;
                    mem[I+2]=V[X]%10;break;
                case 0x55:
                    memcpy(&mem[I], V, (X+1) * sizeof(uint8_t));
                    if (!superF)I+=X+1;break;
                case 0x65:
                    memcpy(V, &mem[I], (X+1) * sizeof(uint8_t));
                    if (!superF)I+=X+1;break;

            }break;
        default:        //This should never be reached, unless your computer represent hexadecimals with more than 0xF values (which doesn't make sense, but you never know)
            printf("What is your system?!\n");
    }
}
#endif //_LOGIC_CHIP8