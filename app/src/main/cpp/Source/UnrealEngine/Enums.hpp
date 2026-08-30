#pragma once

#include "Containers.hpp"


#define UE_ENUM_OPERATORS(EEnumClass)                                                                                                                    \
                                                                                                                                                         \
inline constexpr EEnumClass operator|(EEnumClass Left, EEnumClass Right)                                                                                 \
{                                                                                                                                                        \
    return static_cast<EEnumClass>(                                                                                                                      \
        static_cast<std::underlying_type<EEnumClass>::type>(Left) |                                                                                      \
        static_cast<std::underlying_type<EEnumClass>::type>(Right));                                                                                     \
}                                                                                                                                                        \
                                                                                                                                                         \
inline constexpr EEnumClass& operator|=(EEnumClass& Left, EEnumClass Right)                                                                              \
{                                                                                                                                                        \
    Left = static_cast<EEnumClass>(                                                                                                                      \
        static_cast<std::underlying_type<EEnumClass>::type>(Left) |                                                                                      \
        static_cast<std::underlying_type<EEnumClass>::type>(Right));                                                                                     \
    return Left;                                                                                                                                         \
}                                                                                                                                                        \
                                                                                                                                                         \
inline constexpr bool operator&(EEnumClass Left, EEnumClass Right)                                                                                       \
{                                                                                                                                                        \
    return (static_cast<std::underlying_type<EEnumClass>::type>(Left) &                                                                                  \
            static_cast<std::underlying_type<EEnumClass>::type>(Right)) ==                                                                               \
    static_cast<std::underlying_type<EEnumClass>::type>(Right);                                                                                          \
}
																																									

#define ENUM_TO_BYTE(y, x) static_cast<std::underlying_type_t<y>>(x)

enum class EObjectFlags : int32
{
	NoFlags							= 0x00000000,

	Public							= 0x00000001,
	Standalone						= 0x00000002,
	MarkAsNative					= 0x00000004,
	Transactional					= 0x00000008,
	ClassDefaultObject				= 0x00000010,
	ArchetypeObject					= 0x00000020,
	Transient						= 0x00000040,

	MarkAsRootSet					= 0x00000080,
	TagGarbageTemp					= 0x00000100,

	NeedInitialization				= 0x00000200,
	NeedLoad						= 0x00000400,
	KeepForCooker					= 0x00000800,
	NeedPostLoad					= 0x00001000,
	NeedPostLoadSubobjects			= 0x00002000,
	NewerVersionExists				= 0x00004000,
	BeginDestroyed					= 0x00008000,
	FinishDestroyed					= 0x00010000,

	BeingRegenerated				= 0x00020000,
	DefaultSubObject				= 0x00040000,
	WasLoaded						= 0x00080000,
	TextExportTransient				= 0x00100000,
	LoadCompleted					= 0x00200000,
	InheritableComponentTemplate	= 0x00400000,
	DuplicateTransient				= 0x00800000,
	StrongRefOnFrame				= 0x01000000,
	NonPIEDuplicateTransient		= 0x02000000,
	Dynamic							= 0x04000000,
	WillBeLoaded					= 0x08000000, 
};

enum class EFunctionFlags : uint32
{
	None							= 0x00000000,

	Final							= 0x00000001,
	RequiredAPI						= 0x00000002,
	BlueprintAuthorityOnly			= 0x00000004, 
	BlueprintCosmetic				= 0x00000008, 
	Net								= 0x00000040,  
	NetReliable						= 0x00000080, 
	NetRequest						= 0x00000100,	
	Exec							= 0x00000200,	
	Native							= 0x00000400,	
	Event							= 0x00000800,   
	NetResponse						= 0x00001000,  
	Static							= 0x00002000,   
	NetMulticast					= 0x00004000,	
	UbergraphFunction				= 0x00008000,  
	MulticastDelegate				= 0x00010000,
	Public							= 0x00020000,	
	Private							= 0x00040000,	
	Protected						= 0x00080000,
	Delegate						= 0x00100000,	
	NetServer						= 0x00200000,	
	HasOutParms						= 0x00400000,	
	HasDefaults						= 0x00800000,
	NetClient						= 0x01000000,
	DLLImport						= 0x02000000,
	BlueprintCallable				= 0x04000000,
	BlueprintEvent					= 0x08000000,
	BlueprintPure					= 0x10000000,	
	EditorOnly						= 0x20000000,	
	Const							= 0x40000000,
	NetValidate						= 0x80000000,

	AllFlags						= 0xFFFFFFFF,
};

enum class EClassFlags : uint32
{
	CLASS_None						= 0x00000000u,
	Abstract						= 0x00000001u,
	DefaultConfig					= 0x00000002u,
	Config							= 0x00000004u,
	Transient						= 0x00000008u,
	Parsed							= 0x00000010u,
	MatchedSerializers				= 0x00000020u,
	ProjectUserConfig				= 0x00000040u,
	Native							= 0x00000080u,
	NoExport						= 0x00000100u,
	NotPlaceable					= 0x00000200u,
	PerObjectConfig					= 0x00000400u,
	ReplicationDataIsSetUp			= 0x00000800u,
	EditInlineNew					= 0x00001000u,
	CollapseCategories				= 0x00002000u,
	Interface						= 0x00004000u,
	CustomConstructor				= 0x00008000u,
	Const							= 0x00010000u,
	LayoutChanging					= 0x00020000u,
	CompiledFromBlueprint			= 0x00040000u,
	MinimalAPI						= 0x00080000u,
	RequiredAPI						= 0x00100000u,
	DefaultToInstanced				= 0x00200000u,
	TokenStreamAssembled			= 0x00400000u,
	HasInstancedReference			= 0x00800000u,
	Hidden							= 0x01000000u,
	Deprecated						= 0x02000000u,
	HideDropDown					= 0x04000000u,
	GlobalUserConfig				= 0x08000000u,
	Intrinsic						= 0x10000000u,
	Constructed						= 0x20000000u,
	ConfigDoNotCheckDefaults		= 0x40000000u,
	NewerVersionExists				= 0x80000000u,
};

enum class EClassCastFlags : uint64
{
	None = 0x0000000000000000,

	Field								= 0x0000000000000001,
	Int8Property						= 0x0000000000000002,
	Enum								= 0x0000000000000004,
	Struct								= 0x0000000000000008,
	ScriptStruct						= 0x0000000000000010,
	Class								= 0x0000000000000020,
	ByteProperty						= 0x0000000000000040,
	IntProperty							= 0x0000000000000080,
	FloatProperty						= 0x0000000000000100,
	UInt64Property						= 0x0000000000000200,
	ClassProperty						= 0x0000000000000400,
	UInt32Property						= 0x0000000000000800,
	InterfaceProperty					= 0x0000000000001000,
	NameProperty						= 0x0000000000002000,
	StrProperty							= 0x0000000000004000,
	Property							= 0x0000000000008000,
	ObjectProperty						= 0x0000000000010000,
	BoolProperty						= 0x0000000000020000,
	UInt16Property						= 0x0000000000040000,
	Function							= 0x0000000000080000,
	StructProperty						= 0x0000000000100000,
	ArrayProperty						= 0x0000000000200000,
	Int64Property						= 0x0000000000400000,
	DelegateProperty					= 0x0000000000800000,
	NumericProperty						= 0x0000000001000000,
	MulticastDelegateProperty			= 0x0000000002000000,
	ObjectPropertyBase					= 0x0000000004000000,
	WeakObjectProperty					= 0x0000000008000000,
	LazyObjectProperty					= 0x0000000010000000,
	SoftObjectProperty					= 0x0000000020000000,
	TextProperty						= 0x0000000040000000,
	Int16Property						= 0x0000000080000000,
	DoubleProperty						= 0x0000000100000000,
	SoftClassProperty					= 0x0000000200000000,
	Package								= 0x0000000400000000,
	Level								= 0x0000000800000000,
	Actor								= 0x0000001000000000,
	PlayerController					= 0x0000002000000000,
	Pawn								= 0x0000004000000000,
	SceneComponent						= 0x0000008000000000,
	PrimitiveComponent					= 0x0000010000000000,
	SkinnedMeshComponent				= 0x0000020000000000,
	SkeletalMeshComponent				= 0x0000040000000000,
	Blueprint							= 0x0000080000000000,
	DelegateFunction					= 0x0000100000000000,
	StaticMeshComponent					= 0x0000200000000000,
	MapProperty							= 0x0000400000000000,
	SetProperty							= 0x0000800000000000,
	EnumProperty						= 0x0001000000000000,
	USparseDelegateFunction				= 0x0002000000000000,
	FMulticastInlineDelegateProperty	= 0x0004000000000000,
	FMulticastSparseDelegateProperty	= 0x0008000000000000,
	FFieldPathProperty					= 0x0010000000000000,
	FLargeWorldCoordinatesRealProperty	= 0x0080000000000000,
	FOptionalProperty					= 0x0100000000000000,
	FVValueProperty						= 0x0200000000000000,
	UVerseVMClass						= 0x0400000000000000,
	FVRestValueProperty					= 0x0800000000000000,
};

enum class EPropertyFlags : uint64
{
	None								= 0x0000000000000000,

	Edit								= 0x0000000000000001,
	ConstParm							= 0x0000000000000002,
	BlueprintVisible					= 0x0000000000000004,
	ExportObject						= 0x0000000000000008,
	BlueprintReadOnly					= 0x0000000000000010,
	Net									= 0x0000000000000020,
	EditFixedSize						= 0x0000000000000040,
	Parm								= 0x0000000000000080,
	OutParm								= 0x0000000000000100,
	ZeroConstructor						= 0x0000000000000200,
	ReturnParm							= 0x0000000000000400,
	DisableEditOnTemplate 				= 0x0000000000000800,

	Transient							= 0x0000000000002000,
	Config								= 0x0000000000004000,

	DisableEditOnInstance				= 0x0000000000010000,
	EditConst							= 0x0000000000020000,
	GlobalConfig						= 0x0000000000040000,
	InstancedReference					= 0x0000000000080000,	

	DuplicateTransient					= 0x0000000000200000,	
	SubobjectReference					= 0x0000000000400000,	

	SaveGame							= 0x0000000001000000,
	NoClear								= 0x0000000002000000,

	ReferenceParm						= 0x0000000008000000,
	BlueprintAssignable					= 0x0000000010000000,
	Deprecated							= 0x0000000020000000,
	IsPlainOldData						= 0x0000000040000000,
	RepSkip								= 0x0000000080000000,
	RepNotify							= 0x0000000100000000, 
	Interp								= 0x0000000200000000,
	NonTransactional					= 0x0000000400000000,
	EditorOnly							= 0x0000000800000000,
	NoDestructor						= 0x0000001000000000,

	AutoWeak							= 0x0000004000000000,
	ContainsInstancedReference			= 0x0000008000000000,	
	AssetRegistrySearchable				= 0x0000010000000000,
	SimpleDisplay						= 0x0000020000000000,
	AdvancedDisplay						= 0x0000040000000000,
	Protected							= 0x0000080000000000,
	BlueprintCallable					= 0x0000100000000000,
	BlueprintAuthorityOnly				= 0x0000200000000000,
	TextExportTransient					= 0x0000400000000000,
	NonPIEDuplicateTransient			= 0x0000800000000000,
	ExposeOnSpawn						= 0x0001000000000000,
	PersistentInstance					= 0x0002000000000000,
	UObjectWrapper						= 0x0004000000000000, 
	HasGetValueTypeHash					= 0x0008000000000000, 
	NativeAccessSpecifierPublic			= 0x0010000000000000,	
	NativeAccessSpecifierProtected		= 0x0020000000000000,
	NativeAccessSpecifierPrivate		= 0x0040000000000000,	
	SkipSerialization					= 0x0080000000000000, 
};


enum class EExprToken : uint8
{
	// Variable references.
	LocalVariable					= 0x00,	// A local variable.
	InstanceVariable				= 0x01,	// An object variable.
	DefaultVariable					= 0x02, // Default variable for a class context.

	Return							= 0x04,	// Return from function.

	Jump							= 0x06,	// Goto a local address in code.
	JumpIfNot						= 0x07,	// Goto if not expression.

	Assert							= 0x09,	// Assertion.

	Nothing							= 0x0B,	// No operation.
	NothingInt32					= 0x0C, // No operation with an int32 argument (useful for debugging script disassembly)

	Let								= 0x0F,	// Assign an arbitrary size value to a variable.

	BitFieldConst					= 0x11, // assign to a single bit, defined by an FProperty
	ClassContext					= 0x12,	// Class default object context.
	MetaCast						= 0x13, // Metaclass cast.
	LetBool							= 0x14, // Let boolean variable.
	EndParmValue					= 0x15,	// end of default value for optional function parameter
	EndFunctionParms				= 0x16,	// End of function call parameters.
	Self							= 0x17,	// Self object.
	Skip							= 0x18,	// Skippable expression.
	Context							= 0x19,	// Call a function through an object context.
	Context_FailSilent				= 0x1A, // Call a function through an object context (can fail silently if the context is NULL; only generated for functions that don't have output or return values).
	VirtualFunction					= 0x1B,	// A function call with parameters.
	FinalFunction					= 0x1C,	// A prebound function call with parameters.
	IntConst						= 0x1D,	// Int constant.
	FloatConst						= 0x1E,	// Floating point constant.
	StringConst						= 0x1F,	// String constant.
	ObjectConst						= 0x20,	// An object constant.
	NameConst						= 0x21,	// A name constant.
	RotationConst					= 0x22,	// A rotation constant.
	VectorConst						= 0x23,	// A vector constant.
	ByteConst						= 0x24,	// A byte constant.
	IntZero							= 0x25,	// Zero.
	IntOne							= 0x26,	// One.
	True							= 0x27,	// Bool True.
	False							= 0x28,	// Bool False.
	TextConst						= 0x29, // FText constant
	NoObject						= 0x2A,	// NoObject.
	TransformConst					= 0x2B, // A transform constant
	IntConstByte					= 0x2C,	// Int constant that requires 1 byte.
	NoInterface						= 0x2D, // A null interface (similar to EX_NoObject, but for interfaces)
	DynamicCast						= 0x2E,	// Safe dynamic class casting.
	StructConst						= 0x2F, // An arbitrary UStruct constant
	EndStructConst					= 0x30, // End of UStruct constant
	SetArray						= 0x31, // Set the value of arbitrary array
	EndArray						= 0x32,
	PropertyConst					= 0x33, // FProperty constant.
	UnicodeStringConst				= 0x34, // Unicode string constant.
	Int64Const						= 0x35,	// 64-bit integer constant.
	UInt64Const						= 0x36,	// 64-bit unsigned integer constant.
	DoubleConst						= 0x37, // Double constant.
	Cast							= 0x38,	// A casting operator which reads the type as the subsequent byte
	SetSet							= 0x39,
	EndSet							= 0x3A,
	SetMap							= 0x3B,
	EndMap							= 0x3C,
	SetConst						= 0x3D,
	EndSetConst						= 0x3E,
	MapConst						= 0x3F,
	EndMapConst						= 0x40,
	Vector3fConst					= 0x41,	// A float vector constant.
	StructMemberContext				= 0x42, // Context expression to address a property within a struct
	LetMulticastDelegate			= 0x43, // Assignment to a multi-cast delegate
	LetDelegate						= 0x44, // Assignment to a delegate
	LocalVirtualFunction			= 0x45, // Special instructions to quickly call a virtual function that we know is going to run only locally
	LocalFinalFunction				= 0x46, // Special instructions to quickly call a final function that we know is going to run only locally
	//								= 0x47, // CST_ObjectToBool
	LocalOutVariable				= 0x48, // local out (pass by reference) function parameter
	//								= 0x49, // CST_InterfaceToBool
	DeprecatedOp4A					= 0x4A,
	InstanceDelegate				= 0x4B,	// const reference to a delegate or normal function object
	PushExecutionFlow				= 0x4C, // push an address on to the execution flow stack for future execution when a EX_PopExecutionFlow is executed.   Execution continues on normally and doesn't change to the pushed address.
	PopExecutionFlow				= 0x4D, // continue execution at the last address previously pushed onto the execution flow stack.
	ComputedJump					= 0x4E,	// Goto a local address in code, specified by an integer value.
	PopExecutionFlowIfNot			= 0x4F, // continue execution at the last address previously pushed onto the execution flow stack, if the condition is not true.
	Breakpoint						= 0x50, // Breakpoint.  Only observed in the editor, otherwise it behaves like EX_Nothing.
	InterfaceContext				= 0x51,	// Call a function through a native interface variable
	ObjToInterfaceCast				= 0x52,	// Converting an object reference to native interface variable
	EndOfScript						= 0x53, // Last byte in script code
	CrossInterfaceCast				= 0x54, // Converting an interface variable reference to native interface variable
	InterfaceToObjCast				= 0x55, // Converting an interface variable reference to an object

	WireTracepoint					= 0x5A, // Trace point.  Only observed in the editor, otherwise it behaves like EX_Nothing.
	SkipOffsetConst					= 0x5B, // A CodeSizeSkipOffset constant
	AddMulticastDelegate			= 0x5C, // Adds a delegate to a multicast delegate's targets
	ClearMulticastDelegate			= 0x5D, // Clears all delegates in a multicast target
	Tracepoint						= 0x5E, // Trace point.  Only observed in the editor, otherwise it behaves like EX_Nothing.
	LetObj							= 0x5F,	// assign to any object ref pointer
	LetWeakObjPtr					= 0x60, // assign to a weak object pointer
	BindDelegate					= 0x61, // bind object and name to delegate
	RemoveMulticastDelegate			= 0x62, // Remove a delegate from a multicast delegate's targets
	CallMulticastDelegate			= 0x63, // Call multicast delegate
	LetValueOnPersistentFrame		= 0x64,
	ArrayConst						= 0x65,
	EndArrayConst					= 0x66,
	SoftObjectConst					= 0x67,
	CallMath						= 0x68, // static pure function from on local call space
	SwitchValue						= 0x69,
	InstrumentationEvent			= 0x6A, // Instrumentation event
	ArrayGetByRef					= 0x6B,
	ClassSparseDataVariable			= 0x6C, // Sparse data variable
	FieldPathConst					= 0x6D,

	AutoRtfmTransact				= 0x70, // AutoRTFM: run following code in a transaction
	AutoRtfmStopTransact			= 0x71, // AutoRTFM: if in a transaction, abort or break, otherwise no operation
	AutoRtfmAbortIfNot				= 0x72, // AutoRTFM: evaluate bool condition, abort transaction on false
	Max								= 0xFF,
};

// Script instrumentation event types
enum class EScriptInstrumentation : uint8
{
	Class = 0,
	ClassScope,
	Instance,
	Event,
	InlineEvent,
	ResumeEvent,
	PureNodeEntry,
	NodeDebugSite,
	NodeEntry,
	NodeExit,
	PushState,
	RestoreState,
	ResetState,
	SuspendState,
	PopState,
	TunnelEndOfThread,
	Stop
};

enum class EBlueprintTextLiteralType : uint8
{
	/* Text is an empty string. The bytecode contains no strings, and you should use FText::GetEmpty() to initialize the FText instance. */
	Empty,
	/** Text is localized. The bytecode will contain three strings - source, key, and namespace - and should be loaded via FInternationalization */
	LocalizedText,
	/** Text is culture invariant. The bytecode will contain one string, and you should use FText::AsCultureInvariant to initialize the FText instance. */
	InvariantText,
	/** Text is a literal FString. The bytecode will contain one string, and you should use FText::FromString to initialize the FText instance. */
	LiteralString,
	/** Text is from a string table. The bytecode will contain an object pointer (not used) and two strings - the table ID, and key - and should be found via FText::FromStringTable */
	StringTableEntry,
};

enum class EMappingsTypeFlags : uint8
{
	ByteProperty,
	BoolProperty,
	IntProperty,
	FloatProperty,
	ObjectProperty,
	NameProperty,
	DelegateProperty,
	DoubleProperty,
	ArrayProperty,
	StructProperty,
	StrProperty,
	TextProperty,
	InterfaceProperty,
	MulticastDelegateProperty,
	WeakObjectProperty, //
	LazyObjectProperty, // When deserialized, these 3 properties will be SoftObjects
	AssetObjectProperty, //
	SoftObjectProperty,
	UInt64Property,
	UInt32Property,
	UInt16Property,
	Int64Property,
	Int16Property,
	Int8Property,
	MapProperty,
	SetProperty,
	EnumProperty,
	FieldPathProperty,
	OptionalProperty,

	Unknown = 0xFF
};

enum class EInternalObjectFlags : int32
{
    None = 0,
    //~ All the other bits are reserved, DO NOT ADD NEW FLAGS HERE!

    ReachableInCluster = 1 << 23, ///< External reference to object in cluster exists
    ClusterRoot = 1 << 24, ///< Root of a cluster
    Native = 1 << 25, ///< Native (UClass only).
    Async = 1 << 26, ///< Object exists only on a different thread than the game thread.
    AsyncLoading = 1 << 27, ///< Object is being asynchronously loaded.
    Unreachable = 1 << 28, ///< Object is not reachable on the object graph.
    PendingKill = 1 << 29, ///< Objects that are pending destruction (invalid for gameplay but valid objects)
    RootSet = 1 << 30, ///< Object will not be garbage collected, even if unreferenced.
    //~ UnusedFlag = 1 << 31,

    GarbageCollectionKeepFlags = Native | Async | AsyncLoading,

    //~ Make sure this is up to date!
    AllFlags = ReachableInCluster | ClusterRoot | Native | Async | AsyncLoading | Unreachable | PendingKill | RootSet
};


UE_ENUM_OPERATORS(EObjectFlags);
UE_ENUM_OPERATORS(EFunctionFlags);
UE_ENUM_OPERATORS(EClassFlags);
UE_ENUM_OPERATORS(EClassCastFlags);
UE_ENUM_OPERATORS(EPropertyFlags);
UE_ENUM_OPERATORS(EMappingsTypeFlags);
UE_ENUM_OPERATORS(EInternalObjectFlags);