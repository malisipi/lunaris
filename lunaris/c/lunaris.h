#include <stdbool.h>
#include <stdint.h>

typedef void lunaris_window;
lunaris_window* lunaris_new_window();
void lunaris_window_set_decoration(lunaris_window* win, bool state);
void lunaris_window_set_title(lunaris_window* win, char* title);
void lunaris_window_set_fullscreen(lunaris_window* win, bool state);
void lunaris_window_resize(lunaris_window* win, int width, int height);
void lunaris_window_loop(lunaris_window* win);
void lunaris_window_register_draw_handler(lunaris_window* win, void (*draw_handler)(lunaris_window* win, uint32_t* buffer));
int lunaris_window_get_width(lunaris_window* win);
int lunaris_window_get_height(lunaris_window* win);
void lunaris_window_graphics_line(lunaris_window* win, int x1, int y1, int x2, int y2, uint32_t color);
void lunaris_window_graphics_line_thick(lunaris_window* win, int x1, int y1, int x2, int y2, int s, uint32_t color);
void lunaris_window_graphics_rect(lunaris_window* win, int x, int y, int w, int h, uint32_t color);