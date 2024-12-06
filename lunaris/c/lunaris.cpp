#include "../windowing.hpp"

extern "C" {
    /* Typedefs */
    typedef lunaris::window lunaris_window;
    typedef lunaris::layer lunaris_layer;

    /* General */
    lunaris_window* lunaris_new_window(){
        return lunaris::new_window();
    };
    bool lunaris_window_set_decoration(lunaris_window* win, bool state){
        return win->set_decoration(state);
    };
    void lunaris_window_set_title(lunaris_window* win, char* title){
        return win->set_title(title);
    };
    void lunaris_window_set_fullscreen(lunaris_window* win, bool state){
        return win->set_fullscreen(state);
    };
    void lunaris_window_resize(lunaris_window* win, int width, int height){
        return win->resize(width, height);
    };
    void lunaris_window_loop(lunaris_window* win){
        return win->loop();
    };
    void lunaris_window_register_draw_handler(lunaris_window* win, void (*draw_handler)(lunaris_window* win, uint32_t* buffer)){
        win->draw_handler = draw_handler;
    };
    int lunaris_window_get_width(lunaris_window* win){
        return win->width;
    };
    int lunaris_window_get_height(lunaris_window* win){
        return win->height;
    };

    /* Graphics */
    void lunaris_window_graphics_line(lunaris_window* win, int x1, int y1, int x2, int y2, uint32_t color){
        return win->graphics.line(x1, y1, x2, y2, color);
    };
    void lunaris_window_graphics_line_thick(lunaris_window* win, int x1, int y1, int x2, int y2, int t, uint32_t color){
        return win->graphics.line_thick(x1, y1, x2, y2, t, color);
    };
    void lunaris_window_graphics_rect(lunaris_window* win, int x, int y, int w, int h, uint32_t color){
        return win->graphics.rect(x, y, w, h, color);
    };
    void lunaris_window_graphics_rect_empty(lunaris_window* win, int x, int y, int w, int h, int t, uint32_t color){
        return win->graphics.rect_empty(x, y, w, h, t, color);
    };
    void lunaris_window_graphics_circle(lunaris_window* win, int x, int y, int r, uint32_t color){
        return win->graphics.circle(x, y, r, color);
    };
    void lunaris_window_graphics_circle_empty(lunaris_window* win, int x, int y, int r, int t, uint32_t color){
        return win->graphics.circle_empty(x, y, r, t, color);
    };
    void lunaris_window_graphics_rounded_rect(lunaris_window* win, int x, int y, int w, int h, int r, uint32_t color){
        return win->graphics.rounded_rect(x, y, w, h, r, color);
    };
    void lunaris_window_graphics_rounded_rect_empty(lunaris_window* win, int x, int y, int w, int h, int r, int t, uint32_t color){
        return win->graphics.rounded_rect_empty(x, y, w, h, r, t, color);
    };
    void lunaris_window_graphics_text(lunaris_window* win, int x, int y, int line_height, const char* text, uint32_t color){
        return win->graphics.text(x, y, line_height, text, color);
    };
    void lunaris_window_graphics_text_bounding_area(lunaris_window* win, int* w, int* h, int line_height, const char* text){
        std::pair<int, int> area = win->graphics.text_bounding_area(line_height, text);
        *w = area.first;
        *h = area.second;
    };
    void lunaris_window_graphics_image(lunaris_window* win, lunaris_layer* img, int x, int y, int width, int height){
        return win->graphics.image(img, x, y, width, height);
    };
    
    /* Image */
    lunaris_layer* lunaris_new_image(const char* path){
        return lunaris::new_image(path);
    };
}