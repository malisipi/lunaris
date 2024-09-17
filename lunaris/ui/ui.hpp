#include "anchor.hpp"

namespace lunaris::ui {
    int last_registered_id = 0;

    int request_new_id(){
        return ++last_registered_id;
    };

    typedef struct widget {
        // Relative to layout
        int rx = 0;
        int ry = 0;
        int rw = 0;
        int rh = 0;
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
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){ /*printf("Mouse at %f, %f\n", x, y);*/ };
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
    };

    const uint32_t button_id = request_new_id();
    typedef struct button:widget {
        virtual uint32_t get_type(){
            return button_id;
        };
        std::string text = "";
        void draw(lunaris::window* win, uint32_t* buffer){
            win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFFFFFFFF);
            win->graphics.text(this->fx, this->fy, std::min(this->fh, 20), this->text.c_str(), 0xFFFF0000);
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){
            printf("Click at %f, %f\n", x, y);
            if(pressed)
                win->focused = (void*)this;
        };
    };

    const uint32_t textbox_id = request_new_id();
    typedef struct textbox:widget {
        virtual uint32_t get_type(){
            return textbox_id;
        };
        std::string text = "";
        bool multiline = false;
        uint64_t pos = 0;
        void draw(lunaris::window* win, uint32_t* buffer){
            win->graphics.rect(this->fx, this->fy, this->fw, this->fh, 0xFFFFFFFF);
            char the_text[this->text.size()+1];
            sprintf(the_text, "%s¦%s", this->text.substr(0,this->pos).c_str(), this->text.substr(this->pos,this->text.size()-this->pos).c_str());
            win->graphics.text(this->fx, this->fy, std::min(this->fh, 20), the_text, 0xFFFF0000);
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){
            printf("Click at %f, %f\n", x, y);
            if(pressed)
                win->focused = (void*)this;
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
        }
    };

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
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){
            widget* last_child = NULL;
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                widget* child = *child_ptr;
                if(((child->fx-this->fx<=x) && (child->fx-this->fx+child->fw>=x)) && ((child->fy-this->fy<=y) && (child->fy-this->fy+child->fh>=y))){
                    last_child = child;
                };
            };
            if(last_child != NULL){
                last_child->mouse_event(win, x-last_child->fx+this->fx, y-last_child->fy+this->fy, pressed, event);
            };
        };
    };

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
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){
            int grid_index = -1;
            int grid_planned_count = row_count*col_count;
            for(std::vector<widget*>::iterator child_ptr = this->childs.begin(); child_ptr != this->childs.end(); child_ptr++){
                if(++grid_index >= grid_planned_count) break; // Exceed the planned area of grid, so i can not place them if
                widget* child = *child_ptr;
                if(((child->fx-this->fx<=x) && (child->fx-this->fx+child->fw>=x)) && ((child->fy-this->fy<=y) && (child->fy-this->fy+child->fh>=y))){
                    child->mouse_event(win, x-child->fx+this->fx, y-child->fy+this->fy, pressed, event);
                    return;
                }
            };
        };
    };

    namespace query_type {
        typedef enum query {
            width,
            height
        };
    }

    const uint32_t dynamic_id = request_new_id();
    typedef struct dynamic:layout {
        virtual uint32_t get_type(){
            return grid_id;
        };
        std::map<int, widget*> layouts;
        query_type::query _type = query_type::width;
        template <typename T> void place(T child, int max_size) {
            this->layouts[max_size] = (widget*)child;
        };
        void ___get_child(widget** child){
            for(std::map<int, widget*>::iterator _layout = this->layouts.begin(); _layout != this->layouts.end(); _layout++){
                if((_type == query_type::width && _layout->first < this->fw)
                    ||(_type == query_type::height && _layout->first < this->fh)){
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
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, lunaris::mouse::mouse event){
            widget* child = NULL;
            ___get_child(&child);
            if(child == NULL) return;
            child->mouse_event(win, x, y, pressed, event);
        };
    };

};
