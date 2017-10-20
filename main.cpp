//-----------------------------------------------------------------------------
// Simple Astroids game written using SDL2.0
//
// Author: D.N. Amerasinghe <nivanthaka@gmail.com>
//
// Press left and right arrow keys to move the ship and ctrl key to fire
//-----------------------------------------------------------------------------
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm> //std::find

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

#include "types.h"
#include "events.h"
#include "img_pcx.h"

using namespace std;


//------------------------------ Function Prototypes ---------------------------------------//
void erase_player();
void erase_objects();
void handle_inputs();
void player_logic();
void object_logic();                
void collision_detection();
void transform_objects();
void transform_player();
void draw_objects();
void draw_player();
void update_screen();
void play_media();
void misc_activity();
bool init(const string title, int xpos, int ypos, int width, int height, int flags);
bool load_media();
void cleanup();
void fire_bullet(bool isPlayer, int x, int y, HeadingDirection dir);
void init_enemy(enemy_t *o, int x, int y);
void init_player(int x, int y);
void init_cosmic_object(int x, int y, int vel_x, int vel_y, HeadingDirection dir);
void show_splash();
void show_gameover();
void show_menu();
void init_dashboard();
void print_text(const char *str, uint16_t x, uint16_t y);
void restart_game();
void game_event_handler(event_t *ev);
void menu_event_handler(event_t *ev);
//---------------------------------------------------------------------------------------------//

//------------------------------- Defines ----------------------------------------------------//
#define PLAYER_WIDTH   32
#define PLAYER_HEIGHT  32

#define ENEMY_WIDTH    32
#define ENEMY_HEIGHT   32

#define ENEMY_START_VELX 0
#define ENEMY_START_VELY 2
#define ENEMY_RANGE_OFFSET 8

#define BULLET_WIDTH   2
#define BULLET_HEIGHT  2

#define BULLET_START_VELX  0
#define BULLET_START_VELY  1

#define MAX_ENEMY_COUNT    10
#define MAX_COSMIC_OBJECTS 15 // Max objects in one frame

#define MENU_WIDTH  200
#define MENU_HEIGHT 100
//---------------------------------------------------------------------------------------------//

bool gameIsRunning = true;

//const static int g_ScreenWidth  = 800;
//const static int g_ScreenHeight = 600;
const static int FRAME_RATE = 60;
const static int TICKS_PER_FRAME = 1000 / FRAME_RATE;
static int g_ScreenWidth;
static int g_ScreenHeight;

SDL_Window   *m_pWindow;
SDL_Renderer *m_pRenderer;
SDL_Texture  *m_pDashboard;

//TODO improve the menu
SDL_Texture  *m_pMenu;
SDL_Texture  *m_pGameOver;

struct player_t player;
vector<enemy_t> enemies;

vector<SoundEvent> soundQueue;
vector<bullet_t> g_Bullets;
vector<cosmic_t> g_CosmicObjects;
vector<SDL_Texture *> g_Images;
vector<menustate_t> g_MenuStack;
vector<Mix_Chunk *> g_AudioClips;

vector<int> g_enemyXCoords;

int randMotion[] = {1, 0, 1, -1, 1, 1};

uint16_t playerKills = 0;
uint16_t playerHits  = 0;

gamestate_t g_State = SPLASH;

uint8_t g_MenuSelection = 0;

int main(int argc, char *argv[])
{
        uint32_t capTimer;
        int countedFrames = 0;
        //Initial state as splash

        init("AstroGame",  0,  0,  0,  0,  SDL_WINDOW_FULLSCREEN);

        //player = new struct player_t();

        load_media();

        srand(time(nullptr));


        for(int i=0;i<MAX_ENEMY_COUNT;i++){
            enemy_t tmp;

            init_enemy(&tmp, rand() % (g_ScreenWidth - PLAYER_WIDTH), 0);
            tmp.vel_y = 2;

            enemies.push_back(tmp);
        }

        for(int i=0;i<MAX_COSMIC_OBJECTS;i++){
            init_cosmic_object(rand() % (g_ScreenWidth - 2), rand() % (g_ScreenHeight - 2), 0, 2, SOUTH);
        }

        //fpsTimer = SDL_GetTicks();
        
        init_player((g_ScreenWidth / 2) - (PLAYER_WIDTH / 2), g_ScreenHeight);

        while(gameIsRunning){
        
            capTimer = SDL_GetTicks();

            if(g_State == PLAYING){

                erase_player();
                erase_objects();
                handle_inputs();
                player_logic();
                object_logic();                
                collision_detection();
                transform_objects();
                transform_player();
                draw_objects();
                draw_player();
                update_screen();
                play_media();
                misc_activity();

            }else if(g_State == MENU){
                erase_player();
                erase_objects();
                handle_inputs();
                transform_objects();
                transform_player();
                draw_objects();
                draw_player();
                show_menu();
                update_screen();
                play_media();
            }else if(g_State == GAMEOVER){
                erase_player();
                erase_objects();
                handle_inputs();
                transform_objects();
                transform_player();
                draw_objects();
                draw_player();
                show_gameover();
                update_screen();
                play_media();
            }else if(g_State == SPLASH){
                handle_inputs();
                show_splash();
                update_screen();
                play_media();
            }else if(g_State == RESTART){
                restart_game();

                g_State = PLAYING;
            }

            countedFrames++;

            int frameTicks = SDL_GetTicks() - capTimer;
            if(frameTicks < TICKS_PER_FRAME){
                
                //TODO Write a better frame rate handler, this is not the proper way
                SDL_Delay(TICKS_PER_FRAME - frameTicks);
            }

        }
        cleanup();

        //cout << "You shot down " << playerKills << " enemy ships." << endl;
        exit(EXIT_SUCCESS);

        return 0;
}

void erase_player()
{
    // Cleaning the back buffer, The framebuffer is cleared on each SDL_RenderPresent call
    SDL_RenderClear(m_pRenderer);
}

void erase_objects()
{

}

void restart_game()
{
   g_enemyXCoords.clear();
   g_Bullets.clear();
   soundQueue.clear();

   for(int i=0;i<MAX_ENEMY_COUNT;i++){
       enemy_t *tmp = &enemies[i];

       init_enemy(tmp, rand() % (g_ScreenWidth - PLAYER_WIDTH), 0);
       tmp->vel_y = 2;

   }

   init_player((g_ScreenWidth / 2) - (PLAYER_WIDTH / 2), g_ScreenHeight);
   
   playerKills = 0;
   playerHits  = 0;
}

void show_splash()
{
//TODO Update frames here and change the state to PLAYING
    SDL_Rect src, dest;

    src.w = 300;
    src.h = 100;
    src.x = 0;
    src.y = 0;

    dest.w = 300;
    dest.h = 100;
    dest.x = (g_ScreenWidth / 2) - (dest.w / 2);
    dest.y = (g_ScreenHeight / 2) - (dest.h / 2);
    //SDL_RenderClear(m_pRenderer);
    
    //SDL_RenderClear(m_pRenderer);
    
    SDL_RenderCopy(m_pRenderer, m_pMenu, &src, &dest);

    //With Padding
    print_text("ASTEROIDS GAME", dest.x + (dest.w / 2) - 70, dest.y + 5);
    print_text("(C) D.N. AMERASINGHE", dest.x + (dest.w / 2) - 100, dest.y + 25);
    print_text("PRESS ENTER TO START PLAYING", dest.x + 15, dest.y + (dest.h - 20) );
    
    //print_text("*", dest.x + 5, (dest.y + 5) + (g_MenuSelection * 13));
}

void show_gameover()
{
    SDL_Rect src, dest;

    src.w = 400;
    src.h = 100;
    src.x = 0;
    src.y = 0;

    dest.w = 400;
    dest.h = 100;
    dest.x = (g_ScreenWidth / 2) - (dest.w / 2);
    dest.y = (g_ScreenHeight / 2) - (dest.h / 2);
    //SDL_RenderClear(m_pRenderer);
    
    //SDL_RenderClear(m_pRenderer);
    
    SDL_RenderCopy(m_pRenderer, m_pGameOver, &src, &dest);

    print_text("GAME OVER!!!", dest.x + (dest.w / 2) - 60, dest.y + 10);
    print_text("PRESS 'R' TO RESTART OR 'X' TO QUIT", dest.x + 20, dest.y + (dest.h / 2) + 20);
}

void show_menu()
{
    SDL_Rect src, dest;

    src.w = 300;
    src.h = 100;
    src.x = 0;
    src.y = 0;

    dest.w = 300;
    dest.h = 100;
    dest.x = (g_ScreenWidth / 2) - (dest.w / 2);
    dest.y = (g_ScreenHeight / 2) - (dest.h / 2);
    //SDL_RenderClear(m_pRenderer);
    
    //SDL_RenderClear(m_pRenderer);
    
    SDL_RenderCopy(m_pRenderer, m_pMenu, &src, &dest);

    //With Padding
    print_text("NEW GAME", dest.x + 15, dest.y + 5);
    print_text("CONFIGURE", dest.x + 15, dest.y + 25);
    print_text("EXIT", dest.x + 15, dest.y + 45);
    
    print_text("*", dest.x + 5, (dest.y + 5) + (g_MenuSelection * 20));
}

void game_event_handler(event_t *ev)
{
    if(ev->event == KEYDOWN){
       if(ev->data1 == SDL_SCANCODE_ESCAPE){
           if(g_State == PLAYING){
               //g_State = PAUSED;
               g_State = MENU;
               SoundEvent se = MENU_APPEAR;
               soundQueue.push_back(se);
               //Start with the first item
               g_MenuSelection = 0;
           }else if(g_State == MENU){
               g_State = PLAYING;
           }else if(g_State == GAMEOVER){
               //g_State = MENU;
           }
       }else if(ev->data1 == SDL_SCANCODE_RETURN){
           if(g_State == SPLASH){
               g_State = PLAYING;
           }else if(g_State == PAUSED){
               g_State = PLAYING;
           }else if(g_State == MENU){
               if(g_MenuSelection == 0){
                   g_State = RESTART;
               }else if(g_MenuSelection == 2){
                   //Exit game
                   gameIsRunning = false;
               }
           } 
       }else if(ev->data1 == SDL_SCANCODE_RIGHT){
           if(g_State == PLAYING){
               player.vel_x = 3;
           }
       }else if(ev->data1 == SDL_SCANCODE_LEFT){
           if(g_State == PLAYING){
               player.vel_x = -3;
           }
       }else if(ev->data1 == SDL_SCANCODE_RCTRL || ev->data1 == SDL_SCANCODE_LCTRL){
           if(g_State == PLAYING){
               fire_bullet(true, player.x + (player.w / 2) - (BULLET_WIDTH / 2), player.y - player.h, NORTH);
               //Add firing sound to the sound queue
               SoundEvent se = PLAYER_FIRE;
               soundQueue.push_back(se);
           }
       }else if(ev->data1 == SDL_SCANCODE_UP){
          if(g_State == MENU){
           if(g_MenuSelection > 0){
               g_MenuSelection--;
               SoundEvent se = MENU_TRAVERSE;
               soundQueue.push_back(se);
           } 
          } 
       }else if(ev->data1 == SDL_SCANCODE_DOWN){
          if(g_State == MENU){
           g_MenuSelection++;
           g_MenuSelection %= 3;
           SoundEvent se = MENU_TRAVERSE;
           soundQueue.push_back(se);
          } 
       }else if(ev->data1 == SDL_SCANCODE_R){
           if(g_State == GAMEOVER){
               g_State = RESTART;
           }
       }else if(ev->data1 == SDL_SCANCODE_X){
           if(g_State == GAMEOVER){
               gameIsRunning = false;
           }
       }else{
           //If any other key play if the game is paused
           if(g_State == PAUSED){
               g_State = PLAYING;
           } 
       }
    }else if(ev->event == KEYUP){
       if(ev->data1 == SDL_SCANCODE_RIGHT || ev->data1 == SDL_SCANCODE_LEFT){
           player.vel_y = 0;
           player.vel_x = 0;
       }
    }
}

void menu_event_handler(event_t *ev)
{

}

//TODO improve event handling
void handle_inputs()
{
    SDL_Event event;
    event_t e;

    if(SDL_PollEvent(&event)){
       switch(event.type){
        case SDL_QUIT:
            gameIsRunning = true;

            e.event = QUIT;
            event_push(&e);

            break;

        case SDL_KEYDOWN:
            e.event = KEYDOWN;
            e.data1 = event.key.keysym.scancode;
            event_push(&e);
            
        break;

        case SDL_KEYUP:
            e.event = KEYUP;
            e.data1 = event.key.keysym.scancode;
            event_push(&e);
        break;

        default:
            break;

       } 
    }

    event_t *ev;

    while((ev = event_pop()) != nullptr){
         //Check for game state

         game_event_handler(ev);
    }
}

void init_player(int x, int y)
{
    player.w = PLAYER_WIDTH;
    player.h = PLAYER_HEIGHT;
    player.x = x;
    player.y = y;
    player.vel_x = 0;
    player.vel_y = 0;
    player.acc_x = 0;
    player.acc_y = 0;
    //Max hit count
    player.hit_count = 4;
    player.tPlayer = g_Images[PLAYER];
}

void init_enemy(enemy_t *o, int x, int y)
{
    int nx;

    o->w = ENEMY_WIDTH;
    o->h = ENEMY_HEIGHT;
    o->y = y;
    o->vel_x = 0;
    o->vel_y = 0;
    o->acc_x = 0;
    o->acc_y = 0;
    o->fire_delay = 0;
    o->hit_count  = 2;
    o->tEnemy = g_Images[ENEMY1];

    vector<int>::iterator it;
    int tmp = 0;
    
    //Unique X coord for enemy ships
    do{
        nx = rand() % (g_ScreenWidth - PLAYER_WIDTH);
        it = find(g_enemyXCoords.begin(), g_enemyXCoords.end(), nx);

    }while(it != g_enemyXCoords.end());

    o->x = nx;

    tmp = rand() % (1000 + 1); 
    o->delay_ticks = tmp;
    o->isVisible = false;

    g_enemyXCoords.push_back(tmp);
}

void init_dashboard()
{
    //Init scoreboard, ammo counts etc.
}


bool init(const string title, int xpos, int ypos, int width, int height, int flags)
{
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0){
                m_pWindow = SDL_CreateWindow(title.c_str(), xpos, ypos, width, height, flags);

        if(m_pWindow == NULL){
                std::cerr << "SDL Error: Failed to create window: " << SDL_GetError() << std::endl;
                SDL_Quit();
                return false;
        }

        m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

        if(m_pRenderer == NULL){
                std::cerr << "SDL Error: Failed to create renderer: " << SDL_GetError() << std::endl;
                SDL_DestroyWindow(m_pWindow);
                SDL_Quit();
                return false;
        }
    }else{
        std::cerr << "SDL Error: Failed to initialize: " << SDL_GetError() << std::endl;
        return false;
    }

    //Hide the mouse cursor
    SDL_ShowCursor(SDL_DISABLE);

    SDL_GetWindowSize(m_pWindow, &g_ScreenWidth, &g_ScreenHeight);


    //Initialize the Audio mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        std::cerr << "SDL Error: Failed to initialize: " << SDL_GetError() << std::endl;
        return false;
    }


    //init_player((g_ScreenWidth / 2) - (player.w / 2), g_ScreenHeight);
    SDL_Surface *s;

    s = SDL_CreateRGBSurface(0, 200, 100, 32, 0, 0, 0, 0);
    SDL_FillRect(s, nullptr, SDL_MapRGB(s->format, 255, 0, 0));
    m_pGameOver = SDL_CreateTextureFromSurface(m_pRenderer, s);

    SDL_FreeSurface(s);

    s = SDL_CreateRGBSurface(0, 300, 100, 32, 0, 0, 0, 0);
    SDL_FillRect(s, nullptr, SDL_MapRGB(s->format, 0, 0, 255));
    m_pMenu = SDL_CreateTextureFromSurface(m_pRenderer, s);

    SDL_FreeSurface(s);


    return true;
}

bool load_media()
{

   vector<string> sprites = {"shipsc2.pcx", "shipsc3.pcx", "shipsc3.pcx", "fonts.pcx"};

   for(unsigned int i=0;i<sprites.size();i++){
    SDL_Surface *tmp;
    tmp = IMG_Load(sprites[i].c_str());

    if(tmp == nullptr){
        cerr << "Cannot load image :" << SDL_GetError() << endl;
        return false;
    }
    SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 0xFF, 0, 0xFF));

    SDL_Texture *s= SDL_CreateTextureFromSurface(m_pRenderer, tmp);
    g_Images.push_back(s);
    
    SDL_FreeSurface(tmp);
   }


   //Reading sound mixes
   vector<string> mixes = {"fire.wav", "explosion.wav", "panel.wav", "switch.wav"};

   for(unsigned int i=0;i<mixes.size();i++){
    Mix_Chunk *tmp = Mix_LoadWAV(mixes[i].c_str());
    g_AudioClips.push_back(tmp);
   }

   IMG_PCX *image1 = new IMG_PCX("shipsc2.pcx");
   image1->read();

   return true;
}

void init_cosmic_object(int x, int y, int vel_x, int vel_y, HeadingDirection dir)
{
    uint8_t wh = (rand() % 3) + 1;

    cosmic_t object;
    object.w = wh;  //Equal random width and height
    object.h = wh;
    object.x = x;
    object.y = y;
    object.vel_x = vel_x;
    object.vel_y = vel_y;
    object.isVisible = true;
    
    if(dir == NORTH){
        //Up
        object.vel_y = -4;
        object.acc_x = 0;
        object.acc_y = 0;
    }else{
        //Down
        object.vel_y = (rand() % 4) + 1;
        object.acc_x = 0;
        object.acc_y = 0;
    }

    SDL_Surface* s;

    s = SDL_CreateRGBSurface(0, object.w, object.h, 32, 0, 0, 0, 0);
    SDL_FillRect(s, nullptr, SDL_MapRGB(s->format, (rand() % 255) + 1, (rand() % 255) + 1, (rand() % 255) + 1));
    object.tCosmic = SDL_CreateTextureFromSurface(m_pRenderer, s);


    g_CosmicObjects.push_back(object);
}

void fire_bullet(bool isPlayer, int x, int y, HeadingDirection dir)
{
    bullet_t bullet;
    bullet.w = BULLET_WIDTH;
    bullet.h = BULLET_HEIGHT;
    bullet.x = x;
    bullet.y = y;
    bullet.vel_x = 0;
    bullet.isVisible = true;
    bullet.isPlayerFired = isPlayer;
    
    if(dir == NORTH){
        //Up
        bullet.vel_y = -4;
        bullet.acc_x = 0;
        bullet.acc_y = 0;
    }else{
        //Down
        bullet.vel_y = 4;
        bullet.acc_x = 0;
        bullet.acc_y = 0;
    }

    SDL_Surface* s;

    s = SDL_CreateRGBSurface(0, bullet.w, bullet.h, 32, 0, 0, 0, 0);
    SDL_FillRect(s, nullptr, SDL_MapRGB(s->format, 255, 255, 255));
    bullet.tBullet = SDL_CreateTextureFromSurface(m_pRenderer, s);


    g_Bullets.push_back(bullet);
}

void cleanup()
{
    //delete player;
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);

    SDL_DestroyTexture(m_pDashboard);
    SDL_DestroyTexture(m_pMenu);

    while(!g_AudioClips.empty()){
        Mix_FreeChunk(g_AudioClips.back());
        g_AudioClips.pop_back();
    }
    while(!g_Images.empty()){
        SDL_DestroyTexture(g_Images.back());
        g_Images.pop_back();
    }

    g_enemyXCoords.clear();
    g_Bullets.clear();
    g_CosmicObjects.clear();
    soundQueue.clear();

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void player_logic()
{
    player.x += player.vel_x;
    player.y += player.vel_y;
}

void object_logic()
{

    for(vector<bullet_t>::iterator it = g_Bullets.begin();it != g_Bullets.end(); ++it){

        //Move bullet
        it->y += it->vel_y;
    }
    for(vector<cosmic_t>::iterator it = g_CosmicObjects.begin();it != g_CosmicObjects.end(); ++it){

        //Move 
        if(it->isVisible){
            it->y += it->vel_y;
            it->x += it->vel_x;
        }else{
            it->isVisible = true;
        }
    }
    for(vector<enemy_t>::iterator it = enemies.begin();it != enemies.end(); ++it){

        //Move enemies down 
        if(it->delay_ticks == 0){
            //Occasional wobble
            /*if((rand() % 3) > 1){
                it->vel_x = randMotion[rand() % 6];
                it->delay_ticks = rand() % 100;
            }else{
                it->vel_x = 0;
            }*/
            it->vel_x = rand() % 2;

            if(rand() % 2)
                it->vel_x = -(it->vel_x);


            it->y += it->vel_y;
            it->x += it->vel_x;
            it->isVisible = true;
        }else{
            /*if(it->isVisible){
                // Random motion
                it->x += it->vel_x;
                it->y += it->vel_y;
            }*/

            it->delay_ticks -= 1;
        }

        //Check if player x coord are within range and fire, 8 pixels more from each boundary
        if(player.x + (PLAYER_WIDTH / 2) >= (it->x - ENEMY_RANGE_OFFSET) && player.x + (PLAYER_WIDTH / 2) <= (it->x + PLAYER_WIDTH + ENEMY_RANGE_OFFSET)){
            if(it->fire_delay <= 0 && it->isVisible){
                fire_bullet(false, it->x + (ENEMY_WIDTH / 2), it->y + ENEMY_HEIGHT, SOUTH);
                it->fire_delay = rand() % 200;
            }else{
                it->fire_delay -= 1;
            }
        }
    }
}
                
void collision_detection()
{
    //Check player boundaries
    if(player.x < 0){
        player.x = 0;
        player.vel_x = 0;
    }else if(player.x + player.w > g_ScreenWidth){
        player.x = g_ScreenWidth - player.w;
        player.vel_x = 0;
    }

    //Cheking enemy boundaries
    for(vector<enemy_t>::iterator it = enemies.begin();it != enemies.end(); ++it){

        //it->y += it->vel_y;
        if(it->y > g_ScreenHeight){
            it->y = 0; 
            it->x = rand() % (g_ScreenWidth - ENEMY_WIDTH);
            it->isVisible = false;
            it->delay_ticks = rand() % (1000 + 1);
        }
    }

    //Enemy Hits
    for(vector<enemy_t>::iterator it = enemies.begin();it != enemies.end(); ++it){

        for(vector<bullet_t>::iterator it2 = g_Bullets.begin();it2 != g_Bullets.end(); ++it2){

            if(it2->isPlayerFired && it2->isVisible && it->isVisible){

                if(it2->x >= it->x && it2->x <= (it->x + ENEMY_WIDTH) && it2->y - (BULLET_HEIGHT / 2) <= (it->y + ENEMY_HEIGHT) && it2->y - (BULLET_HEIGHT / 2) >= it->y){
                    //Collision
                    if(it->hit_count > 0){
                        it->hit_count -= 1;
                    }

                    //Hide the bullet, otherwise two hits are registered
                    it2->isVisible = false;

                    if(it->hit_count == 0){
                        it->y = 0; 
                        it->x = rand() % (g_ScreenWidth - PLAYER_WIDTH);
                        it->isVisible = false;
                        it->delay_ticks = rand() % (1000 + 1);
                        it->hit_count = 2;
                        playerKills++;

                        SoundEvent se = ENEMY_EXPLOSION;
                        soundQueue.push_back(se);
                    }

                }
            }
        }
    }

    //Player Hits
    for(vector<bullet_t>::iterator it2 = g_Bullets.begin();it2 != g_Bullets.end(); ++it2){

        if(!it2->isPlayerFired && it2->isVisible){

            if(it2->x >= player.x && it2->x <= (player.x + PLAYER_WIDTH) && it2->y - (BULLET_HEIGHT / 2) <= (player.y + PLAYER_HEIGHT) && it2->y - (BULLET_HEIGHT / 2) >= player.y){
            //Collision
                if(player.hit_count > 0){
                    player.hit_count -= 1;
                    playerHits++;
                }

                //Hide the bullet, otherwise two hits are registered
                it2->isVisible = false;

                if(player.hit_count == 0){
                    g_State = GAMEOVER;

                    SoundEvent se = ENEMY_EXPLOSION;
                    soundQueue.push_back(se);
                }

            }
        }
   }
       

   for(vector<bullet_t>::iterator it = g_Bullets.begin();it != g_Bullets.end(); ++it){
        //cout << "playerBullets" << endl;
        if(it->isVisible && (it->y > g_ScreenHeight || it->y < 0)){
             it->isVisible  = false;
        }
   }
   //Remove cosmic objects that are out of the screen area
   for(vector<cosmic_t>::iterator it = g_CosmicObjects.begin();it != g_CosmicObjects.end(); ++it){
        if(it->isVisible && (it->y > g_ScreenHeight || it->y < 0)){
             it->isVisible  = false;
             //it->y = rand() % (g_ScreenHeight - 2); 
             it->y = 0; 
             it->x = rand() % (g_ScreenWidth - 2);
             //it->delay_ticks = rand() % (1000 + 1);
        }
   }
}

void transform_objects()
{

}

void transform_player()
{


}


void draw_objects()
{
   SDL_Rect src, dest;
   char buff[80];

   src.w = BULLET_WIDTH;
   src.h = BULLET_HEIGHT;
   src.x = 0;
   src.y = 0;
   //First draw stars and other cosmic objects
   for(vector<cosmic_t>::iterator it = g_CosmicObjects.begin();it != g_CosmicObjects.end(); ++it){
        if(it->isVisible){
            cosmic_t b = *it;


            dest.w = b.w;
            dest.h = b.h;
            dest.x = b.x;
            dest.y = b.y;
            SDL_RenderCopy(m_pRenderer, b.tCosmic, &src, &dest);
        }
   }

   for(vector<bullet_t>::iterator it = g_Bullets.begin();it != g_Bullets.end(); ++it){
        //cout << "playerBullets" << endl;
        if(it->isVisible){
            bullet_t b = *it;


            dest.w = b.w;
            dest.h = b.h;
            dest.x = b.x;
            dest.y = b.y;
            SDL_RenderCopy(m_pRenderer, b.tBullet, &src, &dest);
        }
   }


   //Drawing enemies

   src.w = PLAYER_WIDTH;
   src.h = PLAYER_HEIGHT;
   src.x = 0;
   src.y = 0;
   for(vector<enemy_t>::iterator it = enemies.begin();it != enemies.end(); ++it){
        enemy_t b = *it;
        //cout << "Enemies " << b.w << " - " << b.h << " - " << b.x << endl;

        if(b.isVisible){
            dest.w = b.w;
            dest.h = b.h;
            dest.x = b.x;
            dest.y = b.y;
            SDL_RenderCopy(m_pRenderer, b.tEnemy, &src, &dest);
        }
   }

   //Finally draw the dashboard
   sprintf(buff, "KILLS: %d", playerKills);
   print_text(buff, g_ScreenWidth - 200, 5);
   sprintf(buff, "PLAYER HITS: %d", playerHits);
   print_text(buff, 200, 5);
}

void print_text(const char *str, uint16_t x, uint16_t y)
{
   SDL_Rect src, dest;
   uint16_t i = 0;
   uint8_t row = 0;

   while(*str != '\0'){
    //printf("%c ", *str);

    uint8_t loc = *str;
    //printf("%d ", loc);

    src.w = 10;
    src.h = 10;
    if(loc >= 65){
        //This is a letter
        row   = (loc - 65) / 15;
        src.x = ((loc - 65) % 15) * 10;
        //printf("row=%d ", row);
        src.y = row * 10;
    }else if(loc >= 48){
        //This is a number
        row   = (((loc - 48) + 11) / 15) + 1;
        src.x = (((loc - 48) + 11) % 15) * 10;
        src.y = row * 10;
    }else if(loc >= 32){
        //printf("Space");
        //Space and chars
        //row   = 3;
        //src.x = 0;
        //src.y = row * 10;
        row   = (((loc - 32) + 8) / 15) + 2;
        src.x = (((loc - 32) + 8) % 15) * 10;
        src.y = row * 10;
    }
    //dest.w = g_ScreenWidth;
    dest.w = 10;
    //dest.h = 64;
    dest.h = 10;
    dest.x = x + (i * 10);
    dest.y = y;
    SDL_RenderCopy(m_pRenderer, g_Images[NUMBERS], &src, &dest);

    i++;
    str++;
   }
}

void draw_player()
{
   SDL_Rect src, dest;

   src.w = player.w;
   src.h = player.h;
   src.x = 0;
   src.y = 0;

   dest.w = player.w;
   dest.h = player.h;
   dest.x = player.x;
   //dest.y = player.y;
   dest.y = g_ScreenHeight - PLAYER_HEIGHT;

   //cout << g_ScreenWidth << " - " << g_ScreenHeight << endl;
   //cout << player.y << endl;

   SDL_RenderCopy(m_pRenderer, player.tPlayer, &src, &dest);
}


void update_screen()
{
        //The backbuffer should be considered invalidated after each present; do not assume that previous contents will exist between frames.
        SDL_RenderPresent(m_pRenderer);
}


void play_media()
{

    int i = 0;
    //empty the sound queue
    while(!soundQueue.empty()){
        SoundEvent se = soundQueue[i];
        //cout << se << endl;
        if(se == PLAYER_FIRE){
            Mix_PlayChannel(-1, g_AudioClips[0], 0);
        }else if(se == ENEMY_EXPLOSION){
            Mix_PlayChannel(-1, g_AudioClips[1], 0);
        }else if(se == MENU_APPEAR){
            Mix_PlayChannel(-1, g_AudioClips[2], 0);
        }else if(se == MENU_TRAVERSE){
            Mix_PlayChannel(-1, g_AudioClips[3], 0);
        }
        //Inefficient
        soundQueue.erase(soundQueue.begin());
        
        //i++;
    }
}


void misc_activity()
{
    //Remove bullets that have gone beyond screen boundaries
   for(unsigned int i=0;i<g_Bullets.size();i++){
        //cout << "playerBullets" << endl;
        if(! g_Bullets[i].isVisible){
            g_Bullets.erase(g_Bullets.begin() + i);
        }

   }
}
