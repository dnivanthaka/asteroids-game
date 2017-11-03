#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "SDL2/SDL.h"


void draw_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2);
void draw_circle(SDL_Renderer *renderer, int x, int y, int radius);
#endif
