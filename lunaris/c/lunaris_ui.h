#include "./lunaris.h"

/* Typedefs */
typedef void lunaris_ui_widget;

/* General */
void lunaris_ui_connect(lunaris_window* win);
void lunaris_ui_mount(lunaris_window* win, lunaris_ui_widget* widget);

/* Widgets */
/* Grid */
lunaris_ui_widget* lunaris_ui_grid(void);
void lunaris_ui_grid_place(lunaris_ui_widget* grid, lunaris_ui_widget* widget);

/* Button */
lunaris_ui_widget* lunaris_ui_button(void);
void lunaris_ui_button_set_text(lunaris_ui_widget* button, char* text);
void lunaris_ui_button_register_click_handler(lunaris_ui_widget* button, void(*handler)(lunaris_window*, lunaris_ui_widget*));