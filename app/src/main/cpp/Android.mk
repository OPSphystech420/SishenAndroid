###############################################################################
# Top‐level definitions
###############################################################################
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# Path to your prebuilt Keystone library
KITTYMEMORY_PATH := KittyMemory

###############################################################################
# 1) Prebuilt Keystone static library (no change)
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE    := Keystone
LOCAL_SRC_FILES := $(KITTYMEMORY_PATH)/Deps/Keystone/libs-android/$(TARGET_ARCH_ABI)/libkeystone.a
include $(PREBUILT_STATIC_LIBRARY)

###############################################################################
# 2) fmt static library (no change)
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE      := fmt
LOCAL_SRC_FILES   := src/format.cc
LOCAL_C_INCLUDES  := Includes/fmt
include $(BUILD_STATIC_LIBRARY)

###############################################################################
# 3) Compile Setup.cpp WITHOUT Hikari plugin flags
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE      := Sishen_setup
LOCAL_SRC_FILES   := Source/Icon.cpp
include $(BUILD_STATIC_LIBRARY)

###############################################################################
# 4) Main Sishen shared library WITH Hikari plugin flags
###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE      := Sishen

# list _all_ your other source files here (omit Setup.cpp)
LOCAL_SRC_FILES   := Setup.cpp \
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
    ELF64/fix.cpp

# link against the two static libs we built above
LOCAL_STATIC_LIBRARIES := \
    Keystone \
    Sishen_setup

# include paths
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/ImGui \
    $(LOCAL_PATH)/Includes \
    $(LOCAL_PATH)/Core \
    $(LOCAL_PATH)/uli \
    $(LOCAL_PATH)/json \
    $(LOCAL_PATH)/ImGui/backends

# Hikari plugin location (adjust NDK_ROOT path if needed)
HIKARI_PLUGIN := $(NDK_ROOT)/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so

# you still want your normal warning flags…
LOCAL_CFLAGS      := \
    -w \
    -s \
    -Wno-error=format-security \
    -fvisibility=hidden \
    -fpermissive \
    -fexceptions \
    -fpass-plugin=$(HIKARI_PLUGIN) \
    -Xclang -load \
    -Xclang=$(HIKARI_PLUGIN) \
    -mllvm -enable-strcry

LOCAL_CPPFLAGS    := \
    -w \
    -s \
    -Wno-error=format-security \
    -fvisibility=hidden \
    -std=c++20 \
    -Wno-error=c++11-narrowing \
    -fpermissive \
    -Wall \
    -fexceptions \
    -fpass-plugin=$(HIKARI_PLUGIN) \
    -Xclang -load \
    -Xclang=$(HIKARI_PLUGIN) \
    -mllvm -enable-strcry

# linker flags
LOCAL_LDFLAGS    += -Wl,--gc-sections,--strip-all
LOCAL_LDLIBS      := \
    -llog \
    -landroid \
    -lEGL \
    -lGLESv3

include $(BUILD_SHARED_LIBRARY)

# Some old bullshit
# LOCAL_PATH := $(call my-dir)
# include $(CLEAR_VARS)
#
# KITTYMEMORY_PATH = KittyMemory
# include $(CLEAR_VARS)
# LOCAL_MODULE := Keystone
# LOCAL_SRC_FILES := $(KITTYMEMORY_PATH)/Deps/Keystone/libs-android/$(TARGET_ARCH_ABI)/libkeystone.a
# include $(PREBUILT_STATIC_LIBRARY)
#
#
#
# include $(CLEAR_VARS)
# LOCAL_MODULE := fmt
# LOCAL_SRC_FILES := $(LOCAL_PATH)/src/format.cc
# LOCAL_C_INCLUDES := $(LOCAL_PATH)/Includes/fmt
# include $(BUILD_STATIC_LIBRARY)
#
# # Here is the name of your lib.
# # When you change the lib name, change also on System.loadLibrary("") under OnCreate method on StaticActivity.java
# # Both must have same name
# include $(CLEAR_VARS)
# LOCAL_MODULE    := Sishen
#
# # -std=c++17 is required to support AIDE app with NDK
# LOCAL_CFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -fpermissive -fexceptions
# LOCAL_CPPFLAGS := -w -s -Wno-error=format-security -fvisibility=hidden -Werror -std=c++20
# LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fpermissive -Wall -fexceptions
# LOCAL_LDFLAGS += -Wl,--gc-sections,--strip-all,-llog
# LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv3
# LOCAL_ARM_MODE := arm
#
# HIKARI_PLUGIN := $(NDK_ROOT)/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so
#
# LOCAL_CFLAGS   += -fpass-plugin=$(HIKARI_PLUGIN) -Xclang -load
# LOCAL_CFLAGS   += -Xclang=$(HIKARI_PLUGIN)
# LOCAL_CFLAGS   += -mllvm -enable-strcry
#
# LOCAL_CPPFLAGS += -fpass-plugin=$(HIKARI_PLUGIN) -Xclang -load
# LOCAL_CPPFLAGS += -Xclang=$(HIKARI_PLUGIN)
# LOCAL_CPPFLAGS += -mllvm -enable-strcry
#
# LOCAL_C_INCLUDES += $(LOCAL_PATH)
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/Includes/
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/Core/
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/uli
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/json
# LOCAL_C_INCLUDES += $(LOCAL_PATH)/ImGui/backends
#
#
#
# # Here you add the cpp file to compile
# LOCAL_SRC_FILES :=  Source/Icon.cpp \
#     Setup.cpp \
#     Source/Menu.cpp \
#     Source/Main.cpp \
#     Source/UnrealEngine/ScriptCore.cpp \
#     Includes/Utils.cpp \
#     src/format.cc \
#     Substrate/hde64.c \
#     Substrate/SubstrateDebug.cpp \
#     Substrate/SubstrateHook.cpp \
#     Substrate/SubstratePosixMemory.cpp \
#     Substrate/SymbolFinder.cpp \
#     KittyMemory/KittyArm64.cpp \
#     KittyMemory/KittyScanner.cpp \
#     KittyMemory/KittyMemory.cpp \
#     KittyMemory/KittyUtils.cpp \
#     KittyMemory/MemoryPatch.cpp \
#     KittyMemory/MemoryBackup.cpp \
#     And64InlineHook/And64InlineHook.cpp \
#     ImGui/imgui.cpp \
#     ImGui/imgui_draw.cpp \
#     ImGui/imgui_widgets.cpp \
#     ImGui/imgui_tables.cpp \
#     ImGui/backends/imgui_impl_opengl3.cpp \
#     ImGui/backends/imgui_impl_android.cpp \
#     ELF64/fix.cpp \
#
# LOCAL_STATIC_LIBRARIES := Keystone
#
#
# include $(BUILD_SHARED_LIBRARY)
