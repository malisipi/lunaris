
namespace lunaris {
    namespace __framebuffer::__mouse {
        bool __is_mouse_controller_running = false;
        int fd;

        int cursor_x = 20;
        int cursor_y = 20;
        
        void initalize(){
            if(__is_mouse_controller_running) return;
            __is_mouse_controller_running = true;

            fd = open("/dev/input/mice", O_RDONLY | O_NONBLOCK);
        };

        int8_t mouse_data[3];
        void loop(window* win){
            int readed_byte_count = read(fd, mouse_data, sizeof(mouse_data));
            if(readed_byte_count == 3){
                if(mouse_data[0] == 9){
                    win->mouse_handler(win, cursor_x, cursor_y, true, 0, 0, ::lunaris::mouse::first);
                    win->mouse_handler(win, cursor_x, cursor_y, false, 0, 0, ::lunaris::mouse::first);
                } else {
                    cursor_x += mouse_data[1];
                    cursor_y -= mouse_data[2];
                };
            };
        };
    };
};