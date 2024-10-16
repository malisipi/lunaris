namespace lunaris::ui {
    const uint32_t button_id = request_new_id();
    typedef struct button:widget {
        virtual uint32_t get_type(){
            return button_id;
        };
        std::string text = "";
        void draw(lunaris::window* win, uint32_t* buffer){
            win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 10, 0xFFE6E6E6);
            const int text_line_height = std::min(this->fh, 20);
            std::pair<int, int> text_size = win->graphics.text_bounding_area(text_line_height, this->text.c_str());
            win->graphics.text(this->fx+(this->fw-text_size.first)/2, this->fy+(this->fh-text_size.second)/2, text_line_height, this->text.c_str(), 0xFF000000);
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
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
            const int text_size = this->text.size();
            if((event == keyboard::pressed || event == keyboard::repeated) && (key == keycode::enter || key == keycode::np_enter || key == keycode::space)){
                this->mouse_event(win, 0, 0, 1, 0, 0, lunaris::mouse::first);
            };
        };
    } button;
};