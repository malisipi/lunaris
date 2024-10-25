namespace lunaris::styles {
    std::vector<uint8_t> get_system_color(){
        std::vector<uint8_t> rgb = {0, 0, 0};
        #ifdef linux
            char kdeglobals_path[4096];
            sprintf(kdeglobals_path, "%s/.config/kdeglobals", getenv("HOME"));
            FILE* kdeglobals = fopen(kdeglobals_path, "r");
            char new_line[1024];
            while(fgets(new_line, sizeof(new_line), kdeglobals)) {
                if(strcmp(new_line, "AccentColor") == '='){ // Wanted line: "AccentColor=61,174,233" (r,g,b values can be changed, due to user preferences)
                    int start_index_of_first_number = strlen("AccentColor=");
                    uint8_t rgb_index = 0;
                    for(uint8_t index = 0; rgb_index<3 && index < 25; index++){
                        if(isdigit(new_line[start_index_of_first_number+index])){
                            rgb[rgb_index] = rgb[rgb_index]*10 + (new_line[start_index_of_first_number+index] - '0');
                        };
                        if(new_line[start_index_of_first_number+index] == ','){
                            rgb_index++;
                        };
                    };
                    return rgb;
                };
            };
        #endif
        return rgb;
    };
};