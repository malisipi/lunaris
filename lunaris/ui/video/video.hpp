#pragma once

#include "./mpv_dynamic_loader.hpp"

void poll_mpv_events (void* data);

namespace lunaris {

    namespace ui {
        const uint32_t video_id = request_new_id();
        typedef struct video:widget {
            virtual uint32_t get_type(){
                return video_id;
            };
            mpv_handle* _mpv_handle = NULL;
            mpv_render_context* mpv_context = NULL;
            lunaris::window* _win = NULL; // TODO: Remove in future, it's ridiculous and unsafe
            int _event_pooled_timer = 0;
            void(*time_pos_update_handler)(lunaris::window*, lunaris::ui::widget*, double) = NULL;
            void(*duration_update_handler)(lunaris::window*, lunaris::ui::widget*, double) = NULL;
            video(){
                if(!_is_mpv_loaded){
                    _load_mpv();
                };
                if(!_is_mpv_loaded) return;

                this->_mpv_handle = dmpv_create();
                dmpv_set_option_string(this->_mpv_handle, (char*)"vo", (char*)"libmpv");
                if(dmpv_initialize(this->_mpv_handle) < 0){
                    printf("dmpv_initialize failed\n");
                    return;
                };

                int advanced_control = 0;
                mpv_render_param params[] = {
                    {MPV_RENDER_PARAM_API_TYPE, (char*)"sw"},
                    {MPV_RENDER_PARAM_ADVANCED_CONTROL, &advanced_control},
                    {MPV_RENDER_PARAM_INVALID, NULL}
                };
                dmpv_wakeup(this->_mpv_handle);
                if(dmpv_render_context_create(&this->mpv_context, this->_mpv_handle, params) < 0){
                    printf("dmpv_render_context_create failed\n");
                    return;
                };
            };
            void load_media(std::string file_name){
                dmpv_set_property_string(this->_mpv_handle, (char*)"pause", (char*)"yes");
                const char* file[] = {(char*)"loadfile", file_name.c_str(), NULL};
                dmpv_command_async(this->_mpv_handle, 0, file);
            };
            void play(){
                dmpv_set_property_string(this->_mpv_handle, (char*)"pause", (char*)"no");
            };
            void pause(){
                dmpv_set_property_string(this->_mpv_handle, (char*)"pause", (char*)"yes");
            };
            void set_time_pos(int time_pos){
                char time_pos_str[12];
                sprintf(time_pos_str, "%d", time_pos);
                dmpv_set_property_string(this->_mpv_handle, "time-pos", time_pos_str);
            };
            uint32_t* pixels = NULL;
            uint32_t pixel_count = 0;
            void draw(lunaris::window* win, uint32_t* buffer){
                if(this->_win == NULL) this->_win = win;

                const int resolution[2] = {this->fw, this->fh};
                const int pitch = this->fw*4;

                const int new_pixel_count = (this->fw+24)*(this->fh+24);
                if(pixels == NULL){
                    pixel_count = new_pixel_count;
                    pixels = (uint32_t*)malloc(new_pixel_count*sizeof(uint32_t));
                } else if(pixel_count != new_pixel_count) {
                    pixels = (uint32_t*)realloc((void*)pixels, new_pixel_count*sizeof(uint32_t));
                };

                mpv_render_param render_params[] = {
                    {MPV_RENDER_PARAM_SW_SIZE, (void*)resolution},
                    {MPV_RENDER_PARAM_SW_FORMAT, (char*)"bgr0"},
                    {MPV_RENDER_PARAM_SW_STRIDE, (void*)&pitch},
                    {MPV_RENDER_PARAM_SW_POINTER, pixels},
                    {MPV_RENDER_PARAM_INVALID, NULL},
                };

                if(dmpv_render_context_render(this->mpv_context, render_params) < 0) {
                    printf("dmpv_render_context_render failed\n");
                    return;
                };
                if(this->_event_pooled_timer++%5==0) poll_mpv_events((void*)this);

                dmpv_observe_property(this->_mpv_handle, 0, "duration", MPV_FORMAT_DOUBLE);
                dmpv_observe_property(this->_mpv_handle, 0, "time-pos", MPV_FORMAT_DOUBLE);

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
    };
};

void poll_mpv_events (void* widget_ptr){
    lunaris::ui::video* widget = (lunaris::ui::video*)widget_ptr;
    for(;;){
        mpv_event* event = dmpv_wait_event(widget->_mpv_handle, 0);
        if(event->event_id == MPV_EVENT_NONE) break;

        if(event->event_id == MPV_EVENT_PROPERTY_CHANGE){
            struct mpv_event_property* prop = (struct mpv_event_property*)event->data;
            if(strcmp(prop->name, "time-pos") == 0){
                if(prop->format == MPV_FORMAT_DOUBLE){
                    if(widget->time_pos_update_handler != NULL){
                        widget->time_pos_update_handler(widget->_win, widget, *((double*)(prop->data)));
                    };
                };
            } else if(strcmp(prop->name, "duration") == 0){
                if(prop->format == MPV_FORMAT_DOUBLE){
                    if(widget->duration_update_handler != NULL){
                        widget->duration_update_handler(widget->_win, widget, *((double*)(prop->data)));
                    };
                };
            };
        };
    };
};
