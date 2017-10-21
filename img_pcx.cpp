#include <cstdio>

#include "img_pcx.h"

namespace gamelib {

    IMG_PCX::IMG_PCX(std::string file)
    {
        m_FileName = file;
    }
    
    IMG_PCX::~IMG_PCX()
    {

        this->close();
   
        if(m_Image.buffer != NULL)
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
        
        uint16_t cnt = 0, buffsize = m_Image.header.width * m_Image.header.height;
    
        m_Image.buffer = (uint8_t *)malloc(buffsize + 1);
    
    
        //Decompress RLE data
        while(cnt < buffsize){
            uint16_t val = fgetc(m_pFp);
    
            if(val < 192){
                m_Image.buffer[cnt++] = (uint8_t)val;
            }else if(val >= 192 && val <= 255){
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

        uint16_t r, g, b;

        cnt = 0;

        while(!feof(m_pFp)){
            r = fgetc(m_pFp);
            g = fgetc(m_pFp);
            b = fgetc(m_pFp);

            m_Image.palette[cnt].r = (uint8_t)(r >> 2);
            m_Image.palette[cnt].g = (uint8_t)(g >> 2);
            m_Image.palette[cnt++].b = (uint8_t)(b >> 2);
        }


        //Close the file we're done
        this->close();
        
    
    
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
        if(m_pFp != NULL){
            fclose(m_pFp);
        }
    }
}
