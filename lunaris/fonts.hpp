#include <stdio.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION 
#include "thirdparty/stb/stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

namespace lunaris {
    #define uint_color_red(color)   ((color >> (4*4)) & 0xFF)
    #define uint_color_green(color) ((color >> (2*4)) & 0xFF)
    #define uint_color_blue(color)  ((color >> (0*4)) & 0xFF)
    #define uint_color_alpha(color) (color & 0xFF000000)
    #define uint_rgb_color_from(red, green, blue) ((((uint32_t)red << (4*4)) & 0x00FF0000) + (((uint32_t)green << (2*4)) & 0x0000FF00) + (((uint32_t)blue << (0*4)) & 0x000000FF))
    #define uint_rgba_color_from(red, green, blue, alpha) ((((uint32_t)red << (4*4)) & 0x00FF0000) + (((uint32_t)green << (2*4)) & 0x0000FF00) + (((uint32_t)blue << (0*4)) & 0x000000FF) + (alpha & 0xFF000000))


    char* get_system_font(){
        #if defined(_WIN32)
            return (char*)"C:/Windows/Fonts/segoeui.ttf";
        #elif defined(linux)
            return (char*)"/usr/share/fonts/noto/NotoSans-Regular.ttf";
        #elif defined(__HAIKU__)
            return (char*)"/Haiku/system/data/fonts/ttfonts/NotoSans-Regular.ttf";
        #else
            #error "No pre-defined system font, will crash"
        #endif
    };

    unsigned char color_mix_one_color(unsigned char color1, unsigned char color2, float mix_value){
        if(color1-color2 < 0) return color_mix_one_color(color2, color1, 1-mix_value);
        return color1-(color1-color2)*mix_value;
    }

    uint32_t color_mix(uint32_t color1, uint32_t color2, float mix_value){
        if(mix_value == 0) return color1;
        if(mix_value == 1) return color2;
        return 0xFF000000|uint_rgba_color_from(
            color_mix_one_color(uint_color_red(color1), uint_color_red(color2), mix_value),
            color_mix_one_color(uint_color_green(color1), uint_color_green(color2), mix_value),
            color_mix_one_color(uint_color_blue(color1), uint_color_blue(color2), mix_value),
            color_mix_one_color(uint_color_alpha(color1), uint_color_alpha(color2), mix_value)
        );
    }

    typedef struct font {
        unsigned char* font_buffer;
        stbtt_fontinfo info;
        void draw_text(uint32_t* buffer, const int buffer_width, const int buffer_height, const int text_start_x, int text_start_y, const int line_height, const char* text, uint32_t color){
            float scale = stbtt_ScaleForPixelHeight(&this->info, line_height);
            
            int pos_x = text_start_x;
            int pos_y = text_start_y;
            
            int ascent, descent, line_gap;
            stbtt_GetFontVMetrics(&this->info, &ascent, &descent, &line_gap);
            ascent *= scale;
            descent *= scale;
            line_gap *= scale;
            
            for (int i = 0; i < strlen(text); i++){
                uint32_t the_glyph = 0;

                if(text[i] == '\n'){ // It's multi-line hack shift
                    pos_x = text_start_x;
                    pos_y += line_height;
                };

                if(buffer_height<=pos_y) return; // If potantial-area finished, return. Cannot write buffer
                if(buffer_width<=pos_x) continue; // Exceed width, skip until new line

                // https://en.wikipedia.org/wiki/UTF-8
                if((text[i]&0x80) == 0){ // 1 char unicode (a)
                    the_glyph = text[i];
                } else if ((text[i] & 0xC0) == 0x80){ // Invalid continuous unicode
                    continue;
                } else if((text[i] & 0xE0) == 0xC0){ // 2 char unicode (ş)
                    the_glyph = ((text[i]&0x1F)<<6) | (text[++i]&0x3F);
                } else if((text[i] & 0xF0) == 0xE0) { // 3 char unicode (ह)
                    the_glyph = ((text[i]&0x0F)<<12) | ((text[++i]&0x3F)<<6) | (text[++i]&0x3F);
                } else if((text[i] & 0xF8) == 0xF0) { // 4 char unicode (🐺)
                    the_glyph = ((text[i]&0x07)<<18) | ((text[++i]&0x3F)<<12) | ((text[++i]&0x3F)<<6) | (text[++i]&0x3F);
                } else { // Unknown unicode
                    continue;
                };
                if(stbtt_FindGlyphIndex(&info,the_glyph) == 0){
                    //printf("The glyph is not exist on this font!\n");
                    continue;
                };

                int advance_width;
                int left_side_bearing;
                stbtt_GetCodepointHMetrics(&this->info, the_glyph, &advance_width, &left_side_bearing);
                advance_width *= scale;
                left_side_bearing *= scale;

                int ix0, iy0, ix1, iy1;
                stbtt_GetCodepointBitmapBox(&this->info, the_glyph, scale, scale, &ix0, &iy0, &ix1, &iy1);
                int char_y = pos_y + ascent + iy0;

                int bitmap_w = 0;
                int bitmap_h = 0;

                unsigned char* the_char = stbtt_GetGlyphBitmapSubpixel(&this->info, scale, scale, 0.0f, 0.0f, stbtt_FindGlyphIndex(&info, the_glyph), &bitmap_w, &bitmap_h, 0, 0);
                int bitmap_stride = bitmap_w;
                
                if(buffer_height<=bitmap_h+char_y){
                    bitmap_h = buffer_height-char_y;
                };
                if(buffer_width<=pos_x+bitmap_w){
                    bitmap_w = buffer_width-pos_x;
                };

                for(int bitmap_y=0; bitmap_y<bitmap_h; bitmap_y++){
                    for(int bitmap_x=0; bitmap_x<bitmap_w; bitmap_x++){
                        int target_byte = (bitmap_y+char_y)*buffer_width + bitmap_x + left_side_bearing + pos_x;
                        buffer[target_byte] = color_mix(buffer[target_byte], color, (float)the_char[bitmap_stride*bitmap_y + bitmap_x]/(float)255);
                    };
                };
                free(the_char);

                pos_x += advance_width;
                
                /*int kern = stbtt_GetCodepointKernAdvance(&this->info, the_glyph, text[i + 1]); // TODO: multi-char unicodes will change i, not safe for those function calls, please fix it.
                pos_x += kern * scale;*/
            };
        };
        void destroy (){
            free(this->font_buffer);
            free(this);
        };
    } font;


    lunaris::font* new_font(char* font_path){
        if (access(font_path, F_OK) != 0) return NULL; // If not exist
        lunaris::font* the_font = (lunaris::font*)malloc(sizeof(lunaris::font)); // TODO: Fix memory leak
        
        FILE* font_file = fopen(font_path, "rb");
        fseek(font_file, 0, SEEK_END);
        uint32_t size = ftell(font_file);
        fseek(font_file, 0, SEEK_SET);
        
        the_font->font_buffer = (unsigned char*)malloc(size);
        
        fread(the_font->font_buffer, size, 1, font_file);
        fclose(font_file);

        if (!stbtt_InitFont(&the_font->info, the_font->font_buffer, 0)){
            printf("TTF Font initalition failed\n");
        };
        return the_font;
    };
};
