LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

KITTYMEMORY_PATH = KittyMemory
include $(CLEAR_VARS)
LOCAL_MODULE := Keystone
LOCAL_SRC_FILES := $(KITTYMEMORY_PATH)/Deps/Keystone/libs-android/$(TARGET_ARCH_ABI)/libkeystone.a
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := fmt
LOCAL_SRC_FILES := $(LOCAL_PATH)/src/format.cc
LOCAL_C_INCLUDES := $(LOCAL_PATH)/Includes/fmt
include $(BUILD_STATIC_LIBRARY)

# Here is the name of your lib.
# When you change the lib name, change also on System.loadLibrary("") under OnCreate method on StaticActivity.java
# Both must have same name
include $(CLEAR_VARS)
LOCAL_MODULE    := Sishen

# -std=c++17 is required to support AIDE app with NDK
LOCAL_CFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
LOCAL_CPPFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++20
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all,-llog
LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/Core/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/uli
LOCAL_C_INCLUDES += $(LOCAL_PATH)/json
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui/backends



# Here you add the cpp file to compile
LOCAL_SRC_FILES := Setup.cpp \
    Source/Menu.cpp \
    Source/Main.cpp \
    Source/UnrealEngine/ScriptCore.cpp \
    Includes/Utils.cpp \
    src/format.cc \
    Substrate/hde64.c \
    Substrate/SubstrateDebug.cpp \
    Substrate/SubstrateHook.cpp \
    Substrate/SubstratePosixMemory.cpp \
    Substrate/SymbolFinder.cpp \
    KittyMemory/KittyArm64.cpp \
    KittyMemory/KittyScanner.cpp \
    KittyMemory/KittyMemory.cpp \
    KittyMemory/KittyUtils.cpp \
    KittyMemory/MemoryPatch.cpp \
    KittyMemory/MemoryBackup.cpp \
    And64InlineHook/And64InlineHook.cpp \
    ImGui/imgui.cpp \
    ImGui/imgui_draw.cpp \
    ImGui/imgui_widgets.cpp \
    ImGui/imgui_tables.cpp \
    ImGui/backends/imgui_impl_opengl3.cpp \
    ImGui/backends/imgui_impl_android.cpp \
    ELF64/fix.cpp \
    
LOCAL_STATIC_LIBRARIES := Keystone


include $(BUILD_SHARED_LIBRARY)
