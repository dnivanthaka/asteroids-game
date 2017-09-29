#ifndef TYPES_H
#define TYPES_H
struct player_t{
    int x, y;
    int w, h;
    int vel_x, vel_y;
    int acc_x, acc_y;
    int hit_count;
    SDL_Texture *tPlayer;
    SDL_Texture *back;
};

typedef struct bullet_t{
    int x, y;
    int w, h;
    int vel_x, vel_y;
    int acc_x, acc_y;
    bool isVisible;
    bool isPlayerFired;
    SDL_Texture *tBullet;
    SDL_Texture *back;
} bullet_t;

typedef struct cosmic_t{
    int x, y;
    int w, h;
    int vel_x, vel_y;
    int acc_x, acc_y;
    bool isVisible;
    bool isPlayerFired;
    SDL_Texture *tCosmic;
    SDL_Texture *back;
} cosmic_t;

enum SoundEvent{
    PLAYER_FIRE = 0,
    ENEMY_FIRE,
    ENEMY_EXPLOSION,
    PLAYER_EXPLOSION
};

enum Sprite{
    PLAYER = 0,
    ENEMY1,
    ENENY2
};

enum HeadingDirection{
    NORTH = 0,
    SOUTH
};

typedef struct enemy_t{
    int x, y;
    int w, h;
    int vel_x, vel_y;
    int acc_x, acc_y;
    int hit_count;
    int fire_delay;
    uint32_t delay_ticks;
    bool isVisible;
    HeadingDirection dir;
    SDL_Texture *tEnemy;
    SDL_Texture *back;
} enemy_t;

typedef enum{
    SPLASH = 0,
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
} gamestate_t;
#endif
