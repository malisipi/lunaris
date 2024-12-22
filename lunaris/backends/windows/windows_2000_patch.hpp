#pragma once
#include <windows.h>
#include <math.h>

#ifndef __uint128_t
#define __uint128_t uint64_t
#endif

#define MAPVK_VK_TO_VSC 0

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