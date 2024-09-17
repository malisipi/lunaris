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

    lunaris::layer* new_image(char* path){
        if (access(path, F_OK) != 0) return NULL; // If not exist
        lunaris::layer* layer = (lunaris::layer*)malloc(sizeof(lunaris::layer));
        int width;
        int height;
        int channels;
        uint32_t* temp_data = (uint32_t*)stbi_load(path, &width, &height, &channels, 0); // TODO: May has problems with <4 channel images
        if(layer->data == NULL){
            free(layer);
            printf("Cannot loaded the image that has the path: %s", path);
            return NULL;
        };
        layer->data = (uint32_t*)malloc(sizeof(uint32_t)*width*height);
        for(int px_index=0; px_index<width*height; px_index++){
            layer->data[px_index] = (temp_data[px_index]&0xFF00FF00)|((temp_data[px_index]&0x000000FF)<<16)|((temp_data[px_index]&0x00FF0000)>>16);
        };
        layer->width = width;
        layer->height = height;
        stbi_image_free(temp_data);
        return layer;
    }
}
