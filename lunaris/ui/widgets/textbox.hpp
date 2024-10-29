namespace lunaris::ui {
    int __how_many_char(const char* text, char schar) {
        int count = 0;
        uint64_t text_len = strlen(text);
        for(uint64_t i=0; i<text_len;i++){
            if(text[i] == schar) count++;
        };
        return count;
    };

    const uint32_t textbox_id = request_new_id();
    typedef struct textbox:widget {
        virtual uint32_t get_type(){
            return textbox_id;
        };
        std::string text = "";
        std::string placeholder = "";
        bool multiline = false;
        uint64_t pos = 0;
        scrollbar* vscrollbar = NULL;
        textbox(){
            this->vscrollbar = new lunaris::ui::scrollbar;
        };
        ~textbox(){
            delete this->vscrollbar;
        };
        virtual void set_hovering(bool is_hovering){
            this->is_hovering = is_hovering;
            if(!this->is_hovering) this->vscrollbar->set_hovering(false);
        };
        void(*submit_handler)(lunaris::window*, lunaris::ui::widget*) = NULL;
        // Only available at non-multiline textbox.
        // This event will only fired when clicked enter
        void draw(lunaris::window* win, uint32_t* buffer){ // TODO: Fix text cursor
            if(this->multiline){
                win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 5, win->colors->other_color);
                win->graphics.rounded_rect_empty(this->fx, this->fy, this->fw, this->fh, 5, 2, win->colors->border_color);
                // TODO: Support placeholder
                char the_text[this->text.size()+1];
                sprintf(the_text, "%s¦%s", this->text.substr(0,this->pos).c_str(), this->text.substr(this->pos,this->text.size()-this->pos).c_str());
                win->graphics.text(this->fx, this->fy-this->vscrollbar->value*20, std::min(this->fh, 20), the_text, win->colors->text_color);
                // Vscrollbar 
                this->vscrollbar->max = __how_many_char(this->text.c_str(), '\n');
                this->vscrollbar->view = this->fh/20-1;
                //TODO: Make sure value doesn't exceed max value: this->vscrollbar->value;
                this->vscrollbar->__set_f_size(this->fx+this->fw-16, this->fy+0, 16, this->fh);
                this->vscrollbar->draw(win, buffer);
            } else {
                win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFFFFFFFF);
                if(this->text != ""){
                    char the_text[this->text.size()+1];
                    sprintf(the_text, "%s¦%s", this->text.substr(0,this->pos).c_str(), this->text.substr(this->pos,this->text.size()-this->pos).c_str());
                    win->graphics.text(this->fx, this->fy+(this->fh-std::min(this->fh, 20))/2, std::min(this->fh, 20), the_text, 0xFF000000);
                } else {
                    win->graphics.text(this->fx+5, this->fy+(this->fh-std::min(this->fh, 20))/2, std::min(this->fh, 20), this->placeholder.c_str(), 0xFF444444);
                    win->graphics.text(this->fx, this->fy+(this->fh-std::min(this->fh, 20))/2, std::min(this->fh, 20), "¦", 0xFF000000);
                }
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            // Vscrollbar
            this->vscrollbar->set_hovering(x>this->fw-16);
            if(x>this->fw-16 || event == lunaris::mouse::scroll){
                return this->vscrollbar->mouse_event(win, x-this->fw, y, pressed, dx, dy, event);
            };
            if(pressed){
                win->focused = (void*)this;
                if(this->multiline){
                    this->pos = win->graphics.font->get_clicking_pos(20, this->text.c_str(), x, y, this->vscrollbar->value);
                } else {
                    this->pos = win->graphics.font->get_clicking_pos(20, this->text.c_str(), x, -1, this->vscrollbar->value);
                };
            };
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
            const int text_size = this->text.size();
            if(event == keyboard::pressed || event == keyboard::repeated){
                if(this->pos>=text_size) this->pos = text_size;
                if(this->pos<0) this->pos = 0;

                if(key == keycode::enter || key == keycode::np_enter){
                    if(this->multiline){
                        this->text.insert(this->pos++, "\n");
                    } else {
                        if(this->submit_handler != NULL){
                            this->submit_handler(win, this);
                        };
                    };
                } else if(key == keycode::backspace){
                    if(this->pos>0) this->text.erase(--this->pos, 1); // TODO: Support Unicode UTF-8
                } else if(key == keycode::arrow_left){
                    if(this->pos>0) this->pos--; // TODO: Support Unicode UTF-8
                } else if(key == keycode::arrow_right){
                    if(this->pos<text_size) this->pos++; // TODO: Support Unicode UTF-8
                } else if(new_char != NULL && new_char[0] != '\0'){
                    if(event == keyboard::pressed) {
                        //this->text += new_char;
                        this->text.insert(this->pos, new_char);
                        this->pos += strlen(new_char);
                    };
                };
            };
        };
    } textbox;
};