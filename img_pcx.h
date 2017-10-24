#ifndef IMG_PCX_H
#define IMG_PCX_H

#include <string>

#include <cstdint>
#include <cstdio>

// Methods needed
// init(), delete(), load(), copy_to_buffer();
namespace gamelib {

    class IMG_PCX {
        public:
            IMG_PCX(std::string file);
            bool read();
            void free();
            SDL_Surface *toSDLSurface();
            ~IMG_PCX();
        private:
            typedef struct RGB_Color{
                uint8_t r, g, b;
            } RGB_Color;
        
            typedef struct pcx_header{
                uint8_t   manufacturer;
                uint8_t   version;
                uint8_t   encoding;
                uint8_t   bits_per_pixel;
                uint16_t  x, y; //Starting location of image
                uint16_t  width, height;
                uint16_t  hRes;
                uint16_t  vRes;
                uint8_t   ega_palette[48];
                uint8_t   reserved;
                uint8_t   num_color_planes;
                uint16_t  bytes_per_line;
                uint16_t  palette_type;
                uint8_t   padding[58];
        
            } pcx_header;
        
            typedef struct pcx_image{
                pcx_header header;
                RGB_Color  palette[256];
                uint8_t    *buffer; /*Raw image data*/
            } pcx_image;
    
            pcx_image m_Image;
            FILE *m_pFp;
            std::string m_FileName;
    
            void debug();
    };
}
#endif

