#pragma once

#include <cstdio>
#include <stdint.h>

namespace lunaris {
    LRESULT CALLBACK __WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam);
    HKL keyboard_layout = NULL;

    HBITMAP __createHBitmap (HWND hwnd, int width, int height, void** pixels){
        HBITMAP bitmap = NULL;
        BITMAPINFO bitmap_info;
        ZeroMemory(&bitmap_info, sizeof(bitmap_info));
        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = -height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = 32;
        bitmap_info.bmiHeader.biCompression = BI_RGB;

        HDC hdc = GetDC(hwnd);
        bitmap = CreateDIBSection(hdc, &bitmap_info, DIB_RGB_COLORS, pixels, NULL, 0);
        ReleaseDC(hwnd, hdc);
        return bitmap;
    };
    HBITMAP __createHBitmap (lunaris::window* win){
        return __createHBitmap(win->__hwnd, win->width, win->height, &win->__buffer);
    };

    void __request_new_frame(lunaris::window* win){ // Invalidate rect and force to create new WM_PAINT event
        InvalidateRect(win->__hwnd, NULL, TRUE);
        UpdateWindow(win->__hwnd);
    };

    lunaris::keycode::keycode __convert_event_code_to_lunaris_code(WPARAM wparam, LPARAM lparam){ /* https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes / https://learn.microsoft.com/en-us/uwp/api/windows.system.virtualkey */
    using namespace lunaris::keycode;
        switch (wparam) {
            case VK_ESCAPE: return escape;
            case 0x31: return _1;
            case 0x32: return _2;
            case 0x33: return _3;
            case 0x34: return _4;
            case 0x35: return _5;
            case 0x36: return _6;
            case 0x37: return _7;
            case 0x38: return _8;
            case 0x39: return _9;
            case 0x30: return _0;
            #ifndef LUNARIS_SUPPORT_WIN_2000
            case VK_OEM_MINUS: return minus;
            case VK_OEM_PLUS: return _equal;
            #endif
            case VK_BACK: return backspace;
            case VK_TAB: return tab;
            case 0x51: return q;
            case 0x57: return w;
            case 0x45: return e;
            case 0x52: return r;
            case 0x54: return t;
            case 0x59: return y;
            case 0x55: return u;
            case 0x49: return i;
            case 0x4F: return o;
            case 0x50: return p;
            case VK_OEM_4: return left_brace;
            case VK_OEM_6: return right_brace;
            case VK_RETURN: { // VK_SEPARATOR is also numpad enter, however Windows sometimes can report numpad-enter as enter
                if((lparam & 0x01000000) != 0) return np_enter; // Numpad-enter: Send same key with VK_SEPARATOR
                return enter;
            };
            case VK_LCONTROL: return left_ctrl;
            case VK_LSHIFT: return left_shift;
            case VK_LMENU: return left_alt;
            case VK_LWIN: return left_meta;
            case VK_RSHIFT: return right_shift;
            case VK_RCONTROL: return right_ctrl;
            case VK_RMENU: return right_alt;
            case VK_RWIN: return right_meta;
            case VK_CONTROL: { // Windows may report LEFT/RIGHT CTRL without place info (like VK_RETURN/VK_SEPERATOR)
                if((lparam & 0x01000000) != 0) return right_ctrl; // VK_RCONTROL
                return left_ctrl; // VK_LCONTROL
            };
            case VK_SHIFT: { // Windows may report LEFT/RIGHT SHIFT without place info (like VK_RETURN/VK_SEPERATOR)
                if(((lparam>>18)&0x01) == 0x01 /*This is highly dependent on OEM behaviour, checked byte is from OEM spesific-region however it's works mostly*/) return right_shift; // VK_RSHIFT
                return left_shift; // VK_LSHIFT
            };
            case VK_MENU: { // Windows may report LEFT/RIGHT ALT without place info (like VK_RETURN/VK_SEPERATOR)
                if((lparam & 0x01000000) != 0) return right_alt; // VK_RMENU
                return left_alt; // VK_LMENU
            };
            case 0x41: return a;
            case 0x53: return s;
            case 0x44: return d;
            case 0x46: return f;
            case 0x47: return g;
            case 0x48: return h;
            case 0x4A: return j;
            case 0x4B: return k;
            case 0x4C: return l;
            case VK_OEM_1: return semicolon;
            case VK_OEM_7: return apostrophe;
            case VK_OEM_3: return grave;
            case VK_OEM_5: return backslash;
            case 0x5A: return z;
            case 0x58: return x;
            case 0x43: return c;
            case 0x56: return v;
            case 0x42: return b;
            case 0x4E: return n;
            case 0x4D: return m;
            #ifndef LUNARIS_SUPPORT_WIN_2000
            case VK_OEM_COMMA: return comma;
            case VK_OEM_PERIOD: return dot;
            #endif
            case VK_OEM_2: return slash;
            case VK_SPACE: return space;
            case VK_CAPITAL: return caps_lock;
            case VK_F1: return f1;
            case VK_F2: return f2;
            case VK_F3: return f3;
            case VK_F4: return f4;
            case VK_F5: return f5;
            case VK_F6: return f6;
            case VK_F7: return f7;
            case VK_F8: return f8;
            case VK_F9: return f9;
            case VK_F10: return f10;
            case VK_F11: return f11;
            case VK_F12: return f12;
            case VK_NUMLOCK: return num_lock;
            case VK_SCROLL: return scroll_lock;
            case VK_NUMPAD1: return np_1;
            case VK_NUMPAD2: return np_2;
            case VK_NUMPAD3: return np_3;
            case VK_NUMPAD4: return np_4;
            case VK_NUMPAD5: return np_5;
            case VK_NUMPAD6: return np_6;
            case VK_NUMPAD7: return np_7;
            case VK_NUMPAD8: return np_8;
            case VK_NUMPAD9: return np_9;
            case VK_NUMPAD0: return np_0;
            case VK_MULTIPLY: return np_asterisk;
            case VK_SUBTRACT: return np_minus;
            case VK_ADD: return np_plus;
            case VK_DECIMAL: return np_dot;
            case VK_SEPARATOR: return np_enter;
            case VK_DIVIDE: return np_slash;
            #ifndef LUNARIS_SUPPORT_WIN_2000
            case VK_OEM_102:  return _102nd;
            #endif
            case VK_UP: return arrow_up;
            case VK_LEFT: return arrow_left;
            case VK_RIGHT: return arrow_right;
            case VK_DOWN: return arrow_down;
            case VK_PRIOR: return page_up;
            case VK_NEXT: return page_down;
            case VK_HOME: return home;
            case VK_END: return end;
            case VK_INSERT: return insert;
            case VK_DELETE: return _delete;
            case VK_PAUSE: return _pause;
            default: return null;
        };
    };

    #ifdef WINDOWS_EXTENDED_CURSORS
    #include "extended_windows_cursors/cursors.h"
    HCURSOR __load_extended_cursor_uncached(int cursor_id){
        return (HCURSOR)LoadImage(
            GetModuleHandle(NULL),
            MAKEINTRESOURCE(cursor_id),
            IMAGE_CURSOR,
            0, 0,
            LR_DEFAULTSIZE | LR_SHARED
        );
    };

    HCURSOR __load_extended_cursor(int cursor_id){
        static HCURSOR cursor_alias = __load_extended_cursor_uncached(CURSORS_ALIAS);
        static HCURSOR cursor_context_menu = __load_extended_cursor_uncached(CURSORS_CONTEXT_MENU);
        static HCURSOR cursor_copy = __load_extended_cursor_uncached(CURSORS_COPY);
        static HCURSOR cursor_no_drop = __load_extended_cursor_uncached(CURSORS_NO_DROP);
        static HCURSOR cursor_grab = __load_extended_cursor_uncached(CURSORS_OPENHAND);
        static HCURSOR cursor_grabbing = __load_extended_cursor_uncached(CURSORS_CLOSEDHAND);
        static HCURSOR cursor_cell = __load_extended_cursor_uncached(CURSORS_PLUS);
        static HCURSOR cursor_col_resize = __load_extended_cursor_uncached(CURSORS_SPLIT_H);
        static HCURSOR cursor_row_resize = __load_extended_cursor_uncached(CURSORS_SPLIT_V);
        static HCURSOR cursor_vertical_text = __load_extended_cursor_uncached(CURSORS_VERTICAL_TEXT);
        static HCURSOR cursor_zoom_in = __load_extended_cursor_uncached(CURSORS_ZOOM_IN);
        static HCURSOR cursor_zoom_out = __load_extended_cursor_uncached(CURSORS_ZOOM_OUT);
        
        if(cursor_id == CURSORS_ALIAS) return cursor_alias;
        if(cursor_id == CURSORS_CLOSEDHAND) return cursor_grabbing;
        if(cursor_id == CURSORS_CONTEXT_MENU) return cursor_context_menu;
        if(cursor_id == CURSORS_COPY) return cursor_copy;
        if(cursor_id == CURSORS_NO_DROP) return cursor_no_drop;
        if(cursor_id == CURSORS_OPENHAND) return cursor_grab;
        if(cursor_id == CURSORS_PLUS) return cursor_cell;
        if(cursor_id == CURSORS_SPLIT_H) return cursor_col_resize;
        if(cursor_id == CURSORS_SPLIT_V) return cursor_row_resize;
        if(cursor_id == CURSORS_VERTICAL_TEXT) return cursor_vertical_text;
        if(cursor_id == CURSORS_ZOOM_IN) return cursor_zoom_in;
        if(cursor_id == CURSORS_ZOOM_OUT) return cursor_zoom_out;
        return cursor_cell; // Fallback :D
    };
    #endif

    void __backend_set_cursor(lunaris::window* win, lunaris::cursor::cursor cursor){ // https://learn.microsoft.com/en-us/windows/win32/menurc/about-cursors
        HCURSOR new_cursor;
        /*if(cursor == arrow)*/ new_cursor = LoadCursor(NULL, IDC_ARROW);
        /*else*/ if(cursor == lunaris::cursor::text) new_cursor = LoadCursor(NULL, IDC_IBEAM);
        else if(cursor == lunaris::cursor::wait) new_cursor = LoadCursor(NULL, IDC_WAIT);
        else if(cursor == lunaris::cursor::help) new_cursor = LoadCursor(NULL, IDC_HELP);
        else if(cursor == lunaris::cursor::pointer) new_cursor = LoadCursor(NULL, IDC_HAND);
        else if(cursor == lunaris::cursor::crosshair) new_cursor = LoadCursor(NULL, IDC_CROSS);
        else if(cursor == lunaris::cursor::not_allowed) new_cursor = LoadCursor(NULL, IDC_NO);
        else if(cursor == lunaris::cursor::resize_ew) new_cursor = LoadCursor(NULL, IDC_SIZEWE);
        else if(cursor == lunaris::cursor::resize_ns) new_cursor = LoadCursor(NULL, IDC_SIZENS);
        else if(cursor == lunaris::cursor::resize_nwse) new_cursor = LoadCursor(NULL, IDC_SIZENWSE);
        else if(cursor == lunaris::cursor::resize_nesw) new_cursor = LoadCursor(NULL, IDC_SIZENESW);
        else if(cursor == lunaris::cursor::move) new_cursor = LoadCursor(NULL, IDC_SIZEALL);
        else if(cursor == lunaris::cursor::progress) new_cursor = LoadCursor(NULL, IDC_APPSTARTING);
        #ifdef WINDOWS_EXTENDED_CURSORS
        else if(cursor == lunaris::cursor::alias) new_cursor = __load_extended_cursor(CURSORS_ALIAS);
        else if(cursor == lunaris::cursor::context_menu) new_cursor = __load_extended_cursor(CURSORS_CONTEXT_MENU);
        else if(cursor == lunaris::cursor::copy) new_cursor = __load_extended_cursor(CURSORS_COPY);
        else if(cursor == lunaris::cursor::no_drop) new_cursor = __load_extended_cursor(CURSORS_NO_DROP);
        else if(cursor == lunaris::cursor::grab) new_cursor = __load_extended_cursor(CURSORS_OPENHAND);
        else if(cursor == lunaris::cursor::grabbing) new_cursor = __load_extended_cursor(CURSORS_CLOSEDHAND);
        else if(cursor == lunaris::cursor::cell) new_cursor = __load_extended_cursor(CURSORS_PLUS);
        else if(cursor == lunaris::cursor::col_resize) new_cursor = __load_extended_cursor(CURSORS_SPLIT_H);
        else if(cursor == lunaris::cursor::row_resize) new_cursor = __load_extended_cursor(CURSORS_SPLIT_V);
        else if(cursor == lunaris::cursor::vertical_text) new_cursor = __load_extended_cursor(CURSORS_VERTICAL_TEXT);
        else if(cursor == lunaris::cursor::zoom_in) new_cursor = __load_extended_cursor(CURSORS_ZOOM_IN);
        else if(cursor == lunaris::cursor::zoom_out) new_cursor = __load_extended_cursor(CURSORS_ZOOM_OUT);
        #endif
        SetClassLongPtr(win->__hwnd, GCLP_HCURSOR, (LONG_PTR)new_cursor);
        SetCursor(new_cursor);
    };

    void* __frame_triggerer(void* data) {
        lunaris::window* win = (lunaris::window*)data;
        const int frame_time = 1000/60;
        while(win->is_alive) {
            auto start = std::chrono::high_resolution_clock::now();
            __request_new_frame(win);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end - start;

            if (elapsed.count() < frame_time) {
                #ifndef LUNARIS_SUPPORT_WIN_2000
                    std::this_thread::sleep_for(std::chrono::milliseconds(frame_time) - elapsed);
                #else
                    usleep(frame_time-elapsed.count());
                #endif
            }
        }
        return NULL;
    };

    lunaris::window* new_window(void){
        lunaris::window* win = (lunaris::window*)malloc(sizeof(lunaris::window));
        win->graphics.__init(win);
        win->is_alive = true;

        const char CLASS_NAME[] = "lunaris_window";

        WNDCLASS wc = {};
        wc.lpfnWndProc = __WindowProc;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);

        RegisterClass(&wc);

        #ifndef WIN32_ALPHA_SUPPORT
            win->__hwnd = CreateWindowEx(0, CLASS_NAME, "<Untitled>", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 10, 10, NULL, NULL, NULL, NULL);
        #else
            win->__hwnd = CreateWindowEx(WS_EX_LAYERED, CLASS_NAME, "<Untitled>", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 10, 10, NULL, NULL, NULL, NULL);
        #endif

        if (win->__hwnd == NULL) return 0;

        SetWindowLongPtr(win->__hwnd, GWLP_USERDATA, (LONG_PTR)(win));
        ShowWindow(win->__hwnd, SW_NORMAL);
        win->resize(800, 600);
        win->set_decoration(false);

        pthread_t thread;
        int result = pthread_create(&thread, NULL, __frame_triggerer, (void*)win);
        pthread_detach(thread);
        if (result != 0) {
            fprintf(stderr, "Error creating thread\n");
            return NULL;
        };
        
        return win;
    };

    void __backend_loop(lunaris::window* win){
        MSG msg = {};
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    };

    void __backend_start_move(lunaris::window* win){
        PostMessage(win->__hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    };

    void __backend_set_visibility(lunaris::window* win, bool visible){
        if(visible){
            ShowWindow(win->__hwnd, SW_NORMAL);
        } else {
            ShowWindow(win->__hwnd, SW_HIDE);
        };
    };

    void __backend_resize(lunaris::window* win, int width, int height){
        win->width = width;
        win->height = height;
        RECT rect = {0, 0, width, height};
        AdjustWindowRect(&rect, GetWindowLong(win->__hwnd, GWL_STYLE), FALSE);
        SetWindowPos(win->__hwnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
        if(win->__bitmap != NULL) DeleteObject(win->__bitmap);
        win->__bitmap = __createHBitmap(win);
    };

    void __backend_destroy(lunaris::window* win){
        DestroyWindow(win->__hwnd);
    };

    void __backend_set_title(lunaris::window* win, const char* title){
        SetWindowText(win->__hwnd, title);
    };

    bool __backend_set_decoration(lunaris::window* win, bool decorated){
        LONG lStyle;
        if(decorated){
            lStyle = GetWindowLong(win->__hwnd, GWL_STYLE);
            lStyle |= WS_OVERLAPPEDWINDOW;
        } else {
            lStyle = GetWindowLong(win->__hwnd, GWL_STYLE);
            lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        }
        SetWindowLong(win->__hwnd, GWL_STYLE, lStyle);
        SetWindowPos(win->__hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
        return true;
    };

    void __backend_set_fullscreen(lunaris::window* win, bool fullscreen){
        win->set_decoration(!fullscreen);
        if(fullscreen) {
            ShowWindow(win->__hwnd, SW_MAXIMIZE);
        } else {
            ShowWindow(win->__hwnd, SW_RESTORE);
        }
    };

    void __backend_minimize(lunaris::window* win){
        ShowWindow(win->__hwnd, SW_MINIMIZE);
    };
    void __backend_maximize(lunaris::window* win){
        #ifndef WIN32_ALPHA_SUPPORT
            ShowWindow(win->__hwnd, SW_MAXIMIZE);
        #else
            POINT cursor_pos;
            GetCursorPos(&cursor_pos);
            HMONITOR hmonitor = MonitorFromPoint(cursor_pos, MONITOR_DEFAULTTONEAREST);

            MONITORINFO monitor_info;
            monitor_info.cbSize = sizeof(MONITORINFO);
            GetMonitorInfo(hmonitor, &monitor_info);

            GetWindowRect(win->__hwnd, &win->__unmaximized_size);

            SetWindowPos(win->__hwnd, NULL,
                        monitor_info.rcWork.left, monitor_info.rcWork.top,
                        monitor_info.rcWork.right-monitor_info.rcWork.left, monitor_info.rcWork.bottom-monitor_info.rcWork.top, 0);
        #endif
    };
    void __backend_restore(lunaris::window* win){
        #ifndef WIN32_ALPHA_SUPPORT
            ShowWindow(win->__hwnd, SW_RESTORE);
        #else
            SetWindowPos(win->__hwnd, NULL,
                        win->__unmaximized_size.left, win->__unmaximized_size.top,
                        win->__unmaximized_size.right-win->__unmaximized_size.left, win->__unmaximized_size.bottom-win->__unmaximized_size.top, 0);
        #endif
    };
    void __backend_start_resize(lunaris::window* win){
        PostMessage(win->__hwnd, WM_SYSCOMMAND, SC_SIZE + WMSZ_BOTTOMRIGHT, 0);
    };
    void __backend_show_window_menu(lunaris::window* win, int rx, int ry){
        HMENU context_menu = GetSystemMenu(win->__hwnd, FALSE);
        POINT cursor_point;
        GetCursorPos(&cursor_point);
        TrackPopupMenu(context_menu, TPM_RIGHTBUTTON, cursor_point.x, cursor_point.y, 0, win->__hwnd, NULL);
    };

    LRESULT CALLBACK __WindowProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam) {
        lunaris::window* win = (lunaris::window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        switch (msg) { // https://learn.microsoft.com/en-us/windows/win32/winmsg/about-messages-and-message-queues#system-defined-messages
            case WM_CREATE: {
                keyboard_layout = GetKeyboardLayout(0); // Get the current keyboard layout
                return 0;
            };
            case WM_PAINT: {
                PAINTSTRUCT paint_struct;
                HDC hdc = BeginPaint(hwnd, &paint_struct);

                if (win->__bitmap) {
                    HDC memdc = CreateCompatibleDC(hdc);
                    #ifndef WIN32_ALPHA_SUPPORT
                        SelectObject(memdc, win->__bitmap);
                    #else
                        HBITMAP holdbitmap = (HBITMAP)SelectObject(memdc, win->__bitmap);
                    #endif

                    win->buffer = (uint32_t*)win->__buffer;
                    if(win->draw_handler != NULL){
                        win->draw_handler(win, win->buffer);
                    } else {
                        for (int i = 0; i < win->width * win->height; i++) {
                            win->buffer[i] = 0xFF000000;
                        };
                    };

                    #ifndef WIN32_ALPHA_SUPPORT
                        BitBlt(hdc, 0, 0, win->width, win->height, memdc, 0, 0, SRCCOPY);
                    #else
                        SIZE win_size = {win->width, win->height};
                        POINT src_point = {0, 0};
                        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
                        UpdateLayeredWindow(hwnd, hdc, NULL, &win_size, memdc, &src_point, 0, &blend, ULW_ALPHA);
                        SelectObject(memdc, holdbitmap);
                    #endif
                    DeleteDC(memdc);
                    #ifdef WIN32_ALPHA_SUPPORT
                        ReleaseDC(NULL, hdc);
                    #endif
                }

                EndPaint(hwnd, &paint_struct);
                return 0;
            };
            case WM_DESTROY: {
                win->is_alive = false;
                if (win->__bitmap != NULL) {
                    DeleteObject(win->__bitmap);
                }
                PostQuitMessage(0);
                return 0;
            };
            case WM_SIZE: {
                win->width = LOWORD(lparam);
                win->height = HIWORD(lparam);
                if(win->__bitmap != NULL) DeleteObject(win->__bitmap);
                win->__bitmap = __createHBitmap(win);
                return 0;
            };
            case WM_MOUSEHOVER: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@enter %dx%d\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::enter);
                };
                return 0;
            };
            case WM_MOUSELEAVE: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@leave %dx%d\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::leave);
                };
                return 0;
            };
            case WM_MOUSEMOVE: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@motion %dx%d\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::motion);
                };
                return 0;
            };
            case WM_LBUTTONDOWN: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button left %dx%d | press\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 1, 0, 0, lunaris::mouse::first);
                };
                return 0;
            };
            case WM_MBUTTONDOWN: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button middle %dx%d | press\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 1, 0, 0, lunaris::mouse::middle);
                };
                return 0;
            };
            case WM_RBUTTONDOWN: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button right %dx%d | press\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 1, 0, 0, lunaris::mouse::second);
                }
                return 0;
            };

            case WM_XBUTTONDOWN: {
                if(win->mouse_handler != NULL){
                    if((XBUTTON1&(wparam>>16)) == XBUTTON1) { // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-xbuttondown
                        #ifdef LUNARIS_DEBUG
                        printf("#pointer@button x1 %dx%d | press\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                        #endif
                        win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 1, 0, 0, lunaris::mouse::first);
                        return 0;
                    } else {
                        #ifdef LUNARIS_DEBUG
                        printf("#pointer@button x2 %dx%d | press\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                        #endif
                        win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 1, 0, 0, lunaris::mouse::first);
                    };
                };
                return 0;
            };

            case WM_LBUTTONUP: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button left %dx%d | release\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::first);
                };
                return 0;
            };
            case WM_MBUTTONUP: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button middle %dx%d | release\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::middle);
                };
                return 0;
            };
            case WM_RBUTTONUP: {
                if(win->mouse_handler != NULL){
                    #ifdef LUNARIS_DEBUG
                    printf("#pointer@button right %dx%d | release\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                    #endif
                    win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::second);
                };
                return 0;
            };

            case WM_XBUTTONUP: {
                if(win->mouse_handler != NULL){
                    if((XBUTTON1&(wparam>>16)) == XBUTTON1) { // https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-xbuttondown
                        #ifdef LUNARIS_DEBUG
                        printf("#pointer@button x1 %dx%d | release\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                        #endif
                        win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::first);
                    } else {
                        #ifdef LUNARIS_DEBUG
                        printf("#pointer@button x2 %dx%d | release\n", GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                        #endif
                        win->mouse_handler(win, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0, 0, 0, lunaris::mouse::first);
                    };
                    return 0;
                };
                return 0;
            };

            case WM_MOUSEWHEEL: {
                POINT cursor_pos;
                cursor_pos.x = GET_X_LPARAM(lparam);
                cursor_pos.y = GET_Y_LPARAM(lparam);
                ScreenToClient(hwnd, &cursor_pos);
                win->mouse_handler(win, cursor_pos.x, cursor_pos.y, 0, 0, GET_WHEEL_DELTA_WPARAM(wparam)/-8 /*Windows sends 120, needs to mapped as 15. So (wparam*15/120) * -1 (to fix axis) */, lunaris::mouse::scroll);
                return 0;
            };
            #ifndef LUNARIS_SUPPORT_WIN_XP
                case WM_MOUSEHWHEEL: {
                    POINT cursor_pos;
                    cursor_pos.x = GET_X_LPARAM(lparam);
                    cursor_pos.y = GET_Y_LPARAM(lparam);
                    ScreenToClient(hwnd, &cursor_pos);
                    win->mouse_handler(win, cursor_pos.x, cursor_pos.y, 0, GET_WHEEL_DELTA_WPARAM(wparam)/-8 /*Windows sends 120, needs to mapped as 15. So (wparam*15/120) * -1 (to fix axis) */, 0, lunaris::mouse::scroll);
                    return 0;
                };
            #endif
            case WM_KEYDOWN:
            case WM_KEYUP: {
                bool is_keydown_event = ((lparam>>31)&0x01)==0; // 1 for KEYDOWN; 0 for KEYUP
                BYTE keyboard_state[256];
                GetKeyboardState(keyboard_state);

                wchar_t pressed_key_w[2] = {0};
                char pressed_key[8] = {0};
                bool is_actually_a_char = false;

                if((ToUnicodeEx((uint32_t)wparam, (uint32_t)MapVirtualKey((uint32_t)wparam, MAPVK_VK_TO_VSC), keyboard_state, pressed_key_w, 1, 0, keyboard_layout) > 0) &&
                    (WideCharToMultiByte(CP_UTF8, 0, pressed_key_w, -1, pressed_key, sizeof(pressed_key), NULL, NULL) > 0)){
                        is_actually_a_char = true;
                };

                if(win->keyboard_handler != NULL){
                    if(is_keydown_event){
                        if(win->__last_pressed_key == (uint32_t)wparam){
                            if(is_actually_a_char){
                                win->keyboard_handler(win, pressed_key, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::repeated);
                            } else {
                                win->keyboard_handler(win, NULL, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::repeated);
                            }
                        } else {
                            if(is_actually_a_char){
                                win->keyboard_handler(win, pressed_key, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::pressed);
                            } else {
                                win->keyboard_handler(win, NULL, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::pressed);
                            };
                            win->__last_pressed_key = (uint32_t)wparam;
                        };
                    } else {
                        if(is_actually_a_char){
                            win->keyboard_handler(win, pressed_key, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::released);
                        } else {
                            win->keyboard_handler(win, NULL, __convert_event_code_to_lunaris_code(wparam, lparam), 0, lunaris::keyboard::released);
                        };
                        win->__last_pressed_key = 0;
                    };
                };

                return 0;
            };
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
    };
};
