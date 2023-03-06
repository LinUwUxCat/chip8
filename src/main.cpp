#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "graphics.hpp"
#include "logic.hpp"
#include <stdio.h>
#include <string>
#include <future> 

/**
 * @brief Open and loads file into memory.
 * 
 * @param filename Name of the file to open
 * @return int 0 if success, 1 if file open error, and 2 if file too big.
 */
int OpenFile(const char* filename){
    FILE* f = fopen(filename, "r");
    if (f == NULL) return 1;
    int c;
    uint16_t p = 0x200;
    c = fgetc(f);
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
    ///////////VARIABLES////////////
    ////////////////////////////////
    bool done = false;
    bool hasToOpen = false;
    int SecondAdjustment = 16;
    LPSLastTime = SDL_GetTicks();
    FPSLastTime = SDL_GetTicks();
    LastRefreshAt = SDL_GetTicks();

    ////////////////////////////////
    ///////////MAIN LOOP////////////
    ////////////////////////////////
    while (!done){

        //////////////////////
        ////////RENDER////////
        //////////////////////
        if (SDL_GetTicks() - LastRefreshAt >= SecondAdjustment){
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
                                B1=true;break;
                            case SDL_SCANCODE_2:
                                B2=true;break;
                            case SDL_SCANCODE_3:
                                B3=true;break;
                            case SDL_SCANCODE_4:
                                BC=true;break;
                            case SDL_SCANCODE_Q:
                                B4=true;break;
                            case SDL_SCANCODE_W:
                                B5=true;break;
                            case SDL_SCANCODE_E:
                                B6=true;break;
                            case SDL_SCANCODE_R:
                                BD=true;break;
                            case SDL_SCANCODE_A:
                                B7=true;break;
                            case SDL_SCANCODE_S:
                                B8=true;break;
                            case SDL_SCANCODE_D:
                                B9=true;break;
                            case SDL_SCANCODE_F:
                                BE=true;break;
                            case SDL_SCANCODE_Z:
                                BA=true;break;
                            case SDL_SCANCODE_X:
                                B0=true;break;
                            case SDL_SCANCODE_C:
                                BB=true;break;
                            case SDL_SCANCODE_V:
                                BF=true;break;
                        }
                    case SDL_KEYUP:
                        switch (event.key.keysym.scancode){
                            case SDL_SCANCODE_1:
                                B1=false;break;
                            case SDL_SCANCODE_2:
                                B2=false;break;
                            case SDL_SCANCODE_3:
                                B3=false;break;
                            case SDL_SCANCODE_4:
                                BC=false;break;
                            case SDL_SCANCODE_Q:
                                B4=false;break;
                            case SDL_SCANCODE_W:
                                B5=false;break;
                            case SDL_SCANCODE_E:
                                B6=false;break;
                            case SDL_SCANCODE_R:
                                BD=false;break;
                            case SDL_SCANCODE_A:
                                B7=false;break;
                            case SDL_SCANCODE_S:
                                B8=false;break;
                            case SDL_SCANCODE_D:
                                B9=false;break;
                            case SDL_SCANCODE_F:
                                BE=false;break;
                            case SDL_SCANCODE_Z:
                                BA=false;break;
                            case SDL_SCANCODE_X:
                                B0=false;break;
                            case SDL_SCANCODE_C:
                                BB=false;break;
                            case SDL_SCANCODE_V:
                                BF=false;break;
                        }
                }
            }
            SecondAdjustment = SecondAdjustment==16?17:16;
            LastRefreshAt = SDL_GetTicks();
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
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help")){
                    ImGui::MenuItem("No help >:(", NULL, false);
                    ImGui::MenuItem("About CHIP-8", NULL, &done);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
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
            OpenFile("IBM.ch8");
            fileLoaded = true;
        }
        
        if (fileLoaded){
            FDE();
        }


        //LPS
        LPSFrames++;
        if (LPSLastTime < SDL_GetTicks() - 1000){
            LPSLastTime = SDL_GetTicks();
            LPSCurrent = LPSFrames;
            LPSFrames = 0;
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