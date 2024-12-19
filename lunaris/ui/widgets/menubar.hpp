#pragma once

namespace lunaris::ui {
    const uint32_t menubar_id = request_new_id();
    
    typedef struct menubar_subitem {
        std::string text;
        void (*handler)(menubar_subitem*) = NULL;
    } menubar_subitem;

    typedef struct menubar_item {
        std::string text;
        std::vector<menubar_subitem*> items;
    } menubar_item;

    typedef struct menubar:layout {
        virtual uint32_t get_type(){
            return menubar_id;
        };
        std::vector<menubar_item*> menu = {};
        widget* child = NULL;
        int __selected = -1;
        int __hovered = -1;
        int __sub_menu_x = -1;
        int __sub_menu_w = -1;
        int __sub_menu_hovered = -1;
        const int menubar_size = 24;
        virtual void draw (lunaris::window* win, uint32_t* buffer){
            if(this->child != NULL){
                this->child->__set_f_size(this->child->rx+this->fx, this->child->ry+this->fy+menubar_size, this->fw, this->fh-menubar_size);
                this->child->draw(win, buffer);
            };
            win->graphics.rect(this->fx, this->fy, this->fw, menubar_size, win->colors->background_color);
            int item_pos_x = 0;
            int i = 0;
            for(std::vector<menubar_item*>::iterator it = this->menu.begin(); it != this->menu.end(); it++){
                menubar_item* menu_item = *it;
                std::pair<int, int> text_bounding_area = win->graphics.text_bounding_area(menubar_size-2, menu_item->text.c_str());
                if(i == this->__hovered) win->graphics.rounded_rect(this->fx+item_pos_x, this->fy, text_bounding_area.first+8, menubar_size, 4, win->colors->hover_color);
                if(i == this->__selected) win->graphics.rounded_rect(this->fx+item_pos_x, this->fy, text_bounding_area.first+8, menubar_size, 4, win->colors->other_color);
                win->graphics.text(this->fx+4+item_pos_x, this->fy+1, menubar_size-2, menu_item->text.c_str(), win->colors->text_color);
                if(this->__selected == i && (menu_item->items.size() > 0)){
                    int max_sub_text_width = 0;
                    for(std::vector<menubar_subitem*>::iterator sub_it = menu_item->items.begin(); sub_it != menu_item->items.end(); sub_it++){
                        menubar_subitem* sub_menu_item = *sub_it;
                        std::pair<int, int> sub_text_bounding_area = win->graphics.text_bounding_area(menubar_size-2, sub_menu_item->text.c_str());
                        max_sub_text_width = std::max(sub_text_bounding_area.first, max_sub_text_width);
                    };
                    this->__sub_menu_x = item_pos_x;
                    this->__sub_menu_w = max_sub_text_width;
                    win->graphics.rounded_rect(this->fx + item_pos_x, this->fy+menubar_size, max_sub_text_width + 8, menu_item->items.size()*menubar_size, 4, win->colors->other_color);
                    int sub_i = 0;
                    for(std::vector<menubar_subitem*>::iterator sub_it = menu_item->items.begin(); sub_it != menu_item->items.end(); sub_it++){
                        menubar_subitem* sub_menu_item = *sub_it;
                        if(this->__sub_menu_hovered == sub_i) win->graphics.rounded_rect(this->fx+item_pos_x, this->fy+menubar_size*(this->__sub_menu_hovered+1), max_sub_text_width+8, menubar_size, 4, win->colors->hover_color);
                        win->graphics.text(this->fx+4+item_pos_x, this->fy+1+(sub_i+1)*menubar_size, menubar_size-2, sub_menu_item->text.c_str(), win->colors->text_color);
                        sub_i++;
                    };
                };
                item_pos_x += text_bounding_area.first;
                item_pos_x += 8 + 2;
                i++;
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            if(y <= menubar_size) {
                int item_pos_x = 0;
                int i = 0;
                for(std::vector<menubar_item*>::iterator it = this->menu.begin(); it != this->menu.end(); it++){
                    menubar_item* menu_item = *it;
                    std::pair<int, int> text_bounding_area = win->graphics.text_bounding_area(menubar_size-2, menu_item->text.c_str());
                    item_pos_x += text_bounding_area.first + 8 + 2;
                    if(x < item_pos_x) {
                        if(event == lunaris::mouse::first){
                            if(this->menu[i]->items.size() >= 1){
                                this->__selected = i;
                            } else {
                                this->__selected = -1;
                            };
                        };
                        if(event == lunaris::mouse::motion) this->__hovered = i;
                        return;
                    };
                    i++;
                };
                if(event == lunaris::mouse::motion) this->__hovered = -1;
            } else {
                if(this->__selected != -1 ){
                    if(x >= this->__sub_menu_x && x <= this->__sub_menu_x + this->__sub_menu_w){
                        this->__sub_menu_hovered = y/this->menubar_size - 1; // Includes main menubar to this area, -1 operation is caused by main menubar
                        const menubar_item* selected_menu =  this->menu[this->__selected];
                        if(this->__sub_menu_hovered >= selected_menu->items.size()) this->__sub_menu_hovered = -1;
                        if(event == lunaris::mouse::first && pressed){
                            if(this->__sub_menu_hovered != -1){
                                menubar_subitem* sub_menu = selected_menu->items[this->__sub_menu_hovered];
                                if(sub_menu->handler != NULL) sub_menu->handler(sub_menu);
                            };
                            this->__selected = -1;
                        }
                        if(this->__sub_menu_hovered != -1) return;
                    };
                };
                if(event == lunaris::mouse::first) this->__selected = -1;
                if(event == lunaris::mouse::motion){
                    this->__sub_menu_hovered = -1;
                    this->__hovered = -1;
                };
                this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
            };
        };
    } menubar;
}