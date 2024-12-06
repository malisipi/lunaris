#pragma once

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include </usr/include/linux/input-event-codes.h>

namespace lunaris {
    namespace __framebuffer::__keyboard {
        lunaris::keycode::keycode __convert_event_code_to_lunaris_code(uint32_t key){ /* /usr/include/linux/input-event-codes.h */
            using namespace keycode;

            switch (key) {
                case KEY_ESC: return escape;
                case KEY_1: return _1;
                case KEY_2: return _2;
                case KEY_3: return _3;
                case KEY_4: return _4;
                case KEY_5: return _5;
                case KEY_6: return _6;
                case KEY_7: return _7;
                case KEY_8: return _8;
                case KEY_9: return _9;
                case KEY_0: return _0;
                case KEY_MINUS: return minus;
                case KEY_EQUAL: return _equal;
                case KEY_BACKSPACE: return backspace;
                case KEY_TAB: return tab;
                case KEY_Q: return q;
                case KEY_W: return w;
                case KEY_E: return e;
                case KEY_R: return r;
                case KEY_T: return t;
                case KEY_Y: return y;
                case KEY_U: return u;
                case KEY_I: return i;
                case KEY_O: return o;
                case KEY_P: return p;
                case KEY_LEFTBRACE: return left_brace;
                case KEY_RIGHTBRACE: return right_brace;
                case KEY_ENTER: return enter;
                case KEY_LEFTCTRL: return left_ctrl;
                case KEY_RIGHTSHIFT: return right_shift;
                case KEY_LEFTALT: return left_alt;
                case KEY_LEFTMETA: return left_meta;
                case KEY_RIGHTCTRL: return right_ctrl;
                case KEY_LEFTSHIFT: return left_shift;
                case KEY_RIGHTALT: return right_alt;
                case KEY_RIGHTMETA: return right_meta;
                case KEY_A: return a;
                case KEY_S: return s;
                case KEY_D: return d;
                case KEY_F: return f;
                case KEY_G: return g;
                case KEY_H: return h;
                case KEY_J: return j;
                case KEY_K: return k;
                case KEY_L: return l;
                case KEY_SEMICOLON: return semicolon;
                case KEY_APOSTROPHE: return apostrophe;
                case KEY_GRAVE: return grave;
                case KEY_BACKSLASH: return backslash;
                case KEY_Z: return z;
                case KEY_X: return x;
                case KEY_C: return c;
                case KEY_V: return v;
                case KEY_B: return b;
                case KEY_N: return n;
                case KEY_M: return m;
                case KEY_COMMA: return comma;
                case KEY_DOT: return dot;
                case KEY_SLASH: return slash;
                case KEY_SPACE: return space;
                case KEY_CAPSLOCK: return caps_lock;
                case KEY_F1: return f1;
                case KEY_F2: return f2;
                case KEY_F3: return f3;
                case KEY_F4: return f4;
                case KEY_F5: return f5;
                case KEY_F6: return f6;
                case KEY_F7: return f7;
                case KEY_F8: return f8;
                case KEY_F9: return f9;
                case KEY_F10: return f10;
                case KEY_F11: return f11;
                case KEY_F12: return f12;
                case KEY_NUMLOCK: return num_lock;
                case KEY_SCROLLLOCK: return scroll_lock;
                case KEY_KP1: return np_1;
                case KEY_KP2: return np_2;
                case KEY_KP3: return np_3;
                case KEY_KP4: return np_4;
                case KEY_KP5: return np_5;
                case KEY_KP6: return np_6;
                case KEY_KP7: return np_7;
                case KEY_KP8: return np_8;
                case KEY_KP9: return np_9;
                case KEY_KP0: return np_0;
                case KEY_KPASTERISK: return np_asterisk;
                case KEY_KPMINUS: return np_minus;
                case KEY_KPPLUS: return np_plus;
                case KEY_KPDOT: return np_dot;
                case KEY_KPENTER: return np_enter;
                case KEY_KPSLASH: return np_slash;
                case KEY_102ND:  return _102nd;
                case KEY_UP: return arrow_up;
                case KEY_LEFT: return arrow_left;
                case KEY_RIGHT: return arrow_right;
                case KEY_DOWN: return arrow_down;
                case KEY_PAGEUP: return page_up;
                case KEY_PAGEDOWN: return page_down;
                case KEY_HOME: return home;
                case KEY_END: return end;
                case KEY_INSERT: return insert;
                case KEY_DELETE: return _delete;
                case KEY_PAUSE: return _pause;
                default: return null;
            };
        };

        bool __is_mouse_controller_running = false;
        int flags;
        struct termios previous_term_state;
        
        void initalize(){
            if(__is_mouse_controller_running) return;
            __is_mouse_controller_running = true;

            struct termios term_state;
            if (tcgetattr(STDIN_FILENO, &term_state) == -1) return;
            memcpy(&previous_term_state, &term_state, sizeof(term_state));

            term_state.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF);
            term_state.c_lflag &= ~(ICANON | ISIG | IUCLC | IEXTEN | ECHO);
            term_state.c_oflag &= ~OPOST;
            term_state.c_iflag &= ~(IXANY | IMAXBEL);
            term_state.c_lflag &= ~XCASE;

            if (tcsetattr(STDIN_FILENO, TCSANOW, &term_state) == -1) return;

            flags = fcntl(STDIN_FILENO, F_GETFL, 0);
            fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
            printf("\033[?25l");
            fflush(stdout);
        };
        
        void shutdown(){
            fcntl(STDIN_FILENO, F_SETFL, flags);
            tcsetattr(STDIN_FILENO, TCSANOW, &previous_term_state);
            printf("\033[?25h");
            fflush(stdout);
        };

        int8_t mouse_data[3];
        void loop(window* win){
			int key = getchar();
			if(key == 3){
                shutdown();
			};
			if(key != -1){
                char buffer[2];
                if((key>=65 && key<=90) || (key>=97 && key<=122)){
                    sprintf(buffer, "%c", key);
                } else {
                    sprintf(buffer, "");
                };
                win->keyboard_handler(win, buffer, __convert_event_code_to_lunaris_code(key), 0, keyboard::pressed);
                win->keyboard_handler(win, buffer, __convert_event_code_to_lunaris_code(key), 0, keyboard::released);
            };
        };
    };
};