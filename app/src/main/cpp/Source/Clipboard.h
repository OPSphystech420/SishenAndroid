#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <jni.h>


#define LOG_TAG "ClipBoardJNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL Java_the_best_cheat_ClipBoard_copyToClipboard(JNIEnv *env, jclass clazz, jstring text) {
    const char *nativeString = env->GetStringUTFChars(text, 0);
    jclass clipBoardClass = env->FindClass("ark/use/sishen/ClipBoard");
    if (clipBoardClass == nullptr) {
        return;
    }
    jmethodID copyToClipboardMethod = env->GetStaticMethodID(clipBoardClass, "copyToClipboard", "(Ljava/lang/String;)V");
    if (copyToClipboardMethod == nullptr) {
        return;
    }
    env->CallStaticVoidMethod(clipBoardClass, copyToClipboardMethod, text);

    // Release the jstring
    env->ReleaseStringUTFChars(text, nativeString);
}
#endif