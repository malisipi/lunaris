#include "./mpv_dynamic_loader.hpp"

namespace lunaris::ui {
    const uint32_t video_id = request_new_id();
    typedef struct video:widget {
        virtual uint32_t get_type(){
            return video_id;
        };
        mpv_handle* mpv_handle = NULL;
        mpv_render_context* mpv_context = NULL;
        video(){
            if(!_is_mpv_loaded){
                _load_mpv();
            };
            if(!_is_mpv_loaded) return;

            this->mpv_handle = dmpv_create();
            dmpv_set_option_string(this->mpv_handle, (char*)"vo", (char*)"libmpv");
            if(dmpv_initialize(this->mpv_handle) < 0){
                printf("dmpv_initialize failed\n");
                return;
            };

            int advanced_control = 0;
            mpv_render_param params[] = {
                {MPV_RENDER_PARAM_API_TYPE, (char*)"sw"},
                {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advanced_control},
                {MPV_RENDER_PARAM_INVALID, NULL}
            };

            if(dmpv_render_context_create(&this->mpv_context, this->mpv_handle, params) < 0){
                printf("dmpv_render_context_create failed\n");
                return;
            };
        };
        void load_media(std::string file_name){
            dmpv_set_property_string(this->mpv_handle, (char*)"pause", (char*)"yes");
            const char* file[] = {(char*)"loadfile", file_name.c_str(), NULL};
            dmpv_command_async(this->mpv_handle, 0, file);
        };
        void play(){
            dmpv_set_property_string(this->mpv_handle, (char*)"pause", (char*)"no");
        };
        void pause(){
            dmpv_set_property_string(this->mpv_handle, (char*)"pause", (char*)"yes");
        };
        void draw(lunaris::window* win, uint32_t* buffer){
            const int resolution[2] = {this->fw, this->fh};
            const int pitch = this->fw*4;
            uint32_t pixels[(this->fw+24)*(this->fh+24)];
            mpv_render_param render_params[] = {
                {MPV_RENDER_PARAM_SW_SIZE, (void*)resolution},
                {MPV_RENDER_PARAM_SW_FORMAT, (char*)"bgr0"},
                {MPV_RENDER_PARAM_SW_STRIDE, (void*)&pitch},
                {MPV_RENDER_PARAM_SW_POINTER, &pixels},
                {MPV_RENDER_PARAM_INVALID, NULL},
            };

            if(dmpv_render_context_render(this->mpv_context, render_params) < 0) {
                printf("dmpv_render_context_render failed\n");
                return;
            };

            for(int render_y=0; render_y<this->fh; render_y++){
                for(int render_x=0; render_x<this->fw; render_x++){
                    const uint32_t pixel = pixels[render_y*this->fw+render_x];
                    win->graphics.pixel(render_x+this->fx, render_y+this->fy, pixel | 0xFF000000);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){};
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } video;
}