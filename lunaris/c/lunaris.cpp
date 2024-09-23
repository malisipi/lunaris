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
}