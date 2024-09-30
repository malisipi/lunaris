namespace lunaris::ui::game {
    const uint32_t game_surface_id = request_new_id();
    typedef struct game_surface:widget {
        virtual uint32_t get_type(){
            return game_surface_id;
        };
        int render_width = 100;
        int render_height = 100;
        uint32_t void_color = 0xFF000000;
        void draw(lunaris::window* win, uint32_t* buffer){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                int calculated_render_width = this->fh*render_aspect_ratio;
                int void_size = (this->fw-calculated_render_width)/2;
                win->graphics.rect(this->fx+void_size, this->fy, calculated_render_width, this->fh, 0xFFFF0000);
                win->graphics.rect(this->fx, this->fy, void_size, this->fh, this->void_color);
                win->graphics.rect(this->fx+this->fw-void_size, this->fy, void_size, this->fh, this->void_color);
            } else {
                int calculated_render_height = this->fw/render_aspect_ratio;
                int void_size = (this->fh-calculated_render_height)/2;
                win->graphics.rect(this->fx, this->fy+void_size, this->fw, calculated_render_height, 0xFFFF0000);
                win->graphics.rect(this->fx, this->fy, this->fw, void_size, this->void_color);
                win->graphics.rect(this->fx, this->fy+this->fh-void_size, this->fw, void_size, this->void_color);
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){};
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } game_surface;
}