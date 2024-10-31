#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <chrono>
#include <thread>
#include <math.h>
#include <vector>

#if !(defined(windows) || defined(wayland) || defined(x11) || defined(haiku) || defined(framebuffer))
    #ifdef _WIN32
    #define windows
    #endif

    #ifdef linux
    #define wayland
    #endif

    #ifdef __HAIKU__
    #define haiku
    #endif
#endif

#ifdef windows
    #include <windows.h>
    #include <windowsx.h>
#endif

#ifdef x11
    #include <X11/X.h>
    #include <X11/Xlib.h>
    #include <X11/extensions/XShm.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <X11/Xutil.h>
    #include <stdint.h>
    #include <sys/shm.h>
    #include <sys/ipc.h>
#endif

#ifdef haiku
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Screen.h>
#include <Bitmap.h>
#include <GraphicsDefs.h>
#include <Message.h>
#include <string.h>
#endif

#include "internal.hpp"

#include "fonts.hpp"
#include "images.hpp"
#include "styles.hpp"

namespace lunaris {
    typedef struct window window;

    namespace mouse {
        typedef enum mouse {
            motion, // no click
            first, // left click
            middle,
            second, // right click
            x1, // backward
            x2, // forward
            scroll, // vertical delta will in y. horizontal delta will in x.
            enter, // it's may not well supported, some platforms may not send the event; please assume the event as a type of motion; please not hard depent this event
            leave // it's may not well supported, some platforms may not send the event; please assume the event as a type of motion; please not hard depent this event
        } lunaris_button;
    };

    namespace cursor {
        typedef enum cursor {
            arrow, // WIN32, Wayland
            text, // WIN32, Wayland
            wait, // WIN32, Wayland
            help, // WIN32, Wayland
            pointer, // WIN32, Wayland
            crosshair, // WIN32, Wayland
            not_allowed, // WIN32, Wayland
            resize_ew, // WIN32, Wayland (Horizontal) (-)
            resize_ns, // WIN32, Wayland (Vertical) (|)
            resize_nwse, // WIN32, Wayland (\)
            resize_nesw, // WIN32, Wayland (/)
            move, // WIN32, Wayland
            progress, // WIN32, Wayland

            alias, // WIN32 (Extended Cursors), Wayland
            context_menu, // WIN32 (Extended Cursors), Wayland
            copy, // WIN32 (Extended Cursors), Wayland
            no_drop, // WIN32 (Extended Cursors), Wayland
            grab, // WIN32 (Extended Cursors), Wayland
            grabbing, // WIN32 (Extended Cursors), Wayland
            cell, // WIN32 (Extended Cursors), Wayland
            col_resize, // WIN32 (Extended Cursors), Wayland
            row_resize, // WIN32 (Extended Cursors), Wayland
            vertical_text, // WIN32 (Extended Cursors), Wayland
            zoom_in, // WIN32 (Extended Cursors), Wayland
            zoom_out // WIN32 (Extended Cursors), Wayland
        } cursor;
    };

    namespace keyboard {
        typedef enum keyboard {
            pressed,
            released,
            repeated
        } keyboard;
    };

    namespace keycode {
        typedef enum keycode {
            null,
            escape,
            _1,_2,_3,_4,_5,_6,_7,_8,_9,_0,
            minus,
            _equal,
            backspace,
            tab,
            q,w,e,r,t,y,u,i,o,p,
            left_brace,
            right_brace,
            enter,
            left_ctrl,
            left_shift,
            left_alt,
            left_meta,
            right_ctrl,
            right_shift,
            right_alt,
            right_meta,
            a,s,d,f,g,h,j,k,l,
            semicolon,
            apostrophe,
            grave,
            backslash,
            z,x,c,v,b,n,m,
            comma,
            dot,
            slash,
            space,
            caps_lock,
            f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,
            num_lock,
            scroll_lock,
            np_1,np_2,np_3,np_4,np_5,np_6,np_7,np_8,np_9,np_0,
            np_asterisk,
            np_minus,
            np_plus,
            np_dot,
            np_enter,
            np_slash,
            _102nd,
            arrow_up,
            arrow_left,
            arrow_right,
            arrow_down,
            page_up,
            page_down,
            home,
            end,
            insert,
            _delete,
            _pause
        } keycode;
    };

    lunaris::window* new_window(void);
    void __backend_loop(lunaris::window*);
    void __backend_resize(lunaris::window*, int, int);
    void __backend_destroy(lunaris::window*);
    void __backend_set_title(lunaris::window*, const char*);
    bool __backend_set_decoration(lunaris::window*, bool);
    void __backend_set_fullscreen(lunaris::window*, bool);
    void __backend_set_cursor(lunaris::window*, lunaris::cursor::cursor);
    void __backend_start_move(lunaris::window*);
    void __backend_set_visibility(lunaris::window*, bool);
    void __backend_minimize(lunaris::window*);
    void __backend_maximize(lunaris::window*);
    void __backend_restore(lunaris::window*);
    void __backend_start_resize(lunaris::window*);
    void __backend_show_window_menu(lunaris::window*, int, int);

    struct window {
        bool is_alive = false;
        void destroy(void) {
            if(this->is_alive){
                if(this->graphics.font != NULL){
                    this->graphics.font->destroy();
                };
                __backend_destroy(this);
            };
        };
        void resize(int w, int h){ return __backend_resize(this, w, h); }
        void set_title(const char* title){ return __backend_set_title(this, title); }
        void loop(void){ return __backend_loop(this); }
        bool set_decoration(bool decorated){ return __backend_set_decoration(this, decorated); }
        void set_fullscreen(bool fullscreen){ return __backend_set_fullscreen(this, fullscreen); }
        void set_cursor(lunaris::cursor::cursor cursor) { return __backend_set_cursor(this, cursor); };
        void set_visibility(bool visible) { return __backend_set_visibility(this, visible); };
        void show_window_menu(int x, int y) { return __backend_show_window_menu(this, x, y); };
        void start_move(){ return __backend_start_move(this); };
        void minimize(){ return __backend_minimize(this); };
        void maximize(){ return __backend_maximize(this); };
        void restore(){ return __backend_restore(this); };
        void start_resize(){ return __backend_start_resize(this); };
        int width;
        int height;
        void* data; // Area for user data; Not used by library
        void(*draw_handler)(lunaris::window*, uint32_t*) = NULL;
        void(*mouse_handler)(lunaris::window*, float x, float y, bool pressed, float dx, float dy, lunaris::mouse::mouse event);
        void(*keyboard_handler)(lunaris::window*, const char* new_char, lunaris::keycode::keycode key, uint32_t modifiers, lunaris::keyboard::keyboard event);
        uint32_t* buffer; // Only for draw_handler access, is not threadsafe; exist for graphics API
        struct {
            lunaris::window* outer = NULL;
            lunaris::font* font = NULL;
            void __init(lunaris::window* outer) {
                this->outer = outer;
                this->font = new_font(get_system_font());
            };
            inline void pixel (int x, int y, uint32_t color){
                if(x>=0 && y>=0 && x<outer->width && y<outer->height) outer->buffer[outer->width*y+x] = color;
            };
            inline void block (int x, int y, int w, uint32_t color){
                // TODO: optimize with memcpy
                for(int _x=x;_x<x+w;_x++){
                    this->pixel(_x,y,color);
                };
            }
            void rect(int x, int y, int w, int h, uint32_t color){
                for(int _y=y;_y<y+h;_y++){
                    this->block(x, _y, w, color);
                };
            };
            void line(int x1, int y1, int x2, int y2, uint32_t color){
                const int width = abs(x1-x2);
                const int height = abs(y1-y2);

                if(width > height){
                    const int xs = std::min(x1, x2);
                    const int ys = (xs==x1) ? y1 : y2;
                    const int ye = (xs!=x1) ? y1 : y2;
                    
                    for(int x=0; x < width; x++){
                        this->pixel(xs+x, ys+(ye-ys)*x/width, color);
                    };
                } else {
                    const int ys = std::min(y1, y2);
                    const int xs = (ys==y1) ? x1 : x2;
                    const int xe = (ys!=y1) ? x1 : x2;
                    
                    for(int y=0; y < height; y++){
                        this->pixel(xs+(xe-xs)*y/height, ys+y, color);
                    };
                };
            };
            void line_thick(int x1, int y1, int x2, int y2, int thickness, uint32_t color){
                const int width = abs(x1-x2);
                const int height = abs(y1-y2);

                if(width > height){
                    const int xs = std::min(x1, x2);
                    const int ys = (xs==x1) ? y1 : y2;
                    const int ye = (xs!=x1) ? y1 : y2;
                    
                    for(int x=0; x <= width; x++){
                        const int rel_start_y = -thickness/2;
                        for(int rel_y=rel_start_y;rel_y<thickness+rel_start_y;rel_y++){
                            this->pixel(xs+x, ys+(ye-ys)*x/width+rel_y, color);
                        };
                    };
                } else {
                    const int ys = std::min(y1, y2);
                    const int xs = (ys==y1) ? x1 : x2;
                    const int xe = (ys!=y1) ? x1 : x2;
                    
                    for(int y=0; y <= height; y++){
                        const int rel_start_x = -thickness/2;
                        for(int rel_x=rel_start_x;rel_x<thickness+rel_start_x;rel_x++){
                            this->pixel(xs+(xe-xs)*y/height+rel_x, ys+y, color);
                        };
                    };
                };
            };
            void rect_empty (int x, int y, int w, int h, int thickness, uint32_t color){
                int margin = thickness/2;
                this->line_thick(x-margin, y, x+w-margin+thickness, y, thickness, color);
                this->line_thick(x, y, x, y+h, thickness, color);
                this->line_thick(x+w, y, x+w, y+h, thickness, color);
                this->line_thick(x-margin, y+h, x+w-margin+thickness, y+h, thickness, color);
            };
            void circle (int x, int y, int r, uint32_t color){
                for(int sy=0;sy<r;sy++){
                    const int w = (float)r*std::sqrt((float)1-std::pow(((float)(sy)+.5)/(float)(r),2) );
                    /* this->line(x, y-sy, x+w, y-sy, color); // top right part
                    this->line(x, y+sy, x+w, y+sy, color); // bottom right part
                    this->line(x-w, y-sy, x, y-sy, color); // top left part
                    this->line(x-w, y+sy, x, y+sy, color); // bottom left part */
                    /* this->line(x-w, y-sy, x+w, y-sy, color); // top part
                    this->line(x-w, y+sy, x+w, y+sy, color); // bottom part */
                    this->block(x-w, y-sy, w*2, color);
                    this->block(x-w, y+sy, w*2, color);
                };
            };
            void circle_empty (int x, int y, int r, int thickness, uint32_t color){
                int prev_w = r;
                for(int sy=-1;sy<=r;sy++){
                    const int w = (float)r*std::sqrt((float)1-std::pow(
                        std::min(std::max((float)(sy)+.5,(double)0),(double)r)
                        /(float)(r),2) );

                    const int rel_start_y = -thickness/2;
                    for(int rel_y=rel_start_y;rel_y<thickness+rel_start_y;rel_y++){
                        this->block(x-w-thickness/2, y+sy+rel_y, std::max(prev_w-w,thickness), color);
                        this->block(x+w-thickness/2, y+sy+rel_y, std::max(prev_w-w,thickness), color);
                        this->block(x-w-thickness/2, y-sy+rel_y, std::max(prev_w-w,thickness), color);
                        this->block(x+w-thickness/2, y-sy+rel_y, std::max(prev_w-w,thickness), color);
                    };
                    prev_w = w;
                };
            };
            void __partial_circle(int x, int y, int r, bool left, bool top, uint32_t color){
                for(int sy=0;sy<r;sy++){
                    const int w = (float)r*std::sqrt((float)1-std::pow(((float)(sy)+.5)/(float)(r),2) )+1;
                    if(left) {
                        if(top){
                            this->line(x-w, y-sy, x, y-sy, color); // top left part
                        } else {
                            this->line(x-w, y+sy, x, y+sy, color); // bottom left part
                        };
                    } else {
                        if(top){
                            this->line(x, y-sy, x+w, y-sy, color); // top right part
                        } else {
                            this->line(x, y+sy, x+w, y+sy, color); // bottom right part
                        };
                    };
                };
            };
            void __partial_circle_outline(int x, int y, int r, bool left, bool top, int thickness, uint32_t color){
                int prev_w = r;
                for(int sy=-1;sy<=r;sy++){
                    const int w = (float)r*std::sqrt((float)1-std::pow(
                        std::min(std::max((float)(sy)+.5,(double)0),(double)r)
                        /(float)(r),2) );

                    const int rel_start_y = -thickness/2;
                    for(int rel_y=rel_start_y;rel_y<thickness+rel_start_y;rel_y++){
                        if(left) {
                            if(top){
                                this->block(x-w-thickness/2, y-sy+rel_y, std::max(prev_w-w,thickness), color); // top left part
                            } else {
                                this->block(x-w-thickness/2, y+sy+rel_y, std::max(prev_w-w,thickness), color); // bottom left part
                            };
                        } else {
                            if(top){
                                this->block(x+w-thickness/2, y-sy+rel_y, std::max(prev_w-w,thickness), color); // top right part
                            } else {
                                this->block(x+w-thickness/2, y+sy+rel_y, std::max(prev_w-w,thickness), color); // bottom right part
                            };
                        };
                    };
                    prev_w = w;
                };
            };
            void rounded_rect(int x, int y, int w, int h, int radius, uint32_t color){
                this->rect(x+radius, y, w-radius*2, h, color);
                this->rect(x, y+radius, radius, h-radius*2, color);
                this->rect(x+w-radius, y+radius, radius, h-radius*2, color);
                this->__partial_circle(x+radius, y+radius, radius, true, true, color);
                this->__partial_circle(x+w-radius, y+radius, radius, false, true, color);
                this->__partial_circle(x+radius, y+h-radius, radius, true, false, color);
                this->__partial_circle(x+w-radius, y+h-radius, radius, false, false, color);
            };
            void rounded_rect_empty(int x, int y, int w, int h, int radius, int thickness, uint32_t color){
                this->line_thick(x+radius, y, x+w-radius, y, thickness, color);
                this->line_thick(x+radius, y+h, x+w-radius, y+h, thickness, color);
                this->line_thick(x, y+radius, x, y+h-radius, thickness, color);
                this->line_thick(x+w, y+radius, x+w, y+h-radius, thickness, color);
                this->__partial_circle_outline(x+radius, y+radius, radius, true, true, thickness, color);
                this->__partial_circle_outline(x+w-radius, y+radius, radius, false, true, thickness, color);
                this->__partial_circle_outline(x+radius, y+h-radius, radius, true, false, thickness, color);
                this->__partial_circle_outline(x+w-radius, y+h-radius, radius, false, false, thickness, color);
            };
            void poly_empty(std::vector<std::vector<int>> list, int thickness, uint32_t color){
                const int len = list.size();
                for(int i=0;i<len-1;i++){
                    this->line_thick(list[i][0], list[i][1], list[i+1][0], list[i+1][1], thickness, color);
                };
                this->line_thick(list[len-1][0], list[len-1][1], list[0][0], list[0][1], thickness, color);
            };
            void text(int x, int y, int line_height, const char* text, uint32_t color){
                this->font->draw_text(outer->buffer, outer->width, outer->height, x, y, line_height, text, color);
            };
            std::pair<int, int> text_bounding_area(int line_height, const char* text){
                return this->font->bounding_area(line_height, text);
            };
            void image(lunaris::layer* img, int x, int y, int width, int height){ // TODO: Add options to flip image and background transparency (skipping editting for transparent pixels like <127 alpha)
                if(img == NULL) return;
                // TODO: Also add clipping some region and draw it to buffer support
                for(int py=0; py<height; py++){
                    for(int px=0; px<width; px++){
                        int index = (int)(py*img->height/height)*img->width+(int)(px*img->width/width);
                        if((int)((img->data[index]&0xFF000000)>>6)<127) continue;
                        outer->buffer[(py+y)*outer->width+px+x] = img->data[index];
                    };
                };
            };
        } graphics;
    #ifdef wayland
        struct wl_display* __display = NULL;
        struct wl_registry* __registry = NULL;
        struct wl_compositor* __compositor = NULL;
        struct wl_shm* __shm = NULL;
        struct wl_surface* __surface = NULL;
        struct wl_shm_pool* __shm_pool = NULL;
        int __shm_fd = -1;
        uint8_t* __shm_data = NULL;
        struct wl_buffer* __buffer = NULL;
        struct wl_shm_pool* __seconder_shm_pool = NULL;
        int __seconder_shm_fd = -1;
        uint8_t* __seconder_shm_data = NULL;
        struct wl_buffer* __seconder_buffer = NULL;
        struct xdg_wm_base* __xdg_wm_base = NULL;
        struct xdg_surface* __xdg_surface = NULL;
        struct xdg_toplevel* __xdg_toplevel = NULL;
        struct wl_seat* __seat = NULL;
        struct wl_pointer* __pointer = NULL;
        struct wl_keyboard* __keyboard = NULL;
        uint32_t __zxdg_decoration_manager_id = 0;
        struct wp_cursor_shape_manager_v1* __cursor_shape_manager = NULL;
        struct wp_cursor_shape_device_v1* __cursor_shape_device = NULL;
        struct zxdg_decoration_manager_v1* __zxdg_decoration_manager = NULL;
        struct zxdg_toplevel_decoration_v1* __zxdg_decoration = NULL;
        // To safely shutdown // TODO: Replace it with more stable API
        bool safe_shutdown = true;
        float __mouse_x = 0;
        float __mouse_y = 0;
        /* I need to serial to change cursor, thanks Wayland */
        uint32_t __mouse_last_serial = 0;
        /* Since Wayland is not supporting sending key repeat information,
        I need to parse events manually */
        char __last_press_buffer[8] = {0};
        uint32_t __last_press_key = 0;
        uint8_t __last_press_state = -1;
        /* Wayland gives no API to hide windows, so I need to attach/deattach surface to hide/show window */
        int __hidden = 0;
    #endif
    #ifdef x11
        Display* __display;
        Window __window;
        int __screen;
        GC __gc;
        XShmSegmentInfo __shminfo;
        XImage* __image;
    #endif
    #ifdef windows
        HWND __hwnd;
        HBITMAP __bitmap = NULL;
        void* __buffer = NULL;
        /* To determine key repeates, since Windows doesn't report that */
        uint32_t __last_pressed_key = 0;
    #endif
    #ifdef haiku
        void* __bwindow = NULL;
    #endif
    #ifdef framebuffer
        void* __buffer = NULL;
    #endif
    #ifdef LUNARIS_UI
        void* layout = NULL;
        void* focused = NULL;
        lunaris::styles::color_palette* colors = NULL;
    #endif
    };

};

#ifdef wayland
#include "backends/wayland/wayland.hpp"
#endif

#ifdef x11
#include "backends/x11/x11.hpp"
#endif

#ifdef windows
#include "backends/windows/windows.hpp"
#endif

#ifdef haiku
#include "backends/haiku/haiku.hpp"
#endif

#ifdef framebuffer
#include "backends/framebuffer/framebuffer.hpp"
#endif
