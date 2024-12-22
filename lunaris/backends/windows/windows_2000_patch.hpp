#pragma once
#include <windows.h>
#include <math.h>

// 32-bit support
#ifndef __uint128_t
#define __uint128_t uint64_t
#endif

#ifndef MAPVK_VK_TO_VSC
#define MAPVK_VK_TO_VSC 0
#endif

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#endif

#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C
#endif

#ifndef XBUTTON1
#define XBUTTON1 0x0001
#endif

#ifndef XBUTTON2
#define XBUTTON2 0x0002
#endif

typedef void* pthread_t;
int pthread_create(pthread_t* thread, void* _, void*(*thread_fn)(void *), void* args){
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_fn, args, 0, NULL);
    return 0;
};

int pthread_detach(pthread_t thread){
    return 0;
};

namespace std {
    double pow(double base, int exponent) {
        return ::pow(base, exponent);
    };

    double abs(double x) {
        return ::abs(x);
    };

    double sqrt(double x) {
        return ::sqrt(x);
    };
};
