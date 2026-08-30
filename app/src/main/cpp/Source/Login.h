#ifndef LOGIN_H
#define LOGIN_H

#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <locale>
#include <thread>
#include <chrono>
#include <algorithm>
#include <android/log.h>

#include "Includes.h"

extern "C" JNIEXPORT void JNICALL
Java_ark_use_sishen_GiveKey_sendKeyToCpp(JNIEnv* env, jobject obj, jstring key) {

}

extern "C" JNIEXPORT void JNICALL
Java_ark_use_sishen_GiveKey_sendKeysToCpp(JNIEnv* env, jobject obj, jobjectArray keysArray) {

}

extern "C" JNIEXPORT jboolean JNICALL
Java_ark_use_sishen_GiveKey_validateKeyFromCpp(JNIEnv* env, jobject obj) {
  return JNI_FALSE;
}

extern "C" JNIEXPORT jstring JNICALL
Java_ark_use_sishen_GiveKey_getObfuscatedUrl(JNIEnv* env, jobject obj)
{
    return env->NewStringUTF("");
}

extern "C" JNIEXPORT void JNICALL
Java_ark_use_sishen_GiveKey_setJavaContext(JNIEnv* env, jobject obj, jobject context, jobject giveKey) {
    //globalEnv = env;
    globalContext = env->NewGlobalRef(context);
    globalGiveKey = env->NewGlobalRef(giveKey);
}
#endif