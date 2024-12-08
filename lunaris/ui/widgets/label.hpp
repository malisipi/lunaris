#pragma once

namespace lunaris::ui {
    const uint32_t label_id = request_new_id();
    typedef struct label:widget {
        virtual uint32_t get_type(){
            return label_id;
        };
        std::string text = "";
        int hor_padding = 4;
        int ver_padding = 2;
        void draw(lunaris::window* win, uint32_t* buffer){
            const int text_line_height = std::min(this->fh-ver_padding*2, 20);
            win->graphics.text(this->fx+hor_padding, this->fy+ver_padding, text_line_height, this->text.c_str(), win->colors->text_color);
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){};
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } label;
};