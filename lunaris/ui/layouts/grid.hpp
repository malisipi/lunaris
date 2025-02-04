#pragma once

namespace lunaris {
    namespace ui {
        const uint32_t grid_id = request_new_id();
        typedef struct grid:layout {
            virtual uint32_t get_type(){
                return grid_id;
            };
            int col_count = 1;
            int row_count = 1;
            int col_gap = 0;
            int row_gap = 0;
            int col_margin = 0;
            int row_margin = 0;
            // bool auto_place = true; If not place will have a option to set grid parts.
            // TODO: resizable grid is important feature. Implement it later
            template <typename T> void place(T child) {
                this->childs.push_back((widget*)child);
            };
            void draw (lunaris::window* win, uint32_t* buffer){
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
                    child->draw(win, buffer);
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
    };
};