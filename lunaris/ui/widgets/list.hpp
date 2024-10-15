namespace lunaris::ui {
    const uint32_t list_id = request_new_id();
    typedef struct list:widget {
        virtual uint32_t get_type(){
            return list_id;
        };
        scrollbar* vscrollbar = NULL;
        list(){
            this->vscrollbar = new lunaris::ui::scrollbar;
        };
        ~list(){
            delete this->vscrollbar;
        };
        virtual void set_hovering(bool is_hovering){
            this->is_hovering = is_hovering;
            if(!this->is_hovering) this->vscrollbar->set_hovering(false);
        };
        std::vector<std::string> items;
        void draw(lunaris::window* win, uint32_t* buffer){
            const int items_size = this->items.size();
            win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFFFFFFFF);
            for(int text_y=0;text_y<items_size;text_y++){
                win->graphics.rounded_rect_empty(this->fx, this->fy+30*text_y-30*this->vscrollbar->value, this->fw-16, 30, 5, 1, 0xFF000000);
                win->graphics.text(this->fx+4, this->fy+5+30*text_y-30*this->vscrollbar->value, 20, this->items[text_y].c_str(), 0xFF000000);
            };
            // Vscrollbar 
            this->vscrollbar->max = items_size;
            this->vscrollbar->view = this->fh/30;
            //TODO: Make sure value doesn't exceed max value: this->vscrollbar->value;
            this->vscrollbar->__set_f_size(this->fx+this->fw-16, this->fy+0, 16, this->fh);
            this->vscrollbar->draw(win, buffer);
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            // Vscrollbar
            this->vscrollbar->set_hovering(x>this->fw-16);
            if(x>this->fw-16 || event == lunaris::mouse::scroll){
                return this->vscrollbar->mouse_event(win, x-this->fw, y, pressed, dx, dy, event);
            };
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } list;
}