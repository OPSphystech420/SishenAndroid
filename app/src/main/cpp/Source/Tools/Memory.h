//
// Created by Leon Benkovic on 27.05.25.
//

#pragma once

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

class IMemoryUtils
{
public:
    static IMemoryUtils* Get(const char* TargetLibrary = nullptr)
    {
        static IMemoryUtils Singleton(TargetLibrary);
        return &Singleton;
    }

public:
    bool IsLibraryLoaded() const
    {
        return dlopen(TargetLibrary, RTLD_NOLOAD) != nullptr;
    }

    void* FindSymbol(char* const Symbol) const
    {
        void* Handle = dlopen(TargetLibrary, RTLD_NOLOAD);
        return (Handle != nullptr) ? dlsym(Handle, Symbol) : nullptr;
    }

    uint8_t* GetLibraryBase() const
    {
        char Buffer[1024] = {};
        uintptr_t Address = 0;

        FILE* File = fopen("/proc/self/maps", "rt");
        if (!File)
        {
            perror("fopen");
            return nullptr;
        }

        while (fgets(Buffer, sizeof(Buffer), File))
        {
            if (__builtin_strstr(Buffer, TargetLibrary))
            {
                Address = strtoul(Buffer, nullptr, 16);
                break;
            }
        }

        fclose(File);
        return reinterpret_cast<uint8_t*>(Address);
    }

    uint8_t* GetAddress(uint64_t Offset) const
    {
        return GetLibraryBase() + Offset;
    }

    uint64_t GetOffset(uint8_t* Address) const
    {
        return Address - GetLibraryBase();
    }

private:

    IMemoryUtils() = default;
    explicit IMemoryUtils(const char* InTargetLibrary) : TargetLibrary(InTargetLibrary ? InTargetLibrary : "") {}

    const char* TargetLibrary = "";
};
