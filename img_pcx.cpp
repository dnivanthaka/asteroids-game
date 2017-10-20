#include <cstdio>

#include "img_pcx.h"


IMG_PCX::IMG_PCX(std::string file)
{
    m_FileName = file;
}

IMG_PCX::~IMG_PCX()
{
    if(m_pFp != NULL){
        fclose(m_pFp);
    }

    free(m_Image.buffer);
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

    m_Image.buffer = (uint8_t *)malloc((m_Image.header.width * m_Image.header.height) + 1);

    uint16_t cnt = 0;

    //Decompress RLE data
    while(!feof(m_pFp)){
        uint16_t val = fgetc(m_pFp);

        if(val < 192){
            m_Image.buffer[cnt++] = (uint8_t)val;
        }else{
            uint16_t runs = val - 192;

            //read next value
            val = fgetc(m_pFp);
            for(int i=0;i<runs;i++){
                m_Image.buffer[cnt++] = (uint8_t)val;
            }
        }
    }

    //TODO Reading palette data
    fseek(m_pFp, -768L, SEEK_END);

    


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

void IMG_PCX::close()
{

}
