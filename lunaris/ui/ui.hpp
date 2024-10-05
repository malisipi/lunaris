#include "anchor.hpp"

namespace lunaris::ui {
    int last_registered_id = 0;

    int request_new_id(){
        return ++last_registered_id;
    };

    __uint128_t __get_event_time(){
        return (__uint128_t)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    typedef struct widget {
        // Relative to layout
        float rx = 0;
        float ry = 0;
        float rw = 0;
        float rh = 0;
        // Relative to layout, should be used while storing anchor points
        bool used_anchor = false;
        const char* ax = 0;
        const char* ay = 0;
        const char* aw = 0;
        const char* ah = 0;
        // Sticked to window
        int fx = 0;
        int fy = 0;
        int fw = 0;
        int fh = 0;
        bool is_hovering = false; // all layouts and semi-layout widgets (the widgets that has widgets)
        virtual void set_hovering(bool is_hovering){
            this->is_hovering = is_hovering;
        };
        //bool can_exceed_size;
        virtual uint32_t get_type(){
            return 0;
        };
        virtual bool is_layout(){
            return false;
        };
        /*
         *        int ver_margin = 0;
         *        int hor_margin = 0;
         *        int ver_padding = 0;
         *        int hor_padding = 0;
         *        int ver_border = 0;
         *        int hor_border = 0;
         */
        virtual void draw(lunaris::window* win, uint32_t* buffer){};
        void __set_size(int x, int y, int w, int h){
            this->rx = x;
            this->ry = y;
            this->rw = w;
            this->rh = h;
        };
        void __set_f_size(int x, int y, int w, int h){
            this->fx = x;
            this->fy = y;
            this->fw = w;
            this->fh = h;
        };
        void __set_a_size(const char* x, const char* y, const char* w, const char* h){
            this->used_anchor = true;
            this->ax = x;
            this->ay = y;
            this->aw = w;
            this->ah = h;
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){ /*printf("Mouse at %f, %f\n", x, y);*/ };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } widget;

    const uint32_t layout_id = request_new_id();
    typedef struct layout:widget {
        std::vector<widget*> childs;
        virtual bool is_layout(){
            return true;
        };
        virtual uint32_t get_type(){
            return layout_id;
        };
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){};
    } layout;

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
                    win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 5, 0xFFF1F1F1);
                    if(this->is_hovering && this->_mouse_y<16) win->graphics.rounded_rect(this->fx, this->fy, this->fw, 16, 5, 0xFFCCCCCC);
                    win->graphics.rect(this->fx+this->fw/2-4, this->fy+4, 8, 8, 0xFFA0A0A0);
                    float size = (float)(this->fh-36)/(float)(this->max);
                    uint32_t hovered_color = 0xFFC0C0C0;
                    if(this->is_hovering && ((18+size*this->value)<this->_mouse_y) && ((18+size*this->value+size*this->view)>this->_mouse_y)) hovered_color = 0xFFA0A0A0;
                    win->graphics.rounded_rect(this->fx, this->fy+18+size*this->value, this->fw, size*this->view, 5, hovered_color);
                    if(this->is_hovering && this->fh-this->_mouse_y<16) win->graphics.rounded_rect(this->fx, this->fy+this->fh-16, this->fw, 16, 5, 0xFFCCCCCC);
                    win->graphics.rect(this->fx+this->fw/2-4, this->fy+this->fh-12, 8, 8, 0xFFA0A0A0);
                } else { // horizontal
                    win->graphics.rounded_rect(this->fx, this->fy, this->fw, this->fh, 5, 0xFFF1F1F1);
                    if(this->is_hovering && this->_mouse_x<16) win->graphics.rounded_rect(this->fx, this->fy, 16, this->fh, 5, 0xFFCCCCCC);
                    win->graphics.rect(this->fx+4, this->fy+this->fh/2-4, 8, 8, 0xFFA0A0A0);
                    float size = (float)(this->fw-36)/(float)(this->max);
                    uint32_t hovered_color = 0xFFC0C0C0;
                    if(this->is_hovering && ((18+size*this->value)<this->_mouse_x) && ((18+size*this->value+size*this->view)>this->_mouse_x)) hovered_color = 0xFFA0A0A0;
                    win->graphics.rounded_rect(this->fx+18+size*this->value, this->fy, size*this->view, this->fh, 5, hovered_color);
                    if(this->is_hovering && this->fw-this->_mouse_x<16) win->graphics.rounded_rect(this->fx+this->fw-16, this->fy, 16, this->fh, 5, 0xFFCCCCCC);
                    win->graphics.rect(this->fx+this->fw-12, this->fy+this->fh/2-4, 8, 8, 0xFFA0A0A0);
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
                if(pressed){
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
                if(pressed){
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

    int __how_many_char(const char* text, char schar) {
        int count = 0;
        uint64_t text_len = strlen(text);
        for(int i=0; i<text_len;i++){
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
        void draw(lunaris::window* win, uint32_t* buffer){
            win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFFFFFFFF);
            char the_text[this->text.size()+1];
            sprintf(the_text, "%s¦%s", this->text.substr(0,this->pos).c_str(), this->text.substr(this->pos,this->text.size()-this->pos).c_str());
            win->graphics.text(this->fx, this->fy-this->vscrollbar->value*20, std::min(this->fh, 20), the_text, 0xFFFF0000);
            // Vscrollbar 
            this->vscrollbar->max = __how_many_char(this->text.c_str(), '\n');
            this->vscrollbar->view = this->fh/20-1;
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
            if(pressed){
                win->focused = (void*)this;
                this->pos = win->graphics.font->get_clicking_pos(20, this->text.c_str(), x, y, this->vscrollbar->value);
            };
        };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
            const int text_size = this->text.size();
            if(event == keyboard::pressed || event == keyboard::repeated){
                if(this->pos>=text_size) this->pos = text_size;
                if(this->pos<0) this->pos = 0;

                if(key == keycode::enter || key == keycode::np_enter){
                    if(this->multiline) this->text.insert(this->pos++, "\n");
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

    const uint32_t fixed_id = request_new_id();
    typedef struct fixed:layout {
        virtual uint32_t get_type(){
            return fixed_id;
        };
        template <typename T> void place(T child, int x, int y, int w, int h) {
            this->childs.push_back((widget*)child);
            child->__set_size(x, y, w, h);
        };
        template <typename T> void place(T child, std::string x, std::string y, std::string w, std::string h) {
            this->childs.push_back((widget*)child);
            child->__set_a_size(x.c_str(), y.c_str(), w.c_str(), h.c_str());
        };
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                widget* child = *child_ptr;
                if(child->used_anchor){
                    child->__set_f_size(anchor::calc(this->fw, this->fh, child->ax)+this->fx, anchor::calc(this->fw, this->fh, child->ay)+this->fy, anchor::calc(this->fw, this->fh, child->aw), anchor::calc(this->fw, this->fh, child->ah));
                } else {
                    child->__set_f_size(child->rx+this->fx, child->ry+this->fy, child->rw, child->rh);
                };
                if(child->is_layout()){
                    ((layout*)child)->calculate_n_draw(win, buffer);
                } else {
                    child->draw(win, buffer);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            widget* last_child = NULL;
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                widget* child = *child_ptr;
                if(((child->fx-this->fx<=x) && (child->fx-this->fx+child->fw>=x)) && ((child->fy-this->fy<=y) && (child->fy-this->fy+child->fh>=y))){
                    last_child = child;
                };
            };
            if(last_child != NULL){
                last_child->mouse_event(win, x-last_child->fx+this->fx, y-last_child->fy+this->fy, pressed, dx, dy, event);
            };
        };
    } fixed;

    const uint32_t grid_id = request_new_id();
    typedef struct grid:layout {
        virtual uint32_t get_type(){
            return grid_id;
        };
        uint32_t col_count = 1;
        uint32_t row_count = 1;
        uint32_t col_gap = 0;
        uint32_t row_gap = 0;
        uint32_t col_margin = 0;
        uint32_t row_margin = 0;
        // bool auto_place = true; If not place will have a option to set grid parts.
        // TODO: resizable grid is important feature. Implement it later
        template <typename T> void place(T child) {
            this->childs.push_back((widget*)child);
        };
        void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            int grid_index = -1;
            int grid_planned_count = row_count*col_count;
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                if(++grid_index >= grid_planned_count) break; // Exceed the planned area of grid, so i can not place them if
                widget* child = *child_ptr;
                const int col_index = (grid_index)%this->col_count;
                const int row_index = ((int)grid_index)/this->col_count;
                const int col_width = (this->fw-this->col_margin*2-this->col_gap*(this->col_count-1))/this->col_count;
                const int row_height = (this->fh-this->row_margin*2-this->row_gap*(this->row_count-1))/this->row_count;

                child->__set_f_size(this->fx+this->col_margin+(col_width+col_gap)*col_index, this->fy+this->row_margin+(row_height+row_gap)*row_index,
                                    col_width, row_height);
                if(child->is_layout()){
                    ((layout*)child)->calculate_n_draw(win, buffer);
                } else {
                    child->draw(win, buffer);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            int grid_index = -1;
            int grid_planned_count = row_count*col_count;
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                if(++grid_index >= grid_planned_count) break; // Exceed the planned area of grid, so i can not place them if
                widget* child = *child_ptr;
                if(((child->fx-this->fx<=x) && (child->fx-this->fx+child->fw>=x)) && ((child->fy-this->fy<=y) && (child->fy-this->fy+child->fh>=y))){
                    child->mouse_event(win, x-child->fx+this->fx, y-child->fy+this->fy, pressed, dx, dy, event);
                    child->set_hovering(true);
                    //return;
                } else {
                    child->set_hovering(false);
                }
            };
        };
    } grid;

    namespace query {
        typedef enum query {
            width,
            height
        } query;
    }

    const uint32_t dynamic_id = request_new_id();
    typedef struct dynamic:layout {
        virtual uint32_t get_type(){
            return grid_id;
        };
        std::map<int, widget*> layouts;
        query::query _type = query::width;
        template <typename T> void place(T child, int max_size) {
            this->layouts[max_size] = (widget*)child;
        };
        void ___get_child(widget** child){
            for(std::map<int, widget*>::iterator _layout = this->layouts.begin(); _layout != this->layouts.end(); _layout++){
                if((_type == query::width && _layout->first < this->fw)
                    ||(_type == query::height && _layout->first < this->fh)){
                    *child = (widget*)(_layout->second);
                    };
            };
        };
        void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            widget* child = NULL;
            ___get_child(&child);
            if(child == NULL) return;
            child->__set_f_size(this->fx, this->fy, this->fw, this->fh);
            if(child->is_layout()){
                ((layout*)child)->calculate_n_draw(win, buffer);
            } else {
                child->draw(win, buffer);
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            widget* child = NULL;
            ___get_child(&child);
            if(child == NULL) return;
            child->mouse_event(win, x, y, pressed, dx, dy, event);
        };
    } dynamic;

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

    void draw_handler(lunaris::window* win, uint32_t* buffer){
        win->graphics.rect(0, 0, win->width, win->height, 0xFFFAFAFA);

        lunaris::ui::layout* layout = (lunaris::ui::layout*)win->layout;
        layout->__set_f_size(0, 0, win->width, win->height);
        layout->calculate_n_draw(win, buffer);

        /*if(win->focused != NULL) {
            win->graphics.rect_empty(((lunaris::ui::widget*)win->focused)->fx-1, ((lunaris::ui::widget*)win->focused)->fy-1, ((lunaris::ui::widget*)win->focused)->fw+2, ((lunaris::ui::widget*)win->focused)->fh+2, 2, 0xFFAAAAAA);
        };*/
    };

    void mouse_handler(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
        lunaris::ui::layout* layout = (lunaris::ui::layout*)win->layout;
        layout->mouse_event(win, x, y, pressed, dx, dy, event);
    };

    void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
        if(win->focused != NULL) ((lunaris::ui::widget*)win->focused)->keyboard_handler(win, new_char, key, modifiers, event);
    };

    void assign_handlers(lunaris::window* win){
        win->draw_handler = lunaris::ui::draw_handler;
        win->mouse_handler = lunaris::ui::mouse_handler;
        win->keyboard_handler = lunaris::ui::keyboard_handler;
    };
};
