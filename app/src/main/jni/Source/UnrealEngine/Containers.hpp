#pragma once
// Container implementations with iterators. See https://github.com/Fischsalat/UnrealContainers
#include "../Includes.h"
#include <__config>
#include <iostream>
#include <chrono>
#include <string>
#include <cstdarg>
#include <cwchar>
#include <memory>
#include <stdexcept>
#include <string>
#include <initializer_list>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <type_traits>


namespace FMemory
{
	inline void* (*EngineRealloc)(void* Block, uint64 NewSize, uint32 Alignment) = nullptr;

	inline int32 AllocCount = 0x0;

	inline void Init(void* ReallocAddress)
	{
		EngineRealloc = reinterpret_cast<decltype(EngineRealloc)>(ReallocAddress);
	}

	[[nodiscard]] inline void* Malloc(uint64 Size, uint32 Alignment = 0x0 /* auto */)
	{
		return EngineRealloc(nullptr, Size, Alignment);
	}

	[[nodiscard]] inline void* Realloc(void* Ptr, uint64 Size, uint32 Alignment = 0x0 /* auto */)
	{
		return EngineRealloc(Ptr, Size, Alignment);
	}

	inline void Free(void* Ptr)
	{
		EngineRealloc(Ptr, 0x0, 0x0);
	}
}

template<typename CharType>
struct TStringUtils
{

    static constexpr FORCEINLINE uint32 ToUnsigned(CharType Char)
    {
        return (std::make_unsigned_t<CharType>)Char;
    }

    static CharType ToUpper(CharType Char)
    {
        return (CharType)(ToUnsigned(Char) - ((uint32(Char) - 'a' < 26u) << 5));
    }

    static CharType ToLower(CharType Char)
    {
        return (CharType)(ToUnsigned(Char) + ((uint32(Char) - 'A' < 26u) << 5));
    }

    static constexpr uint8 LowerAscii[128] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
        'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',  0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
    };

    template<typename CharType1, typename CharType2>
    static FORCEINLINE bool BothAscii(CharType1 C1, CharType2 C2)
    {
        return (((uint32)C1 | (uint32)C2) & 0xffffff80) == 0;
    }

    static int32 Strlen(const CharType* String)
    {
        int32 Length = -1;
        do
        {
            Length++;
        }
        while (*String++);

        return Length;
    }

    static int32 Strcmp(const CharType* String1, const CharType* String2)
    {
        // walk the strings, comparing them case sensitively
        for (; *String1 || *String2; String1++, String2++)
        {
            CharType A = *String1, B = *String2;
            if (A != B)
            {
                return A - B;
            }
        }
        return 0;
    }

    static int32 Strncmp(const CharType* String1, const CharType* String2, size_t Count)
    {
        // walk the strings, comparing them case sensitively, up to a max size
        for (; (*String1 || *String2) && Count; String1++, String2++, Count--)
        {
            CharType A = *String1, B = *String2;
            if (A != B)
            {
                return A - B;
            }
        }
        return 0;
    }

    static int32 Strnicmp(const CharType* String1, const CharType* String2, size_t Count)
    {
        for (; Count > 0; --Count)
        {
            // Quickly move on if characters are identical but
            // return equals if we found two null terminators
            CharType C1 = *String1++;
            CharType C2 = *String2++;

            if (C1 == C2)
            {
                if (C1)
                {
                    continue;
                }

                return 0;
            }
            else if (BothAscii(C1, C2))
            {
                if (int32 Diff = LowerAscii[ToUnsigned(C1)] - LowerAscii[ToUnsigned(C2)])
                {
                    return Diff;
                }
            }
            else
            {
                return ToUnsigned(C1) - ToUnsigned(C2);
            }
        }

        return 0;
    }

    static const CharType* Stristr(const CharType* Str, const CharType* Find)
    {
        // both strings must be valid
        if (Find == NULL || Str == NULL)
        {
            return NULL;
        }

        // get upper-case first letter of the find string (to reduce the number of full strnicmps)
        CharType FindInitial = ToUpper(*Find);
        // get length of find string, and increment past first letter
        int32   Length = Strlen(++Find) - 1;
        // get the first letter of the search string, and increment past it
        CharType StrChar = *Str++;
        // while we aren't at end of string...
        while (StrChar)
        {
            // make sure it's upper-case
            StrChar = ToUpper(StrChar);
            // if it matches the first letter of the find string, do a case-insensitive string compare for the length of the find string
            if (StrChar == FindInitial && !Strnicmp(Str, Find, Length))
            {
                // if we found the string, then return a pointer to the beginning of it in the search string
                return Str - 1;
            }
            // go to next letter
            StrChar = *Str++;
        }
        
        // if nothing was found, return NULL
        return NULL;
    }

    static CharType* Stristr(CharType* String, const CharType* Find)
    {
        return const_cast<CharType*>(Stristr((const CharType*)String, Find));
    }

    static const CharType* Strstr(const CharType* String, const CharType* Find)
    {
        CharType Char1, Char2;
        if ((Char1 = *Find++) != 0)
        {
            size_t Length = Strlen(Find);

            do
            {
                do
                {
                    if ((Char2 = *String++) == 0)
                    {
                        return nullptr;
                    }
                }
                while (Char1 != Char2);
            }
            while (Strncmp(String, Find, Length) != 0);

            String--;
        }

        return String;
    }

    static CharType* Strstr(CharType* String, const CharType* Find)
    {
        return const_cast<CharType*>(Strstr((const CharType*)String, Find));
    }

};

typedef char16_t /* wchar_t */ TCHAR; // TCHAR for Unreal Engine 4.21+

#define TEXT(x) u##x
 
typedef TStringUtils<TCHAR> FCString;



template<typename ArrayElementType>
class TArray;

template<typename ArrayElementType>
class TIndirectArray;

template<typename ArrayElementType>
class TFreedArray;

template<typename SparseArrayElementType>
class TSparseArray;

template<typename SetElementType>
class TSet;

template<typename KeyElementType, typename ValueElementType>
class TMap;

template<typename KeyElementType, typename ValueElementType>
class TPair;

namespace Iterators
{
	class FSetBitIterator;

	template<typename ArrayType>
	class TArrayIterator;

	template<class ContainerType>
	class TContainerIterator;

	template<typename SparseArrayElementType>
	using TSparseArrayIterator = TContainerIterator<TSparseArray<SparseArrayElementType>>;

	template<typename SetElementType>
	using TSetIterator = TContainerIterator<TSet<SetElementType>>;

	template<typename KeyElementType, typename ValueElementType>
	using TMapIterator = TContainerIterator<TMap<KeyElementType, ValueElementType>>;
}

namespace ContainerImpl
{
	namespace HelperFunctions
	{
		inline uint32 CountLeadingZeros(uint32 Value)
		{
			if (Value == 0)
				return 32;
		
			return __builtin_clz(Value);
		}

		inline uint32 CountTrailingZeros(uint32 Value)
		{
			if (Value == 0)
				return 32;

			return __builtin_ctz(Value);
		}
	}

	template<int32 Size, uint32 Alignment>
	struct TAlignedBytes
	{
		alignas(Alignment) uint8 Pad[Size];
	};

	template<uint32 NumInlineElements>
	class TInlineAllocator
	{
	public:
		template<typename ElementType>
		class ForElementType
		{
		private:
			static constexpr int32 ElementSize = sizeof(ElementType);
			static constexpr int32 ElementAlign = alignof(ElementType);

			static constexpr int32 InlineDataSizeBytes = NumInlineElements * ElementSize;

		private:
			TAlignedBytes<ElementSize, ElementAlign> InlineData[NumInlineElements];
			ElementType* SecondaryData;

		public:
			ForElementType()
				: InlineData{{0x0}}, SecondaryData(nullptr)
			{
			}

			ForElementType(ForElementType&&) = default;
			ForElementType(const ForElementType&) = default;

		public:
			ForElementType& operator=(ForElementType&&) = default;
			ForElementType& operator=(const ForElementType&) = default;

		public:
			inline const ElementType* GetAllocation() const { return SecondaryData ? SecondaryData : reinterpret_cast<const ElementType*>(&InlineData); }

			inline uint32 GetNumInlineBytes() const { return NumInlineElements; }
            
		};
	};

	class FBitArray
	{
	protected:
		static constexpr int32 NumBitsPerDWORD = 32;
		static constexpr int32 NumBitsPerDWORDLogTwo = 5;
        
        typedef uint32 WordType;
        static constexpr WordType FullWordMask = (WordType)-1;

	private:
		TInlineAllocator<4>::ForElementType<int32> Data;
		int32 NumBits;
		int32 MaxBits;

	public:
		FBitArray()
			: NumBits(0), MaxBits(Data.GetNumInlineBytes() * NumBitsPerDWORD)
		{
		}

		FBitArray(const FBitArray&) = default;

		FBitArray(FBitArray&&) = default;

	public:
		FBitArray& operator=(FBitArray&&) = default;

		FBitArray& operator=(const FBitArray& Other) = default;

	private:
		inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }
        
	public:
		inline int32 Num() const { return NumBits; }
		inline int32 Max() const { return MaxBits; }

		inline const uint32* GetData() const { return reinterpret_cast<const uint32*>(Data.GetAllocation()); }

		inline bool IsValidIndex(int32 Index) const { return Index >= 0 && Index < NumBits; }

		inline bool IsValid() const { return GetData() && NumBits > 0; }
        

	public:
		inline bool operator[](int32 Index) const { VerifyIndex(Index); return GetData()[Index / NumBitsPerDWORD] & (1 << (Index & (NumBitsPerDWORD - 1))); }

		inline bool operator==(const FBitArray& Other) const { return NumBits == Other.NumBits && GetData() == Other.GetData(); }
		inline bool operator!=(const FBitArray& Other) const { return NumBits != Other.NumBits || GetData() != Other.GetData(); }

	public:
		friend Iterators::FSetBitIterator begin(const FBitArray& Array);
		friend Iterators::FSetBitIterator end  (const FBitArray& Array);
	};

	template<typename SparseArrayType>
	union TSparseArrayElementOrFreeListLink
	{
		SparseArrayType ElementData;

		struct
		{
			int32 PrevFreeIndex;
			int32 NextFreeIndex;
		};
	};

	template<typename SetType>
	class SetElement
	{
	private:
		template<typename SetDataType>
		friend class TSet;

	public:
		SetType Value;
		int32 HashNextId;
		int32 HashIndex;
	};
}


template <typename KeyType, typename ValueType>
class TPair
{
public:
	KeyType First;
	ValueType Second;

public:
	TPair(KeyType Key, ValueType Value)
		: First(Key), Second(Value)
	{
	}

public:
	inline       KeyType& Key()       { return First; }
	inline const KeyType& Key() const { return First; }

	inline       ValueType& Value()       { return Second; }
	inline const ValueType& Value() const { return Second; }
};



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
template<typename ArrayElementType>
class TArray
{
private:
	template<typename TAllocatedElementType>
	friend class TAllocatedArray;

	template<typename ArrayElement>
	friend class TFreedArray;

	template<typename SparseArrayElementType>
	friend class TSparseArray;

protected:
	static constexpr uint64 ElementAlign = alignof(ArrayElementType);
	static constexpr uint64 ElementSize = sizeof(ArrayElementType);

protected:
	ArrayElementType* Data;
	int32 NumElements;
	int32 MaxElements;

public:
	TArray()
		: Data(nullptr), NumElements(0), MaxElements(0)
	{
	}

	TArray(const TArray&) = default;

	TArray(TArray&&) = default;

public:
	TArray& operator=(TArray&&) = default;
	TArray& operator=(const TArray&) = default;

private:
	inline int32 GetSlack() const { return MaxElements - NumElements; }

	inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

	inline       ArrayElementType& GetUnsafe(int32 Index)       { return Data[Index]; }
	inline const ArrayElementType& GetUnsafe(int32 Index) const { return Data[Index]; }

public:
	inline int32 Num() const { return NumElements; }
	inline int32 Max() const { return MaxElements; }

	inline void* GetData() const { return Data; }

	inline bool IsValidIndex(int32 Index) const { return Data && Index >= 0 && Index < NumElements; }

	inline bool IsValid() const { return Data && NumElements > 0 && MaxElements >= NumElements; }

public:
	inline       ArrayElementType& operator[](int32 Index)       { VerifyIndex(Index); return Data[Index]; }
	inline const ArrayElementType& operator[](int32 Index) const { VerifyIndex(Index); return Data[Index]; }

	inline bool operator==(const TArray<ArrayElementType>& Other) const { return Data == Other.Data; }
	inline bool operator!=(const TArray<ArrayElementType>& Other) const { return Data != Other.Data; }

	inline explicit operator bool() const { return IsValid(); };

public:
	template<typename T> friend Iterators::TArrayIterator<T> begin(const TArray& Array);
	template<typename T> friend Iterators::TArrayIterator<T> end  (const TArray& Array);
};



template<typename ArrayElementType>
class TFreedArray : public TArray<ArrayElementType>
{
public:
	TFreedArray()
	{
		this->Data = nullptr; 
		this->NumElements = 0; 
		this->MaxElements = 0;
	}

	TFreedArray(int32 Size)
	{
		this->Data = static_cast<ArrayElementType*>(FMemory::Malloc(Size * this->ElementSize, this->ElementAlign)); 
		this->NumElements = 0; 
		this->MaxElements = Size;
	}

	TFreedArray(const TArray<ArrayElementType>& Other)
	{
		this->Data = nullptr; 
		this->NumElements = 0; 
		this->MaxElements = 0;

		this->CopyFrom(Other);
	}

	TFreedArray(TArray<ArrayElementType>&& Other) noexcept
	{
		this->Data = Other.Data; 
		this->NumElements = Other.NumElements; 
		this->MaxElements = Other.MaxElements;

		Other.Data = nullptr;
		Other.NumElements = 0x0;
		Other.MaxElements = 0x0;
	}
    
    TFreedArray(std::initializer_list<ArrayElementType> List)
    {
        this->Data = static_cast<ArrayElementType*>(FMemory::Realloc(this->Data, this->ElementSize * List.size()));
        this->NumElements = this->MaxElements = List.size();
        
        memcpy(this->Data, List.begin(), List.size() * this->ElementSize);
    }

	~TFreedArray()
	{
		Free(); 
	}

public:
	TFreedArray& operator=(TArray<ArrayElementType>&& Other) noexcept
	{
		if (this == reinterpret_cast<decltype(this)>(&Other))
			return *this;

		Free();

		this->Data = Other.Data;
		this->NumElements = Other.NumElements;
		this->MaxElements = Other.MaxElements;

		Other.Data = nullptr;
		Other.NumElements = 0x0;
		Other.MaxElements = 0x0;

		return *this;
	}

	TFreedArray& operator=(const TArray<ArrayElementType>& Other)
	{
		this->CopyFrom(Other);

		return *this;
	}

public:
	FORCEINLINE void Reserve(int32 Count)
	{
		if (this->GetSlack() < Count)
			this->MaxElements += Count; 
		
		this->Data = static_cast<ArrayElementType*>(FMemory::Realloc(this->Data, this->MaxElements * this->ElementSize, this->ElementAlign));
	}

	FORCEINLINE void Add(const ArrayElementType& Element)
	{
		if (this->GetSlack() <= 0)
			Reserve(3);

		this->Data[this->NumElements] = Element;
		this->NumElements++;
	}
    

	FORCEINLINE void CopyFrom(const TArray<ArrayElementType>& Other)
	{
		if (this == reinterpret_cast<decltype(this)>(&Other) || Other.NumElements == 0)
			return;

		this->NumElements = Other.NumElements;

		if (this->MaxElements >= Other.NumElements)
		{
			memcpy(this->Data, Other.Data, Other.NumElements);
			return;
		}

		this->Data = static_cast<ArrayElementType*>(FMemory::Realloc(this->Data, Other.NumElements * this->ElementSize, this->ElementAlign));
		this->MaxElements = Other.NumElements;
		memcpy(this->Data, Other.Data, Other.NumElements * this->ElementSize);
	}

	FORCEINLINE void Remove(int32 Index)
	{
		if (!this->IsValidIndex(Index))
			return;

		this->NumElements--;

		for (int i = Index; i < this->NumElements; i++)
		{
			/* NumElements was decremented, acessing i + 1 is safe */
			this->Data[i] = this->Data[i + 1];
		}
	}

	FORCEINLINE void Clear()
	{
		this->NumElements = 0;

		if (this->Data)
			memset(this->Data, 0, this->NumElements * this->ElementSize);
	}

	FORCEINLINE void Free() noexcept
	{
		if (this->Data)
			FMemory::Free(this->Data);

		this->NumElements = 0x0;
		this->MaxElements = 0x0;
	}

public:
	FORCEINLINE operator       TArray<ArrayElementType>()       { return *reinterpret_cast<      TArray<ArrayElementType>*>(this); }
	FORCEINLINE operator const TArray<ArrayElementType>() const { return *reinterpret_cast<const TArray<ArrayElementType>*>(this); }

};

template<typename ArrayElementType>
class TIndirectArray
{
public:
	typedef TArray<void*> 			 InternalArrayType;

	TIndirectArray() = default;
	TIndirectArray(TIndirectArray&&) = default;

	FORCEINLINE int32 Num() const
	{
		return Array.Num();
	}

	FORCEINLINE ArrayElementType** GetData()
	{
		return (ArrayElementType**)Array.GetData();
	}

	FORCEINLINE const ArrayElementType** GetData() const
	{
		return (const ArrayElementType**)Array.GetData();
	}

	uint32 GetTypeSize() const
	{
		return sizeof(ArrayElementType*);
	}

	FORCEINLINE ArrayElementType& operator[](int32 Index)
	{
		return *(ArrayElementType*)Array[Index];
	}

	FORCEINLINE const ArrayElementType& operator[](int32 Index) const
	{
		return *(ArrayElementType*)Array[Index];
	}

	FORCEINLINE bool IsValidIndex(int32 Index) const
	{
		return Array.IsValidIndex(Index);
	}

	size_t GetAllocatedSize() const
	{
        return Array.Max() * GetTypeSize() + Array.Num() * sizeof(ArrayElementType);
	}

private:

	InternalArrayType Array;
};

class FFreedString : public TFreedArray<TCHAR>
{
public:
	using TFreedArray::TFreedArray;

	FFreedString()
	{
		Data = nullptr;
		NumElements = 0;
		MaxElements = 0;
	}

	FFreedString(const TCHAR* other)
	{
        const uint32 NullTerminatedLength = FCString::Strlen(other) + 1;

		*this = FFreedString(NullTerminatedLength);

		NumElements = NullTerminatedLength;
		memcpy(Data, other, NullTerminatedLength * sizeof(TCHAR));
	}

public:
	inline std::string ToString() const
	{
		if (*this)
		{
			std::u16string WData(Data);
			return std::string(WData.begin(), WData.end());
		}

		return "";
	}

	inline std::u16string ToWString() const
	{
		if (*this)
			return std::u16string(Data);

		return u"";
	}

    FORCEINLINE int32 Len() const
    {
        return Num() ? Num() - 1 : 0;
    }
    
    FORCEINLINE const TCHAR* operator*() const
    {
        return Num() ? Data : u"";
    }

public:
	inline       TCHAR* CStr()       { return Data; }
	inline const TCHAR* CStr() const { return Data; }

public:
	inline bool operator==(const FFreedString& Other) const { return Other ? NumElements == Other.NumElements && FCString::Strcmp(Data, Other.Data) == 0 : false; }
	inline bool operator!=(const FFreedString& Other) const { return Other ? NumElements != Other.NumElements || FCString::Strcmp(Data, Other.Data) != 0 : true; }
};


#pragma clang diagnostic pop

class FString : public TArray<TCHAR>
{
public:
	using TArray::TArray;

	
	FString(const TCHAR* other)
	{
        MaxElements = NumElements = *other ? FCString::Strlen(other) + 1 : 0;

		if (NumElements)
		{
			Data = const_cast<TCHAR*>(other);
		}
	}

public:
	inline std::string ToString() const
	{
		if (*this)
		{
			std::u16string WData(Data);
			return std::string(WData.begin(), WData.end());
		}

		return "";
	}

	inline std::u16string ToWString() const
	{
		if (*this)
			return std::u16string(Data);

		return u"";
	}
    
    inline bool IsEmpty() const
    {
        return this->Num() <= 1;
    }
    
    FORCEINLINE int32 Len() const
    {
        return Num() ? Num() - 1 : 0;
    }
    
    FORCEINLINE const TCHAR* operator*() const
    {
        return Num() ? Data : u"";
    }

public:
    inline       TCHAR* CStr()       { return Data; }
	inline const TCHAR* CStr() const { return Data; }

public:
	inline bool operator==(const FString& Other) const { return Other ? NumElements == Other.NumElements && FCString::Strcmp(Data, Other.Data) == 0 : false; }
	inline bool operator!=(const FString& Other) const { return Other ? NumElements != Other.NumElements || FCString::Strcmp(Data, Other.Data) != 0 : true; }
};

/*
* Class to allow construction of a TArray, that uses c-style standard-library memory allocation.
* 
* Useful for calling functions that expect a buffer of a certain size and do not reallocate that buffer.
* This avoids leaking memory, if the array would otherwise be allocated by the engine, and couldn't be freed without FMemory-functions.
*/
template<typename TAllocatedElementType>
class TAllocatedArray : public TArray<TAllocatedElementType>
{
public:
	TAllocatedArray() = delete;

public:
	TAllocatedArray(int32 Size)
	{
		this->Data = static_cast<TAllocatedElementType*>(malloc(Size * sizeof(TAllocatedElementType)));
		this->NumElements = 0x0;
		this->MaxElements = Size;
	}

	~TAllocatedArray()
	{
		if (this->Data)
			free(this->Data);

		this->NumElements = 0x0;
		this->MaxElements = 0x0;
	}
public:
	/* Adds to the array if there is still space for one more element */
	FORCEINLINE bool Add(const TAllocatedElementType& Element)
	{
		if (this->GetSlack() <= 0)
			return false;

		this->Data[this->NumElements] = Element;
		this->NumElements++;

		return true;
	}
    
    FORCEINLINE void AddUnique(const TAllocatedElementType& Element)
    {
        if (this->Find(Element))
            return;
        
        return Add(Element);
    }

	FORCEINLINE bool Remove(int32 Index)
	{
		if (!this->IsValidIndex(Index))
			return false;

		this->NumElements--;

		for (int i = Index; i < this->NumElements; i++)
		{
			/* NumElements was decremented, acessing i + 1 is safe */
			this->Data[i] = this->Data[i + 1];
		}

		return true;
	}

	FORCEINLINE void Clear()
	{
		this->NumElements = 0;

		if (!this->Data)
			memset(this->Data, 0, this->NumElements * this->ElementSize);
	}

public:
	inline operator       TArray<TAllocatedElementType>()       { return *reinterpret_cast<      TArray<TAllocatedElementType>*>(this); }
	inline operator const TArray<TAllocatedElementType>() const { return *reinterpret_cast<const TArray<TAllocatedElementType>*>(this); }
};

/*
* Class to allow construction of an FString, that uses c-style standard-library memory allocation.
*
* Useful for calling functions that expect a buffer of a certain size and do not reallocate that buffer.
* This avoids leaking memory, if the array would otherwise be allocated by the engine, and couldn't be freed without FMemory-functions.
*/
class FAllocatedString : public FString
{
public:
	FAllocatedString() = delete;

public:
	FAllocatedString(int32 Size)
	{
        Data = static_cast<TCHAR*>(malloc(Size * sizeof(TCHAR)));
		NumElements = 0x0;
		MaxElements = Size;
	}

	~FAllocatedString()
	{
		if (Data)
            free(Data);

		NumElements = 0x0;
		MaxElements = 0x0;
	}

public:
	/*inline operator       FString()       { return *reinterpret_cast<      FString*>(this); }
	inline operator const FString() const { return *reinterpret_cast<const FString*>(this); }*/
};

struct FSparseArrayAllocationInfo
{
    int32 Index;
    void* Pointer;
};

template<typename SparseArrayElementType>
class TSparseArray
{
private:
	static constexpr uint32 ElementAlign = alignof(SparseArrayElementType);
	static constexpr uint32 ElementSize = sizeof(SparseArrayElementType);

private:
	using FElementOrFreeListLink = ContainerImpl::TSparseArrayElementOrFreeListLink<ContainerImpl::TAlignedBytes<ElementSize, ElementAlign>>;

private:
	TArray<FElementOrFreeListLink> Data;
	ContainerImpl::FBitArray AllocationFlags;
	int32 FirstFreeIndex;
	int32 NumFreeIndices;

public:
	TSparseArray()
		: FirstFreeIndex(-1), NumFreeIndices(0)
	{
	}

	TSparseArray(TSparseArray&&) = default;
	TSparseArray(const TSparseArray&) = default;

public:
	TSparseArray& operator=(TSparseArray&&) = default;
	TSparseArray& operator=(const TSparseArray&) = default;

private:
	inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

public:
	inline int32 NumAllocated() const { return Data.Num(); }

	inline int32 Num() const { return NumAllocated() - NumFreeIndices; }
	inline int32 Max() const { return Data.Max(); }
    
    int32 GetMaxIndex() const
    {
        return Data.Num();
    }
    
    bool IsAllocated(int32 Index) const { return AllocationFlags[Index]; }

	inline bool IsValidIndex(int32 Index) const { return Data.IsValidIndex(Index) && AllocationFlags[Index]; }

	inline bool IsValid() const { return Data.IsValid() && AllocationFlags.IsValid(); }
    void* GetData() const { return Data.GetData(); }
    

public:
	const ContainerImpl::FBitArray& GetAllocationFlags() const { return AllocationFlags; }

public:
	inline       SparseArrayElementType& operator[](int32 Index)       { VerifyIndex(Index); return *reinterpret_cast<SparseArrayElementType*>(&Data.GetUnsafe(Index).ElementData); }
	inline const SparseArrayElementType& operator[](int32 Index) const { VerifyIndex(Index); return *reinterpret_cast<const SparseArrayElementType*>(&Data.GetUnsafe(Index).ElementData); }

	inline bool operator==(const TSparseArray<SparseArrayElementType>& Other) const { return Data == Other.Data; }
	inline bool operator!=(const TSparseArray<SparseArrayElementType>& Other) const { return Data != Other.Data; }

public:
	template<typename T> friend Iterators::TSparseArrayIterator<T> begin(const TSparseArray& Array);
	template<typename T> friend Iterators::TSparseArrayIterator<T> end  (const TSparseArray& Array);
};


template<typename SetElementType>
class TSet
{
private:
	static constexpr uint32 ElementAlign = alignof(SetElementType);
	static constexpr uint32 ElementSize = sizeof(SetElementType);

private:
	using SetDataType = ContainerImpl::SetElement<SetElementType>;
	using HashType = ContainerImpl::TInlineAllocator<1>::ForElementType<int32>;
    using FSetElementId = int32;

private:
	TSparseArray<SetDataType> Elements;
	HashType Hash;
	int32 HashSize;

public:
	TSet()
		: HashSize(0)
	{
	}

	TSet(TSet&&) = default;
	TSet(const TSet&) = default;

public:
	TSet& operator=(TSet&&) = default;
	TSet& operator=(const TSet&) = default;

private:
	inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

public:
	inline int32 NumAllocated() const { return Elements.NumAllocated(); }

	inline int32 Num() const { return Elements.Num(); }
	inline int32 Max() const { return Elements.Max(); }

	inline bool IsValidIndex(int32 Index) const { return Elements.IsValidIndex(Index); }

	inline bool IsValid() const { return Elements.IsValid(); }
    void* GetData() const { return Elements.GetData(); }

public:
	const ContainerImpl::FBitArray& GetAllocationFlags() const { return Elements.GetAllocationFlags(); }

public:
	inline       SetElementType& operator[] (int32 Index)       { return Elements[Index].Value; }
	inline const SetElementType& operator[] (int32 Index) const { return Elements[Index].Value; }

	inline bool operator==(const TSet<SetElementType>& Other) const { return Elements == Other.Elements; }
	inline bool operator!=(const TSet<SetElementType>& Other) const { return Elements != Other.Elements; }

public:
	template<typename T> friend Iterators::TSetIterator<T> begin(const TSet& Set);
	template<typename T> friend Iterators::TSetIterator<T> end  (const TSet& Set);
};

template<typename KeyElementType, typename ValueElementType>
class TMap
{
public:
	using ElementType = TPair<KeyElementType, ValueElementType>;

private:
	TSet<ElementType> Elements;

private:
	inline void VerifyIndex(int32 Index) const { if (!IsValidIndex(Index)) throw std::out_of_range("Index was out of range!"); }

public:
	inline int32 NumAllocated() const { return Elements.NumAllocated(); }

	inline int32 Num() const { return Elements.Num(); }
	inline int32 Max() const { return Elements.Max(); }

	inline bool IsValidIndex(int32 Index) const { return Elements.IsValidIndex(Index); }

	inline bool IsValid() const { return Elements.IsValid(); }
    
    void* GetData() const { return Elements.GetData(); }

public:
	const ContainerImpl::FBitArray& GetAllocationFlags() const { return Elements.GetAllocationFlags(); }

public:
	inline ValueElementType Find(const KeyElementType& Key, bool(*Equals)(const KeyElementType& LeftKey, const KeyElementType& RightKey) = [](auto R, auto L){ return R == L; }) const
	{
		for (auto It = begin(*this); It != end(*this); ++It)
		{
			if (Equals(It->Key(), Key))
				return It->Value();
		}
	
		return ValueElementType{};
	}

public:
	inline       ElementType& operator[] (int32 Index)       { return Elements[Index]; }
	inline const ElementType& operator[] (int32 Index) const { return Elements[Index]; }

	inline bool operator==(const TMap<KeyElementType, ValueElementType>& Other) const { return Elements == Other.Elements; }
	inline bool operator!=(const TMap<KeyElementType, ValueElementType>& Other) const { return Elements != Other.Elements; }

public:
	template<typename KeyType, typename ValueType> friend Iterators::TMapIterator<KeyType, ValueType> begin(const TMap& Map);
	template<typename KeyType, typename ValueType> friend Iterators::TMapIterator<KeyType, ValueType> end  (const TMap& Map);
};

namespace Iterators
{
	class FRelativeBitReference
	{
	protected:
		static constexpr int32 NumBitsPerDWORD = 32;
		static constexpr int32 NumBitsPerDWORDLogTwo = 5;

	public:
		inline explicit FRelativeBitReference(int32 BitIndex)
			: WordIndex(BitIndex >> NumBitsPerDWORDLogTwo)
			, Mask(1 << (BitIndex & (NumBitsPerDWORD - 1)))
		{
		}

		int32  WordIndex;
		uint32 Mask;
	};

	class FSetBitIterator : public FRelativeBitReference
	{
	private:
		const ContainerImpl::FBitArray& Array;

		uint32 UnvisitedBitMask;
		int32 CurrentBitIndex;
		int32 BaseBitIndex;

	public:
		explicit FSetBitIterator(const ContainerImpl::FBitArray& InArray, int32 StartIndex = 0)
			: FRelativeBitReference(StartIndex)
			, Array(InArray)
			, UnvisitedBitMask((~0U) << (StartIndex & (NumBitsPerDWORD - 1)))
			, CurrentBitIndex(StartIndex)
			, BaseBitIndex(StartIndex & ~(NumBitsPerDWORD - 1))
		{
			if (StartIndex != Array.Num())
				FindFirstSetBit();
		}

	public:
		inline FSetBitIterator& operator++()
		{
			UnvisitedBitMask &= ~this->Mask;

			FindFirstSetBit();

			return *this;
		}

		inline FSetBitIterator& operator--()
		{
			UnvisitedBitMask &= ~this->Mask;

			FindLastSetBit();

			return *this;
		}

		inline explicit operator bool() const { return CurrentBitIndex < Array.Num(); }

		inline bool operator==(const FSetBitIterator& Rhs) const { return CurrentBitIndex == Rhs.CurrentBitIndex && &Array == &Rhs.Array; }
		inline bool operator!=(const FSetBitIterator& Rhs) const { return CurrentBitIndex != Rhs.CurrentBitIndex || &Array != &Rhs.Array; }

	public:
		inline int32 GetIndex() { return CurrentBitIndex; }

		void FindFirstSetBit()
		{
			const uint32* ArrayData = Array.GetData();
			const int32   ArrayNum = Array.Num();
			const int32   LastWordIndex = (ArrayNum - 1) / NumBitsPerDWORD;

			uint32 RemainingBitMask = ArrayData[this->WordIndex] & UnvisitedBitMask;
			while (!RemainingBitMask)
			{
				++this->WordIndex;
				BaseBitIndex += NumBitsPerDWORD;
				if (this->WordIndex > LastWordIndex)
				{
					CurrentBitIndex = ArrayNum;
					return;
				}

				RemainingBitMask = ArrayData[this->WordIndex];
				UnvisitedBitMask = ~0;
			}

			const uint32 NewRemainingBitMask = RemainingBitMask & (RemainingBitMask - 1);

			this->Mask = NewRemainingBitMask ^ RemainingBitMask;

			CurrentBitIndex = BaseBitIndex + NumBitsPerDWORD - 1 - ContainerImpl::HelperFunctions::CountLeadingZeros(this->Mask);

			if (CurrentBitIndex > ArrayNum)
				CurrentBitIndex = ArrayNum;
		}

		void FindLastSetBit()
		{
			const uint32* ArrayData     = Array.GetData();
			const int32   ArrayNum      = Array.Num();
			const int32   LastWordIndex = (ArrayNum - 1) / NumBitsPerDWORD;

			uint32 RemainingBitMask = ArrayData[this->WordIndex] & UnvisitedBitMask;
			while (!RemainingBitMask) {
				--this->WordIndex;
				BaseBitIndex -= NumBitsPerDWORD;
				if (this->WordIndex < 0) {
					CurrentBitIndex = 0;
					return;
				}

				RemainingBitMask = ArrayData[this->WordIndex];
				UnvisitedBitMask = ~0;
			}

			const uint32 NewRemainingBitMask = RemainingBitMask & (RemainingBitMask + 1);

			this->Mask = RemainingBitMask - NewRemainingBitMask;

			CurrentBitIndex = BaseBitIndex + 1 + ContainerImpl::HelperFunctions::CountTrailingZeros(this->Mask);

			if (CurrentBitIndex < 0)
				CurrentBitIndex = 0;
		}
	};

	template<typename ArrayType>
	class TArrayIterator
	{
	private:
		TArray<ArrayType>& IteratedArray;
		int32 Index;

	public:
		TArrayIterator(const TArray<ArrayType>& Array, int32 StartIndex = 0x0)
			: IteratedArray(const_cast<TArray<ArrayType>&>(Array)), Index(StartIndex)
		{
		}

	public:
		inline int32 GetIndex() { return Index; }

		inline int32 IsValid() { return IteratedArray.IsValidIndex(GetIndex()); }

	public:
		inline TArrayIterator& operator++() { ++Index; return *this; }
		inline TArrayIterator& operator--() { --Index; return *this; }

		inline       ArrayType& operator*()       { return IteratedArray[GetIndex()]; }
		inline const ArrayType& operator*() const { return IteratedArray[GetIndex()]; }

		inline       ArrayType* operator->()       { return &IteratedArray[GetIndex()]; }
		inline const ArrayType* operator->() const { return &IteratedArray[GetIndex()]; }

		inline bool operator==(const TArrayIterator& Other) const { return &IteratedArray == &Other.IteratedArray && Index == Other.Index; }
		inline bool operator!=(const TArrayIterator& Other) const { return &IteratedArray != &Other.IteratedArray || Index != Other.Index; }
	};

	template<class ContainerType>
	class TContainerIterator
	{
	private:
		ContainerType& IteratedContainer;
		FSetBitIterator BitIterator;

	public:
		TContainerIterator(const ContainerType& Container, const ContainerImpl::FBitArray& BitArray, int32 StartIndex = 0x0)
			: IteratedContainer(const_cast<ContainerType&>(Container)), BitIterator(BitArray, StartIndex)
		{
		}

	public:
		inline int32 GetIndex() { return BitIterator.GetIndex(); }

		inline int32 IsValid() { return IteratedContainer.IsValidIndex(GetIndex()); }

	public:
		inline TContainerIterator& operator++() { ++BitIterator; return *this; }
		inline TContainerIterator& operator--() { --BitIterator; return *this; }

		inline       auto& operator*()       { return IteratedContainer[GetIndex()]; }
		inline const auto& operator*() const { return IteratedContainer[GetIndex()]; }

		inline       auto* operator->()       { return &IteratedContainer[GetIndex()]; }
		inline const auto* operator->() const { return &IteratedContainer[GetIndex()]; }

		inline bool operator==(const TContainerIterator& Other) const { return &IteratedContainer == &Other.IteratedContainer && BitIterator == Other.BitIterator; }
		inline bool operator!=(const TContainerIterator& Other) const { return &IteratedContainer != &Other.IteratedContainer || BitIterator != Other.BitIterator; }
	};
}

inline Iterators::FSetBitIterator begin(const ContainerImpl::FBitArray& Array) { return Iterators::FSetBitIterator(Array, 0); }
inline Iterators::FSetBitIterator end  (const ContainerImpl::FBitArray& Array) { return Iterators::FSetBitIterator(Array, Array.Num()); }

template<typename T> inline Iterators::TArrayIterator<T> begin(const TArray<T>& Array) { return Iterators::TArrayIterator<T>(Array, 0); }
template<typename T> inline Iterators::TArrayIterator<T> end  (const TArray<T>& Array) { return Iterators::TArrayIterator<T>(Array, Array.Num()); }

template<typename T> inline Iterators::TSparseArrayIterator<T> begin(const TSparseArray<T>& Array) { return Iterators::TSparseArrayIterator<T>(Array, Array.GetAllocationFlags(), 0); }
template<typename T> inline Iterators::TSparseArrayIterator<T> end  (const TSparseArray<T>& Array) { return Iterators::TSparseArrayIterator<T>(Array, Array.GetAllocationFlags(), Array.NumAllocated()); }

template<typename T> inline Iterators::TSetIterator<T> begin(const TSet<T>& Set) { return Iterators::TSetIterator<T>(Set, Set.GetAllocationFlags(), 0); }
template<typename T> inline Iterators::TSetIterator<T> end  (const TSet<T>& Set) { return Iterators::TSetIterator<T>(Set, Set.GetAllocationFlags(), Set.NumAllocated()); }

template<typename T0, typename T1> inline Iterators::TMapIterator<T0, T1> begin(const TMap<T0, T1>& Map) { return Iterators::TMapIterator<T0, T1>(Map, Map.GetAllocationFlags(), 0); }
template<typename T0, typename T1> inline Iterators::TMapIterator<T0, T1> end  (const TMap<T0, T1>& Map) { return Iterators::TMapIterator<T0, T1>(Map, Map.GetAllocationFlags(), Map.NumAllocated()); }

static_assert(sizeof(TArray<int32>) == 0x10, "TArray has a wrong size!");
static_assert(sizeof(TSet<int32>) == 0x50, "TSet has a wrong size!");
static_assert(sizeof(TMap<int32, int32>) == 0x50, "TMap has a wrong size!");

