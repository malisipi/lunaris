/* Flags */
// #define LUNARIS_DISABLE_DIRECT_BUFFER_ACCESS_FROM_JAVA /* This will improve the performance hugely, by disabling syncing jintArray. Use it if you will not access directly the buffer. */

#include <jni.h>
#include <stdint.h>
#include <string.h>
#include "Lunaris.h"
#include <map>

#include "../../lunaris/c/lunaris.cpp"

/* Java Things & Global storage & Typedefs */

JavaVM* javaVM = NULL;

typedef struct draw_handler_for_java {
    jobject object;
    jmethodID method_id;
} draw_handler_for_java;

typedef struct lunaris_window_java_data {
    int size;
    jintArray jbuffer;
    draw_handler_for_java* draw_handler;
} lunaris_window_java_data;

std::map<lunaris_window*, lunaris_window_java_data> lunaris_global_storage_for_java; // TODO: Add a mechanism to clear this storage

/* General */

JNIEXPORT jlong JNICALL Java_Lunaris_newWindow(JNIEnv* env, jobject) {
    if(javaVM == NULL) env->GetJavaVM(&javaVM);
    lunaris_window* win = lunaris_new_window();
    return (long)win;
};

JNIEXPORT void JNICALL Java_Lunaris_windowLoop(JNIEnv*, jobject, jlong win) {
    lunaris_window_loop((lunaris_window*)win);
};

JNIEXPORT void JNICALL Java_Lunaris_windowSetDecoration(JNIEnv*, jobject, jlong win, jboolean state){
    lunaris_window_set_decoration((lunaris_window*)win, state);
};

JNIEXPORT void JNICALL Java_Lunaris_windowResize(JNIEnv*, jobject, jlong win, jint width, jint height){
    lunaris_window_resize((lunaris_window*)win, width, height);
};

JNIEXPORT void JNICALL Java_Lunaris_windowSetFullscreen(JNIEnv*, jobject, jlong win, jboolean state){
    lunaris_window_set_fullscreen((lunaris_window*)win, state);
};

JNIEXPORT void JNICALL Java_Lunaris_windowSetTitle(JNIEnv* env, jobject, jlong win, jstring title){
    lunaris_window_set_title((lunaris_window*)win, (char*)env->GetStringUTFChars(title, 0)); /* TODO: Casted const char* to char*, change function declaration later */
};

void java_function_handler(lunaris_window* win, uint32_t* buffer){
    JNIEnv* env;
    if (javaVM->AttachCurrentThread((void**)&env, NULL) == 0) {
        #ifndef LUNARIS_DISABLE_DIRECT_BUFFER_ACCESS_FROM_JAVA
            // Create new java buffer from current buffer
            const int size = win->width*win->height;
            lunaris_global_storage_for_java[win].size = size;
            jintArray jbuffer = env->NewIntArray(size);
            lunaris_global_storage_for_java[win].jbuffer = jbuffer;
            env->SetIntArrayRegion(jbuffer, 0, size, (jint*)buffer);

            env->CallVoidMethod(lunaris_global_storage_for_java[win].draw_handler->object, lunaris_global_storage_for_java[win].draw_handler->method_id, win, jbuffer);

            // Recopy it (java buffer) to current buffer and free java buffer
            jint* jnativebuffer = env->GetIntArrayElements(jbuffer, 0);
            for(int i=0;i<size;i++){
                buffer[i] = jnativebuffer[i];
            };
            env->ReleaseIntArrayElements(jbuffer, jnativebuffer, 0);
        #else
            env->CallVoidMethod(lunaris_global_storage_for_java[win].draw_handler->object, lunaris_global_storage_for_java[win].draw_handler->method_id, win, NULL);
        #endif

        javaVM->DetachCurrentThread();
    };
};

JNIEXPORT void JNICALL Java_Lunaris_windowRegisterDrawHandler(JNIEnv* env, jobject, jlong _win, jobject draw_handler){
    lunaris_window* win = (lunaris_window*)_win;
    lunaris_global_storage_for_java[win].draw_handler = (draw_handler_for_java*)malloc(sizeof(draw_handler_for_java)); // TODO: Free later

    jclass draw_handler_clazz = env->GetObjectClass(draw_handler);

    lunaris_global_storage_for_java[win].draw_handler->object = env->NewGlobalRef(draw_handler); // TODO: Don't forget remove reference
    lunaris_global_storage_for_java[win].draw_handler->method_id = env->GetMethodID(draw_handler_clazz, "draw", "(J[I)V");
    lunaris_window_register_draw_handler(win, java_function_handler);
};

JNIEXPORT jint JNICALL Java_Lunaris_getWindowWidth(JNIEnv*, jobject, jlong win){
    return lunaris_window_get_width((lunaris_window*)win);
};

JNIEXPORT jint JNICALL Java_Lunaris_getWindowHeight(JNIEnv*, jobject, jlong win){
    return lunaris_window_get_height((lunaris_window*)win);
};

/* Graphics */

#ifndef LUNARIS_DISABLE_DIRECT_BUFFER_ACCESS_FROM_JAVA
    #define LUNARIS_PRE_DRAWING() \
        lunaris_window* win = (lunaris_window*)_win;\
        jintArray jbuffer = lunaris_global_storage_for_java[win].jbuffer;\
        jint* jnativebuffer = env->GetIntArrayElements(jbuffer, 0);\
        for(int i=0;i<lunaris_global_storage_for_java[win].size;i++){\
            win->buffer[i] = jnativebuffer[i];\
        };


    #define LUNARIS_POST_DRAWING() \
        for(int i=0;i<lunaris_global_storage_for_java[win].size;i++){\
            jnativebuffer[i] = win->buffer[i];\
        };\
        env->ReleaseIntArrayElements(jbuffer, jnativebuffer, 0);
#else
    #define LUNARIS_PRE_DRAWING() ;
    #define LUNARIS_POST_DRAWING() ;
#endif

JNIEXPORT void JNICALL Java_Lunaris_windowGraphicsLine(JNIEnv* env, jobject, jlong _win, jint x1, jint y1, jint x2, jint y2, jlong color){
    LUNARIS_PRE_DRAWING()
    lunaris_window_graphics_line((lunaris_window*)_win, x1, y1, x2, y2, color);
    LUNARIS_POST_DRAWING()
};

JNIEXPORT void JNICALL Java_Lunaris_windowGraphicsRect(JNIEnv* env, jobject, jlong _win, jint x, jint y, jint w, jint h, jlong color){
    LUNARIS_PRE_DRAWING()
    lunaris_window_graphics_rect((lunaris_window*)_win, x, y, w, h, color);
    LUNARIS_POST_DRAWING()
};
