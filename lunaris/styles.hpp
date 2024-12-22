#pragma once
#include <ctype.h>

namespace lunaris {
    namespace styles {
        typedef struct color_palette {
            uint32_t seconder_color;
            uint32_t seconder_text_color;
            uint32_t other_color;
            uint32_t background_color;
            uint32_t border_color;
            uint32_t hover_color;
            uint32_t text_color;
        } color_palette;

        const color_palette light_color_palette = {
            .seconder_color = 0xFF474747,
            .seconder_text_color = 0xFFCCCCCC,
            .other_color = 0xFFAAAAAA,
            .background_color = 0xFFDDDDDD,
            .border_color = 0xFF888888,
            .hover_color = 0xFF999999,
            .text_color = 0xFF000000
        };

        const color_palette dark_color_palette = {
            .seconder_color = 0xFF474747,
            .seconder_text_color = 0xFF222222,
            .other_color = 0xFF444444,
            .background_color = 0xFF111111,
            .border_color = 0xFF777777,
            .hover_color = 0xFF555555,
            .text_color = 0xFFFFFFFF
        };

        color_palette* generate_color_palette(bool is_light, uint32_t mix_color){
            color_palette* new_palette = (color_palette*)malloc(sizeof(color_palette));
            if(mix_color == 0x00000000){
                if(is_light){
                    memcpy(new_palette, &light_color_palette, sizeof(color_palette));
                } else {
                    memcpy(new_palette, &dark_color_palette, sizeof(color_palette));
                };
            } else {
                color_palette mixed_color_palette = light_color_palette;
                float mixed_percent = .5;
                if(!is_light){
                    mixed_color_palette = dark_color_palette;
                    mixed_percent = .3;
                };
                new_palette->seconder_color = color_mix(mixed_color_palette.seconder_color, mix_color, mixed_percent);
                new_palette->seconder_text_color = color_mix(mixed_color_palette.seconder_text_color, mix_color, mixed_percent);
                new_palette->other_color = color_mix(mixed_color_palette.other_color, mix_color, mixed_percent);
                new_palette->background_color = color_mix(mixed_color_palette.background_color, mix_color, mixed_percent);
                new_palette->border_color = color_mix(mixed_color_palette.border_color, mix_color, mixed_percent);
                new_palette->hover_color = color_mix(mixed_color_palette.hover_color, mix_color, mixed_percent);
                new_palette->text_color = color_mix(mixed_color_palette.text_color, mix_color, mixed_percent);
            }
            return new_palette;
        };

        color_palette* generate_color_palette(bool is_light){
            return generate_color_palette(is_light, 0x00000000);
        };

        uint32_t get_system_color(){
            #if defined(linux)
                char kdeglobals_path[4096];
                sprintf(kdeglobals_path, "%s/.config/kdeglobals", getenv("HOME"));
                FILE* kdeglobals = fopen(kdeglobals_path, "r");
                char new_line[1024];
                while(fgets(new_line, sizeof(new_line), kdeglobals)) {
                    std::vector<uint8_t> rgb = {0, 0, 0};
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
                        return 0xFF000000 | (rgb[0]<<16) | (rgb[1]<<8) | (rgb[2]);
                    };
                };
            #elif defined(_WIN32)
                #ifdef LUNARIS_SUPPORT_WIN_XP
                    COLORREF legacy_accent_color = GetSysColor(COLOR_HIGHLIGHT);
                    return 0xFF000000 | (GetRValue(legacy_accent_color)<<16) | (GetGValue(legacy_accent_color)<<8) | (GetBValue(legacy_accent_color));
                #else
                    uint32_t color_abgr = 0;
                    lunaris::__internal::get_regedit_dword_value((char*)"Software\\Microsoft\\Windows\\DWM", (char*)"AccentColor", &color_abgr);
                    if(color_abgr == 0) return 0x00000000;
                    return 0xFF000000 | (color_abgr & 0x0000FF00) | ((color_abgr<<16) & 0x00FF0000) | ((color_abgr>>16) & 0x000000FF);
                #endif
            #elif defined(__EMSCRIPTEN__)
                uint32_t color = 0xFF000000 | EM_ASM_INT({
                    let calculator_element = document.createElement("div");
                    calculator_element.style.setProperty("color", "highlight");
                    document.body.append(calculator_element);
                    let computed_style = window.getComputedStyle(calculator_element) ;
                    let accent_color = computed_style.getPropertyValue("color").match(/[0-9]+/g).slice(0,3).map(a=>Number(a));
                    calculator_element.remove();
                    return (accent_color[2]<<16) | (accent_color[1]<<8) | accent_color[0];
                });
                if(color == 0xFFC64100 /* Chrome */ || color == 0xFFE48434 /* Safari (aka Webkit) */) { // It's browsers default highlight color, so it's not a accent color. It's pre-defined color to reduce fingerprint.
                    return 0x00000000;
                };
                return color;
            #endif
            return 0x00000000;
        };

        bool is_system_theme_light(){
            #if defined(__EMSCRIPTEN__)
                return EM_ASM_({
                    return (!(window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches));
                });
            #elif defined(_WIN32)
                #ifdef LUNARIS_SUPPORT_WIN_XP
                    COLORREF legacy_background_color = GetSysColor(COLOR_WINDOW);
                    return GetRValue(legacy_background_color)+GetGValue(legacy_background_color)+GetBValue(legacy_background_color) > 550; // 0-765 > 550
                #else
                    uint32_t is_light = 0;
                    lunaris::__internal::get_regedit_dword_value((char*)"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", (char*)"AppsUseLightTheme", &is_light);
                    return is_light > 0;
                #endif
            #elif defined(__linux__)
                FILE* gsettings_output;
                char gsettings_value[128];

                gsettings_output = popen("gsettings get org.gnome.desktop.interface color-scheme", "r");
                if (gsettings_output == NULL){
                    pclose(gsettings_output);
                    return true;
                };

                fgets(gsettings_value, sizeof(gsettings_value), gsettings_output);
                bool is_light_theme = true;
                if(strcmp(gsettings_value, "'prefer-dark'") == '\n') is_light_theme = false;

                pclose(gsettings_output);
                return is_light_theme;
            #endif

            return true;
        }

        color_palette* generate_color_palette_from_system(){
            return generate_color_palette(is_system_theme_light(), get_system_color()); // TODO: Get system light/dark prefence.
        };

        void make_more_transparent(color_palette* palette){
            palette->seconder_color -= 0x66000000;
            palette->other_color -= 0x66000000;
            palette->background_color -= 0x99000000;
            palette->border_color -= 0x22000000;
            palette->hover_color -= 0x44000000;
        };
    };
};