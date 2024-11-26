namespace lunaris {
    #include <wayland-client.h>
    #include <wayland-egl.h>
    #include <sys/mman.h>
    // keyboard
    #include <xkbcommon/xkbcommon.h>

    struct xkb_context* __xkb_context = NULL;
    struct xkb_keymap* __xkb_keymap = NULL;
    struct xkb_state* __xkb_state = NULL;

    #include </usr/include/linux/input-event-codes.h>
    // xdg
    #include "../../thirdparty/xdg/xdg-shell.h"
    #include "../../thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.h"
    #include "../../thirdparty/xdg/cursor-shape-v1.h"
    #include "../../thirdparty/xdg/tablet-v2.h"
    #include "../../thirdparty/xdg/xdg-shell.c"
    #include "../../thirdparty/xdg/xdg-decoration-unstable-v1-client-protocol.c"
    #include "../../thirdparty/xdg/cursor-shape-v1.c"
    #include "../../thirdparty/xdg/tablet-v2.c"

    #define COLOR_CHANNELS 4
    #define SHM_POOL_SIZE (1920*COLOR_CHANNELS*1080*4)

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

    int __create_shm_file(off_t size) {
        const char file_loc[] = "/wl_shm-XXXXXX";
        const char* path = getenv("XDG_RUNTIME_DIR");
        if (!path) {
            fprintf(stderr, "XDG_RUNTIME_DIR is undefined\n");
            exit(1);
        };

        char* name = (char*)malloc(strlen(path) + strlen(file_loc) + 1);
        sprintf(name, "%s%s", path, file_loc);
        int fd = mkstemp(name);
        if (fd < 0) {
            fprintf(stderr, "SHM file cannot be created: %s\n", strerror(errno));
            exit(1);
        };
        unlink(name);
        free(name);

        if (ftruncate(fd, size) < 0) {
            fprintf(stderr, "SHM file cannot be resized: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
        return fd;
    };

    const struct wl_keyboard_listener keyboard_listener = {
        .keymap = [](void* data, struct wl_keyboard* keyboard, uint32_t format, int fd, uint32_t size) { // Keymap aka keyboard layout
            if(__xkb_context == NULL){
                __xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

                char* keymap_string = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (keymap_string == MAP_FAILED) return;

                __xkb_keymap = xkb_keymap_new_from_string(__xkb_context, keymap_string, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
                close(fd);
                __xkb_state = xkb_state_new(__xkb_keymap);
            };
        },
        .enter = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface, struct wl_array* keys) {/* Keyboard focus (considered as window focus, mostly?) */},
        .leave = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, struct wl_surface* surface) {
            /* Keyboard unfocus (considered as window focus, mostly?) */
            lunaris::window* win = (lunaris::window*)data;
            win->__last_press_state = 0;
        },
        .key = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) { // Key un/presses
            lunaris::window* win = (lunaris::window*)data;
            if(win->keyboard_handler != NULL){
                xkb_keycode_t xkb_keycode = key + 8;
                xkb_keysym_t sym = xkb_state_key_get_one_sym(__xkb_state, xkb_keycode);

                char buffer[8] = "";
                xkb_keysym_to_utf8(sym, buffer, sizeof(buffer));

                if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                    if(strcmp(buffer,"") != 0){
                        win->keyboard_handler(win, buffer, __convert_event_code_to_lunaris_code(key), 0, keyboard::pressed);
                        sprintf(win->__last_press_buffer, "%s", buffer);
                    } else {
                        win->keyboard_handler(win, NULL, __convert_event_code_to_lunaris_code(key), 0, keyboard::pressed);
                    };
                    win->__last_press_state = 1;
                } else {
                    if(strcmp(buffer,"") != 0){
                        win->keyboard_handler(win, buffer, __convert_event_code_to_lunaris_code(key), 0, keyboard::released);
                        sprintf(win->__last_press_buffer, "%s", buffer);
                    } else {
                        win->keyboard_handler(win, NULL, __convert_event_code_to_lunaris_code(key), 0, keyboard::released);
                    };
                    win->__last_press_state = 0;
                };
                win->__last_press_key = key;
            };
        },
        .modifiers = [](void* data, struct wl_keyboard* keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) { // Modifiers change like ctrl, alt, shift
            xkb_state_update_mask(__xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, 0);
        },
        .repeat_info = [](void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t delay) {
            // TODO: Get repeating info from here to handle keyboard repeating as more system consistent
        }
    };

    void __backend_start_move(lunaris::window* win){
        xdg_toplevel_move(win->__xdg_toplevel, win->__seat, win->__mouse_last_serial);
    }

    void __backend_set_cursor(lunaris::window* win, lunaris::cursor::cursor cursor){
        if(win->__cursor_shape_manager == NULL) return;
        using namespace cursor;

        if(cursor == arrow) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);
        else if(cursor == text) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_TEXT);
        else if(cursor == wait) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_WAIT);
        else if(cursor == help) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_HELP);
        else if(cursor == pointer) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER);
        else if(cursor == crosshair) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CROSSHAIR);
        else if(cursor == not_allowed) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NOT_ALLOWED);
        else if(cursor == resize_ew) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_EW_RESIZE);
        else if(cursor == resize_ns) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NS_RESIZE);
        else if(cursor == resize_nwse) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NWSE_RESIZE);
        else if(cursor == resize_nesw) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NESW_RESIZE);
        else if(cursor == move) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_ALL_SCROLL);
        else if(cursor == progress) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_PROGRESS);
        else if(cursor == alias) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_ALIAS);
        else if(cursor == context_menu) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CONTEXT_MENU);
        else if(cursor == copy) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_COPY);
        else if(cursor == no_drop) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_NO_DROP);
        else if(cursor == grab) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_GRAB);
        else if(cursor == grabbing) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_GRABBING);
        else if(cursor == cell) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_CELL);
        else if(cursor == col_resize) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_COL_RESIZE);
        else if(cursor == row_resize) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_ROW_RESIZE);
        else if(cursor == vertical_text) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_VERTICAL_TEXT);
        else if(cursor == zoom_in) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_ZOOM_IN);
        else if(cursor == zoom_out) wp_cursor_shape_device_v1_set_shape(win->__cursor_shape_device, win->__mouse_last_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_ZOOM_OUT);
    };

    const struct wl_pointer_listener pointer_listener = {
        .enter=[](void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface, wl_fixed_t surface_x, wl_fixed_t surface_y){
            lunaris::window* win = (lunaris::window*)data;
            #ifdef LUNARIS_DEBUG
                printf("#pointer@enter %dx%d\n", (int)wl_fixed_to_double(surface_x), (int)wl_fixed_to_double(surface_y));
            #endif
            win->__mouse_x = (float)wl_fixed_to_double(surface_x);
            win->__mouse_y = (float)wl_fixed_to_double(surface_y);
            win->__mouse_last_serial = serial;
            if(win->mouse_handler != NULL){
                win->mouse_handler(win, win->__mouse_x, win->__mouse_y, 0, 0, 0, mouse::enter);
            };
        },
        .leave=[](void* data, struct wl_pointer* pointer, uint32_t serial, struct wl_surface* surface) {
            lunaris::window* win = (lunaris::window*)data;
            #ifdef LUNARIS_DEBUG
                printf("#pointer@left\n");
            #endif
            win->__mouse_last_serial = serial;
            if(win->mouse_handler != NULL){
                win->mouse_handler(win, win->__mouse_x, win->__mouse_y, 0, 0, 0, mouse::leave);
            };
        },
        .motion=[](void* data, struct wl_pointer* pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
            lunaris::window* win = (lunaris::window*)data;
            #ifdef LUNARIS_DEBUG
                printf("#pointer@move %dx%d\n", (int)wl_fixed_to_double(surface_x), (int)wl_fixed_to_double(surface_y));
            #endif
            win->__mouse_x = (float)wl_fixed_to_double(surface_x);
            win->__mouse_y = (float)wl_fixed_to_double(surface_y);
            if(win->mouse_handler != NULL){
                win->mouse_handler(win, win->__mouse_x, win->__mouse_y, 0, 0, 0, mouse::motion);
            }
        },
        .button=[](void* data, struct wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
            lunaris::window* win = (lunaris::window*)data;
            #ifdef LUNARIS_DEBUG
                const char* states[] = {"Released", "Pressed"};
                printf("#pointer@button %d | %s\n", button, states[(int)(state == WL_POINTER_BUTTON_STATE_PRESSED)]);
            #endif
            win->__mouse_last_serial = serial;
            if(win->mouse_handler != NULL){
                lunaris::mouse::mouse the_event = mouse::first;
                if(button == 272) the_event = mouse::first;
                if(button == 273) the_event = mouse::second;
                if(button == 274) the_event = mouse::middle;
                if(button == 275) the_event = mouse::x1;
                if(button == 276) the_event = mouse::x2;
                win->mouse_handler(win, win->__mouse_x, win->__mouse_y, state == WL_POINTER_BUTTON_STATE_PRESSED, 0, 0, the_event);
            };
        },
        .axis=[](void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value) {
            lunaris::window* win = (lunaris::window*)data;
            #ifdef LUNARIS_DEBUG
                const char* states[] = {"Vertical (Y)", "Horizontal (X)"};
                printf("#pointer@axis %f | %s\n", wl_fixed_to_double(value), states[axis]);
            #endif
            if(win->mouse_handler != NULL){
                if(axis == 0){ // Vertical (Y)
                    win->mouse_handler(win, win->__mouse_x, win->__mouse_y, 0, 0, wl_fixed_to_double(value), mouse::scroll);
                } else { // == 1; Horizontal (X)
                    win->mouse_handler(win, win->__mouse_x, win->__mouse_y, 0, wl_fixed_to_double(value), 0, mouse::scroll);
                };
            };
        },
        .frame=[](void* data, struct wl_pointer* pointer) {},
        .axis_source=[](void* data, struct wl_pointer* pointer, uint32_t axis_source) {},
        .axis_stop=[](void* data, struct wl_pointer* pointer, uint32_t time, uint32_t axis) {},
        .axis_discrete=[](void* data, struct wl_pointer* pointer, uint32_t axis, int32_t discrete) {},
        .axis_value120=[](void* data, struct wl_pointer* pointer, uint32_t axis, int32_t value120){},
        .axis_relative_direction=[](void* data, struct wl_pointer* pointer, uint32_t axis, uint32_t direction){}
    };

    struct wl_seat_listener seat_listener = {
        .capabilities = [](void*  data, struct wl_seat *seat, uint32_t capabilities) {
            lunaris::window* win = (lunaris::window*)data;
            if (capabilities & WL_SEAT_CAPABILITY_POINTER) {
                // Bind the pointer object when available
                if(win->__pointer == NULL) {
                    win->__pointer = wl_seat_get_pointer(seat);
                    wl_pointer_add_listener(win->__pointer, &pointer_listener, win);
                    if(win->__cursor_shape_manager != NULL){
                        win->__cursor_shape_device = wp_cursor_shape_manager_v1_get_pointer(win->__cursor_shape_manager, win->__pointer);
                    };
                }
            }
            if (capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
                win->__keyboard = wl_seat_get_keyboard(seat);
                wl_keyboard_add_listener(win->__keyboard, &keyboard_listener, win);
            }
        },
        .name = [](void* data, struct wl_seat *seat, const char *name) {}
    };

    void __backend_set_fullscreen(lunaris::window* win, bool fullscreen) {
        if (win->__xdg_toplevel) {
            if(fullscreen){
                xdg_toplevel_set_fullscreen(win->__xdg_toplevel, NULL /*wl_output: for choosing which screen*/);
            } else {
                xdg_toplevel_unset_fullscreen(win->__xdg_toplevel);
            }
            wl_surface_commit(win->__surface);
        }
    }

    const struct wl_registry_listener registry_listener = {
        .global = [](void* data, struct wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
            lunaris::window* win = (lunaris::window*)data;
            if (strcmp(interface, wl_compositor_interface.name) == 0) {
                win->__compositor = (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 4);
            } else if (strcmp(interface, wl_shm_interface.name) == 0) {
                win->__shm = (struct wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
            } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
                win->__xdg_wm_base = (struct xdg_wm_base*)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
            } else if (strcmp(interface, wl_pointer_interface.name) == 0) {
                win->__pointer = (struct wl_pointer*)wl_registry_bind(registry, id, &wl_pointer_interface, version);
            } else if (strcmp(interface, wl_seat_interface.name) == 0) {
                win->__seat = (struct wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, version);
                wl_seat_add_listener(win->__seat, &seat_listener, win);
            } else if (strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0) {
                win->__zxdg_decoration_manager_id = id;
            } else if (strcmp(interface, wp_cursor_shape_manager_v1_interface.name) == 0) {
                win->__cursor_shape_manager = (struct wp_cursor_shape_manager_v1*)wl_registry_bind(registry, id, &wp_cursor_shape_manager_v1_interface, 1);
            }
        },
        .global_remove = [](void* data, struct wl_registry* registry, uint32_t id) {},
    };

    const struct xdg_wm_base_listener xdg_wm_base_listener = {
        .ping = [](void* data, struct xdg_wm_base* xdg_wm_base, uint32_t serial) {
            xdg_wm_base_pong(xdg_wm_base, serial);
        },
    };

    void draw(lunaris::window* win){
        if(win->__hidden == 2) return; // Don't update buffer while window is hidden.
        win->buffer = (uint32_t*)win->__shm_data;
        if(win->draw_handler != NULL){
            win->draw_handler(win, win->buffer);
        } else {
            for (int i = 0; i < win->width * win->height; i++) {
                win->buffer[i] = 0xFF000000;
            };
        };

        if(win->__hidden){
            wl_surface_attach(win->__surface, NULL, 0, 0);
            xdg_toplevel_set_minimized(win->__xdg_toplevel); // Minimize the window to hide flickering while changing buffer at undecorated windows
            win->__hidden = 2;
        } else { // win->__hidden == 0
            wl_surface_attach(win->__surface, win->__buffer, 0, 0);
        };
        wl_surface_damage(win->__surface, 0, 0, win->width, win->height);
        wl_surface_commit(win->__surface);
        wl_display_flush(win->__display);
        std::swap(win->__shm_fd, win->__seconder_shm_fd);
        std::swap(win->__shm_data, win->__seconder_shm_data);
        std::swap(win->__buffer, win->__seconder_buffer);
    };

    const struct xdg_surface_listener xdg_surface_listener = {
        .configure = [](void* data, struct xdg_surface* xdg_surface, uint32_t serial) {
            lunaris::window* win = (lunaris::window*)data;
            xdg_surface_ack_configure(xdg_surface, serial);
            //draw(win);
        },
    };

    void __backend_set_visibility(lunaris::window* win, bool visible){
        win->__hidden = !visible;
    }

    bool __backend_set_decoration(lunaris::window* win, bool decorated) {
        if(win->__zxdg_decoration_manager_id == 0){
            return false;
        };
        if(win->__zxdg_decoration_manager == NULL) win->__zxdg_decoration_manager = (zxdg_decoration_manager_v1*)wl_registry_bind(wl_display_get_registry(win->__display), win->__zxdg_decoration_manager_id, &zxdg_decoration_manager_v1_interface, 1);
        if(win->__zxdg_decoration == NULL) win->__zxdg_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(win->__zxdg_decoration_manager, win->__xdg_toplevel);
        if(decorated){
            zxdg_toplevel_decoration_v1_set_mode(win->__zxdg_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
        } else {
            zxdg_toplevel_decoration_v1_set_mode(win->__zxdg_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE);
        };
        return true;
    };

    void __backend_resize(lunaris::window* win, int w, int h){
        win->width = w;
        win->height = h;
        win->__shm_pool = wl_shm_create_pool(win->__shm, win->__shm_fd, SHM_POOL_SIZE);
        win->__buffer = wl_shm_pool_create_buffer(win->__shm_pool, 0, win->width, win->height, win->width*COLOR_CHANNELS, WL_SHM_FORMAT_ARGB8888);
        wl_shm_pool_destroy(win->__shm_pool);
        win->__seconder_shm_pool = wl_shm_create_pool(win->__shm, win->__seconder_shm_fd, SHM_POOL_SIZE);
        win->__seconder_buffer = wl_shm_pool_create_buffer(win->__seconder_shm_pool, 0, win->width, win->height, win->width*COLOR_CHANNELS, WL_SHM_FORMAT_ARGB8888);
        wl_shm_pool_destroy(win->__seconder_shm_pool);
    };

    const struct xdg_toplevel_listener toplevel_listener = {
        .configure = [](void* data, struct xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, struct wl_array* states) {
            if (width > 0 && height > 0) {
                lunaris::window* win = (lunaris::window*)data;
                win->resize(width, height);
                //draw(win);
            }
        },
        .close = [](void* data, struct xdg_toplevel *xdg_toplevel) {
            lunaris::window* win = (lunaris::window*)data;
            win->destroy();
        }
    };


    void* __frame_triggerer(void* data) {
        lunaris::window* win = (lunaris::window*)data;
        const int frame_time = 1000/60;
        while(win->is_alive) {
            auto start = std::chrono::high_resolution_clock::now();
            draw(win);
            if(win->__last_press_state > 0){
                if(++win->__last_press_state > 14 /*16ms*13(=12+1[init value])=208ms */ && win->__last_press_state%2 == 0 /*16ms*3=32ms*/){
                    win->keyboard_handler(win, win->__last_press_buffer, __convert_event_code_to_lunaris_code(win->__last_press_key), 0, keyboard::repeated);
                };
            };
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

            if (elapsed.count() < frame_time) {
                std::this_thread::sleep_for(std::chrono::milliseconds(frame_time) - elapsed);
            };
        };
        win->safe_shutdown = false;
        return NULL;
    };

    lunaris::window* new_window(){
        lunaris::window* win = (lunaris::window*)malloc(sizeof(lunaris::window));
        win->graphics.__init(win);
        win->is_alive = true;

        win->__display = wl_display_connect(NULL);
        if (!win->__display) {
            fprintf(stderr, "Failed to connect to Wayland display\n");
            return (lunaris::window*)NULL;
        };

        win->__registry = wl_display_get_registry(win->__display);
        wl_registry_add_listener(win->__registry, &registry_listener, win);
        wl_display_roundtrip(win->__display);

        if (!win->__compositor || !win->__shm || !win->__xdg_wm_base) {
            fprintf(stderr, "Failed to get required Wayland globals\n");
            return (lunaris::window*)NULL;
        };

        xdg_wm_base_add_listener(win->__xdg_wm_base, &xdg_wm_base_listener, win);

        win->__surface = wl_compositor_create_surface(win->__compositor);
        win->__xdg_surface = xdg_wm_base_get_xdg_surface(win->__xdg_wm_base, win->__surface);
        xdg_surface_add_listener(win->__xdg_surface, &xdg_surface_listener, win);
        win->__xdg_toplevel = xdg_surface_get_toplevel(win->__xdg_surface);

        win->set_fullscreen(false);
        win->set_title((char*)"<Untitled>");

        xdg_toplevel_add_listener(win->__xdg_toplevel, &toplevel_listener, win);

        wl_surface_commit(win->__surface);

        win->__shm_fd = __create_shm_file(SHM_POOL_SIZE);
        win->__shm_data = (uint8_t*)mmap(NULL, SHM_POOL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, win->__shm_fd, 0);
        if (win->__shm_data == MAP_FAILED) {
            fprintf(stderr, "mmap failed: %s\n", strerror(errno));
            close(win->__shm_fd);
            return (lunaris::window*)NULL;
        }

        win->__seconder_shm_fd = __create_shm_file(SHM_POOL_SIZE);
        win->__seconder_shm_data = (uint8_t*)mmap(NULL, SHM_POOL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, win->__seconder_shm_fd, 0);
        if (win->__seconder_shm_data == MAP_FAILED) {
            fprintf(stderr, "mmap failed: %s\n", strerror(errno));
            close(win->__seconder_shm_fd);
            return (lunaris::window*)NULL;
        }

        win->resize(800, 600);
        return win;
    };

    void __backend_loop(lunaris::window* win){
        pthread_t thread;
        int result = pthread_create(&thread, NULL, __frame_triggerer, (void*)win);
        if (result != 0) {
            fprintf(stderr, "Error creating thread\n");
            return;
        };
        pthread_detach(thread);

        draw(win);
        while (win->is_alive && wl_display_dispatch(win->__display) != -1) {};

        pthread_join(thread, NULL);
        for(;win->safe_shutdown;){};

        win->destroy();
    };

    void __backend_destroy(lunaris::window* win){
        if(win->is_alive){
            win->is_alive = false;
            for(;win->safe_shutdown;){};
            munmap(win->__shm_data, SHM_POOL_SIZE);
            win->__shm_data = NULL;
            close(win->__shm_fd);
            win->__shm_fd = 0;
            wl_pointer_destroy(win->__pointer);
            win->__pointer = NULL;
            wl_buffer_destroy(win->__buffer);
            win->__buffer = NULL;
            wl_buffer_destroy(win->__seconder_buffer);
            win->__seconder_buffer = NULL;
            xdg_toplevel_destroy(win->__xdg_toplevel);
            win->__xdg_toplevel = NULL;
            xdg_surface_destroy(win->__xdg_surface);
            win->__xdg_surface = NULL;
            wl_surface_destroy(win->__surface);
            win->__surface = NULL;
            xdg_wm_base_destroy(win->__xdg_wm_base);
            win->__xdg_wm_base = NULL;
            wl_compositor_destroy(win->__compositor);
            win->__compositor = NULL;
            wl_shm_destroy(win->__shm);
            win->__shm = NULL;
            wl_registry_destroy(win->__registry);
            win->__registry = NULL;
            wl_display_disconnect(win->__display);
            win->__display = NULL;
        };
    };

    void __backend_show_window_menu(lunaris::window* win, int x, int y){
        xdg_toplevel_show_window_menu(win->__xdg_toplevel, win->__seat, win->__mouse_last_serial, x, y);
    };

    void __backend_minimize(lunaris::window* win){
        xdg_toplevel_set_minimized(win->__xdg_toplevel);
    };

    void __backend_maximize(lunaris::window* win){
        xdg_toplevel_set_maximized(win->__xdg_toplevel);
    };

    void __backend_restore(lunaris::window* win){
        xdg_toplevel_unset_maximized(win->__xdg_toplevel);
    };

    void __backend_start_resize(lunaris::window* win){
        xdg_toplevel_resize(win->__xdg_toplevel, win->__seat, win->__mouse_last_serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT);
    };

    void __backend_set_title(lunaris::window* win, const char* title){
        xdg_toplevel_set_title(win->__xdg_toplevel, title);
    };
}
