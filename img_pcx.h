#ifndef IMG_PCX_H
#define IMG_PCX_H
typedef struct RGB_Color{
    uint8_t r, g, b;
} RGB_Color;

typedef struct pcx_header{
    uint8_t   manufacturer;
    uint8_t   version;
    uint8_t   encoding;
    uint8_t   bits_per_pixel;
    uint16_t  x, y;
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

typedef struct pcx_picture{
    pcx_header header;
    RGB_Color  palette[256];
    uint8_t    *buffer /*Raw image data*/
} pcx_picture;

// Methods needed
// init(), delete(), load(), copy_to_buffer();
class IMG_PCX {
    public:
        IMG_PCX(std::string file);
        bool read();
        ~IMG_PCX();
    private:
        pcx_picture *image;
}
#endif

