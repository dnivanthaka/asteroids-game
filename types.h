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
    PLAYER_EXPLOSION,
    MENU_APPEAR,
    MENU_TRAVERSE
};

enum Sprite{
    PLAYER = 0,
    ENEMY1,
    ENEMY2,
    NUMBERS
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
    RESTART,
    GAMEOVER
} gamestate_t;

typedef struct menustate_t{

} menustate_t;

typedef enum sevent_t{
    //TODO Add joystick events
    KEYUP = 0,
    KEYDOWN,
    QUIT
} sevent_t;

typedef struct event_t{
    sevent_t event;
    uint8_t data1, data2;
} event_t;
#endif
