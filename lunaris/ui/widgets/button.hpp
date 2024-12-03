namespace lunaris::ui {
    const uint32_t button_id = request_new_id();
    typedef struct button:widget {
        virtual uint32_t get_type(){
            return button_id;
        };
        std::string text = "";
        std::string icon_text = "";
        int icon_size = 30;
        bool prefer_icon = true;
        bool quiet = false; // Styling
        lunaris::layer* icon = NULL;
        void draw(lunaris::window* win, uint32_t* buffer){
            if(!this->quiet){
                win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 10, win->colors->other_color);
            };
            const int text_line_height = std::min(this->fh, 20);
            const std::pair<int, int> text_size = win->graphics.text_bounding_area(text_line_height, this->text.c_str());
            const int icon_text_line_height = std::min(this->fh, icon_size);
            const std::pair<int, int> icon_text_size = win->graphics.text_bounding_area(icon_text_line_height, this->icon_text.c_str());
            /* | icon/4 pad | icon | centered text | */
            if(this->icon_text == "" && this->icon == NULL){
                win->graphics.text(this->fx+(this->fw-text_size.first)/2, this->fy+(this->fh-text_size.second)/2, text_line_height, this->text.c_str(), win->colors->text_color);
            } else {
                if(this->fw<100) {
                    if(this->prefer_icon) {
                        if(this->icon_text != ""){
                            win->graphics.text(this->fx+(this->fw-icon_text_size.first)/2, this->fy+(this->fh-icon_text_size.second)/2, icon_text_size.second, this->icon_text.c_str(), win->colors->text_color);
                        } else { // this->icon != NULL
                            // draw image to center
                        }; 
                    } else {
                        win->graphics.text(this->fx+(this->fw-text_size.first)/2, this->fy+(this->fh-text_size.second)/2, text_line_height, this->text.c_str(), win->colors->text_color);
                    };
                } else {
                    if(this->text != ""){
                        if(this->icon_text != ""){
                            win->graphics.text(this->fx+icon_size/4, this->fy+(this->fh-icon_text_size.second)/2, icon_text_size.second, this->icon_text.c_str(), win->colors->text_color);
                        } else { // this->icon != NULL
                            // draw image to left
                        };
                        win->graphics.text(this->fx+(this->fw-(icon_size*5/4)-text_size.first)/2+(icon_size*5/4), this->fy+(this->fh-text_size.second)/2, text_line_height, this->text.c_str(), win->colors->text_color);
                    } else {
                        if(this->icon_text != ""){
                            win->graphics.text(this->fx+(this->fw-icon_text_size.first)/2, this->fy+(this->fh-icon_text_size.second)/2, icon_text_size.second, this->icon_text.c_str(), win->colors->text_color);
                        } else { // this->icon != NULL
                            // draw image to center
                        }; 
                    }
                }
            };
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