#pragma once

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "mouse_controller.hpp"
#include "keyboard_controller.hpp"

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
		win->graphics.rect(__framebuffer::__mouse::cursor_x, __framebuffer::__mouse::cursor_y, 20, 20, 0xFF000000);
		win->graphics.rect(__framebuffer::__mouse::cursor_x+4, __framebuffer::__mouse::cursor_y+4, 12, 12, 0xFFFFFFFF);
		memcpy(win->__buffer, win->buffer, 1920*1080*4);
	};
	void __backend_set_visibility(lunaris::window* win, bool visible){}
	bool __backend_set_decoration(lunaris::window* win, bool decorated) {
		return false;
	};
	void __backend_resize(lunaris::window* win, int w, int h){};

	lunaris::window* new_window(){
		lunaris::window* win = (lunaris::window*)malloc(sizeof(lunaris::window));
		win->graphics.__init(win);
		win->is_alive = true;
		__framebuffer::__mouse::initalize();
		__framebuffer::__keyboard::initalize();

		int fb = open("/dev/fb0", O_RDWR);
		uint8_t* buffer = (uint8_t*)mmap(0, 4*1920*1080, PROT_READ|PROT_WRITE, MAP_SHARED, (int)fb, 0);
		win->buffer = (uint32_t*)malloc(1920*1080*4);
		win->__buffer = (uint32_t*)buffer;

        win->width = 1920;
        win->height = 1080;
		return win;
	};

	void __backend_loop(lunaris::window* win){ // __frame_triggerer
		const int frame_time = 1000/60;
		while(win->is_alive) {
			auto start = std::chrono::high_resolution_clock::now();
			__framebuffer::__mouse::loop(win);
			__framebuffer::__keyboard::loop(win);
			draw(win);
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
