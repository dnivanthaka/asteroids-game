#include "SDL2/SDL.h"

#include "primitives.h"

#define ABS(x) (x < 0)? -1 * x : x
#define SGN(x) (x < 0)? -1: 1

void draw_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int absdx = ABS(dx);
    int absdy = ABS(dy);
    int sdx = SGN(dx);
    int sdy = SGN(dy);
    int x = absdx >> 1;
    int y = absdy >> 1;
    int px = x1;
    int py = y1;

    SDL_RenderDrawPoint(renderer, px, py);

    if(absdx >= absdy){
        //Line is more horizontal
        for(int i=0;i<absdx;i++){
            y += absdy;
            if(y >= absdx){
                y -= absdx;
                py += sdy;
            }
            px += sdx;
        
            SDL_RenderDrawPoint(renderer, px, py);
        }
        
    }else{
        //Line is more vertical
        for(int i=0;i<absdy;i++){
            x += absdx;
            if(x >= absdy){
                x -= absdy;
                px += sdx;
            }
            py += sdy;
        
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
   
}

void draw_circle(SDL_Renderer *renderer, int x, int y, int radius)
{
    int x1 = 0, y1 = radius, p = 3 - 2 * radius;

    while(y1 >= x1){
        SDL_RenderDrawPoint(renderer, x - x1, y - y1);
        SDL_RenderDrawPoint(renderer, x - y1, y - x1);
        SDL_RenderDrawPoint(renderer, x + y1, y - x1);
        SDL_RenderDrawPoint(renderer, x + x1, y - y1);
        SDL_RenderDrawPoint(renderer, x - x1, y + y1);
        SDL_RenderDrawPoint(renderer, x - y1, y + x1);
        SDL_RenderDrawPoint(renderer, x + y1, y + x1);
        SDL_RenderDrawPoint(renderer, x + x1, y + y1);

        if(p < 0) p += 4*x1++ + 6;
        else p+= 4 * (x1++ - y1--) + 10;
    }
}
