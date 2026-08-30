#pragma once

#include "Enums.hpp"
#include "NameTypes.hpp"
#include "CommonTypes.hpp"
#include "ObjectArray.hpp"

#include <__config>
#include <unordered_map>

namespace EIncludeSuperFlag
{
    enum Type
    {
        ExcludeSuper,
        IncludeSuper
    };
}

struct FImplementedInterface
{
    class UClass* Class;
    int32 PointerOffset;
    bool bImplementedByK2;

    FImplementedInterface()
        : Class(nullptr)
        , PointerOffset(0)
        , bImplementedByK2(false)
    {}
    FImplementedInterface(UClass* InClass, int32 InOffset, bool InImplementedByK2)
        : Class(InClass)
        , PointerOffset(InOffset)
        , bImplementedByK2(InImplementedByK2)
    {}
};

struct FOutputDevice
{
    void** VTable;
    /** Whether to output the 'Log: ' type front... */
    bool bSuppressEventTag;
    /** Whether to output a line-terminator after each log call... */
    bool bAutoEmitLineTerminator;
};

struct FOutParmRec
{
    class FProperty* Property;
    uint8*      PropAddr;
    FOutParmRec* NextOutParm;
};

struct FFrame : public FOutputDevice
{
public:
    // Variables.
    class UFunction* Node;
    class UObject* Object;
    uint8* Code;
    uint8* Locals;
    
    class FProperty* MostRecentProperty;
    uint8* MostRecentPropertyAddress;
    
    /** The execution flow stack for compiled Kismet code */
    TArray<void*> FlowStack;
    
    uint8 Padding[0x20];
    
    /** Previous frame on the stack */
    FFrame* PreviousFrame;
    
    /** contains information on any out parameters */
    FOutParmRec* OutParms;
    
    /** If a class is compiled in then this is set to the property chain for compiled-in functions. In that case, we follow the links to setup the args instead of executing by code. */
    class FField* PropertyChainForCompiledIn;
    
    /** Currently executed native function */
    class UFunction* CurrentNativeFunction;
    
    bool bArrayContextFailed;
};


class FFieldClass
{
public:
    FName Name;					  
    uint64 Id;					 
    EClassCastFlags CastFlags;			 
    EClassFlags ClassFlags;		  
    uint8 Pad_1C[0x4];			 
    class FFieldClass *SuperClass; 

    bool IsType(EClassCastFlags Flags) const
    {
        return (Flags != EClassCastFlags::None ? (CastFlags & Flags) : true);
    }
};

class FFieldVariant
{
public:
    using ContainerType = union
    {
        class FField *Field;
        class UObject* Object;
    }; 

    ContainerType Container; 
    bool bIsUObject;
};

class FField
{
public:
    void** VTable;				
    class FFieldClass *ClassPrivate; 
    FFieldVariant Owner;			
    class FField *Next;				
    FName Name;						
    int32 ObjFlags;		

    bool IsA(EClassCastFlags Flags) const
    {
        return (Flags != EClassCastFlags::None ? ClassPrivate->IsType(Flags) : true);
    }		
};

struct FProperty : public FField
{
public:
    int32 ArrayDim;		  
    int32 ElementSize;	 
    uint8 Pad_3C[0x4];	 
    EPropertyFlags PropertyFlags; 
    uint8 Pad_48[0x4];
    int32 Offset_Internal;
    uint8 Pad_50[0x28];
    
    template <typename ValueType>
    ValueType* ContainerPtrToValuePtr(void* Container) const
    {
        return reinterpret_cast<ValueType*>(Container) + Offset_Internal;
    }
};

struct FStructProperty : public FProperty
{
    class UScriptStruct* Struct;

    FStructProperty* GetStruct(const std::string& Name) const;
    FProperty* GetPropertyPtr(const std::string& Name) const;
    int32 GetOffset(const std::string& Name, uint8* OutBit = nullptr) const;
};


struct FBoolProperty : public FProperty
{
	uint8 FieldSize;
	uint8 ByteOffset;
	uint8 ByteMask;
	uint8 FieldMask;
};

class UObject
{
public:
    
    static inline FUObjectArray* GUObjectArray = nullptr;
    
    void**          VTable;
    EObjectFlags    ObjectFlags;
    int32           InternalIndex;
    class UClass*   ClassPrivate;
    class FName     NamePrivate;
    class UObject*  OuterPrivate;
    
public:
    
    std::string GetFullName() const;
    std::string GetName() const;
    bool HasTypeFlag(EClassCastFlags TypeFlags) const;
    bool IsA(EClassCastFlags TypeFlags) const;
    bool IsA(class UClass* TypeClass) const;
    bool IsA(const class FClassData& Data) const;
    bool IsA(FName TypeName) const;
    bool IsDefaultObject() const;
    
    FName GetClassName() const;
    class UFunction* FindFunction(const std::string& InName, bool IncludeSuper = true) const;
    class UFunction* FindFunction(FName InName, EIncludeSuperFlag::Type IncludeSuper = EIncludeSuperFlag::IncludeSuper) const;
    UObject* GetDefault() const;
    UObject* GetTypedOuter(class UClass* Target) const;
    UObject* GetTypedOuter(EClassCastFlags Target) const;
    
    void* GetInterfaceAddress( UClass* InterfaceClass ) const;
    
    FORCEINLINE EObjectFlags GetFlags() const { return ObjectFlags; }
    FORCEINLINE int32 GetIndex() const { return InternalIndex; }
    FORCEINLINE UObject* GetOuter() const { return OuterPrivate; }
    FORCEINLINE class UClass* GetClass() const { return ClassPrivate; }
    
    bool HasAnyFlags(EObjectFlags TypeFlags) const { return ObjectFlags & TypeFlags; };
    
public:
    
    static FUObjectArray& GetObjectArray()
    {
        return *GUObjectArray;
    }

    FORCEINLINE bool IsPendingKill() const
    {
        FUObjectItem const* Item = GUObjectArray->ObjObjects.IndexToObject(InternalIndex);
        if (Item)
        {
            return Item->IsPendingKill();
        }
        return true;
    }

    FORCEINLINE bool IsUnreachable() const
    {
        FUObjectItem const* Item = GUObjectArray->ObjObjects.IndexToObject(InternalIndex);
        if (Item)
        {
            return Item->IsUnreachable();
        }
        return true;
    }

	void ProcessEvent(class UFunction* Function, void* Parms) const
    {
        if ( !Function )
            return;

        reinterpret_cast<void(*)(const UObject*, UFunction*, void*)>(VTable[69])(this, Function, Parms);
    }

	static class UClass* FindClass(const std::string& ClassFullName)
	{
		return FindObject<UClass>(ClassFullName, EClassCastFlags::Class);
	}

	static class UClass* FindClassFast(const std::string& ClassName)
	{
		return FindObjectFast<UClass>(ClassName, EClassCastFlags::Class);
	}

    static class UClass* FindStaticClass(const std::string& ClassName)
    {
        std::string StaticClassName = ClassName.substr(1);
        return FindObjectFast<UClass>(StaticClassName, EClassCastFlags::Class);
    }

    static class UScriptStruct* FindScriptStruct(const std::string& ClassFullName)
    {
        return FindObject<UScriptStruct>(ClassFullName, EClassCastFlags::ScriptStruct);
    }

    static class UScriptStruct* FindScriptStructFast(const std::string& ClassName)
    {
        return FindObjectFast<UScriptStruct>(ClassName, EClassCastFlags::ScriptStruct);
    }
	
	template<typename T = UObject>
	static T* FindObject(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None)
	{
		for (UObject* Object : GetObjectArray())
        {
            if (Object->HasTypeFlag(RequiredType) && Object->GetFullName() == Name)
            {
                return static_cast<T*>(Object);
            }
        }
        return nullptr;
	}

	template<typename T = UObject>
	static T* FindObjectFast(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None)
	{
        for (UObject* Object : GetObjectArray())
        {
            if (Object->HasTypeFlag(RequiredType) && Object->GetName() == Name)
            {
                return static_cast<T*>(Object);
            }
        }
        return nullptr;
	}

    template<typename Type>
    FORCEINLINE Type* GetMemberByOffset(const uint32 Offset) const
    {
        return (Type*)((uint8*)this + Offset);
    }

    static std::unordered_map<UClass*, std::unordered_map<std::string, int32>> OffsetsMap;

    int32 GetOffset(const std::string& Name, uint8* OutBit = nullptr) const;

    template<typename Type>
    Type* GetMember(const std::string& Name) const
    {
        if (ClassPrivate)
        {
            auto& ClassOffsets = OffsetsMap[ClassPrivate];
            if (auto It = ClassOffsets.find(Name); It != ClassOffsets.end() && It->second != 0)
            {
                return GetMemberByOffset<Type>(It->second);
            }

            int32 Offset = GetOffset(Name);
            ClassOffsets[Name] = Offset;
            return GetMemberByOffset<Type>(Offset);
        }
        return nullptr;
    }

    static std::unordered_map<UClass*, std::unordered_map<std::string, std::pair<int32, uint8>>> BitOffsetsMap;

    bool GetBitMember(const std::string& Name) const
    {
        if (ClassPrivate)
        {
            auto& ClassOffsets = BitOffsetsMap[ClassPrivate];
            if (auto It = ClassOffsets.find(Name); It != ClassOffsets.end() && It->second.first != 0)
            {
                return *(uint8*)((uint8*)this + It->second.first) & It->second.second;
            }

            uint8 OutBit;
            int32 Offset = GetOffset(Name, &OutBit);
            ClassOffsets[Name] = std::make_pair(Offset, OutBit);

            return *(uint8*)((uint8*)this + Offset) & OutBit;
        }
        return false;
    }

    void SetBitMember(const std::string& Name, bool Value) const 
    {
        if (ClassPrivate)
        {
            auto& ClassOffsets = BitOffsetsMap[ClassPrivate];
            if (auto It = ClassOffsets.find(Name); It != ClassOffsets.end() && It->second.first != 0)
            {
                uint8* BitFlagAddr = (uint8*)((uint8*)this + It->second.first);
                if (Value)
                    *BitFlagAddr |= It->second.second;
                else 
                    *BitFlagAddr &= ~It->second.second;
            }

            uint8 OutBit;
            int32 Offset = GetOffset(Name, &OutBit);
            ClassOffsets[Name] = std::make_pair(Offset, OutBit);

            uint8* NewBitFlagAddr = (uint8*)((uint8*)this + Offset);
            if (Value)
                *NewBitFlagAddr |= OutBit;
            else 
                *NewBitFlagAddr &= OutBit;
        }
    }


    static void Init(uintptr Address)
    {
        GUObjectArray = reinterpret_cast<FUObjectArray*>(Address);
    }

};


class UField : public UObject
{
public:
	class UField* Next;
};

class UProperty : public UField
{
public:
	uint8 Pad_35[0x40];
};

class FStructBaseChain
{
protected:
	FORCEINLINE bool IsChildOfUsingStructArray(const FStructBaseChain& Parent) const
	{
		int32 NumParentStructBasesInChainMinusOne = Parent.NumStructBasesInChainMinusOne;
		return NumParentStructBasesInChainMinusOne <= NumStructBasesInChainMinusOne && StructBaseChainArray[NumParentStructBasesInChainMinusOne] == &Parent;
	}
    
    FORCEINLINE bool IsChildOfUsingStructArray(const FName& ParentName, int32 ParentDepth) const
    {
        return ParentDepth <= NumStructBasesInChainMinusOne && ((UObject*)StructBaseChainArray[ParentDepth])->NamePrivate == ParentName;
    }
public:
    
    FORCEINLINE int32 GetNumStructBasesInChain() const
    {
        return NumStructBasesInChainMinusOne;
    }

private:
	FStructBaseChain** StructBaseChainArray;
	int32 NumStructBasesInChainMinusOne;

	friend class UStruct;
};

class UStruct : public UField, private FStructBaseChain
{
public:
	class UStruct*  SuperStruct;  
	class UField*   Children;   
	class FField*   ChildProperties;  
	int32           Size;             
	int32           MinAlignemnt;  
	uint8           Pad_38[0x50]; 

    FORCEINLINE bool IsChildOf(const UStruct* SomeBase) const
    {
        return FStructBaseChain::IsChildOfUsingStructArray(*SomeBase);
    }
    
    FORCEINLINE bool IsChildOf(const FName& SomeBaseName, int32 SomeBaseDepth) const
    {
        return FStructBaseChain::IsChildOfUsingStructArray(SomeBaseName, SomeBaseDepth);
    }
    
    FORCEINLINE int32 GetDepth() const
    {
        return GetNumStructBasesInChain();
    }

    int32 GetOffset(const std::string& Name, uint8* OutBit = nullptr)
    {
        FProperty* FoundPropetry = GetPropertyPtr(Name);
        if (FoundPropetry)
        {
            if (OutBit && FoundPropetry->IsA(EClassCastFlags::BoolProperty))
                *OutBit = static_cast<FBoolProperty*>(FoundPropetry)->ByteMask;

            return FoundPropetry->Offset_Internal;
        }
        return 0;
    }

    FProperty* GetPropertyPtr(const std::string& Name)
    {
        for (UStruct* Super = this; Super; Super = Super->SuperStruct)
        {
            for (FField* Field = Super->ChildProperties; Field; Field = Field->Next)
            {
                if (Field->Name.ToString() == Name)
                {
                    return static_cast<FProperty*>(Field);
                }
            }
        }
        return nullptr;
    }

    FStructProperty* GetStruct(const std::string& Name)
    {
        for (UStruct* Super = this; Super; Super = Super->SuperStruct)
        {
            for (FField* Field = Super->ChildProperties; Field; Field = Field->Next)
            {
                if (Field->IsA(EClassCastFlags::StructProperty) && Field->Name.ToString() == Name)
                {
                    return static_cast<FStructProperty*>(Field);
                }
            }
        }
        return nullptr;
    }
};



class UFunction : public UStruct
{
public:
    using Native = void (*)(UObject* Context, struct FFrame& TheStack, void* Result);

	EFunctionFlags FunctionFlags;
    uint8          NumParms;
    uint16         ParmsSize;
    uint16         ReturnValueOffset;
	uint8          Pad_42[0x18];
    Native         Func;
    
    FORCEINLINE bool HasAnyFunctionFlags( EFunctionFlags FlagsToCheck ) const
    {
        return (FunctionFlags&FlagsToCheck) || FlagsToCheck == EFunctionFlags::AllFlags;
    }
    
    void Invoke(UObject* Obj, FFrame& Stack, void* RESULT_PARAM);
};


class UClass : public UStruct
{
public:
	uint8            Pad_3D[0x24];
    EClassFlags      ClassFlags;
	EClassCastFlags  ClassCastFlags;
	uint8            Pad_3E[0x40];       
	class UObject*   ClassDefaultObject;
    uint8            Pad[0x10];
    TMap<FName, UFunction*> FuncMap;
    mutable TMap<FName, UFunction*> SuperFuncMap;
    mutable pthread_rwlock_t SuperFuncMapLock;
    TArray<FImplementedInterface> Interfaces;

public:
    UFunction* FindFunctionByName(FName InName, EIncludeSuperFlag::Type IncludeSuper = EIncludeSuperFlag::IncludeSuper) const;
    
	class UFunction* GetFunction(const std::string& ClassName, const std::string& FuncName) const;
    bool ImplementsInterface(const class UClass* SomeInterface) const;
    
    bool HasAnyClassFlags(EClassFlags Types) const
    {
        return ClassFlags & Types;
    }
    
    UClass* GetSuperClass() const
    {
        return (UClass*)SuperStruct;
    }

    UObject* GetDefaultObj() const
    {
        return ClassDefaultObject;
    }
};

class UScriptStruct : public UStruct
{
public:

};


class FClassData
{
public:
    int32 GetDepth() const
    {
        return NumStructBases;
    }
    
    FName GetName() const
    {
        return ClassName;
    }
 
    void Get(const TCHAR* Path);
    
    operator bool() const
    {
        return ClassName != 0 && NumStructBases;
    }
    
private:
    FName ClassName;
    int32 NumStructBases;
};

class FWeakObjectPtr
{
public:
    int32 ObjectIndex;
    int32 ObjectSerialNumber; 

public:
    class UObject *Get() const;
    class UObject* GetSafe() const;
    class UObject *operator->() const;
    bool operator==(const FWeakObjectPtr &Other) const;
    bool operator!=(const FWeakObjectPtr &Other) const;
    bool operator==(const class UObject *Other) const;
    bool operator!=(const class UObject *Other) const;
    bool IsValid() const;
    bool SerialNumbersMatch(const FUObjectItem *ObjectItem) const;
};

template <typename T>
class TWeakObjectPtr : public FWeakObjectPtr
{
public:
    T *Get() const
    {
        return static_cast<T *>(FWeakObjectPtr::Get());
    }

    T *GetSafe() const
    {
        return static_cast<T *>(FWeakObjectPtr::GetSafe());
    }

    T *operator->() const
    {
        return static_cast<T *>(FWeakObjectPtr::Get());
    }

    inline bool IsValid() const
    {
        return FWeakObjectPtr::IsValid();
    }

    TWeakObjectPtr<T>& operator=(const UObject* Object)
    {
        ObjectIndex        = Object->InternalIndex;
        ObjectSerialNumber = UObject::GUObjectArray->ObjObjects.IndexToObject(ObjectIndex)->SerialNumber;
        return *this;
    }
};

/* Inlined Functions */

FORCEINLINE bool UObject::IsA(EClassCastFlags TypeFlags) const
{
	return (ClassPrivate->ClassCastFlags & TypeFlags);
}

FORCEINLINE int32 UObject::GetOffset(const std::string& Name, uint8* OutBit) const
{
    return ClassPrivate->GetOffset(Name, OutBit);
}

FORCEINLINE UObject* UObject::GetDefault() const
{
    return ClassPrivate->ClassDefaultObject;
}

FORCEINLINE bool UObject::IsA(const FClassData& Data) const
{
    return ClassPrivate->IsChildOf(Data.GetName(), Data.GetDepth());
}

FORCEINLINE bool UObject::IsA(class UClass* TypeClass) const
{
	return ClassPrivate->IsChildOf(TypeClass);
	// if the game doesnt use USTRUCT_ISCHILDOF_STRUCTARRAY
    /*
	if (!TypeClass)
		return false;

	for (UStruct* Super = ClassPrivate; Super; Super = Super->SuperStruct)
	{
		if (Super == TypeClass)
			return true;
	}

	return false;*/
}

FORCEINLINE class UObject* FWeakObjectPtr::Get() const
{
	return UObject::GUObjectArray->ObjObjects[ObjectIndex];
}

FORCEINLINE class UObject* FWeakObjectPtr::operator->() const
{
	return Get();
}

FORCEINLINE bool FWeakObjectPtr::operator==(const FWeakObjectPtr& Other) const
{
	return ObjectIndex == Other.ObjectIndex;
}

FORCEINLINE bool FWeakObjectPtr::operator!=(const FWeakObjectPtr& Other) const
{
	return ObjectIndex != Other.ObjectIndex;
}

FORCEINLINE bool FWeakObjectPtr::operator==(const class UObject* Other) const
{
	return ObjectIndex == Other->InternalIndex;
}

FORCEINLINE bool FWeakObjectPtr::operator!=(const class UObject* Other) const
{
	return ObjectIndex != Other->InternalIndex;
}

FORCEINLINE bool FWeakObjectPtr::SerialNumbersMatch(const FUObjectItem* ObjectItem) const
{
    return ObjectItem->SerialNumber == ObjectSerialNumber;
}

FORCEINLINE FStructProperty* FStructProperty::GetStruct(const std::string& Name) const
{
    return Struct->GetStruct(Name);
}

FORCEINLINE FProperty* FStructProperty::GetPropertyPtr(const std::string& Name) const
{
    return Struct->GetPropertyPtr(Name);
}

FORCEINLINE int32 FStructProperty::GetOffset(const std::string &Name, uint8 *OutBit) const
{
    return Offset_Internal + Struct->GetOffset(Name, OutBit);
}