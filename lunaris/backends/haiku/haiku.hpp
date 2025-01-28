#pragma once

#define LUNARIS_INVALIDATE 'lniv'

namespace lunaris {
    class LunarisBufferView:public BView {
    public:
        LunarisBufferView(BRect frame):BView(frame, "LunarisBufferView", B_FOLLOW_ALL, B_WILL_DRAW) {};
        uint32_t* buffer = NULL;
        uint32_t* seconder_buffer = NULL;
        int buffer_width = 1;
        int buffer_height = 1;
        lunaris::window* win;
        bool buffer_lock = false;
        bool should_render = false;

        void Draw(BRect updateRect) override {
            should_render = true;
            if(!buffer_lock){
                buffer_lock = true;
                BBitmap bitmap(BRect(0, 0, buffer_width-1, buffer_height-1), B_RGB32, false, false);
                memcpy(bitmap.Bits(), buffer, buffer_width*buffer_height*sizeof(uint32_t));
                DrawBitmap(&bitmap, BPoint(0, 0));
                buffer_lock = false;
            };
        };

        void ResizeTo(float width, float height) {
            if(!buffer_lock){
                buffer_lock = true;
                const int new_size = sizeof(uint32_t)*((int)width)*((int)height);
                if(buffer == NULL){
                    buffer = (uint32_t*)malloc(new_size);
                    seconder_buffer = (uint32_t*)malloc(new_size);
                } else {
                    buffer = (uint32_t*)realloc((void*)buffer, new_size);
                    seconder_buffer = (uint32_t*)realloc((void*)seconder_buffer, new_size);
                };
                buffer_width = (int)width;
                buffer_height = (int)height;
                win->width = (int)width;
                win->height = (int)height;
                BView::ResizeTo(width, height);
                buffer_lock = false;
                this->Invalidate();
            };
        };

        void MessageReceived(BMessage* message) override {
            switch(message->what){
                case LUNARIS_INVALIDATE: {
                    this->Invalidate();
                    break;
                };
                default: {
                    BView::MessageReceived(message);
                };
            };
        };

        uint32_t last_mouse_buttons = 0;
        bool move_mouse_down = false;
        BPoint move_mouse_down_point;
        bool resize_mouse_down = false;
        BPoint resize_mouse_down_point;

        void MouseDown(BPoint where) override {
            MakeFocus(true);
            move_mouse_down_point = where;
            resize_mouse_down_point = where;
            uint32_t buttons = 0;
            GetMouse(NULL, &buttons);
            int new_button = last_mouse_buttons^buttons;
            last_mouse_buttons = buttons;
            if(new_button == B_PRIMARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 1, 0, 0, lunaris::mouse::first);
            if(new_button == B_SECONDARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 1, 0, 0, lunaris::mouse::second);
            if(new_button == B_TERTIARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 1, 0, 0, lunaris::mouse::middle);
        };
        void MouseUp(BPoint where) override {
            move_mouse_down = false;
            resize_mouse_down = false;
            uint32_t buttons = 0;
            GetMouse(NULL, &buttons);
            int new_button = last_mouse_buttons^buttons;
            last_mouse_buttons = buttons;
            if(new_button == B_PRIMARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 0, 0, 0, lunaris::mouse::first);
            if(new_button == B_SECONDARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 0, 0, 0, lunaris::mouse::second);
            if(new_button == B_TERTIARY_MOUSE_BUTTON) win->mouse_handler(win, where.x, where.y, 0, 0, 0, lunaris::mouse::middle);
        };
        void MouseMoved(BPoint where, uint32_t code, const BMessage* drag_message) override {
            if(move_mouse_down){
                ((BWindow*)win->__bwindow)->MoveBy(where.x - move_mouse_down_point.x, where.y - move_mouse_down_point.y);
                return;
            } else if(resize_mouse_down){
                ((BWindow*)win->__bwindow)->ResizeBy(where.x - resize_mouse_down_point.x, where.y - resize_mouse_down_point.y);
                resize_mouse_down_point = where;
                return;
            };
            win->mouse_handler(win, where.x, where.y, 0, 0, 0, lunaris::mouse::motion);
        };
        void KeyDown(const char* bytes, int32_t num_bytes) override {
            char format_str[16];
            char buffer[5];
            sprintf(format_str, "%%%ds", std::min(num_bytes, 4));
            sprintf(buffer, format_str, bytes);
            if(num_bytes>1 || bytes[0] == ' ' || bytes[0] == '\n' || (bytes[0] >= 33 && bytes[0] <= 126)) win->keyboard_handler(win, buffer, lunaris::keycode::null, 0, keyboard::pressed);
            if(bytes[0] == '\x08') win->keyboard_handler(win, buffer, lunaris::keycode::backspace, 0, keyboard::pressed);
        };
        void KeyUp(const char* bytes, int32_t num_bytes) override {
            char format_str[16];
            char buffer[5];
            sprintf(format_str, "%%%ds", std::min(num_bytes, 4));
            sprintf(buffer, format_str, bytes);
            if(num_bytes>1 || bytes[0] == ' ' || bytes[0] == '\n' || (bytes[0] >= 33 && bytes[0] <= 126)) win->keyboard_handler(win, buffer, lunaris::keycode::null, 0, keyboard::released);
            if(bytes[0] == '\x08') win->keyboard_handler(win, buffer, lunaris::keycode::backspace, 0, keyboard::released);
        };
    };

    class LunarisWindow:public BWindow {
    public:
        LunarisWindow():BWindow(BRect(100, 100, 100+800, 100+600), "<Untitled>", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE){}
        LunarisBufferView* the_view = NULL;

        void FrameResized(float newWidth, float newHeight) override {
            the_view->ResizeTo(newWidth, newHeight);
        };
    };

    void __request_new_frame(lunaris::window* win){ // Invalidate rect and force to create new paint event
        win->buffer = (uint32_t*)((LunarisWindow*)(win->__bwindow))->the_view->seconder_buffer;
        if(win->draw_handler != NULL){
            win->draw_handler(win, win->buffer);
        } else {
            for (int i = 0; i < win->width * win->height; i++) {
                win->buffer[i] = 0xFF000000;
            };
        };
        std::swap(((LunarisWindow*)(win->__bwindow))->the_view->buffer, ((LunarisWindow*)(win->__bwindow))->the_view->seconder_buffer);
        static BMessenger* messanger = new BMessenger(((LunarisWindow*)(win->__bwindow))->the_view); // TODO: Fix for later to multi windows
        messanger->SendMessage(LUNARIS_INVALIDATE);
    };

    void __backend_set_cursor(lunaris::window* win, lunaris::cursor::cursor cursor){}; // TODO

    void* __frame_triggerer(void* data) {
        lunaris::window* win = (lunaris::window*)data;
        const int frame_time = 1000/60;
        while(win->is_alive) {
            auto start = std::chrono::high_resolution_clock::now();

            if(((LunarisWindow*)win->__bwindow)->the_view->should_render) __request_new_frame(win);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

            if (elapsed.count() < frame_time) {
                std::this_thread::sleep_for(std::chrono::milliseconds(frame_time) - elapsed);
            }
        }
        return NULL;
    };

    BApplication* haiku_app = NULL;

    lunaris::window* new_window(void){
        lunaris::window* win = new lunaris::window;
        win->graphics.__init(win);
        win->is_alive = true;

        if(haiku_app == NULL){
            haiku_app = new BApplication("application/x-vnd.LunarisWindow");
        };

        LunarisWindow* bwindow = new LunarisWindow();

        bwindow->the_view = new LunarisBufferView(bwindow->Bounds());
        bwindow->the_view->win = win;
        bwindow->the_view->ResizeTo(800, 600);
        bwindow->AddChild(bwindow->the_view);

        win->__bwindow = (void*)bwindow;
        win->set_decoration(false);
        #ifdef LUNARIS_UI
            win->layout = NULL;
        #endif

        pthread_t thread;
        int result = pthread_create(&thread, NULL, __frame_triggerer, (void*)win);
        pthread_detach(thread);
        if (result != 0) {
            fprintf(stderr, "Error creating thread\n");
            return NULL;
        };
        
        return win;
    };

    bool is_app_runned = false;
    void __backend_loop(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->Show();
        if(is_app_runned) return;
        is_app_runned = true;
        haiku_app->Run();
    };

    void __backend_start_move(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->the_view->move_mouse_down = true;
    };

    void __backend_set_visibility(lunaris::window* win, bool visible){}; // TODO

    void __backend_resize(lunaris::window* win, int width, int height){
        win->width = width;
        win->height = height;
        ((LunarisWindow*)(win->__bwindow))->ResizeTo(width, height);
        ((LunarisWindow*)(win->__bwindow))->the_view->ResizeTo(width, height);
    };

    void __backend_destroy(lunaris::window* win){
        haiku_app->Quit();
    };

    void __backend_set_title(lunaris::window* win, const char* title){
        ((LunarisWindow*)win->__bwindow)->SetTitle(title);
    };

    bool __backend_set_decoration(lunaris::window* win, bool decorated){
        if(decorated){
            ((LunarisWindow*)win->__bwindow)->SetType(B_TITLED_WINDOW);
        } else {
            ((LunarisWindow*)win->__bwindow)->SetType(B_MODAL_WINDOW);
        };
        ((LunarisWindow*)win->__bwindow)->SetFeel(B_NORMAL_WINDOW_FEEL);
        return true;
    };

    void __backend_set_fullscreen(lunaris::window* win, bool fullscreen){}; // TODO

    void __backend_minimize(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->Minimize(true);
    };
    void __backend_maximize(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->Zoom();
    };
    void __backend_restore(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->Zoom();
    };
    void __backend_start_resize(lunaris::window* win){
        ((LunarisWindow*)win->__bwindow)->the_view->resize_mouse_down = true;
    };
    void __backend_show_window_menu(lunaris::window* win, int rx, int ry){}; // Haiku have not window menu
};
