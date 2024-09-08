#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "graphics.hpp"
#include "logic.hpp"
#include <stdio.h>
#include <string>
#include <future> 
#include <libconfig.h>
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
        64*scale, 
        32*scale + 19, //Imgui bar height 
        SDL_WINDOW_HIGH_PIXEL_DENSITY
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
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
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    ////////////////////////////////
    ///////////IMPORTANT////////////
    ////////////////////////////////
    memcpy(mem, font, 80); //Copy the font to memory.


    ////////////////////////////////
    ///////////VARIABLES////////////
    ////////////////////////////////

    bool done = false;
    bool showDebugMenu = false;
    bool hasToOpen = false;
    const char * fileToOpen = "";
    int newScale = scale;
    bool reloadFile=false;
    bool showInputs = false;
    bool showSettingsWindow=false;
    int SecondAdjustment = 16;
    float onColor[3]={on_color.r/255, on_color.g/255, on_color.b/255};
    float offColor[3]={off_color.r/255, off_color.g/255, off_color.b/255};
    bool showFps;
    bool showIps;
    LPSLastTime = SDL_GetTicks();
    FPSLastTime = SDL_GetTicks();
    FPSLastRefreshAt = SDL_GetTicks();
    IPSLastRefreshAt = std::chrono::high_resolution_clock::now(); //Very high value

    ////////////////////////////////
    //////////CONFIG FILE///////////
    ////////////////////////////////
    const char* configFile = "chip8.conf";
    config_setting_t *root, *setting;
    config_t readConfig;
    config_init(&readConfig);
    
    if (!config_read_file(&readConfig, configFile)){
        preset=0;
        chip8123=false;
        super8=false;
        superB=false;
        superF=true;
        scale=10;
        on_color = {0xFF, 0xFF, 0xFF, 0xFF};
        off_color = {0x00, 0x00, 0x00, 0xFF};
        IPSTarget=700;
        showFps=false;
        showIps=false;
        showDebugMenu=false;   
    } else {
        config_lookup_int(&readConfig, "preset", &preset);
        config_lookup_bool(&readConfig, "chip8123", (int*)&chip8123);
        config_lookup_bool(&readConfig, "super8", (int*)&super8);
        config_lookup_bool(&readConfig, "superB", (int*)&superB);
        config_lookup_bool(&readConfig, "superF", (int*)&superF);
        config_lookup_int(&readConfig, "scale", &newScale);
        long long rgb_on;
        long long rgb_off;
        if(config_lookup_int64(&readConfig, "on_color", &rgb_on)){
            onColor[0]=((rgb_on&0xFF0000)>>16)/255.0f;
            onColor[1]=((rgb_on&0x00FF00)>>8)/255.0f;
            onColor[2]=(rgb_on&0x0000FF)/255.0f;
        }
        printf("%f %f %f\n", onColor[0], onColor[1], onColor[2]);
        if(config_lookup_int64(&readConfig, "off_color", &rgb_off)){
            offColor[0]=((rgb_off&0xFF0000)>>16)/255.0f;
            offColor[1]=((rgb_off&0x00FF00)>>8)/255.0f;
            offColor[2]=(rgb_off&0x0000FF)/255.0f;
        }
        config_lookup_int(&readConfig, "IPSTarget", (int*)&IPSTarget);
        config_lookup_bool(&readConfig, "showFPS", (int*)&showFps);
        config_lookup_bool(&readConfig, "showIPS", (int*)&showIps);
        config_lookup_bool(&readConfig, "showDebugMenu", (int*)&showDebugMenu);
    }
    root=config_root_setting(&readConfig);


    ////////////////////////////////
    ///////////MAIN LOOP////////////
    ////////////////////////////////
    while (!done){

        //////////////////////
        ////////RENDER////////
        //////////////////////
        if (SDL_GetTicks() - FPSLastRefreshAt >= SecondAdjustment){
            /**Graphic settings**/
            if (newScale != scale){
                scale = newScale;
                SDL_SetWindowSize(window, 64*scale, 32*scale + 19);
            }
            on_color.r=onColor[0]*0xFF;
            on_color.g=onColor[1]*0xFF;
            on_color.b=onColor[2]*0xFF;

            off_color.r=offColor[0]*0xFF;
            off_color.g=offColor[1]*0xFF;
            off_color.b=offColor[2]*0xFF;
            //EVENT
            SDL_Event event;
            while (SDL_PollEvent(&event)){
                ImGui_ImplSDL3_ProcessEvent(&event);
                switch (event.type){
                    /****************
                     ****QUITTING****
                     ****************/
                    case SDL_EVENT_QUIT:
                        done=true;
                        break;
                    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                        if (event.window.windowID == SDL_GetWindowID(window)){
                            done=true;
                        }
                        break;
                    /******************
                     **INPUT HANDLING**
                     ******************/
                    //Send help.
                    case SDL_EVENT_KEY_DOWN:
                        switch (event.key.scancode){
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
                    case SDL_EVENT_KEY_UP:
                        switch (event.key.scancode){
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
            ImGui_ImplSDLRenderer3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();

            if(ImGui::BeginMainMenuBar()){
                if (ImGui::BeginMenu("File")){
                    ImGui::MenuItem("Open file", NULL, &hasToOpen);
                    ImGui::MenuItem("Reload current file",NULL,&reloadFile);
                    ImGui::MenuItem("Quit", NULL, &done);
                    ImGui::EndMenu();
                }
                if (showDebugMenu && ImGui::BeginMenu("Debug")){
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
                if (ImGui::BeginMenu("Settings")){
                    ImGui::MenuItem("CHIP-8 settings", NULL, &showSettingsWindow);
                    ImGui::MenuItem("About CHIP-8", NULL, &done, false);
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
            if (showInputs && ImGui::Begin("Inputs", &showInputs, ImGuiWindowFlags_NoCollapse)){ //TODO better view
                ImGui::Text("%s",B.to_string().c_str());
                ImGui::Text("%lX",B._Find_first());
                ImGui::End();
            }

            if (showSettingsWindow && ImGui::Begin("Settings", &showSettingsWindow, ImGuiWindowFlags_NoCollapse)){
                ImGui::BeginTabBar("Tabs");
                if(ImGui::BeginTabItem("Interpreter Settings")){
                    ImGui::Combo("Instruction Preset", &preset, presetStrings, 4);
                    ImGui::BeginDisabled(preset);
                    ImGui::Checkbox("Should 0x8XY[123] reset VF to 0", &chip8123);
                    ImGui::Checkbox("Should 0x8XY6 and 0x8XYE shift in place", &super8);
                    ImGui::Checkbox("Should 0xBXNN jump to XNN + VX", &superB);
                    ImGui::Checkbox("Should I stay the same when storing or loading memory", &superF);
                    ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if(ImGui::BeginTabItem("Personnalization")){
                    ImGui::SliderInt("Scale", &newScale, 1, 100);
                    ImGui::ColorEdit3("ON pixel color", onColor);
                    ImGui::ColorEdit3("OFF pixel color", offColor);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Performance")){
                    ImGui::SliderInt("Target IPS", (int*)&IPSTarget,70, 7000, "%d instructions per second");
                    ImGui::Checkbox("Show Ips", &showIps);
                    ImGui::Checkbox("Show FPS", &showFps);
                    ImGui::Checkbox("Show Inputs", &showInputs);
                    ImGui::Checkbox("Enable debug menu", &showDebugMenu);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
                ImGui::End();
            }

            if (showFps || showIps){
                ImGui::SetNextWindowSize(ImVec2(39, 23*showFps + 23*showIps));
                if (ImGui::Begin("info", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav)){
                    if (showFps)ImGui::Text("%d", FPSCurrent);
                    if (showIps)ImGui::Text("%d", IPSCurrent);
                    ImGui::End();
                }
            }

            ImGui::Render();

            //SDL things
            SDL_SetRenderDrawColor(renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)255);
            SDL_RenderClear(renderer);
            render(renderer);
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
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
            fileToOpen = tinyfd_openFileDialog("Open a CHIP-8 ROM...", "", 1, patterns, "CHIP-8 ROM", 0);
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

        if (reloadFile){
            reloadFile=false;
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


    /////////////////////////////////
    //////////CONFIG SAVE////////////
    /////////////////////////////////
    setting=config_setting_get_member(root,"preset");if(setting==NULL)setting=config_setting_add(root, "preset", CONFIG_TYPE_INT);config_setting_set_int(setting, preset);
    setting=config_setting_get_member(root,"chip8123");if(setting==NULL)setting=config_setting_add(root, "chip8123", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, chip8123);
    setting=config_setting_get_member(root,"super8");if(setting==NULL)setting=config_setting_add(root, "super8", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, super8);
    setting=config_setting_get_member(root,"superB");if(setting==NULL)setting=config_setting_add(root, "superB", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, superB);
    setting=config_setting_get_member(root,"superF");if(setting==NULL)setting=config_setting_add(root, "superF", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, superF);
    setting=config_setting_get_member(root,"scale");if(setting==NULL)setting=config_setting_add(root, "scale", CONFIG_TYPE_INT);config_setting_set_int(setting, scale);
    setting=config_setting_get_member(root,"on_color");if(setting==NULL)setting=config_setting_add(root, "on_color", CONFIG_TYPE_INT64);config_setting_set_int64(setting, (((long long)on_color.r)<<16) + (((long long)on_color.g)<<8) + ((long long)on_color.b));
    setting=config_setting_get_member(root,"off_color");if(setting==NULL)setting=config_setting_add(root, "off_color", CONFIG_TYPE_INT64);config_setting_set_int64(setting, (((long long)off_color.r)<<16) + (((long long)off_color.g)<<8) + ((long long)off_color.b));
    setting=config_setting_get_member(root,"IPSTarget");if(setting==NULL)setting=config_setting_add(root, "IPSTarget", CONFIG_TYPE_INT);config_setting_set_int(setting, IPSTarget);
    setting=config_setting_get_member(root,"showFps");if(setting==NULL)setting=config_setting_add(root, "showFps", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, showFps);
    setting=config_setting_get_member(root,"showIps");if(setting==NULL)setting=config_setting_add(root, "showIps", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, showIps);
    setting=config_setting_get_member(root,"showDebugMenu");if(setting==NULL)setting=config_setting_add(root, "showDebugMenu", CONFIG_TYPE_BOOL);config_setting_set_bool(setting, showDebugMenu);

    if(!config_write_file(&readConfig, "chip8.conf")){
        printf("Error writing config file! Settings will not be saved.\n");
    }

    config_destroy(&readConfig);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}