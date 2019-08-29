#include <iostream>
#include "CHIP_8.h"
#include <chrono>
#include <SDL2/SDL.h>



CHIP_8 myChip;
void drawGraphics(SDL_Renderer * renderer);
void getInput(SDL_Event * e);

int scale = 10;
bool quit = false;
int x = 0;


int main(int argc, char *argv[])
{

    SDL_Event e;
    SDL_Window * window;
    SDL_Renderer * renderer;
    window = SDL_CreateWindow(
        "CHIP-8 EMU",                       // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        64*scale,                               // width, in pixels
        32*scale,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
    renderer = SDL_CreateRenderer(window,
                                     -1,
                                     SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );
    SDL_RenderPresent(renderer);


    myChip.init();
    myChip.loadROM("C:\\Users\\Curtiss-PC\\Documents\\CHIP-8\\CHIP-8-Emulator\\src\\CHIP_8_Emulator\\ROM\\maze.ch8");
    //myChip.printMemory();
    // create a scene




    //Set some rate variables
    int clockHz = 3;
    int timerHz = 60;
    unsigned long long startTime = std::chrono::duration_cast<std::chrono::nanoseconds>
            (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    double clockTime = 1000000000 / clockHz;
    double timerTick = 1000000000 / timerHz;
    double deltaC = 0, deltaT = 0;

    //Main loop
    while(!quit) {
        unsigned long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        deltaC += (currentTime - startTime) / clockTime;
        deltaT += (currentTime - startTime) / timerTick;
        startTime = currentTime;

        //Keep execution rate to this
        if(deltaC >= 1){
            getInput(&e);
            myChip.emulateCycle();
            deltaC--;
        }
        if(deltaT >= 1){
            //Decrement timers here;
            //clr screen


            deltaT--;
        }
        if(myChip.drawFlag){
           drawGraphics(renderer);
           myChip.drawFlag = false;
        }


    }
    return 0;
}


void getInput(SDL_Event * e){
    while( SDL_PollEvent( e ) != 0 )
     {
         //User requests quit
         if( (*e).type == SDL_QUIT )
         {
             quit = true;
         }
         else if ((*e).type == SDL_KEYDOWN) {
             std::cout << "KEYPRESSED" << std::endl;
             switch((*e).key.keysym.sym) {

                     case SDLK_1: myChip.setKeypad(0x1, 1); break;
                     case SDLK_2: myChip.setKeypad(0x2, 1); break;
                     case SDLK_3: myChip.setKeypad(0x3, 1); break;
                     case SDLK_4: myChip.setKeypad(0xC, 1); break;
                     case SDLK_q: myChip.setKeypad(0x4, 1); break;
                     case SDLK_w: myChip.setKeypad(0x5, 1); break;
                     case SDLK_e: myChip.setKeypad(0x6, 1); break;
                     case SDLK_r: myChip.setKeypad(0xD, 1); break;
                     case SDLK_a: myChip.setKeypad(0x7, 1); break;
                     case SDLK_s: myChip.setKeypad(0x8, 1); break;
                     case SDLK_d: myChip.setKeypad(0x9, 1); break;
                     case SDLK_f: myChip.setKeypad(0xE, 1); break;
                     case SDLK_z: myChip.setKeypad(0xA, 1); break;
                     case SDLK_x: myChip.setKeypad(0x0, 1); break;
                     case SDLK_c: myChip.setKeypad(0xB, 1); break;
                     case SDLK_v: myChip.setKeypad(0xF, 1); break;
             }
         } else if ((*e).type == SDL_KEYUP) {
             std::cout << "KEY RELEASED" << std::endl;
             switch((*e).key.keysym.sym) {

                     case SDLK_1: myChip.setKeypad(0x1, 1); break;
                     case SDLK_2: myChip.setKeypad(0x2, 1); break;
                     case SDLK_3: myChip.setKeypad(0x3, 1); break;
                     case SDLK_4: myChip.setKeypad(0xC, 1); break;
                     case SDLK_q: myChip.setKeypad(0x4, 1); break;
                     case SDLK_w: myChip.setKeypad(0x5, 1); break;
                     case SDLK_e: myChip.setKeypad(0x6, 1); break;
                     case SDLK_r: myChip.setKeypad(0xD, 1); break;
                     case SDLK_a: myChip.setKeypad(0x7, 1); break;
                     case SDLK_s: myChip.setKeypad(0x8, 1); break;
                     case SDLK_d: myChip.setKeypad(0x9, 1); break;
                     case SDLK_f: myChip.setKeypad(0xE, 1); break;
                     case SDLK_z: myChip.setKeypad(0xA, 1); break;
                     case SDLK_x: myChip.setKeypad(0x0, 1); break;
                     case SDLK_c: myChip.setKeypad(0xB, 1); break;
                     case SDLK_v: myChip.setKeypad(0xF, 1); break;
             }
     }
    }


}
void drawGraphics(SDL_Renderer * renderer){
    std::cout << "RENDERING NOW" << std::endl;
    //myChip.dumpGFX();
    x++;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect pixel;

    unsigned int gfx_val;
    for(int x = 0; x < 64; x++){
        for(int y = 0; y < 32; y++){
            pixel.x = x * scale;
            pixel.y = y * scale;
            pixel.w = scale;
            pixel.h = scale;
            gfx_val = myChip.getGFX_X_Y(x,y);
            if(gfx_val == 1){
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                SDL_RenderFillRect(renderer, &pixel);
            }
            else if(gfx_val == 0){
                SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
                SDL_RenderFillRect(renderer, &pixel);
            }

        }
    }
    SDL_RenderPresent(renderer);
}
