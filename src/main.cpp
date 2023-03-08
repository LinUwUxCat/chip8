#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
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

    SDL_Window *window;
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CHIP-8", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        64*scale, 
        32*scale + 19, //Imgui bar height 
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (window == NULL){
        printf("SDL Init error : Could not create SDL window.");
        return 1;
    }
    if (renderer == NULL){
        printf("SDL Init error : Could not create renderer.");
        return 1;
    }

    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark(); //#DARKMODEFOREVER #H4ck3rm4n #Codemasterz

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    ////////////////////////////////
    ///////////IMPORTANT////////////
    ////////////////////////////////
    memcpy(mem, font, 80); //Copy the font to memory.


    ////////////////////////////////
    ///////////VARIABLES////////////
    ////////////////////////////////

    bool done = false;
    bool hasToOpen = false;
    bool showInputs = false;
    int SecondAdjustment = 16;
    LPSLastTime = SDL_GetTicks();
    FPSLastTime = SDL_GetTicks();
    FPSLastRefreshAt = SDL_GetTicks();
    IPSLastRefreshAt = std::chrono::high_resolution_clock::now(); //Very high value

    ////////////////////////////////
    ///////////MAIN LOOP////////////
    ////////////////////////////////
    while (!done){

        //////////////////////
        ////////RENDER////////
        //////////////////////
        if (SDL_GetTicks() - FPSLastRefreshAt >= SecondAdjustment){
            //EVENT
            SDL_Event event;
            while (SDL_PollEvent(&event)){
                ImGui_ImplSDL2_ProcessEvent(&event);
                switch (event.type){
                    /****************
                     ****QUITTING****
                     ****************/
                    case SDL_QUIT:
                        done=true;
                        break;
                    case SDL_WINDOWEVENT:
                        if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)){
                            done=true;
                        }
                        break;
                    /******************
                     **INPUT HANDLING**
                     ******************/
                    //Send help.
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.scancode){
                            case SDL_SCANCODE_1:
                                B=0;B[0x1]=1;break;
                            case SDL_SCANCODE_2:
                                B=0;B[0x2]=1;break;
                            case SDL_SCANCODE_3:
                                B=0;B[0x3]=true;break;
                            case SDL_SCANCODE_4:
                                B=0;B[0xC]=true;break;
                            case SDL_SCANCODE_Q:
                                B=0;B[0x4]=1;break;
                            case SDL_SCANCODE_W:
                                B=0;B[0x5]=true;break;
                            case SDL_SCANCODE_E:
                                B=0;B[0x6]=true;break;
                            case SDL_SCANCODE_R:
                                B=0;B[0xD]=true;break;
                            case SDL_SCANCODE_A:
                                B=0;B[0x7]=true;break;
                            case SDL_SCANCODE_S:
                                B=0;B[0x8]=1;break;
                            case SDL_SCANCODE_D:
                                B=0;B[0x9]=true;break;
                            case SDL_SCANCODE_F:
                                B=0;B[0xE]=true;break;
                            case SDL_SCANCODE_Z:
                                B=0;B[0xA]=true;break;
                            case SDL_SCANCODE_X:
                                B=0;B[0x0]=true;break;
                            case SDL_SCANCODE_C:
                                B=0;B[0xB]=true;break;
                            case SDL_SCANCODE_V:
                                B=0;B[0xF]=true;break;
                        }break;
                    case SDL_KEYUP:
                        switch (event.key.keysym.scancode){
                            case SDL_SCANCODE_1:
                                B[0x1]=false;break;
                            case SDL_SCANCODE_2:
                                B[0x2]=false;break;
                            case SDL_SCANCODE_3:
                                B[0x3]=false;break;
                            case SDL_SCANCODE_4:
                                B[0xC]=false;break;
                            case SDL_SCANCODE_Q:
                                B[0x4]=false;break;
                            case SDL_SCANCODE_W:
                                B[0x5]=false;break;
                            case SDL_SCANCODE_E:
                                B[0x6]=false;break;
                            case SDL_SCANCODE_R:
                                B[0xD]=false;break;
                            case SDL_SCANCODE_A:
                                B[0x7]=false;break;
                            case SDL_SCANCODE_S:
                                B[0x8]=false;break;
                            case SDL_SCANCODE_D:
                                B[0x9]=false;break;
                            case SDL_SCANCODE_F:
                                B[0xE]=false;break;
                            case SDL_SCANCODE_Z:
                                B[0xA]=false;break;
                            case SDL_SCANCODE_X:
                                B[0x0]=false;break;
                            case SDL_SCANCODE_C:
                                B[0xB]=false;break;
                            case SDL_SCANCODE_V:
                                B[0xF]=false;break;
                        }break;
                }
            }
            SecondAdjustment = SecondAdjustment==16?17:16;  //This is used so i can render at 61 fps consistently, instead of 63 (render every 16ms) or 59 (render every 17ms).
                                                            //I prefer 61 to finding a stable 60 solution for now, because in case of small lag, it will drop to 60.
            FPSLastRefreshAt = SDL_GetTicks();
            //IMGUI
            ImGui_ImplSDLRenderer_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if(ImGui::BeginMainMenuBar()){
                if (ImGui::BeginMenu("File")){
                    ImGui::MenuItem("Open file", NULL, &hasToOpen);
                    ImGui::MenuItem("Quit", NULL, &done);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Debug")){
                    std::string lpsString = "Loops per s : " + std::to_string(LPSCurrent);
                    ImGui::MenuItem(lpsString.c_str(), NULL, false);
                    std::string fpsString = "Frames per s : " + std::to_string(FPSCurrent);
                    ImGui::MenuItem(fpsString.c_str(), NULL, false);
                    if (ImGui::BeginMenu("Instructions per s")){
                        ImGui::InputInt("IPS", (int*)&IPSTarget);
                        ImGui::MenuItem(std::to_string(IPSCurrent).c_str());
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Registers")){
                        for (int r = 0; r < 16; r++)ImGui::MenuItem(std::to_string(V[r]).c_str());
                        ImGui::EndMenu();
                    }
                    ImGui::MenuItem("PC", std::to_string(PC).c_str());
                    ImGui::MenuItem("I", std::to_string(I).c_str());
                    if (ImGui::BeginMenu("Stack")){
                        for (int s = 0; s < stack.size(); s++){
                            ImGui::MenuItem(std::to_string(stack[s]).c_str());
                        }
                         ImGui::EndMenu();
                    }
                    ImGui::MenuItem("Inputs", NULL, &showInputs);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")){
                    ImGui::MenuItem("No help >:(");
                    ImGui::MenuItem("About CHIP-8", NULL, &done);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            if (showInputs && ImGui::Begin("Inputs")){
                ImGui::Text("%s",B.to_string().c_str());
                ImGui::Text("%lX",B._Find_first());
                ImGui::End();
            }
            ImGui::Render();

            //SDL things
            SDL_SetRenderDrawColor(renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)255);
            SDL_RenderClear(renderer);
            render(renderer);
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(renderer);
            //FPS
            FPSFrames++;
            if (FPSLastTime < SDL_GetTicks() - 1000){
                FPSLastTime = SDL_GetTicks();
                FPSCurrent = FPSFrames;
                FPSFrames = 0;
            }
        }
        /////END OF RENDER/////
        if (hasToOpen){
            hasToOpen = false;
            const char *const patterns[1] = {"*.ch8"};
            const char *fileToOpen = tinyfd_openFileDialog("Open a CHIP-8 ROM...", "", 1, patterns, "CHIP-8 ROM", 0);
            if (!OpenFile(fileToOpen)){
                fileLoaded = true;
                std::string windowTitle("CHIP-8 - ");
                windowTitle+=fileToOpen;
                SDL_SetWindowTitle(window, windowTitle.c_str());
            } else {
                SDL_SetWindowTitle(window, "CHIP-8");
                fileLoaded = false;
            }
        }
        
        if (fileLoaded && (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - IPSLastRefreshAt).count()) >= 1000000/IPSTarget){
            IPSLastRefreshAt = std::chrono::high_resolution_clock::now();
            FDE();
            IPSInstructions++;
            if (IPSLastTime < SDL_GetTicks() - 1000){
                IPSLastTime = SDL_GetTicks();
                IPSCurrent = IPSInstructions;
                IPSInstructions=0;
            }
        }


        //LPS
        LPSLoops++;
        if (LPSLastTime < SDL_GetTicks() - 1000){
            LPSLastTime = SDL_GetTicks();
            LPSCurrent = LPSLoops;
            LPSLoops = 0;
        }
    }
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}