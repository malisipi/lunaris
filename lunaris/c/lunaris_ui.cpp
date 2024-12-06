#include "../ui.hpp"
#include "./lunaris.cpp"

extern "C" {
    /* Typedefs */
    typedef lunaris::ui::widget lunaris_ui_widget;

    /* General */
    void lunaris_ui_connect(lunaris_window* win){
        lunaris::ui::connect(win);
    };

    void lunaris_ui_mount(lunaris_window* win, lunaris_ui_widget* widget){
        win->layout = widget;
    };

    /* Widgets */
    /* Grid */
    lunaris_ui_widget* lunaris_ui_grid(void){
        return new lunaris::ui::grid;
    };

    void lunaris_ui_grid_place(lunaris_ui_widget* grid, lunaris_ui_widget* widget){
        ((lunaris::ui::grid*)grid)->place(widget);
    };

    /* Button */
    lunaris_ui_widget* lunaris_ui_button(void){
        return new lunaris::ui::button;
    };
    void lunaris_ui_button_set_text(lunaris_ui_widget* button, char* text){
        ((lunaris::ui::button*)button)->text = text;
    }
    void lunaris_ui_button_register_click_handler(lunaris_ui_widget* button, void(*handler)(lunaris_window*, lunaris_ui_widget*)){
        ((lunaris::ui::button*)button)->click_handler = handler;
    }
}