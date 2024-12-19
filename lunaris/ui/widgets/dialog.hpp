#pragma once

namespace lunaris::ui {
    const uint32_t dialog_id = request_new_id();
    typedef struct dialog:layout {
        virtual uint32_t get_type(){
            return dialog_id;
        };
        widget* child = NULL;
        widget* dialog = NULL;
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){
            ((layout*)child)->__set_f_size(this->fx, this->fy, this->fw, this->fh);
            if(child->is_layout()){
                ((layout*)child)->calculate_n_draw(win, buffer);
            } else {
                child->draw(win, buffer);
            };
            if(this->dialog != NULL){
                // Shadow
                for(int rx=0;rx<this->fw;rx++){
                    for(int ry=0;ry<this->fh;ry++){
                        if((rx+ry)%3) win->graphics.pixel(this->fx+rx, this->fy+ry, 0xFF000000);
                    };
                };
                // Dialog
                if(dialog->used_anchor){
                    const int f_width = anchor::calc(this->fw, this->fh, dialog->aw);
                    const int f_height = anchor::calc(this->fw, this->fh, dialog->ah);
                    dialog->__set_f_size(this->fx+(this->fw-f_width)/2, this->fy+(this->fh-f_height)/2, f_width, f_height);
                } else {
                    dialog->__set_f_size(this->fx+(this->fw-dialog->rw)/2, this->fy+(this->fh-dialog->rh)/2, this->dialog->rw, dialog->rh);
                };
                win->graphics.rounded_rect(dialog->fx, dialog->fy, dialog->fw, dialog->fh, 10, win->colors->background_color);
                if(dialog->is_layout()){
                    ((layout*)dialog)->calculate_n_draw(win, buffer);
                } else {
                    dialog->draw(win, buffer);
                };
            };
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
            if(this->dialog == NULL) {
                this->child->mouse_event(win, x, y, pressed, dx, dy, event);
            } else {
                if(dialog->fx-this->fx <=x && dialog->fy-this->fy <=y && dialog->fx-this->fx+dialog->fw >=x && dialog->fy-this->fy+dialog->fh >=y){
                    this->dialog->mouse_event(win, x, y, pressed, dx, dy, event);
                };
            };
        };
        virtual void open_dialog(widget* dialog, std::string width, std::string height){
            this->dialog = dialog;
            dialog->__set_a_size((char*)"0", (char*)"0", width.c_str(), height.c_str());
        };
        virtual void open_dialog(widget* dialog, int width, int height){
            this->dialog = dialog;
            dialog->__set_size(0, 0, width, height);
        };
        virtual void close_dialog(){
            this->dialog = NULL;
        }
    } dialog;
}