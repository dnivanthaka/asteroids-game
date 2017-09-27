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
//---------------------------------------------------------------------------------------------//

bool gameOver = false;

//const static int g_ScreenWidth  = 800;
//const static int g_ScreenHeight = 600;
const static int FRAME_RATE = 60;
const static int TICKS_PER_FRAME = 1000 / FRAME_RATE;
static int g_ScreenWidth;
static int g_ScreenHeight;

SDL_Window   *m_pWindow;
SDL_Renderer *m_pRenderer;

struct player_t player;
vector<enemy_t> enemies;

vector<SoundEvent> soundQueue;
vector<bullet_t> g_Bullets;
vector<cosmic_t> g_CosmicObjects;
vector<SDL_Texture *> g_Images;
vector<Mix_Chunk *> g_AudioClips;

vector<int> g_enemyXCoords;

int randMotion[] = {1, 0, 1, -1, 1, 1};

uint16_t playerKills = 0;

int main(int argc, char *argv[])
{
        uint32_t fpsTimer, capTimer;
        int countedFrames = 0;

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

        fpsTimer = SDL_GetTicks();
        
        init_player((g_ScreenWidth / 2) - (PLAYER_WIDTH / 2), g_ScreenHeight);

        while(!gameOver){
        
            capTimer = SDL_GetTicks();

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

            countedFrames++;

            int frameTicks = SDL_GetTicks() - capTimer;
            if(frameTicks < TICKS_PER_FRAME){
                
                //TODO Write a better frame rate handler, this is not the proper way
                SDL_Delay(TICKS_PER_FRAME - frameTicks);
            }

        }
        cleanup();

        cout << "You shot down " << playerKills << " enemy ships." << endl;

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


void handle_inputs()
{
    SDL_Event event;

    if(SDL_PollEvent(&event)){
       switch(event.type){
        case SDL_QUIT:
            gameOver = true;
            break;

        case SDL_KEYDOWN:
            if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                gameOver = true;
            }else if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT){
                player.vel_x = 3;
            }else if(event.key.keysym.scancode == SDL_SCANCODE_LEFT){
                player.vel_x = -3;
            }else if(event.key.keysym.scancode == SDL_SCANCODE_RCTRL || event.key.keysym.scancode == SDL_SCANCODE_LCTRL){
                fire_bullet(true, player.x + (player.w / 2) - (BULLET_WIDTH / 2), player.y - player.h, NORTH);
                //Add firing sound to the sound queue
                SoundEvent se = PLAYER_FIRE;
                soundQueue.push_back(se);
            }
            
        break;

        case SDL_KEYUP:
            if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT || event.key.keysym.scancode == SDL_SCANCODE_LEFT){
                player.vel_y = 0;
                player.vel_x = 0;
            }
        break;

        default:
            break;

       } 
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
    player.tPlayer = g_Images[PLAYER];
}

void init_enemy(enemy_t *o, int x, int y)
{
    int nx;

    o->w = PLAYER_WIDTH;
    o->h = PLAYER_HEIGHT;
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


    return true;
}

bool load_media()
{

   vector<string> sprites = {"shipsc2.pcx", "shipsc3.pcx"};

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
   vector<string> mixes = {"FLASER.wav", "EXP.wav"};

   for(unsigned int i=0;i<mixes.size();i++){
    Mix_Chunk *tmp = Mix_LoadWAV(mixes[i].c_str());
    g_AudioClips.push_back(tmp);
   }

   return true;
}

void init_cosmic_object(int x, int y, int vel_x, int vel_y, HeadingDirection dir)
{
    cosmic_t object;
    object.w = 2;
    object.h = 2;
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

    while(!g_AudioClips.empty()){
        Mix_FreeChunk(g_AudioClips.back());
        g_AudioClips.pop_back();
    }
    while(!g_Images.empty()){
        SDL_DestroyTexture(g_Images.back());
        g_Images.pop_back();
    }

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

    for(vector<enemy_t>::iterator it = enemies.begin();it != enemies.end(); ++it){

        for(vector<bullet_t>::iterator it2 = g_Bullets.begin();it2 != g_Bullets.end(); ++it2){

            if(it2->isPlayerFired && it2->isVisible && it->isVisible){

                if(it2->x >= it->x && it2->x <= (it->x + ENEMY_WIDTH) && it2->y - (BULLET_HEIGHT / 2) <= (it->y + ENEMY_HEIGHT) && it2->y - (BULLET_HEIGHT / 2) >= it->y){
                    //Collision
                    //it->isVisible = false;
                    //cout << "Hit" << endl;
                    if(it->hit_count == 0){

                        it->y = 0; 
                        it->x = rand() % (g_ScreenWidth - PLAYER_WIDTH);
                        it->isVisible = false;
                        it->delay_ticks = rand() % (1000 + 1);
                        it->hit_count = 2;
                        playerKills++;

                        SoundEvent se = ENEMY_EXPLOSION;
                        soundQueue.push_back(se);
                        //cout << "Hit" << endl;
                    }else{
                        it->hit_count -= 1;
                    }

                    //Hide the bullet, otherwise two hits are registered
                    it2->isVisible = false;

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
