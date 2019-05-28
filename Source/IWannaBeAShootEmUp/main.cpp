#include "Game.h"
#include "windows.h"
#include <iostream>
#include <time.h>

const int FPS = 60;
const int DELAY_TIME = 1000.0f / FPS;
//test
int main(int argc, char **argv)
{
	AllocConsole(); // allow a console printout
	FILE* myFile;
	freopen_s(&myFile, "CON", "w", stdout);

	srand(time(NULL)); // Seed random.

    Uint32 frameStart, frameTime;
    
    std::cout << "game init attempt...\n";

    if(TheGame::Instance()->init("I Wanna Be A Shoot'Em Up", 100, 100, 1280, 960, true))
    {
        std::cout << "game init success!\n";

        while(TheGame::Instance()->running())
        {
            frameStart = SDL_GetTicks();
            
            TheGame::Instance()->handleEvents();
            TheGame::Instance()->update();
            TheGame::Instance()->render();
            
            frameTime = SDL_GetTicks() - frameStart;
            
			// delay execution if we haven't spent enough time, used to maintain update and frame rates
            if(frameTime < DELAY_TIME)
            {
                SDL_Delay((int)(DELAY_TIME - frameTime));
            }
        }
    }
    else
    {
        std::cout << "game init failure - " << SDL_GetError() << "\n";
        return -1;
    }
    
    std::cout << "game closing...\n";
    TheGame::Instance()->clean();
    
    return 0;
}
