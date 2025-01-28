namespace lunaris {
    namespace effects {
        namespace for_windows {
            typedef enum material {
                material_none,
                material_mica,
                material_acrylic,
                material_tabbed,
                material_auto
            } material;
        };
    };
};

#if defined(_WIN32) && defined (ENABLE_EFFECTS) && !defined(LUNARIS_SUPPORT_WIN_XP)
    namespace lunaris {
        namespace effects {
            namespace for_windows {
                #include <windows.h>
                #include <dwmapi.h>

                #ifdef DWM_MISSING_DECLARATIONS
                    typedef enum {
                        DWMSBT_AUTO,
                        DWMSBT_NONE, // None
                        DWMSBT_MAINWINDOW, // Mica
                        DWMSBT_TRANSIENTWINDOW, // Acrylic
                        DWMSBT_TABBEDWINDOW // Tabbed
                    } DWM_SYSTEMBACKDROP_TYPE;

                    #define DWMWA_USE_HOSTBACKDROPBRUSH 17
                    #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
                    #define DWMWA_BORDER_COLOR 34
                    #define DWMWA_CAPTION_COLOR 35
                    #define DWMWA_TEXT_COLOR 36
                    #define DWMWA_SYSTEMBACKDROP_TYPE 38
                #endif

                void set_titlebar_background_color(lunaris::window* win, uint32_t color){ // 0xAARRGGBB
                    COLORREF target_color = (color&0x0000FF00) | ((color&0x00FF0000)>>16) | ((color&0x000000FF)<<16); // to 0xAABBGGRR
                    lunaris::effects::for_windows::DwmSetWindowAttribute(win->__hwnd, DWMWA_BORDER_COLOR, &target_color, sizeof(target_color));
                    lunaris::effects::for_windows::DwmSetWindowAttribute(win->__hwnd, DWMWA_CAPTION_COLOR, &target_color, sizeof(target_color));
                };

                void set_titlebar_text_color(lunaris::window* win, uint32_t color){ // 0xAARRGGBB
                    COLORREF target_color = (color&0x0000FF00) | ((color&0x00FF0000)>>16) | ((color&0x000000FF)<<16); // to 0xAABBGGRR
                    lunaris::effects::for_windows::DwmSetWindowAttribute(win->__hwnd, DWMWA_TEXT_COLOR, &target_color, sizeof(target_color));
                };

                void set_titlebar_appearance(lunaris::window* win){
                    set_titlebar_background_color(win, win->colors->background_color);
                    set_titlebar_text_color(win, win->colors->text_color);
                };

                void set_material(lunaris::window* win, lunaris::effects::for_windows::material material, bool is_light_material){
                    HWND hwnd = (win->__hwnd);
                    bool is_dark_mode = !is_light_material;
                    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &is_dark_mode, sizeof(is_dark_mode));

                    MARGINS margins = {-1};
                    HRESULT hres = DwmExtendFrameIntoClientArea(hwnd, &margins);
                    if (!SUCCEEDED(hres)) return;

                    DWM_BLURBEHIND blur_behind = {0};

                    blur_behind.dwFlags = DWM_BB_ENABLE;
                    blur_behind.fEnable = true;
                    blur_behind.hRgnBlur = NULL;

                    hres = DwmEnableBlurBehindWindow(hwnd, &blur_behind);
                    if (!SUCCEEDED(hres)) return;

                    bool host_background_brush_state = true;
                    DwmSetWindowAttribute(hwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &host_background_brush_state, sizeof(host_background_brush_state));

                    DWM_SYSTEMBACKDROP_TYPE backdrop_type = DWMSBT_AUTO;
                    switch(material){
                        case material_auto: {
                            backdrop_type = DWMSBT_AUTO;
                            break;
                        };
                        case material_none: {
                            backdrop_type = DWMSBT_NONE;
                            break;
                        };
                        case material_mica: {
                            backdrop_type = DWMSBT_MAINWINDOW;
                            break;
                        };
                        case material_acrylic: {
                            backdrop_type = DWMSBT_TRANSIENTWINDOW;
                            break;
                        };
                        case material_tabbed: {
                            backdrop_type = DWMSBT_TABBEDWINDOW;
                            break;
                        };
                    };
                    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
                };
            };
        };
    };
#else
    namespace lunaris {
        namespace effects {
            namespace for_windows {
                void set_material(lunaris::window* win, lunaris::effects::for_windows::material material, bool is_light_material){}; // placeholder function
                void set_titlebar_background_color(lunaris::window* win, uint32_t color){};
                void set_titlebar_text_color(lunaris::window* win, uint32_t color){};
                void set_titlebar_appearance(lunaris::window* win){};
            };
        };
    };
#endif

#if defined(__linux__) && defined (ENABLE_EFFECTS) && defined (ENABLE_EXT_BLUR_FOR_PLASMA)
#include "extensions/blur_for_plasma/mpl2/blur_for_plasma.hpp"
    namespace lunaris {
        namespace effects {
            namespace for_linux {
                void set_blur(lunaris::window* win, bool blur_state){ // TODO: Support disabling blur
                    __ext_lunaris_blur_for_plasma_load(win->__display);
                    __ext_lunaris_blur_for_plasma_apply_blur(win->__surface, win->__compositor);
                };
            };
        };
    };
#else
    namespace lunaris {
        namespace effects {
            namespace for_linux {
                void set_blur(lunaris::window* win, bool blur_state){}; // placeholder function
            };
        };
    };
#endif
