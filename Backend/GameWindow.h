#ifndef _GAMEWINDOW_
#define _GAMEWINDOW_

#include <SDL.h>

class GameWindow{
    public:
        GameWindow(int screenWidth, int screenHeight);
        ~GameWindow();
        void RunGame();
        void BlitToWindow(SDL_Surface* sourceSurface, const SDL_Rect* srcRect, SDL_Surface* destSurface, SDL_Rect* destRect);
        SDL_Window* GetWindow();
        SDL_Surface* GetWinSurface();
    private:
        SDL_Window* m_gameWindow;
        int m_screenWidth = 640;
        int m_screenHeight = 480;
};

#endif