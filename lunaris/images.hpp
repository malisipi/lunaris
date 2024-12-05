#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb/stb_image.h"

namespace lunaris {
    typedef struct layer {
        uint32_t* data;
        int width;
        int height;
        void destroy(){
            free(this->data);
            free(this);
        }
    } layer;

    lunaris::layer* new_image(const char* path){
        if (access(path, F_OK) != 0) return NULL; // If not exist
        lunaris::layer* layer = (lunaris::layer*)malloc(sizeof(lunaris::layer));
        int width;
        int height;
        int channels;
        uint8_t* temp_data = (uint8_t*)stbi_load(path, &width, &height, &channels, 0);
        if(temp_data == NULL){
            free(layer);
            stbi_image_free(temp_data);
            printf("Cannot loaded the image that has the path: %s", path);
            return NULL;
        };
        layer->data = (uint32_t*)malloc(sizeof(uint32_t)*width*height);
        for(int px_yi=0; px_yi<height; px_yi++){
            for(int px_xi=0; px_xi<width; px_xi++){
                if(channels == 3){
                    layer->data[px_yi*width+px_xi] = 0xFF000000 |
                                                        (temp_data[(px_yi*width+px_xi)*3+0]<< 16)|
                                                        (temp_data[(px_yi*width+px_xi)*3+1]<< 8) |
                                                        (temp_data[(px_yi*width+px_xi)*3+2]<< 0) ;
                } else if(channels == 4){
                    layer->data[px_yi*width+px_xi] =    (temp_data[(px_yi*width+px_xi)*4+0]<<16)|
                                                        (temp_data[(px_yi*width+px_xi)*4+1]<<8) |
                                                        (temp_data[(px_yi*width+px_xi)*4+2]<<0) |
                                                        (temp_data[(px_yi*width+px_xi)*4+3]<<24);
                };
            };
        };
        layer->width = width;
        layer->height = height;
        stbi_image_free(temp_data);
        return layer;
    };

    lunaris::layer* new_image_from_memory(unsigned char* image_data, int length){
        lunaris::layer* layer = (lunaris::layer*)malloc(sizeof(lunaris::layer));
        int width;
        int height;
        int channels;
        uint8_t* temp_data = (uint8_t*)stbi_load_from_memory((unsigned char*)image_data, length, &width, &height, &channels, 0);
        if(temp_data == NULL){
            free(layer);
            stbi_image_free(temp_data);
            printf("Cannot loaded the image from memory");
            return NULL;
        };
        layer->data = (uint32_t*)malloc(sizeof(uint32_t)*width*height);
        for(int px_yi=0; px_yi<height; px_yi++){
            for(int px_xi=0; px_xi<width; px_xi++){
                if(channels == 3){
                    layer->data[px_yi*width+px_xi] = 0xFF000000 |
                                                        (temp_data[(px_yi*width+px_xi)*3+0]<< 16)|
                                                        (temp_data[(px_yi*width+px_xi)*3+1]<< 8) |
                                                        (temp_data[(px_yi*width+px_xi)*3+2]<< 0) ;
                } else if(channels == 4){
                    layer->data[px_yi*width+px_xi] =    (temp_data[(px_yi*width+px_xi)*4+0]<<16)|
                                                        (temp_data[(px_yi*width+px_xi)*4+1]<<8) |
                                                        (temp_data[(px_yi*width+px_xi)*4+2]<<0) |
                                                        (temp_data[(px_yi*width+px_xi)*4+3]<<24);
                };
            };
        };
        layer->width = width;
        layer->height = height;
        stbi_image_free(temp_data);
        return layer;
    };
};
