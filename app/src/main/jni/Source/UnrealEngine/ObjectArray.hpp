#pragma once

#include "Containers.hpp"
#include "ScriptCore.h"

struct FUObjectItem final
{
public:
	class UObject* Object;
	EInternalObjectFlags Flags;
	int32 ClusterIndex; 
	int32 SerialNumber; 
	uint8 Pad_14[0x4]; 

public:
	bool IsUnreachable() const
	{
	    return !!(Flags & EInternalObjectFlags::Unreachable);
	}

	bool IsPendingKill() const
	{
	    return !!(Flags & EInternalObjectFlags::PendingKill);
	}

    FORCEINLINE void SetRootSet()
    {
        ThisThreadAtomicallySetFlag(EInternalObjectFlags::RootSet);
    }

    FORCEINLINE bool ThisThreadAtomicallySetFlag(EInternalObjectFlags FlagToSet) const
    {
        bool bIChangedIt = false;
        while (1)
        {
            int32 StartValue = int32(Flags);
            if (StartValue & int32(FlagToSet))
            {
                break;
            }
            int32 NewValue = StartValue | int32(FlagToSet);
            if ((int32)__sync_val_compare_and_swap((int32*)&Flags, NewValue, StartValue) == StartValue)
            {
                bIChangedIt = true;
                break;
            }
        }
        return bIChangedIt;
    }
};

class TUObjectArray
{
public:
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32 MaxElements;
	int32 NumElements;
	int32 MaxChunks;
	int32 NumChunks;

public:
	inline int32 Num() const
    {
        return NumElements;
    }

    inline int32 Capacity() const
    {
        return MaxElements;
    }

    inline bool IsValidIndex(int32 Index) const
	{
		return Index < Num() && Index >= 0;
	}

    inline FUObjectItem const* IndexToObject(int32 Index) const
    {
        const int32 ChunkIndex = Index / NumElementsPerChunk;
        const int32 WithinChunkIndex = Index % NumElementsPerChunk;

        if ( !IsValidIndex(Index) ) 
            return nullptr;

        if (ChunkIndex > NumChunks)
            return nullptr;

        if (Index > Capacity())
            return nullptr;

        FUObjectItem* Chunk = Objects[ChunkIndex];
        if ( !Chunk )
            return nullptr;

        return reinterpret_cast<FUObjectItem*>(Chunk + WithinChunkIndex);
    }

    inline UObject* operator[](int32 Index) const
    {
        FUObjectItem const* ObjectItem = IndexToObject(Index);
        if ( ObjectItem )
        {
            return ObjectItem->Object;
        }
        return nullptr;
    }
    
    FORCEINLINE bool IsValid(FUObjectItem const* ObjectItem, bool bEvenIfPendingKill)
    {
        if (ObjectItem)
        {
            return bEvenIfPendingKill ? !ObjectItem->IsUnreachable() : !(ObjectItem->IsUnreachable() || ObjectItem->IsPendingKill());
        }
        return false;
    }
    
    FORCEINLINE bool IsValid(int32 Index, bool bEvenIfPendingKill)
    {
        FUObjectItem const* ObjectItem = IndexToObject(Index);
        if (ObjectItem)
        {
            return IsValid(ObjectItem, bEvenIfPendingKill);
        }
        return false;
    }
};


class FUObjectArray 
{
public:
    int32 ObjFirstGCIndex;
	int32 ObjLastNonGCIndex;
	int32 MaxObjectsNotConsideredByGC;
	bool OpenForDisregardForGC;
	TUObjectArray ObjObjects;

public:
    
    class TIterator
	{
	public:
		enum EEndTagType
		{
			EndTag
		};

		TIterator( const FUObjectArray& InArray, bool bOnlyGCedObjects = false ) :	
			Array(InArray),
			Index(-1),
			CurrentObject(nullptr)
		{
			if (bOnlyGCedObjects)
			{
				Index = Array.ObjLastNonGCIndex;
			}
			Advance();
		}

		TIterator( EEndTagType, const TIterator& InIter ) :	
			Array (InIter.Array),
			Index(Array.ObjObjects.Num())
		{
		}

		FORCEINLINE void operator++()
		{
			Advance();
		}

		friend bool operator==(const TIterator& Lhs, const TIterator& Rhs) { return Lhs.Index == Rhs.Index; }
		friend bool operator!=(const TIterator& Lhs, const TIterator& Rhs) { return Lhs.Index != Rhs.Index; }

		FORCEINLINE explicit operator bool() const
		{ 
			return !!CurrentObject;
		}

		FORCEINLINE bool operator !() const 
		{
			return !(bool)*this;
		}

        FORCEINLINE UObject* operator*() const
        {
            return GetObject();
        }

		FORCEINLINE int32 GetIndex() const
		{
			return Index;
		}

	protected:

		FORCEINLINE UObject* GetObject() const
		{ 
			return CurrentObject;
		}

		FORCEINLINE bool Advance()
		{
			UObject* NextObject = nullptr;
			CurrentObject = nullptr;
			while(++Index < Array.ObjObjects.Num())
			{
				NextObject = Array.ObjObjects[Index];
				if (NextObject)
				{
					CurrentObject = NextObject;
					return true;
				}
			}
			return false;
		}
	private:
		/** the array that we are iterating on, probably always GUObjectArray */
		const FUObjectArray& Array;
		/** index of the current element in the object array */
		int32 Index;
		/** Current object */
		mutable UObject* CurrentObject;
	};

public:

    TIterator begin() const { return TIterator(*this, false); }
    TIterator end()   const { return TIterator(TIterator::EndTag, begin()); }
};




