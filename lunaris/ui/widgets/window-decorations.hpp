#pragma once

namespace lunaris::ui {
    const uint32_t window_decorations_id = request_new_id();
    
    typedef struct window_decorations:layout {
        const int button_width = 50;
        virtual uint32_t get_type(){
            return window_decorations_id;
        };
        widget* child = NULL;
        bool should_decorated = true;
        bool should_resizable = true; // This option will be ignored when decorations disabled
        window_shadow* shadow = NULL;
        /* Decors:
         * | Titlebar (Max) | - (30|50) | [] (30|50) | x (30|50) | -> Height: 30
         */
        bool __is_maximized = false;
        char title[512] = "";
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            int top_offset = 0;
            if(this->should_decorated){
                top_offset = 30;
                win->graphics.rect(this->fx, this->fy, this->fw, top_offset, win->colors->background_color);
                win->graphics.text(this->fx+8, this->fy+4, 22, this->title,  win->colors->text_color);

                // Close
                win->graphics.line_thick(this->fx+this->fw-5-button_width/2,this->fy+10,this->fx+this->fw+5-button_width/2,this->fy+20,2, win->colors->text_color);
                win->graphics.line_thick(this->fx+this->fw+5-button_width/2,this->fy+10,this->fx+this->fw-5-button_width/2,this->fy+20,2, win->colors->text_color);

                if(!this->__is_maximized){
                    // Maximize
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+10,this->fx+this->fw+5-button_width*3/2,this->fy+10,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+10,this->fx+this->fw-5-button_width*3/2,this->fy+20,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+20,this->fx+this->fw+5-button_width*3/2,this->fy+20,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw+5-button_width*3/2,this->fy+10,this->fx+this->fw+5-button_width*3/2,this->fy+20,2, win->colors->text_color);
                } else {
                    // Restore
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+10+3,this->fx+this->fw+5-button_width*3/2-3,this->fy+10+2,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+10+3,this->fx+this->fw-5-button_width*3/2,this->fy+20,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2,this->fy+20,this->fx+this->fw+5-button_width*3/2-3,this->fy+20,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw+5-button_width*3/2-3,this->fy+10+3,this->fx+this->fw+5-button_width*3/2-3,this->fy+20,2, win->colors->text_color);

                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2+3,this->fy+10,this->fx+this->fw+5-button_width*3/2,this->fy+10,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2+3,this->fy+10,this->fx+this->fw-5-button_width*3/2+3,this->fy+20-(10-3),2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw-5-button_width*3/2+(10-3),this->fy+20-3,this->fx+this->fw+5-button_width*3/2,this->fy+20-3,2, win->colors->text_color);
                    win->graphics.line_thick(this->fx+this->fw+5-button_width*3/2,this->fy+10,this->fx+this->fw+5-button_width*3/2,this->fy+20-3,2, win->colors->text_color);
                };

                // Minimize
                win->graphics.line_thick(this->fx+this->fw-5-button_width*5/2,this->fy+15,this->fx+this->fw+5-button_width*5/2,this->fy+15,2, win->colors->text_color);
            };
            
            if(this->child != NULL){
                this->child->__set_f_size(this->child->rx+this->fx, this->child->ry+this->fy+top_offset, this->fw, this->fh-top_offset);
                if(this->child->is_layout()){
                    ((layout*)this->child)->calculate_n_draw(win, buffer);
                } else {
                    this->child->draw(win, buffer);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            if(this->should_decorated){
                if(y<=30){
                    if(pressed) {
                        if(event == lunaris::mouse::first){
                            if(this->fw-x<button_width){
                                win->destroy();
                            } else if(this->fw-x<button_width*2){
                                if(this->__is_maximized){
                                    if(this->shadow != NULL) this->shadow->__is_maximized = false;
                                    win->restore();
                                } else {
                                    if(this->shadow != NULL) this->shadow->__is_maximized = true;
                                    win->maximize();
                                };
                                this->__is_maximized = !this->__is_maximized;
                            } else if(this->fw-x<button_width*3){
                                win->minimize();
                            } else {
                                if(!this->__is_maximized) win->start_move();
                            };
                        } else if(event == lunaris::mouse::second) {
                            win->show_window_menu(this->fx+x, this->fy+y);
                        };
                    };
                } else if(this->child != NULL){
                    if(!this->__is_maximized && this->should_resizable && this->fw+this->fh-30<x+y){
                        if(pressed && event == lunaris::mouse::first) win->start_resize();
                    } else {
                        this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
                    };
                };
            } else {
                this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
            };
        };
        void set_title(const char* new_title){
            sprintf(this->title, "%s", new_title);
        };
        void set_title(lunaris::window* win, const char* new_title){
            this->set_title(new_title);
            win->set_title(new_title);
        };
    } window_decorations;
}