#include "graphics.hpp"
#include "logic.hpp"
#include <stdio.h>

#include <string>
#include <future> 

/**
 * @brief Open and loads file into memory.
 * 
 * @param filename Name of the file to open
 * @return int 0 if success, 1 if file open error, 2 if file too big, 3 if filename NULL or empty.
 */
int OpenFile(const char* filename){
    if (filename == NULL || !strcmp(filename, "")) return 3;
    FILE* f = fopen(filename, "r");
    if (f == NULL) return 1;
    int c;
    uint16_t p = 0x200;
    c = fgetc(f);
    memset(mem,0,4096);                 // Reset the memory...
    memset(V, 0, 15);                   // Registers...
    memcpy(mem, font, 80);              // And copy the font.
    memset(screen, 0, sizeof(screen));  // Reset the screen.
    PC = 0x200;                         // Reset program counter.
    I = 0;                              // Reset I.
    stack.clear();                      // Reset the stack.

    while (c != EOF){
        //printf("%d ", c);
        if (p > 4096) return 2;
        mem[p]=(uint8_t)c;
        ++p;
        c = fgetc(f);
    }
    filesize = p;
    return 0;
}

int main(int argc, char* argv[]){
    
    ////////////////////////////////
    ///////////RENDERING////////////
    ////////////////////////////////
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_BOTTOM, NULL);

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    if (top == NULL){
        printf("C3D error : can't create screen target");
        return 1;
    }

    ////////////////////////////////
    ///////////IMPORTANT////////////
    ////////////////////////////////
    memcpy(mem, font, 80); //Copy the font to memory.

    ////////////////////////////////
    ///////////VARIABLES////////////
    ////////////////////////////////

    bool showDebugMenu = false;
    bool hasToOpen = true;
    const char * fileToOpen = "sdmc:/chip8-test-suite.ch8";
    bool showInputs = false;
    bool showSettingsWindow=false;
    int SecondAdjustment = 16;
    bool showFps=false;
    bool showIps=false;
    LPSLastTime = FPSLastTime = FPSLastRefreshAt = IPSLastRefreshAt = std::chrono::high_resolution_clock::now(); //Very high value

    ////////////////////////////////
    ///////////MAIN LOOP////////////
    ////////////////////////////////
    while (aptMainLoop()){

        //////////////////////
        ////////RENDER////////
        //////////////////////
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - FPSLastRefreshAt).count() >= SecondAdjustment){
            

            SecondAdjustment = SecondAdjustment==16?17:16;  //This is used so i can render at 61 fps consistently, instead of 63 (render every 16ms) or 59 (render every 17ms).
                                                            //I prefer 61 to finding a stable 60 solution for now, because in case of small lag, it will drop to 60.
            FPSLastRefreshAt = std::chrono::high_resolution_clock::now();

            //C2/3D things
            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C2D_TargetClear(top, C2D_Color32(0,0,0,0xFF));
            C2D_SceneBegin(top);
            render();
            C3D_FrameEnd(0);
            // //FPS
            // FPSFrames++;
            // if (FPSLastTime < SDL_GetTicks() - 1000){
            //     FPSLastTime = SDL_GetTicks();
            //     FPSCurrent = FPSFrames;
            //     FPSFrames = 0;
            // }
        }
        /////END OF RENDER/////
        if (hasToOpen){
            
            hasToOpen = false;
            //fileToOpen = "sdmc:/chip8-test-suite.ch8";
            if (!OpenFile(fileToOpen)){
                fileLoaded = true;
            } else {
                fileLoaded = false;
            }
        }

        
        

        //////////////////////
        ////////LOGIC/////////
        //////////////////////
        if (fileLoaded && (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - IPSLastRefreshAt).count()) >= 1000000/IPSTarget){
            IPSLastRefreshAt = std::chrono::high_resolution_clock::now();        
            switch (preset){
                case 0://Custom - Let the user choose
                    break;
                case 1: //CHIP-8
                    chip8123=true;
                    super8=superB=superF=false;break;
                case 2: //SUPER-CHIP
                    chip8123=false;
                    super8=superB=superF=true;break;
                case 3: //XO-CHIP
                    chip8123=super8=superB=superF=false;break;
                default: //??
                    break;
            }

            //EVENT
            hidScanInput();
            u32 kDown = hidKeysDown();
            if (kDown & KEY_START && kDown & KEY_SELECT) break;
            if (kDown & KEY_START){
                hasToOpen = true;
                fileToOpen = "sdmc:/chip8-test-suite.ch8";
            }
            if (kDown & KEY_SELECT){
                hasToOpen=true;
                fileToOpen="sdmc:/cavern.ch8";
            }

            if (kDown & KEY_DUP){B=0;B[0x2]=1;}
            if (kDown & KEY_DDOWN){B=0;B[0x8]=1;}
            if (kDown & KEY_DLEFT){B=0;B[0x4]=1;}
            if (kDown & KEY_DRIGHT){B=0;B[0x6]=1;}
            if (kDown & KEY_L){B=0;B[0x1]=1;}
            if (kDown & KEY_R){B=0;B[0x3]=1;}
            if (kDown & KEY_B){B=0;B[0x5]=1;}
            u32 kUp = hidKeysUp();
            if (kUp & KEY_DUP)B[0x2]=0;
            if (kUp & KEY_DDOWN)B[0x8]=0;
            if (kUp & KEY_DLEFT)B[0x4]=0;
            if (kUp & KEY_DRIGHT)B[0x6]=0;
            if (kUp & KEY_L)B[0x1]=0;
            if (kUp & KEY_R)B[0x3]=0;
            if (kUp & KEY_B)B[0x5]=0;

            FDE();
            // IPSInstructions++;
            // if (IPSLastTime < SDL_GetTicks() - 1000){
            //     IPSLastTime = SDL_GetTicks();
            //     IPSCurrent = IPSInstructions;
            //     IPSInstructions=0;
            // }
        }


        //LPS
        // LPSLoops++;
        // if (LPSLastTime < SDL_GetTicks() - 1000){
        //     LPSLastTime = SDL_GetTicks();
        //     LPSCurrent = LPSLoops;
        //     LPSLoops = 0;
        // }
    }


    return 0;
}