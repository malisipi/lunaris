// clang++ lunaris/c/lunaris.cpp -shared -fPIC -o liblunaris.so

#include "../ui.hpp"

extern "C" {
    typedef lunaris::window lunaris_window;
    lunaris_window* lunaris_new_window(){
        return lunaris::new_window();
    };
    void lunaris_window_set_decoration(lunaris_window* win, bool state){
        win->set_decoration(state);
    };
    void lunaris_window_set_title(lunaris_window* win, char* title){
        win->set_title(title);
    };
    void lunaris_window_set_fullscreen(lunaris_window* win, bool state){
        win->set_fullscreen(state);
    };
    void lunaris_window_resize(lunaris_window* win, int width, int height){
        win->resize(width, height);
    };
    void lunaris_window_loop(lunaris_window* win){
        win->loop();
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
    void lunaris_window_graphics_line(lunaris_window* win, int x1, int y1, int x2, int y2, uint32_t color){
        win->graphics.line(x1, y1, x2, y2, color);
    };
    void lunaris_window_graphics_line_thick(lunaris_window* win, int x1, int y1, int x2, int y2, int s, uint32_t color){
        win->graphics.line_thick(x1, y1, x2, y2, s, color);
    };
    void lunaris_window_graphics_rect(lunaris_window* win, int x, int y, int w, int h, uint32_t color){
        win->graphics.rect(x, y, w, h, color);
    };
}