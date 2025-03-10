#pragma once

#ifndef _WIN32
#include <dlfcn.h>
#endif
#include <mpv/render.h>

#ifndef _WIN32
void* _libmpv_handle;
#else
HMODULE _libmpv_handle;
#endif
bool _is_mpv_loaded = false;

typedef struct mpv_handle mpv_handle;
typedef struct mpv_render_context mpv_render_context;

typedef struct mpv_handle* (*mpv_create_fn)();
typedef int (*mpv_initialize_fn)(struct mpv_handle*);
typedef int (*mpv_set_option_string_fn)(struct mpv_handle*, const char*, const char*);
typedef int (*mpv_set_property_string_fn)(struct mpv_handle*, const char*, const char*);
typedef int (*mpv_command_async_fn)(struct mpv_handle*, uint64_t, const char*[]);
typedef void (*mpv_terminate_destroy_fn)(struct mpv_handle*);
typedef int (*mpv_render_context_create_fn)(struct mpv_render_context**, struct mpv_handle*, struct mpv_render_param*);
typedef int (*mpv_render_context_render_fn)(struct mpv_render_context*, struct mpv_render_param*);
typedef void (*mpv_set_wakeup_callback_fn)(struct mpv_handle*, void(*cb)(void*), void*);
typedef int (*mpv_observe_property_fn)(struct mpv_handle*, uint64_t, const char*, mpv_format);
typedef mpv_event* (*mpv_wait_event_fn)(struct mpv_handle*, double);
typedef void (*mpv_wakeup_fn)(struct mpv_handle*);

mpv_create_fn dmpv_create = NULL;
mpv_initialize_fn dmpv_initialize = NULL;
mpv_set_option_string_fn dmpv_set_option_string = NULL;
mpv_set_property_string_fn dmpv_set_property_string = NULL;
mpv_command_async_fn dmpv_command_async = NULL;
mpv_terminate_destroy_fn dmpv_terminate_destroy = NULL;
mpv_render_context_create_fn dmpv_render_context_create = NULL;
mpv_render_context_render_fn dmpv_render_context_render = NULL;
mpv_set_wakeup_callback_fn dmpv_set_wakeup_callback = NULL;
mpv_observe_property_fn dmpv_observe_property = NULL;
mpv_wait_event_fn dmpv_wait_event = NULL;
mpv_wakeup_fn dmpv_wakeup = NULL;

void _load_mpv(){
    #ifndef _WIN32
    _libmpv_handle = dlopen("libmpv.so", RTLD_LAZY);
    if (!_libmpv_handle) {
        printf("libmpv cannot be loaded due to %s\nWill looking for libmpv.so.2\n", dlerror());
        _libmpv_handle = dlopen("libmpv.so.2", RTLD_LAZY);
        if (!_libmpv_handle) {
            printf("libmpv cannot be loaded due to %s\n", dlerror());
            return;
        };
    };
    #else 
    _libmpv_handle = LoadLibrary("libmpv.dll");
    #endif

    #ifdef _WIN32
    #define dlsym GetProcAddress
    #endif
    dmpv_create = (mpv_create_fn)dlsym(_libmpv_handle, "mpv_create");
    dmpv_initialize = (mpv_initialize_fn)dlsym(_libmpv_handle, "mpv_initialize");
    dmpv_set_option_string = (mpv_set_option_string_fn)dlsym(_libmpv_handle, "mpv_set_option_string");
    dmpv_set_property_string = (mpv_set_property_string_fn)dlsym(_libmpv_handle, "mpv_set_property_string");
    dmpv_command_async = (mpv_command_async_fn)dlsym(_libmpv_handle, "mpv_command_async");
    dmpv_terminate_destroy = (mpv_terminate_destroy_fn)dlsym(_libmpv_handle, "mpv_terminate_destroy");
    dmpv_render_context_create = (mpv_render_context_create_fn)dlsym(_libmpv_handle, "mpv_render_context_create");
    dmpv_render_context_render = (mpv_render_context_render_fn)dlsym(_libmpv_handle, "mpv_render_context_render");
    dmpv_set_wakeup_callback = (mpv_set_wakeup_callback_fn)dlsym(_libmpv_handle, "mpv_set_wakeup_callback");
    dmpv_observe_property = (mpv_observe_property_fn)dlsym(_libmpv_handle, "mpv_observe_property");
    dmpv_wait_event = (mpv_wait_event_fn)dlsym(_libmpv_handle, "mpv_wait_event");
    dmpv_wakeup = (mpv_wakeup_fn)dlsym(_libmpv_handle, "mpv_wakeup");
    #ifdef _WIN32
    #undef dlsym
    #endif
    _is_mpv_loaded = true;
}
