#pragma once

#include <stdio.h>
#include <stdint.h>

namespace lunaris {
	void __backend_start_move(lunaris::window* win){};
	void __backend_set_cursor(lunaris::window* win, lunaris::cursor::cursor cursor){};
	void __backend_set_fullscreen(lunaris::window* win, bool fullscreen) {};

	void draw(lunaris::window* win){
		if(win->draw_handler != NULL){
			win->draw_handler(win, win->buffer);
		} else {
			for (int i = 0; i < win->width * win->height; i++) {
				win->buffer[i] = 0xFF000000;
			};
		};
		EM_ASM_({
			if(Module.context == undefined) Module.context = Module.canvas.getContext("2d", { willReadFrequently: true });
			let image_data = Module.context.getImageData(0, 0, $1, $2);
			image_data.data.set(Module.HEAPU8.slice($0, $0 + $1 * $2 * 4));
			Module.context.putImageData(image_data, 0, 0);
		}, win->buffer, win->width, win->height);
	};
	void __backend_set_visibility(lunaris::window* win, bool visible){}
	bool __backend_set_decoration(lunaris::window* win, bool decorated) {
		return false;
	};
	void __backend_resize(lunaris::window* win, int w, int h){};

	EM_BOOL mousedown_handler(int eventType, const EmscriptenMouseEvent* event, void* data) {
		lunaris::window* win = (lunaris::window*)data;
		win->mouse_handler(win, event->clientX, event->clientY, true, 0, 0, mouse::first);
		return 0;
	};

	EM_BOOL mousemove_handler(int eventType, const EmscriptenMouseEvent* event, void* data) {
		lunaris::window* win = (lunaris::window*)data;
		win->mouse_handler(win, event->clientX, event->clientY, false, 0, 0, mouse::motion);
		return 0;
	};

	EM_BOOL mouseup_handler(int eventType, const EmscriptenMouseEvent* event, void* data) {
		lunaris::window* win = (lunaris::window*)data;
		win->mouse_handler(win, event->clientX, event->clientY, false, 0, 0, mouse::first);
		return 0;
	};

	EM_BOOL keypress_handler(int eventType, const EmscriptenKeyboardEvent* event, void* data) {
		lunaris::window* win = (lunaris::window*)data;
		return 0;
	};

	EM_BOOL resize_handler(int eventType, const EmscriptenUiEvent* event, void* data){
		lunaris::window* win = (lunaris::window*)data;
		emscripten_set_canvas_element_size("#canvas", event->windowInnerWidth, event->windowInnerHeight);
		win->width = event->windowInnerWidth;
		win->height = event->windowInnerHeight;
		return 0;
	}

	lunaris::window* new_window(){
		lunaris::window* win = (lunaris::window*)malloc(sizeof(lunaris::window));
		win->graphics.__init(win);
		win->is_alive = true;

		emscripten_set_mousedown_callback("#canvas", (void*)win, 1, mousedown_handler);
		emscripten_set_mousemove_callback("#canvas", (void*)win, 1, mousemove_handler);
		emscripten_set_mouseup_callback("#canvas", (void*)win, 1, mouseup_handler);
		emscripten_set_keypress_callback("#canvas", (void*)win, 1, keypress_handler);
		emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, (void*)win, 1, resize_handler);

		win->buffer = (uint32_t*)malloc(1920*1080*4);

		const int canvas_width = EM_ASM_INT(return window.innerWidth);
		const int canvas_height = EM_ASM_INT(return window.innerHeight);

		emscripten_set_canvas_element_size("#canvas", canvas_width, canvas_height);
        win->width = canvas_width;
        win->height = canvas_height;
		return win;
	};

	void __backend_loop(lunaris::window* win){ // __frame_triggerer
		const int frame_time = 1000/24;
		while(win->is_alive) {
			auto start = std::chrono::high_resolution_clock::now();
			draw(win);
			emscripten_sleep(20);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;

			if (elapsed.count() < frame_time) {
				std::this_thread::sleep_for(std::chrono::milliseconds(frame_time) - elapsed);
			};
		};
	};

	void __backend_destroy(lunaris::window* win){};
	void __backend_show_window_menu(lunaris::window* win, int x, int y){};
	void __backend_minimize(lunaris::window* win){};
	void __backend_maximize(lunaris::window* win){};
	void __backend_restore(lunaris::window* win){};
	void __backend_start_resize(lunaris::window* win){};
	void __backend_set_title(lunaris::window* win, const char* title){};
}
