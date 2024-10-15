namespace lunaris::ui {
    const uint32_t window_decorations_id = request_new_id();
    typedef struct window_decorations:layout {
        virtual uint32_t get_type(){
            return window_decorations_id;
        };
        widget* child = NULL;
        bool should_decorated = true;
        /* Decors:
         * | Titlebar (Max) | - (30|50) | [] (30|50) | x (30|50) | -> Height: 30
         */
        bool __is_maximized = false;
        char title[512] = "";
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            int top_offset = 0;
            if(this->should_decorated){
                top_offset = 30;
                win->graphics.rect(this->fx, this->fy, this->fw, top_offset, 0xFF666666);

                win->graphics.text(8, 4, 22, this->title, 0xFFFFFFFF);

                // Close
                win->graphics.rect(this->fx+this->fw-50,this->fy, 50, 30, 0xFF999999);
                win->graphics.line_thick(this->fx+this->fw-30,this->fy+10,this->fx+this->fw-20,this->fy+20,2,0xFFFFFFFF);
                win->graphics.line_thick(this->fx+this->fw-20,this->fy+10,this->fx+this->fw-30,this->fy+20,2,0xFFFFFFFF);

                if(!this->__is_maximized){
                    // Maximize
                    win->graphics.rect(this->fx+this->fw-100,this->fy, 50, 30, 0xFF888888);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+10,this->fx+this->fw-20-50,this->fy+10,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+10,this->fx+this->fw-30-50,this->fy+20,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+20,this->fx+this->fw-20-50,this->fy+20,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-20-50,this->fy+10,this->fx+this->fw-20-50,this->fy+20,2,0xFFFFFFFF);
                } else {
                    // Restore
                    win->graphics.rect(this->fx+this->fw-100,this->fy, 50, 30, 0xFF888888);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+10+3,this->fx+this->fw-20-3-50,this->fy+10+2,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+10+3,this->fx+this->fw-30-50,this->fy+20,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50,this->fy+20,this->fx+this->fw-20-3-50,this->fy+20,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-20-50-3,this->fy+10+3,this->fx+this->fw-20-50-3,this->fy+20,2,0xFFFFFFFF);

                    win->graphics.line_thick(this->fx+this->fw-30-50+3,this->fy+10,this->fx+this->fw-20-50,this->fy+10,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50+3,this->fy+10,this->fx+this->fw-30-50+3,this->fy+20-(10-3),2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-30-50+(10-3),this->fy+20-3,this->fx+this->fw-20-50,this->fy+20-3,2,0xFFFFFFFF);
                    win->graphics.line_thick(this->fx+this->fw-20-50,this->fy+10,this->fx+this->fw-20-50,this->fy+20-3,2,0xFFFFFFFF);
                };

                // Minimize
                win->graphics.rect(this->fx+this->fw-150,this->fy, 50, 30, 0xFF777777);
                win->graphics.line_thick(this->fx+this->fw-30-100,this->fy+15,this->fx+this->fw-20-100,this->fy+15,2,0xFFFFFFFF);
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
                            if(this->fw-x<50){
                                win->destroy();
                            } else if(this->fw-x<100){
                                if(this->__is_maximized){
                                    win->restore();
                                } else {
                                    win->maximize();
                                };
                                this->__is_maximized = !this->__is_maximized;
                            } else if(this->fw-x<150){
                                win->minimize();
                            } else {
                                win->start_move();
                            };
                        } else if(event == lunaris::mouse::second) {
                            win->show_window_menu(x, y);
                        }
                    };
                } else if(this->child != NULL){
                    if(win->width+win->height-20<x+y){
                        if(pressed && event == lunaris::mouse::first) win->start_resize();
                    } else {
                        this->child->mouse_event(win, x-this->fx+this->fx, y-this->fy-30, pressed, dx, dy, event);
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