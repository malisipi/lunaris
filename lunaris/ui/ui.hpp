#include "anchor.hpp"

namespace lunaris::ui {
    int last_registered_id = 0;

    int request_new_id(){
        return ++last_registered_id;
    };

    __uint128_t __get_event_time(){
        return (__uint128_t)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    }

    typedef struct widget {
        // Relative to layout
        float rx = 0;
        float ry = 0;
        float rw = 0;
        float rh = 0;
        // Relative to layout, should be used while storing anchor points
        bool used_anchor = false;
        const char* ax = 0;
        const char* ay = 0;
        const char* aw = 0;
        const char* ah = 0;
        // Sticked to window
        int fx = 0;
        int fy = 0;
        int fw = 0;
        int fh = 0;
        bool is_hovering = false; // all layouts and semi-layout widgets (the widgets that has widgets)
        virtual void set_hovering(bool is_hovering){
            this->is_hovering = is_hovering;
        };
        //bool can_exceed_size;
        virtual uint32_t get_type(){
            return 0;
        };
        virtual bool is_layout(){
            return false;
        };
        /*
         *        int ver_margin = 0;
         *        int hor_margin = 0;
         *        int ver_padding = 0;
         *        int hor_padding = 0;
         *        int ver_border = 0;
         *        int hor_border = 0;
         */
        virtual void draw(lunaris::window* win, uint32_t* buffer){};
        void __set_size(int x, int y, int w, int h){
            this->rx = x;
            this->ry = y;
            this->rw = w;
            this->rh = h;
        };
        void __set_f_size(int x, int y, int w, int h){
            this->fx = x;
            this->fy = y;
            this->fw = w;
            this->fh = h;
        };
        void __set_a_size(const char* x, const char* y, const char* w, const char* h){
            this->used_anchor = true;
            this->ax = x;
            this->ay = y;
            this->aw = w;
            this->ah = h;
        };
        virtual void mouse_event(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){ /*printf("Mouse at %f, %f\n", x, y);*/ };
        virtual void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){};
    } widget;

    const uint32_t layout_id = request_new_id();
    typedef struct layout:widget {
        std::vector<widget*> childs;
        virtual bool is_layout(){
            return true;
        };
        virtual uint32_t get_type(){
            return layout_id;
        };
        virtual void calculate_n_draw (lunaris::window* win, uint32_t* buffer){};
    } layout;

    namespace query {
        typedef enum query {
            width,
            height
        } query;
    }

    void draw_handler(lunaris::window* win, uint32_t* buffer){
        win->graphics.rect(0, 0, win->width, win->height, win->colors->background_color);

        lunaris::ui::layout* layout = (lunaris::ui::layout*)win->layout;
        layout->__set_f_size(0, 0, win->width, win->height);
        layout->calculate_n_draw(win, buffer);

        /*if(win->focused != NULL) {
            win->graphics.rect_empty(((lunaris::ui::widget*)win->focused)->fx-1, ((lunaris::ui::widget*)win->focused)->fy-1, ((lunaris::ui::widget*)win->focused)->fw+2, ((lunaris::ui::widget*)win->focused)->fh+2, 2, 0xFFAAAAAA);
        };*/
    };

    void mouse_handler(lunaris::window* win, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event){
        lunaris::ui::layout* layout = (lunaris::ui::layout*)win->layout;
        layout->mouse_event(win, x, y, pressed, dx, dy, event);
    };

    void keyboard_handler(lunaris::window* win, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event){
        if(win->focused != NULL) ((lunaris::ui::widget*)win->focused)->keyboard_handler(win, new_char, key, modifiers, event);
    };

    void connect(lunaris::window* win){
        win->colors = lunaris::styles::generate_color_palette_from_system();
        win->draw_handler = lunaris::ui::draw_handler;
        win->mouse_handler = lunaris::ui::mouse_handler;
        win->keyboard_handler = lunaris::ui::keyboard_handler;
    };
};

#include "widgets/scrollbar.hpp"
#include "layouts/fixed.hpp"
#include "layouts/grid.hpp"
#include "layouts/dynamic.hpp"
#include "widgets/button.hpp"
#include "widgets/list.hpp"
#include "widgets/textbox.hpp"
#include "widgets/slider.hpp"
#include "widgets/window-decorations.hpp"