# -------------------------------
#  CMakeLists.txt for SishenNative
# -------------------------------

cmake_minimum_required(VERSION 3.4.1)

project(SishenNative LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/uli
        ${CMAKE_CURRENT_SOURCE_DIR}/json
        ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/KittyMemory
        ${CMAKE_CURRENT_SOURCE_DIR}/Substrate
        ${CMAKE_CURRENT_SOURCE_DIR}/ELF64
)

add_library( keystone_lib STATIC IMPORTED )
set_target_properties( keystone_lib PROPERTIES
        IMPORTED_LOCATION
            ${CMAKE_CURRENT_SOURCE_DIR}/KittyMemory/Deps/Keystone/libs-android/${ANDROID_ABI}/libkeystone.a
        INTERFACE_INCLUDE_DIRECTORIES
            ${CMAKE_CURRENT_SOURCE_DIR}/KittyMemory/Deps/Keystone/includes
)

add_library( fmt_lib STATIC
        src/format.cc
)
target_include_directories( fmt_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Includes/fmt
)
target_compile_options( fmt_lib PRIVATE
        -w -s -Wno-error=format-security
)

add_library( Sishen SHARED
        Setup.cpp
        Source/Menu.cpp
        Source/Main.cpp
        Source/UnrealEngine/ScriptCore.cpp
        Includes/Utils.cpp
        src/format.cc
        Substrate/hde64.c
        Substrate/SubstrateDebug.cpp
        Substrate/SubstrateHook.cpp
        Substrate/SubstratePosixMemory.cpp
        Substrate/SymbolFinder.cpp
        KittyMemory/KittyArm64.cpp
        KittyMemory/KittyScanner.cpp
        KittyMemory/KittyMemory.cpp
        KittyMemory/KittyUtils.cpp
        KittyMemory/MemoryPatch.cpp
        KittyMemory/MemoryBackup.cpp
        And64InlineHook/And64InlineHook.cpp
        ImGui/imgui.cpp
        ImGui/imgui_draw.cpp
        ImGui/imgui_widgets.cpp
        ImGui/imgui_tables.cpp
        ImGui/backends/imgui_impl_opengl3.cpp
        ImGui/backends/imgui_impl_android.cpp
        ELF64/fix.cpp
)

set(HIKARI_PLUGIN
        "${ANDROID_NDK}/toolchains/llvm/prebuilt/darwin-x86_64/lib/libHikari.so" # set path
)

target_compile_options(Sishen PRIVATE
        # C & C++
        -w
        -s
        -Wno-error=format-security
        -fvisibility=hidden
        -fpermissive
        -fexceptions

        # C++
        $<$<COMPILE_LANGUAGE:CXX>:
        -Werror
        -std=c++20
        -Wno-error=c++11-narrowing
        -Wall
        >

        # Obf
        -fvisibility=hidden
        -fpass-plugin=${HIKARI_PLUGIN}
        -Xclang
        -load
        -Xclang=${HIKARI_PLUGIN}
        -mllvm
        -enable-strcry
)

target_link_libraries( Sishen PRIVATE
        keystone_lib
)

find_library( log-lib     log )
find_library( android-lib android )
find_library( EGL-lib     EGL )
find_library( GLESv3-lib  GLESv3 )

target_link_libraries( Sishen PRIVATE
        ${log-lib}
        ${android-lib}
        ${EGL-lib}
        ${GLESv3-lib}
        fmt_lib
)
