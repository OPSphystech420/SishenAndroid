#pragma once

#include <iostream>
#include <thread>
#include <string>
#include <jni.h>

#include <imgui.h>
#include <imgui_impl_android.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <android/native_window_jni.h>
#include <json.hpp>
#include "obfuscate.h"

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef intptr_t intptr;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uintptr_t uintptr;



#include "android/log.h"

enum LogType {
    oDEBUG = 3,
    oERROR = 6,
    oINFO  = 4,
    oWARN  = 5
};

#define TAG OBFUSCATE("SishenTool")

#define LOGD(...) ((void)__android_log_print(oDEBUG, TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(oERROR, TAG, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(oINFO,  TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(oWARN,  TAG, __VA_ARGS__))


// Global Variables

inline int ScreenWidth = 0;
inline int ScreenHeight = 0;

inline JNIEnv* global_env = nullptr;
inline jobject globalContext;
inline jobject globalGiveKey;

inline ImGuiWindow* g_window = nullptr;
inline bool g_Initialized = false;
inline bool ShowMenu = false;
inline bool CanCloseMenu = false;

inline ANativeWindow* g_NativeWindow = nullptr;


void native_ShowMenuImgui(JNIEnv *env, jclass clazz);

#define FORCEINLINE inline __attribute__((always_inline))
#define DEGTORAD 3.14159265358979323846f / 180.0f
#define RADTODEG 180.0f / 3.14159265358979323846f

template<int32 Len>
struct StringLiteral
{
    char Chars[Len];

    consteval StringLiteral(const char(&String)[Len])
    {
        std::copy_n(String, Len, Chars);
    }

    operator std::string() const
    {
        return static_cast<const char*>(Chars);
    }
};

FORCEINLINE void CrashSafe()
{
    *(volatile int*)0 = 1;
    return;
}

template<typename To>
FORCEINLINE To* Cast(void* Src)
{
    return static_cast<To*>(Src);
}

template<typename To>
FORCEINLINE const To* Cast(const void* Src)
{
    return static_cast<const To*>(Src);
}

template <typename T>
FORCEINLINE T&& MoveTemp(T& Obj)
{
    return static_cast<T&&>(Obj);
}

template <typename T>
FORCEINLINE T CopyTemp(T& Val)
{
    return const_cast<const T&>(Val);
}

template <typename T>
FORCEINLINE T CopyTemp(const T& Val)
{
    return Val;
}

template <typename T>
FORCEINLINE T&& CopyTemp(T&& Val)
{
    // If we already have an rvalue, just return it unchanged, rather than needlessly creating yet another rvalue from it.
    return MoveTemp(Val);
}

template<typename FunctionType>
FORCEINLINE void InitializeFunction(FunctionType& Function, void* Address)
{
    Function = FunctionType(Address);
}

template<typename PointerType>
FORCEINLINE void InitializePointer(PointerType*& Pointer, void* Address)
{
    Pointer = reinterpret_cast<PointerType*>(Address);
}

#include "Settings.h"