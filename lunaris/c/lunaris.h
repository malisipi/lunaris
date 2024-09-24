#include <stdbool.h>
#include <stdint.h>

/* Typedefs */
typedef void lunaris_window;
typedef void lunaris_layer;

/* General*/
lunaris_window* lunaris_new_window();
bool lunaris_window_set_decoration(lunaris_window* win, bool state);
void lunaris_window_set_title(lunaris_window* win, char* title);
void lunaris_window_set_fullscreen(lunaris_window* win, bool state);
void lunaris_window_resize(lunaris_window* win, int width, int height);
void lunaris_window_loop(lunaris_window* win);
void lunaris_window_register_draw_handler(lunaris_window* win, void (*draw_handler)(lunaris_window* win, uint32_t* buffer));
int lunaris_window_get_width(lunaris_window* win);
int lunaris_window_get_height(lunaris_window* win);

/* Graphics */
void lunaris_window_graphics_line(lunaris_window* win, int x1, int y1, int x2, int y2, uint32_t color);
void lunaris_window_graphics_line_thick(lunaris_window* win, int x1, int y1, int x2, int y2, int t, uint32_t color);
void lunaris_window_graphics_rect(lunaris_window* win, int x, int y, int w, int h, uint32_t color);
void lunaris_window_graphics_rect_empty(lunaris_window* win, int x, int y, int w, int h, int t, uint32_t color);
void lunaris_window_graphics_circle(lunaris_window* win, int x, int y, int r, uint32_t color);
void lunaris_window_graphics_circle_empty(lunaris_window* win, int x, int y, int r, int t, uint32_t color);
void lunaris_window_graphics_rounded_rect(lunaris_window* win, int x, int y, int w, int h, int r, uint32_t color);
void lunaris_window_graphics_rounded_rect_empty(lunaris_window* win, int x, int y, int w, int h, int r, int t, uint32_t color);
void lunaris_window_graphics_text(lunaris_window* win, int x, int y, int line_height, const char* text, uint32_t color);
void lunaris_window_graphics_text_bounding_area(lunaris_window* win, int* w, int* h, int line_height, const char* text);
void lunaris_window_graphics_image(lunaris_window* win, lunaris_layer* img, int x, int y, int width, int height);

/* Image */
lunaris_layer* lunaris_new_image(const char* path);