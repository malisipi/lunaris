namespace lunaris {
    namespace effects {
        namespace for_windows {
            typedef enum material {
                material_none,
                material_mica,
                material_acrylic,
                material_tabbed
            } material;
        };
    };
};

#if defined(_WIN32) && defined (ENABLE_EFFECTS)
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
                    #define DWMWA_SYSTEMBACKDROP_TYPE 38
                #endif

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
                        case material_none: {
                            backdrop_type = DWMSBT_AUTO;
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
