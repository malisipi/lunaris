#pragma once

namespace lunaris::ui {
    const uint32_t navigation_id = request_new_id();

    typedef struct navigation_item {
        lunaris::ui::widget* widget;
        int minimal_height;
        int opened_height;
    } navigation_item;
    
    typedef struct navigation:layout {
        virtual uint32_t get_type(){
            return navigation_id;
        };
        widget* child = NULL;
        bool as_overlay = true;
        int minimal_width = 50;
        int opened_width = 200;
        bool is_opened = false;
        std::vector<navigation_item*> items;
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            int side_width = this->minimal_width; // this will show the navigation as overlay
            if(!as_overlay) side_width = (this->is_opened) ? this->opened_width : this->minimal_width;

            if(this->child != NULL){
                this->child->__set_f_size(this->child->rx+this->fx+side_width, this->child->ry+this->fy, this->fw-side_width, this->fh);
                if(this->child->is_layout()){
                    ((layout*)this->child)->calculate_n_draw(win, buffer);
                } else {
                    this->child->draw(win, buffer);
                };
            };
            win->graphics.rect(this->fx, this->fy, (this->is_opened) ? this->opened_width : this->minimal_width, this->fh, win->colors->background_color);

            int top_nav = 0;
            for(std::vector<navigation_item*>::iterator _nav_item = this->items.begin(); _nav_item != this->items.end(); _nav_item++){ // Render navigation objects
                navigation_item* nav_item = *_nav_item;
                nav_item->widget->__set_f_size(this->child->rx+this->fx, this->child->ry+this->fy+top_nav, (this->is_opened) ? this->opened_width : this->minimal_width, (this->is_opened) ? nav_item->opened_height : nav_item->minimal_height);
                top_nav += (this->is_opened) ? nav_item->opened_height : nav_item->minimal_height;

                if(((this->is_opened) ? nav_item->opened_height : nav_item->minimal_height) > 0){ // If the size of widget is bigger than 0, if not it will not rendered; can be used hide widgets in closed/opened
                    if(nav_item->widget->is_layout()){
                        ((layout*)nav_item->widget)->calculate_n_draw(win, buffer);
                    } else {
                        nav_item->widget->draw(win, buffer);
                    };
                };
            };

        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            if(x < ((this->is_opened) ? this->opened_width : this->minimal_width)){
                if(event == lunaris::mouse::first && pressed){
                    int top_nav = 0;
                    bool first_state = this->is_opened;
                    for(std::vector<navigation_item*>::iterator _nav_item = this->items.begin(); _nav_item != this->items.end(); _nav_item++){
                        navigation_item* nav_item = *_nav_item;
                        top_nav += (this->is_opened) ? nav_item->opened_height : nav_item->minimal_height;
                        if(y<top_nav){
                            nav_item->widget->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
                            if(first_state) this->is_opened = false; // Close sidenav when lose focus
                            return;
                        };
                    };
                    if(first_state) this->is_opened = false; // Close sidenav when lose focus
                };
                return;
            };
            if(pressed && this->is_opened) this->is_opened = false; // Close sidenav when lose focus
            this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
        };
        virtual void place(lunaris::ui::widget* widget, int minimal_height, int opened_height){
            navigation_item* new_item = new navigation_item;
            new_item->widget = widget;
            new_item->minimal_height = minimal_height;
            new_item->opened_height = opened_height;
            this->items.push_back(new_item);
        };
    } navigation;
}