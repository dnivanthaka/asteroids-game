#include "SDL2/SDL.h"

#include "fonts.h"


void put_char(SDL_Renderer *renderer, SDL_Color *color, uint8_t code, int x, int y)
{
    //Color
    SDL_SetRenderDrawColor( renderer, color->r, color->g, color->b, color->a );

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(fonts[i][j]){
                SDL_RenderDrawPoint(renderer, x, y); 
            }
        }
    }
}
