#include "GameWindow.h"
#include <SDL.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

//#include <SDL_image.h>
//#include "Collidable.h"
using namespace std;

GameWindow::GameWindow(int screenWidth, int screenHeight){
    SDL_Init(SDL_INIT_VIDEO);
    m_gameWindow = SDL_CreateWindow("a window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000,SDL_WINDOW_OPENGL);
}

GameWindow::~GameWindow(){
    SDL_FreeSurface(this->GetWinSurface());
    m_gameWindow = NULL;
    SDL_DestroyWindow(m_gameWindow);
    m_gameWindow = NULL;
    SDL_Quit();
}
void GameWindow::RunGame(){
    bool quit = false;
    SDL_Event e;
    while(!quit){
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }
    }
}

SDL_Surface* GameWindow::GetWinSurface(){
    return SDL_GetWindowSurface(m_gameWindow);
}

SDL_Window* GameWindow::GetWindow(){
    return m_gameWindow;
}

void GameWindow::BlitToWindow(SDL_Surface* sourceSurface, const SDL_Rect* srcRect, SDL_Surface* destSurface, SDL_Rect* destRect){
    SDL_BlitSurface(sourceSurface, srcRect, destSurface, destRect);
}

int main(int argc, char* argv[]){
    //double rate = atof(argv[1]);
    printf("Test");
    GameWindow window = GameWindow(1000,1000);
    SDL_Renderer* gRender = SDL_CreateRenderer(window.GetWindow(), -1, SDL_RENDERER_ACCELERATED);
    
    SDL_Texture* newTex = NULL;
    SDL_Surface* windowSurface = SDL_GetWindowSurface(window.GetWindow());
    SDL_Surface* sourceSurface = SDL_CreateRGBSurface(0, 1000, 1000, 32,0,0,0,0);
     SDL_SetRenderDrawColor( gRender, 0x00, 0x00, 0x00, 0xFF );
    
    SDL_Surface* img = SDL_LoadBMP("TestImage.bmp");
    newTex = SDL_CreateTextureFromSurface(gRender, img);
    SDL_RenderClear(gRender);
    SDL_RenderCopy(gRender, newTex, NULL, NULL);
    SDL_RenderPresent(gRender);
    /*SDL_Point *p;
    p->x=500;
    p->y=500;*/
    float angle = 0;
    float interval = .45;
    bool quit = false;
    SDL_Event e;
    while(!quit){
             while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
		    else if( e.type == SDL_KEYDOWN){
		      switch(e.key.keysym.sym)
			{
			case SDLK_LEFT:
			  interval -= .05;
			  break;
			case SDLK_RIGHT:
			  interval += .01;
			  break;
			case SDLK_UP:
			  std::cout << interval << std::endl;
		        default:
			  break;
			}
		    }
		    }
      SDL_RenderClear(gRender);
      SDL_RenderCopyEx(gRender, newTex, NULL, NULL, angle,NULL, SDL_FLIP_NONE);
      SDL_RenderPresent(gRender);
      //      SDL_DestroyTexture(newTex);
      SDL_Delay(1);
      angle += interval;

    }
    /*
    SDL_BlitSurface(img, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(window.GetWindow());*/

   
    
    return 0;
}
