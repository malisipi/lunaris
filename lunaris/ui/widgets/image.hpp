#pragma once

namespace lunaris {
    namespace ui {
        const uint32_t image_id = request_new_id();
        typedef struct image:widget {
            virtual uint32_t get_type(){
                return image_id;
            };
            lunaris::layer* image = NULL;
            void draw(lunaris::window* win, uint32_t* buffer){
                win->graphics.image(this->image, this->fx, this->fy, this->fw, this->fh);
            };
            void(*click_handler)(lunaris::window*, lunaris::ui::widget*) = NULL;
            void(*double_click_handler)(lunaris::window*, lunaris::ui::widget*) = NULL;
            __uint128_t previous_click = 0;
            virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
                if(pressed){
                    win->focused = (void*)this;
                    __uint128_t the_click = __get_event_time();
                    __uint128_t elapsed_time = the_click-this->previous_click;
                    if(elapsed_time>350){
                        if(this->click_handler != NULL) this->click_handler(win, this);
                        this->previous_click = the_click;
                    } else {
                        if(this->double_click_handler != NULL) this->double_click_handler(win, this);
                        else if(this->click_handler != NULL) this->click_handler(win, this);
                        this->previous_click = 0;
                    };
                };
            };
            virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
        } image;
    }
};