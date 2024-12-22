#pragma once

namespace lunaris {
    namespace ui {
        const uint32_t window_shadow_id = request_new_id();
        
        typedef struct window_shadow:layout {
            const int shadow_offset = 10;
            virtual uint32_t get_type(){
                return window_shadow_id;
            };
            widget* child = NULL;
            bool should_decorated = true;
            bool __is_maximized = false;
            virtual void draw (lunaris::window* win, uint32_t* buffer){
                int shadow_offset = 0;
                if(this->should_decorated && !this->__is_maximized){
                    shadow_offset = this->shadow_offset;
                };
                if(this->child != NULL){
                    this->child->__set_f_size(this->child->rx+this->fx+shadow_offset, this->child->ry+this->fy+shadow_offset, this->fw-shadow_offset*2, this->fh-shadow_offset*2);
                    this->child->draw(win, buffer);
                };
                if(this->should_decorated && !this->__is_maximized){
                    for(int si=0; si<shadow_offset; si++){
                        uint32_t shadow_current_line_color = 0x00000000|(((si*127/shadow_offset)&0xFF)<<24);
                        win->graphics.line(this->fx+si, this->fy+si, this->fx+si, this->fy+this->fh-si, shadow_current_line_color);
                        win->graphics.line(this->fx+this->fw-si-1, this->fy+si, this->fx+this->fw-si-1, this->fy+this->fh-si, shadow_current_line_color);
                        win->graphics.line(this->fx+si, this->fy+si, this->fx+this->fw-si, this->fy+si, shadow_current_line_color);
                        win->graphics.line(this->fx+si, this->fy+this->fh-si, this->fx+this->fw-si, this->fy+this->fh-si, shadow_current_line_color);
                    };
                };
            };
            virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
                if(this->should_decorated && !this->__is_maximized){
                    if(x < this->fx+this->shadow_offset || y < this->fy+this->shadow_offset || x > this->fx+this->fw-this->shadow_offset || y > this->fy+this->fh-this->shadow_offset) return;
                    this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
                } else {
                    this->child->mouse_event(win, x-this->child->fx+this->fx, y-this->child->fy+this->fy, pressed, dx, dy, event);
                };
            };
        } window_shadow;
    };
};