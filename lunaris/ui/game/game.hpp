namespace lunaris::ui::game {
    const uint32_t node_id = request_new_id();
    typedef struct node:widget {
        /* Node is a type like a widget.
         * Cannot be drawable at itself, it has no pre-defined event handlers.
         * It is a type to extend it to create nodes. Should be avoided using it as raw.
         */
        virtual uint32_t get_type(){
            return node_id;
        };
        virtual bool is_drawable(){ // Will never drawed, draw functions never be called in game_surface
            return false;
        };
        std::vector<node*> childs;
        template <typename T> void place(T child, int x, int y, int w, int h) {
            this->childs.push_back((node*)child);
            child->__set_size(x, y, w, h);
        };
    } node;

    const uint32_t game_surface_id = request_new_id();
    typedef struct game_surface:node {
        virtual uint32_t get_type(){
            return game_surface_id;
        };
        int render_x = 0;
        int render_y = 0;
        int render_width = 100;
        int render_height = 100;
        uint32_t void_color = 0xFF000000;
        uint32_t bg_color = 0xFFFFFFFF;
        void __draw_nodes(lunaris::window* win, uint32_t* buffer){
            this->childs[0]->__set_f_size(this->convert_x(childs[0]->rx), this->convert_y(childs[0]->ry), this->convert_w(childs[0]->rw), this->convert_h(childs[0]->rh));
            this->childs[0]->draw(win, buffer);
        };
        /* This node is only node that can be drawed since it's not drawable.
         * Drawing function of the surface will be called by layouts, not by nodes.
         * So this node should be drawable by layouts, not nodes.
         * Drawing the node under another node should be avoided.
         * Because the node designed as ROOT OF OTHER NODES.
         */
        __uint128_t last_event = __get_event_time(); // Will be used calculating delta
        void draw(lunaris::window* win, uint32_t* buffer){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                int calculated_render_width = this->fh*render_aspect_ratio;
                int void_size = (this->fw-calculated_render_width)/2;
                win->graphics.rect(this->fx+void_size, this->fy, calculated_render_width, this->fh, this->bg_color);
                if(this->game_process != NULL){
                    __uint128_t new_last_event = __get_event_time();
                    this->game_process(win, this, (float)(new_last_event-this->last_event)/1000);
                    this->last_event = new_last_event;
                };
                this->__draw_nodes(win, buffer);
                win->graphics.rect(this->fx, this->fy, void_size, this->fh, this->void_color);
                win->graphics.rect(this->fx+this->fw-void_size, this->fy, void_size, this->fh, this->void_color);
            } else {
                int calculated_render_height = this->fw/render_aspect_ratio;
                int void_size = (this->fh-calculated_render_height)/2;
                win->graphics.rect(this->fx, this->fy+void_size, this->fw, calculated_render_height, this->bg_color);
                if(this->game_process != NULL){
                    __uint128_t new_last_event = __get_event_time();
                    this->game_process(win, this, (float)(new_last_event-this->last_event)/1000);
                    this->last_event = new_last_event;
                };
                this->__draw_nodes(win, buffer);
                win->graphics.rect(this->fx, this->fy, this->fw, void_size, this->void_color);
                win->graphics.rect(this->fx, this->fy+this->fh-void_size, this->fw, void_size, this->void_color);
            };
        };
        int convert_x(int x){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                int calculated_render_width = this->fh*render_aspect_ratio;
                int void_size = (this->fw-calculated_render_width)/2;
                return void_size+this->convert_w(x)+this->fx;
            } else {
                return this->convert_w(x)+this->fx;
            };
        };
        int convert_y(int y){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                return this->convert_h(y)+this->fy;
            } else {
                int calculated_render_height = this->fw/render_aspect_ratio;
                int void_size = (this->fh-calculated_render_height)/2;
                return void_size+this->convert_h(y)+this->fy;
            };
        };
        int convert_w(int w){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                int calculated_render_width = this->fh*render_aspect_ratio;
                int void_size = this->fw-calculated_render_width;
                return w*(this->fw-void_size)/this->render_width;
            } else {
                return w*this->fw/this->render_width;
            };
        };
        int convert_h(int h){
            float aspect_ratio = ((float)this->fw)/((float)this->fh);
            float render_aspect_ratio = ((float)this->render_width)/((float)this->render_height);
            if(render_aspect_ratio<=aspect_ratio){
                return h*this->fh/this->render_height;
            } else {
                int calculated_render_height = this->fw/render_aspect_ratio;
                int void_size = this->fh-calculated_render_height;
                return h*(this->fh-void_size)/this->render_height;
            };
        };
        void (*game_process)(lunaris::window* win, game_surface* surface, float delta) = NULL;
        std::map<lunaris::keycode::keycode, bool> key_states;
        bool is_pressed(lunaris::keycode::keycode keycode){
            return this->key_states[keycode];
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            win->focused = this;
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
            if(event == lunaris::keyboard::pressed) {
                this->key_states[key] = true;
            } else if(event == lunaris::keyboard::released) {
                this->key_states[key] = false;
            }
        };
    } game_surface;

    const uint32_t sprite_id = request_new_id();
    typedef struct sprite:widget {
        virtual uint32_t get_type(){
            return sprite_id;
        };
        virtual bool is_drawable(){ // Will never drawed, draw functions never be called in game_surface
            return true;
        };
        void draw (lunaris::window* win, uint32_t* buffer){
            win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFF0000FF);
        };
    } sprite;
};