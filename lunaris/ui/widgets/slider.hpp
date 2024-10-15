namespace lunaris::ui {
    const uint32_t slider_id = request_new_id();
    typedef struct slider:widget {
        virtual uint32_t get_type(){
            return slider_id;
        };
        int max = 1;
        int min = 0;
        int value = 0;
        int step = 1;
        bool horizontal = true;
        bool is_clicked = false;
        void draw(lunaris::window* win, uint32_t* buffer){
            if(this->horizontal){
                win->graphics.rounded_rect(this->fx+10, this->fy+this->fh/2-4, this->fw-20, 8, 4, 0xFF444444);
                win->graphics.circle(this->fx+10+(this->value-this->min)*this->fw/(this->max-this->min+1), this->fy+this->fh/2, 10, 0xFFCCCCCC);
            } else {
                win->graphics.rounded_rect(this->fx+this->fw/2-4, this->fy+10, 8, this->fh-20, 4, 0xFF444444);
                win->graphics.circle(this->fx+this->fw/2, this->fy+10+(this->value-this->min)*this->fh/(this->max-this->min+1), 10, 0xFFCCCCCC);
            }
        };
        void(*input_handler)(lunaris::window*, lunaris::ui::widget*, int prev_value) = NULL;
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            if(pressed && event == lunaris::mouse::first){
                this->is_clicked = true;
                win->focused = (void*)this;
            };

            if(this->is_clicked){
                if(this->horizontal){
                    const int click_point = x-10;
                    const int max_clickable_width = this->fw-20;
                    if(max_clickable_width<0 || this->min>=this->max) return;
                    int range = this->max-this->min+1;
                    int prev_value = this->value;
                    this->value = this->min + click_point*range/max_clickable_width;
                    if(this->value>this->max) this->value = this->max;
                    int fraction = (this->value-this->min)%this->step;
                    if(fraction != 0){ // TODO: Do it more accurate by rounding with `step` value
                        this->value -= fraction;
                    };
                    if(this->value != prev_value){
                        if(this->input_handler) this->input_handler(win, this, prev_value);
                    };
                    if(!pressed && event != lunaris::mouse::motion){
                        this->is_clicked = false;
                    };
                } else {
                    const int click_point = y-10;
                    const int max_clickable_height = this->fh-20;
                    if(max_clickable_height<0 || this->min>=this->max) return;
                    int range = this->max-this->min+1;
                    int prev_value = this->value;
                    this->value = this->min + click_point*range/max_clickable_height;
                    if(this->value>this->max) this->value = this->max;
                    int fraction = (this->value-this->min)%this->step;
                    if(fraction != 0){ // TODO: Do it more accurate by rounding with `step` value
                        this->value -= fraction;
                    };
                    if(this->value != prev_value){
                        if(this->input_handler) this->input_handler(win, this, prev_value);
                    };
                    if(!pressed && event != lunaris::mouse::motion){
                        this->is_clicked = false;
                    };
                }
            }
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
            if(event == lunaris::keyboard::released) return;
            
            int prev_value = this->value;
            if(key == lunaris::keycode::arrow_left || key == lunaris::keycode::arrow_up){
                this->value -= this->step;
            } else if(key == lunaris::keycode::arrow_right || key == lunaris::keycode::arrow_down){
                this->value += this->step;
            };
            if(this->value<this->min || this->value>this->max){
                this->value = prev_value;
            } else if(this->input_handler) this->input_handler(win, this, prev_value);
        }
    } slider;
};