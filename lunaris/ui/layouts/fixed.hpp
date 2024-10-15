namespace lunaris::ui {
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
};