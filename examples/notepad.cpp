#include "lunaris/ui.hpp"
#include "lunaris/ui/native_dialogs.hpp"

lunaris::ui::textbox* textbox;

void open_file_handler (lunaris::ui::menubar_subitem* self){
    char file_path[4096];
    sprintf(file_path, "%s", lunaris::ui::native_dialogs::open_file().c_str());
    FILE* file = fopen(file_path, "r");
    textbox->text = "";
    char buffer[20000];
    while (fgets(buffer, sizeof(buffer), file) != NULL) textbox->text += buffer;
    fclose(file);
};

void save_file_handler (lunaris::ui::menubar_subitem* self){
    char file_path[4096];
    sprintf(file_path, "%s", lunaris::ui::native_dialogs::save_file().c_str());
    FILE* file = fopen(file_path, "w");
    fwrite(textbox->text.c_str(), sizeof(char), textbox->text.length(), file);
    fclose(file);
};

int main() {
    lunaris::window* win = lunaris::new_window();
    lunaris::ui::connect(win);
    win->set_decoration(true);
    win->set_title("Notepad");

    lunaris::ui::menubar* menubar = new lunaris::ui::menubar;
    lunaris::ui::menubar_item* item;
    lunaris::ui::menubar_subitem* subitem;

    win->layout = (void*)menubar;

    item = new lunaris::ui::menubar_item;
    item->text = "File";
    subitem = new lunaris::ui::menubar_subitem;
    subitem->text = "Open";
    subitem->handler = open_file_handler;
    item->items.push_back(subitem);
    subitem = new lunaris::ui::menubar_subitem;
    subitem->text = "Save";
    subitem->handler = save_file_handler;
    item->items.push_back(subitem);
    menubar->menu.push_back(item);

    textbox = new lunaris::ui::textbox;
    textbox->multiline = true;
    menubar->child = textbox;

    win->loop();
    return 0;
};
