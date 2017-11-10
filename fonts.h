#ifndef FONTS_H
#define FONTS_H
uint8_t fonts[][] = {
    {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}
};

void put_char(SDL_Renderer *renderer, SDL_Color *color, uint8_t code, int x, int y);
#endif
