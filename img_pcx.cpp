#include <cstdio>
#include "SDL2/SDL.h"

#include "img_pcx.h"

namespace gamelib {

    IMG_PCX::IMG_PCX(std::string file)
    {
        m_FileName = file;
    }
    
    IMG_PCX::~IMG_PCX()
    {
        this->free();
    }
    
    bool IMG_PCX::read()
    {
        if((m_pFp = fopen(m_FileName.c_str(), "rb")) == NULL){
            printf("Cannot open file %s\n", m_FileName.c_str());
            return false;
        }
    
        int size = sizeof(pcx_header);
    
    
        if(fread(&m_Image, 1, size, m_pFp) < size){
            printf("Cannot read image file meta");
            return false;
        }
    
        this->debug();
        
        uint16_t cnt = 0, buffsize = (m_Image.header.width + 1) * (m_Image.header.height + 1);
    
        m_Image.buffer = (uint8_t *)malloc(buffsize);
    
    
        //Decompress RLE data
        while(cnt < buffsize){
            uint16_t val = fgetc(m_pFp);
    
            if(val < 192){
                m_Image.buffer[cnt++] = (uint8_t)val;
            }else if(val >= 192 && val <= 255){
                uint16_t runs = val - 192;
                //printf("runs = %d\n", runs);
    
                //read next value
                val = fgetc(m_pFp);
                for(int i=0;i<runs;i++){
                    m_Image.buffer[cnt++] = (uint8_t)val;
                }
            }
        }
    
        fseek(m_pFp, -768L, SEEK_END);

        uint16_t r, g, b;

        cnt = 0;

        while(!feof(m_pFp)){
            r = fgetc(m_pFp);
            g = fgetc(m_pFp);
            b = fgetc(m_pFp);

            m_Image.palette[cnt].r   = (uint8_t)(r >> 2);
            m_Image.palette[cnt].g   = (uint8_t)(g >> 2);
            m_Image.palette[cnt++].b = (uint8_t)(b >> 2);
        }


        //Close the file we're done
        fclose(m_pFp);
        
    
    
        return true;
    }
    
    void IMG_PCX::debug()
    {
        printf("Manufacturer: %X\n", m_Image.header.manufacturer);
        printf("Version: %d\n", m_Image.header.version);
        printf("Encoding: %d\n" , m_Image.header.encoding);
        printf("Bits per pixel: %d\n", m_Image.header.bits_per_pixel);
        printf("X: %d\n", m_Image.header.x);
        printf("Y: %d\n" ,m_Image.header.y);
        printf("Width %d, Height %d\n" , m_Image.header.width, m_Image.header.height);
        printf("Horizontal Res: %d\n" , m_Image.header.hRes);
        printf("Vertical Res: %d\n", m_Image.header.vRes);
        printf("Number of color planes: %d\n", m_Image.header.num_color_planes);
        printf("Bytes per line: %d\n", m_Image.header.bytes_per_line);
        printf("Palette type: %d\n", m_Image.header.palette_type);
    }
    
    void IMG_PCX::free()
    {
        if(m_pFp != NULL){
            fclose(m_pFp);
        }

        if(m_Image.buffer != NULL)
            ::free(m_Image.buffer);
    }

    SDL_Surface * IMG_PCX::toSDLSurface(SDL_Window *m_pWindow)
    {
        SDL_Surface *surface = NULL;
        Uint32 rmask, gmask, bmask, amask;
        rmask = gmask = bmask = amask = 0;

        SDL_Palette palette;

        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            //amask = 0x000000ff;
            amask = 0;
        #else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            //amask = 0xff000000;
            amask = 0;
        #endif

        palette.ncolors = 256;
        palette.colors = m_Image.palette;

        /*
        int depth, pitch;
        if (req_format == STBI_rgb) {
            depth = 24;
            pitch = 3*width; // 3 bytes per pixel * pixels per row
        } else { // STBI_rgb_alpha (RGBA)
            depth = 32;
            pitch = 4*width;
        }
        */



        /*surface = SDL_CreateRGBSurfaceFrom(
            (void *)m_Image.buffer, 
            m_Image.header.width, 
            m_Image.header.height, 
            m_Image.header.bits_per_pixel, //(bits per pixel) 
            m_Image.header.width * 3, //(width*depth_in_bytes, in this case) 
            //m_Image.header.width * (m_Image.header.bits_per_pixel / 8), //(width*depth_in_bytes, in this case) 
            rmask, 
            gmask, 
            bmask, 
            amask);*/ 
        surface = SDL_CreateRGBSurface(
            SDL_SWSURFACE, 
            m_Image.header.width + 1, 
            m_Image.header.height + 1, 
            m_Image.header.bits_per_pixel * 3, //(bits per pixel) 
            //m_Image.header.width * (m_Image.header.bits_per_pixel / 8), //(width*depth_in_bytes, in this case) 
            rmask, 
            gmask, 
            bmask, 
            amask); 
        //surface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Image.header.width + 1,  m_Image.header.height + 1, 24, rmask, gmask, bmask, amask); 

        //SDL_SetColors(surface, m_Image.palette, 0, 256);
        //SDL_SetSurfacePalette(surface, &palette);
        memcpy(surface->pixels, (void *)m_Image.buffer,(m_Image.header.width + 1) * (m_Image.header.height + 1));
        /*SDL_Surface *tmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
        if(tmp == NULL){
            printf(SDL_GetError());
        }*/
        //SDL_FreeSurface(surface);
        //surface->pixels = m_Image.buffer;
        //SDL_LockSurface(surface);
        //memcpy(surface->pixels, m_Image.buffer,(m_Image.header.width + 1) *  (m_Image.header.height + 1));
        //surface->format->palette = &palette;
        //SDL_UnlockSurface(surface);

        return surface;
    }
}
