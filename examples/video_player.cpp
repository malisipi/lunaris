#include "lunaris/ui.hpp"
#include "lunaris/ui/native_dialogs.hpp"
#include "lunaris/ui/video/video.hpp"

lunaris::ui::video* video = NULL;
lunaris::ui::slider* slider = NULL;
lunaris::window* win = NULL;

void change_time(lunaris::window*, lunaris::ui::widget*, int prev_value){
    video->set_time_pos(slider->value);
};

void video_time_changed (lunaris::window*, lunaris::ui::widget*, double time_pos){
    slider->value = (int)time_pos;  
};

void video_duration_changed (lunaris::window*, lunaris::ui::widget*, double duration){
    slider->max = (int)duration;
};

void open_video (lunaris::window*, lunaris::ui::widget*){
    std::string file_name = lunaris::ui::native_dialogs::open_file();
    video->load_media(file_name);
    video->play();
};

int main(){
    win = lunaris::new_window();
    win->set_decoration(true);
    win->set_title((char*)"Video Player");
    
    lunaris::ui::fixed* fixed = new lunaris::ui::fixed;
    win->layout = (lunaris::ui::widget*)fixed;

    video = new lunaris::ui::video;
    fixed->place(video, "0", "0", "100%w", "100%h-30");
    
    lunaris::ui::button* open_file = new lunaris::ui::button;
    open_file->text = "Open File";
    open_file->click_handler = open_video;
    fixed->place(open_file, "0","100%h-30", "100", "30");

    slider = new lunaris::ui::slider;    
    slider->min = 0;
    slider->max = 10;
    slider->step = 1;
    video->time_pos_update_handler = video_time_changed;
    video->duration_update_handler = video_duration_changed;
    slider->input_handler = change_time;
    fixed->place(slider, "120","100%h-30", "100%w-120", "30");

    lunaris::ui::connect(win);
    win->loop();
    return 0;
};
