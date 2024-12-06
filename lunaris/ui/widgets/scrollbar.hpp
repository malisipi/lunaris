#pragma once

namespace lunaris::ui {
    const uint32_t scrollbar_id = request_new_id();
    typedef struct scrollbar:widget {
        virtual uint32_t get_type(){
            return scrollbar_id;
        };
        int max = 1;
        int view = 1;
        int value = 0;
        float clicked_pos = 0;
        int clicked_value = 0;
        bool is_clicked = false;
        bool horizontal = false;
        int _mouse_x = 0;
        int _mouse_y = 0;
        void draw(lunaris::window* win, uint32_t* buffer){
            if(this->view < this->max){
                if(!this->horizontal){ // vertical
                    win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 5, win->colors->background_color);
                    if(this->is_hovering && this->_mouse_y<16) win->graphics.rounded_rect(this->fx, this->fy, this->fw, 16, 5, win->colors->hover_color);

                    // upper-thumb - [size: x: (this->fx+this->fw/2-4) | y: (this->fy+4) | w: 8 | h: 6(+2padding)]
                    win->graphics.triangle((this->fx+this->fw/2-4)+4, (this->fy+4),
                                                (this->fx+this->fw/2-4), (this->fy+4)+6,
                                                (this->fx+this->fw/2-4)+8, (this->fy+4)+6,
                                                win->colors->text_color);

                    float size = (float)(this->fh-36)/(float)(this->max);
                    uint32_t hovered_color = win->colors->other_color;
                    if(this->is_hovering && ((18+size*this->value)<this->_mouse_y) && ((18+size*this->value+size*this->view)>this->_mouse_y)) hovered_color = win->colors->hover_color;
                    win->graphics.rounded_rect(this->fx, this->fy+18+size*this->value, this->fw, size*this->view, 5, hovered_color);
                    if(this->is_hovering && this->fh-this->_mouse_y<16) win->graphics.rounded_rect(this->fx, this->fy+this->fh-16, this->fw, 16, 5, win->colors->other_color);

                    // lower-thumb - [size: x: (this->fx+this->fw/2-4) | y: (this->fy+this->fh-12) | w: 8 | h: (2padding+)6]
                    win->graphics.triangle((this->fx+this->fw/2-4)+4, (this->fy+this->fh-12)+6+2,
                                                (this->fx+this->fw/2-4), (this->fy+this->fh-12)+2,
                                                (this->fx+this->fw/2-4)+8, (this->fy+this->fh-12)+2,
                                                win->colors->text_color);
                } else { // horizontal
                    win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 5, win->colors->background_color);
                    if(this->is_hovering && this->_mouse_x<16) win->graphics.rounded_rect(this->fx, this->fy, 16, this->fh, 5, win->colors->hover_color);

                    // left-thumb - [size: x: (this->fx+4) | y: (this->fy+this->fh/2-4) | w: 6 | h: 8]
                    win->graphics.triangle((this->fx+4), (this->fy+this->fh/2-4)+4,
                                                (this->fx+4)+6, (this->fy+this->fh/2-4),
                                                (this->fx+4)+6, (this->fy+this->fh/2-4)+8,
                                                win->colors->text_color);

                    float size = (float)(this->fw-36)/(float)(this->max);
                    uint32_t hovered_color = win->colors->other_color;
                    if(this->is_hovering && ((18+size*this->value)<this->_mouse_x) && ((18+size*this->value+size*this->view)>this->_mouse_x)) hovered_color = win->colors->hover_color;
                    win->graphics.rounded_rect(this->fx+18+size*this->value, this->fy, size*this->view, this->fh, 5, hovered_color);
                    if(this->is_hovering && this->fw-this->_mouse_x<16) win->graphics.rounded_rect(this->fx+this->fw-16, this->fy, 16, this->fh, 5, win->colors->other_color);

                    // right-thumb - [size: x: (this->fx+this->fw-12) | y: (this->fy+this->fh/2-4) | w: 6 | h: 8]
                    win->graphics.triangle((this->fx+this->fw-12), (this->fy+this->fh/2-4),
                                           (this->fx+this->fw-12), (this->fy+this->fh/2-4)+8,
                                           (this->fx+this->fw-12)+6, (this->fy+this->fh/2-4)+4,
                                           win->colors->text_color);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            int area = this->max-this->view;
            this->_mouse_x = x;
            this->_mouse_y = y;
            if(event == lunaris::mouse::scroll){
                if(dx<0 || dy<0) {
                    if(this->value>0){
                        this->value--;
                    };
                } else {
                    if(area>this->value){
                        this->value++;
                    };
                }
            } else if(!this->horizontal){ // vertical
                if(pressed && event == lunaris::mouse::first){
                    if(y<18){
                        _scroll_up:
                        if(this->value>0){
                            this->value--;
                        };
                    } else if(y>this->fh-18) {
                        _scroll_down:
                        if(area>this->value){
                            this->value++;
                        };
                    } else {
                        float size = (float)(this->fh-36)/(float)(this->max);
                        if(y<18+size*this->value) goto _scroll_up;
                        if(y>18+size*this->value+size*this->view) goto _scroll_down;
                        this->clicked_pos = y;
                        this->is_clicked = true;
                        this->clicked_value = this->value;
                    };
                } else {
                    if(this->is_clicked){
                        float size = (float)(this->fh-36)/(float)(this->max);
                        this->value = this->clicked_value + (int)((y-this->clicked_pos)/size);
                        if(this->value<0) this->value = 0;
                        else if(this->value>this->max-this->view) this->value = this->max-this->view;
                        if(!pressed && event != lunaris::mouse::motion){
                            this->is_clicked = false;
                        };
                    };
                };
            } else { // horizontal
                if(pressed && event == lunaris::mouse::first){
                    if(x<18){
                        _scroll_left:
                        if(this->value>0){
                            this->value--;
                        };
                    } else if(x>this->fw-18) {
                        _scroll_right:
                        if(area>this->value){
                            this->value++;
                        };
                    } else {
                        float size = (float)(this->fw-36)/(float)(this->max);
                        if(x<18+size*this->value) goto _scroll_left;
                        if(x>18+size*this->value+size*this->view) goto _scroll_right;
                        this->clicked_pos = x;
                        this->is_clicked = true;
                        this->clicked_value = this->value;
                    };
                } else {
                    if(this->is_clicked){
                        float size = (float)(this->fw-36)/(float)(this->max);
                        this->value = this->clicked_value + (int)((x-this->clicked_pos)/size);
                        if(this->value<0) this->value = 0;
                        else if(this->value>this->max-this->view) this->value = this->max-this->view;
                        if(!pressed && event != lunaris::mouse::motion){
                            this->is_clicked = false;
                        };
                    };
                };
            };
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
        }
    } scrollbar;
};
