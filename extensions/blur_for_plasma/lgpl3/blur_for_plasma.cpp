// SPDX-FileCopyrightText: 2024 Mehmet Ali Şipi
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <string.h>
#define FOR_LIBRARY
#include <blur_for_plasma.hpp>
#include "thirdparty/plasma_protocols/blur.h"
#include "thirdparty/plasma_protocols/blur.c"

org_kde_kwin_blur_manager* __ext_lunaris_blur_for_plasma_blur_manager = NULL;

extern "C" {
    static const wl_registry_listener __ext_lunaris_blur_for_plasma_registry_listener = {
        .global = [](void* data, wl_registry* registry, uint32_t id, const char* interface, uint32_t version) {
            if (strcmp(interface, "org_kde_kwin_blur_manager") == 0) {
                __ext_lunaris_blur_for_plasma_blur_manager = (org_kde_kwin_blur_manager*)wl_registry_bind(registry, id, &org_kde_kwin_blur_manager_interface, 1);
            };
        },
        .global_remove = [](void* data, wl_registry* registry, uint32_t id) {},
    };

    void __ext_lunaris_blur_for_plasma_load(wl_display* display){
        wl_registry* registry = wl_display_get_registry(display);

        wl_registry_add_listener(registry, &__ext_lunaris_blur_for_plasma_registry_listener, NULL);
        wl_display_roundtrip(display);
        __ext_lunaris_blur_for_plasma_loaded = !!__ext_lunaris_blur_for_plasma_blur_manager;
    };

    void __ext_lunaris_blur_for_plasma_apply_blur(wl_surface* surface, wl_compositor* compositor){
        org_kde_kwin_blur* blur = org_kde_kwin_blur_manager_create(__ext_lunaris_blur_for_plasma_blur_manager, surface);
        wl_region* region = wl_compositor_create_region(compositor);
        org_kde_kwin_blur_commit(blur);
    };
};
