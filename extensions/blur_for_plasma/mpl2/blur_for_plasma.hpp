// SPDX-FileCopyrightText: 2024 Mehmet Ali Şipi
// SPDX-License-Identifier: MPL-2.0

#ifdef FOR_LIBRARY
#include <wayland-client.h>
#else
using namespace lunaris;
#endif

int __ext_lunaris_blur_for_plasma_loaded = false;

extern "C" {
    void __ext_lunaris_blur_for_plasma_load(wl_display* display);
    void __ext_lunaris_blur_for_plasma_apply_blur(wl_surface* surface, wl_compositor* compositor);
}
