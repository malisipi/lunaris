#pragma once

namespace lunaris::ui {
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
        void draw (lunaris::window* win, uint32_t* buffer){
            widget* child = NULL;
            ___get_child(&child);
            if(child == NULL) return;
            child->__set_f_size(this->fx, this->fy, this->fw, this->fh);
            child->draw(win, buffer);
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            widget* child = NULL;
            ___get_child(&child);
            if(child == NULL) return;
            child->mouse_event(win, x, y, pressed, dx, dy, event);
        };
    } dynamic;
}