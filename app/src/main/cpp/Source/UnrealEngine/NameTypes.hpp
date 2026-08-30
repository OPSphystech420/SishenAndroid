#pragma once

#include "Containers.hpp"
#include <cstdint>
#include <array>

/* Change these here based on your game */

#define LenOffset 10                // In FNameEntry::GetPlainNameString() look for something like "v5 = *a2 >> Value" and put the result of 16 - Value;#
#define LowercaseProbeHashOffset 5  // Result of 16 - (LenOffset + 1)
#define AlignOfFNameEntry 2         // In the FName::ToString() function, used to call FNameEntry::GetPlainNameString()

template <typename T>
FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
{
	return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
}

enum { FNameMaxBlockBits = 13 };
enum { FNameBlockOffsetBits = 16 };
enum { FNameMaxBlocks = 1 << FNameMaxBlockBits };
enum { FNameBlockOffsets = 1 << FNameBlockOffsetBits };

typedef char ANSICHAR;
typedef char16_t WIDECHAR; // Use wchar_t if below UE4.21

struct FNameEntryHeader
{
	uint16 bIsWide : 1;
	uint16 LowercaseProbeHash : LowercaseProbeHashOffset;
	uint16 Len : LenOffset;
};

#define FORCEALIGN(x) __attribute__((packed, aligned(x)))

struct FORCEALIGN(2) FNameEntry
{
public:
    FNameEntryHeader Header;
private:
	union
	{
		ANSICHAR AnsiName[0x400];
		WIDECHAR WideName[0x400];
	};
public:

    inline bool IsWide() const
    {
        return Header.bIsWide;
    }
    
    static constexpr int32 GetDataOffset()
    {
        return 2 /* offsetof(FNameEntry, AnsiName) */;
    }

    std::string GetPlainNameString() const
    {
        if ( IsWide() )
        {
            std::u16string ws(WideName, Header.Len);
            return std::string(ws.begin(), ws.end());
        }
        return std::string(AnsiName, Header.Len);
    }

    static int32 GetSize(int32 Length, bool bIsPureAnsi)
    {
        int32 Bytes = GetDataOffset() + Length * (bIsPureAnsi ? sizeof(ANSICHAR) : sizeof(WIDECHAR));
        return Align(Bytes, alignof(FNameEntry));
    }

};

class FNamePool
{
public:
    enum { Stride = AlignOfFNameEntry };
    enum { BlockSizeBytes = AlignOfFNameEntry * FNameBlockOffsets };

    pthread_rwlock_t Lock;
    uint32 CurrentBlock;
	uint32 CurrentByteCursor;
    uint8* Blocks[FNameMaxBlocks];
public:

    FNameEntry& Resolve(int32 Id) const
    {
        const int32 Block  = Id >> FNameBlockOffsetBits;
        const int32 Offset = Id & (FNameBlockOffsets - 1);
        
        return *reinterpret_cast<FNameEntry*>(Blocks[Block] + (Stride * Offset));
    }

    static class FName Find(const char* Name);
    
public:
    void DebugDump(TFreedArray<const FNameEntry*>& Out) const
	{
		//FRWScopeLock _(Lock, FRWScopeLockType::SLT_ReadOnly);

		for (uint32 BlockIdx = 0; BlockIdx < CurrentBlock; ++BlockIdx)
		{
			DebugDumpBlock(Blocks[BlockIdx], BlockSizeBytes, Out);
		}

		DebugDumpBlock(Blocks[CurrentBlock], CurrentByteCursor, Out);
	}

private:
	static void DebugDumpBlock(const uint8* It, uint32 BlockSize, TFreedArray<const FNameEntry*>& Out)
	{
		const uint8* End = It + BlockSize - sizeof(FNameEntryHeader); 
		while (It < End)
		{
			const FNameEntry* Entry = (const FNameEntry*)It;
			if (uint32 Len = Entry->Header.Len)
			{
				Out.Add(Entry);
				It += FNameEntry::GetSize(Len, !Entry->IsWide());
			}
			else // Null-terminator entry found
			{
				break;
			}
		}
	}
};

class FName
{
private:
    int32 ComparisonIndex;
    uint32 Number;

public:
    static inline FNamePool* NamePoolData = nullptr;

    FName() : ComparisonIndex(0), Number(0) {}
    FName(int32 _ComparisonIndex, int32 _Number = 0) : ComparisonIndex(_ComparisonIndex), Number(_Number) {}
    
    explicit FName(TCHAR const* Name);

    static bool Init(uintptr Location)
    {
        NamePoolData = reinterpret_cast<FNamePool*>(Location);
        return NamePoolData ? true : false;
    }
 
    FNamePool& GetNamePool() const
    {
        return *NamePoolData;
    }

    FORCEINLINE int32 GetDisplayIndex() const
    {
        return ComparisonIndex;
    }

    FORCEINLINE uint32 GetNumber() const
    {
        return Number;
    }

    const FNameEntry* GetDisplayNameEntry() const
    {
	    return &GetNamePool().Resolve(GetDisplayIndex());
    }

    FORCEINLINE std::string ToString() const
    {
        const FNameEntry* NameEntry = GetDisplayNameEntry();
        return NameEntry ? NameEntry->GetPlainNameString() : "";
    }

    bool IsAny() const { return false; };
    
    template<typename... TArgs>
    bool IsAny(FName First, TArgs... Rest) const
    {
        if (*this == First)
            return true;
        return IsAny(Rest...);
    }

    inline bool operator==(const FName& other) const { return ComparisonIndex == other.GetDisplayIndex(); }
    inline bool operator!=(const FName& other) const { return ComparisonIndex != other.GetDisplayIndex(); }
};

inline FName FNamePool::Find(const char* InName)
{
    if (!FName::NamePoolData) 
        return 0;
    
    FNamePool& NamePoolData = *FName::NamePoolData;

    pthread_rwlock_rdlock(&NamePoolData.Lock);

    for (uint32 BlockIdx = 0; BlockIdx <= NamePoolData.CurrentBlock; ++BlockIdx)
    {
        const uint8* It = NamePoolData.Blocks[BlockIdx];
        const uint8* End = It + (BlockIdx == NamePoolData.CurrentBlock ? NamePoolData.CurrentByteCursor : FNamePool::BlockSizeBytes);

        while (It < End)
        {
            const FNameEntry* Entry = (const FNameEntry*)It;
            if (uint32 Len = Entry->Header.Len)
            {
                if (Entry->GetPlainNameString() == InName)
                {
                    uint32 EntryOffset = It - NamePoolData.Blocks[BlockIdx];
                    int32 ComparisonIndex = (BlockIdx << FNameBlockOffsetBits) + (EntryOffset / FNamePool::Stride);
                    pthread_rwlock_unlock(&NamePoolData.Lock);
                    return ComparisonIndex;
                }
                It += FNameEntry::GetSize(Len, !Entry->IsWide());
            }
            else 
            {
                break;
            }
        }
    }

    pthread_rwlock_unlock(&NamePoolData.Lock);
    return 0;
}
