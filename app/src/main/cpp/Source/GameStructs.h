#ifndef D774ECC7_7E08_44B8_9F86_DA69E0557911
#define D774ECC7_7E08_44B8_9F86_DA69E0557911

#include "UnrealEngine/ScriptCore.h"


enum class EBlendMode : uint8 
{
    BLEND_Opaque = 0,
    BLEND_Masked = 1,
    BLEND_Translucent = 2,
    BLEND_Additive = 3,
    BLEND_Modulate = 4,
    BLEND_AlphaComposite = 5,
    BLEND_AlphaHoldout = 6,
    BLEND_MAX = 7
};

struct FItemNetID 
{
	uint32_t ItemID1;
	uint32_t ItemID2; 
};

enum class EPrimalItemType : uint8_t {
    MiscConsumable = 0,
    Equipment = 1,
    Weapon = 2,
    Ammo = 3,
    Structure = 4,
    Resource = 5,
    Skin = 6,
    WeaponAttachment = 7,
    Artifact = 8,
    MAX = 9
};

struct FHitResult 
{
	char bBlockingHit : 1; // Offset: 0x0 | Size: 0x1
	char bStartPenetrating : 1; // Offset: 0x0 | Size: 0x1
	char pad_0x0_2 : 6; // Offset: 0x0 | Size: 0x1
	char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
	int32_t FaceIndex; // Offset: 0x4 | Size: 0x4
	float Time; // Offset: 0x8 | Size: 0x4
	float Distance; // Offset: 0xc | Size: 0x4
	FVector Location; // Offset: 0x10 | Size: 0xc
	FVector ImpactPoint; // Offset: 0x1c | Size: 0xc
	FVector Normal; // Offset: 0x28 | Size: 0xc
	FVector ImpactNormal; // Offset: 0x34 | Size: 0xc
	FVector TraceStart; // Offset: 0x40 | Size: 0xc
	FVector TraceEnd; // Offset: 0x4c | Size: 0xc
	float PenetrationDepth; // Offset: 0x58 | Size: 0x4
	int32_t Item; // Offset: 0x5c | Size: 0x4
	char ElementIndex; // Offset: 0x60 | Size: 0x1
	char pad_0x61[0x3]; // Offset: 0x61 | Size: 0x3
	TWeakObjectPtr<UObject> PhysMaterial; // Offset: 0x64 | Size: 0x8
	TWeakObjectPtr<UObject> Actor; // Offset: 0x6c | Size: 0x8
	TWeakObjectPtr<UObject> Component; // Offset: 0x74 | Size: 0x8
	FName BoneName; // Offset: 0x7c | Size: 0x8
	FName MyBoneName; // Offset: 0x84 | Size: 0x8
};

struct FSkeletalMaterial 
{
	UObject* MaterialInterface; 
	FName MaterialSlotName; 
	char pad_0x24[0x18]; 
};

enum class EShadingModel : uint8 
{
	MSM_Unlit = 0,
	MSM_DefaultLit = 1,
	MSM_Subsurface = 2,
	MSM_PreintegratedSkin = 3,
	MSM_ClearCoat = 4,
	MSM_SubsurfaceProfile = 5,
	MSM_TwoSidedFoliage = 6,
	MSM_Hair = 7,
	MSM_Cloth = 8,
	MSM_Eye = 9,
	MSM_SingleLayerWater = 10,
	MSM_ThinTranslucent = 11,
	MSM_NUM = 12,
	MSM_FromMaterialExpression = 13,
	MSM_MAX = 14
};


enum class EMIDCreationFlags : uint8
{
	None = 0,
	Transient = 1,
	EMIDCreationFlags_MAX = 2
};

enum class EMaterialDomain : uint8 
{
	MD_Surface = 0,
	MD_DeferredDecal = 1,
	MD_LightFunction = 2,
	MD_Volume = 3,
	MD_PostProcess = 4,
	MD_UI = 5,
	MD_RuntimeVirtualTexture = 6,
	MD_MAX = 7
};

enum class ENetRole : uint8 
{
	ROLE_None = 0,
	ROLE_SimulatedProxy = 1,
	ROLE_AutonomousProxy = 2,
	ROLE_Authority = 3,
	ROLE_MAX = 4
};
// Object: ScriptStruct ShooterGame.PreferredSnapData
// Inherited Bytes: 0x0 | Struct Size: 0x18
struct FPreferredSnapData {
	// Fields
	bool bUsedPreferredSnap; // Offset: 0x0 | Size: 0x1
	char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
	int32_t MySnapIndex; // Offset: 0x4 | Size: 0x4
	int32_t TheirSnapIndex; // Offset: 0x8 | Size: 0x4
	char pad_0xC[0x4]; // Offset: 0xc | Size: 0x4
	UObject* SnapToActor; // Offset: 0x10 | Size: 0x8
};

struct FPlacementData {
	// Fields
	struct FVector AdjustedLocation; // Offset: 0x0 | Size: 0xc
	struct FRotator AdjustedRotation; // Offset: 0xc | Size: 0xc
	bool bSnapped; // Offset: 0x18 | Size: 0x1
	bool bDisableEncroachmentCheck; // Offset: 0x19 | Size: 0x1
	char pad_0x1A[0x2]; // Offset: 0x1a | Size: 0x2
	int32_t MySnapToIndex; // Offset: 0x1c | Size: 0x4
	int32_t TheirSnapToIndex; // Offset: 0x20 | Size: 0x4
	char pad_0x24[0x4]; // Offset: 0x24 | Size: 0x4
	UObject* FloorHitActor; // Offset: 0x28 | Size: 0x8
	UObject* ParentStructure; // Offset: 0x30 | Size: 0x8
	UObject* ForcePlacedOnFloorParentStructure; // Offset: 0x38 | Size: 0x8
	UObject* ReplacesStructure; // Offset: 0x40 | Size: 0x8
	UObject* AttachToPawn; // Offset: 0x48 | Size: 0x8
	class FName AttachToBone; // Offset: 0x50 | Size: 0x8
	UObject* DinoCharacter; // Offset: 0x58 | Size: 0x8
	struct FPreferredSnapData PreferredSnap; // Offset: 0x60 | Size: 0x18
};



struct FStructureArkPass 
{
	bool isVerified; // Offset: 0x0 | Size: 0x1
	char pad_0x1[0x7]; // Offset: 0x1 | Size: 0x7
	class FString UniqueNetId; // Offset: 0x8 | Size: 0x10
	class TArray<char> ArkPassData; // Offset: 0x18 | Size: 0x10
	uintptr_t ArkPassBought; // Offset: 0x28 | Size: 0x8
	int32_t ArkPassDays; // Offset: 0x30 | Size: 0x4
	class FName Platform; // Offset: 0x34 | Size: 0x8
	char pad_0x3C[0x4]; // Offset: 0x3c | Size: 0x4
};

enum class ETargetingCheckboxImage : uint8_t {
	EC_Targeting = 0,
	EC_TekGloves = 1,
	EC_StructureAltFire = 2,
	EC_Shield = 3,
	EC_PrimalWeaponGPS = 4,
	EC_WeapScissors = 5,
	EC_WeapPaintbrush = 6,
	EC_None = 7,
	EC_MAX = 8
};

enum class EBlendableLocation : uint8
{
	BL_AfterTonemapping = 0,
	BL_BeforeTonemapping = 1,
	BL_BeforeTranslucency = 2,
	BL_ReplacingTonemapper = 3,
	BL_SSRInput = 4,
	BL_MAX = 5
};

struct FAlivePlayerDataInfo 
{
	FString PlayerName;
	FString PlayerSteamName;
	uint64 PlayerID;
	FString TribeName;
	uint64 TargetingTeamID;
};

struct FInstantWeaponData {
	// Fields
	float WeaponSpread; // Offset: 0x0 | Size: 0x4
	float TargetingSpreadMod; // Offset: 0x4 | Size: 0x4
	float FinalWeaponSpreadMultiplier; // Offset: 0x8 | Size: 0x4
	float FiringSpreadIncrement; // Offset: 0xc | Size: 0x4
	float FiringSpreadMax; // Offset: 0x10 | Size: 0x4
	float WeaponRange; // Offset: 0x14 | Size: 0x4
};

struct FFindFloorResult 
{
	char bBlockingHit : 1; // Offset: 0x0 | Size: 0x1
	char bWalkableFloor : 1; // Offset: 0x0 | Size: 0x1
	char bLineTrace : 1; // Offset: 0x0 | Size: 0x1
	char pad_0x0_3 : 5; // Offset: 0x0 | Size: 0x1
	char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
	float FloorDist; // Offset: 0x4 | Size: 0x4
	float LineDist; // Offset: 0x8 | Size: 0x4
	struct FHitResult HitResult; // Offset: 0xc | Size: 0x8c
};

struct FRootMotionMovementParams 
{
	// Fields
	bool bHasRootMotion; // Offset: 0x0 | Size: 0x1
	char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
	float BlendWeight; // Offset: 0x4 | Size: 0x4
	char pad_0x8[0x8]; // Offset: 0x8 | Size: 0x8
	struct FTransform RootMotionTransform; // Offset: 0x10 | Size: 0x30
};

struct FRootMotionSourceGroup {
	// Fields
	char pad_0x0[0x28]; // Offset: 0x0 | Size: 0x28
	char bHasAdditiveSources : 1; // Offset: 0x28 | Size: 0x1
	char bHasOverrideSources : 1; // Offset: 0x28 | Size: 0x1
	char bHasOverrideSourcesWithIgnoreZAccumulate : 1; // Offset: 0x28 | Size: 0x1
	char bIsAdditiveVelocityApplied : 1; // Offset: 0x28 | Size: 0x1
	char pad_0x28_4 : 4; // Offset: 0x28 | Size: 0x1
	char LastAccumulatedSettings; // Offset: 0x29 | Size: 0x1
	char pad_0x2A[0x2]; // Offset: 0x2a | Size: 0x2
	struct FVector LastPreAdditiveVelocity; // Offset: 0x2c | Size: 0xc
};

/** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
class FSavedMove_Character
{
public:
	void** VTable;
	UObject* CharacterOwner;

	uint32 bPressedJump:1;
	uint32 bWantsToCrouch:1;
	uint32 bForceMaxAccel:1;

	/** If true, can't combine this move with another move. */
	uint32 bForceNoCombine:1;

	/** If true this move is using an old TimeStamp, before a reset occurred. */
	uint32 bOldTimeStampBeforeReset:1;

	uint32 bWasJumping:1;

	float TimeStamp;    // Time of this move.
	float DeltaTime;    // amount of time for this move
	float CustomTimeDilation;
	float JumpKeyHoldTime;
	float JumpForceTimeRemaining;
	int32 JumpMaxCount;
	int32 JumpCurrentCount;
	
	uint8 MovementMode;

	// Information at the start of the move
	uint8 StartPackedMovementMode;
	FVector StartLocation;
	FVector StartRelativeLocation;
	FVector StartVelocity;
	FFindFloorResult StartFloor;
	FRotator StartRotation;
	FRotator StartControlRotation;
	FQuat StartBaseRotation;	// rotation of the base component (or bone), only saved if it can move.
	float StartCapsuleRadius;
	float StartCapsuleHalfHeight;
	TWeakObjectPtr<UObject> StartBase;
	FName StartBoneName;
	uint32 StartActorOverlapCounter;
	uint32 StartComponentOverlapCounter;
	TWeakObjectPtr<UObject> StartAttachParent;
	FName StartAttachSocketName;
	FVector StartAttachRelativeLocation;
	FRotator StartAttachRelativeRotation;

	// Information after the move has been performed
	uint8 EndPackedMovementMode;
	FVector SavedLocation;
	FRotator SavedRotation;
	FVector SavedVelocity;
	FVector SavedRelativeLocation;
	FRotator SavedControlRotation;
	TWeakObjectPtr<UObject> EndBase;
	FName EndBoneName;
	uint32 EndActorOverlapCounter;
	uint32 EndComponentOverlapCounter;
	TWeakObjectPtr<UObject> EndAttachParent;
	FName EndAttachSocketName;
	FVector EndAttachRelativeLocation;
	FRotator EndAttachRelativeRotation;

	FVector Acceleration;
	float MaxSpeed;

	// Cached to speed up iteration over IsImportantMove().
	FVector AccelNormal;
	float AccelMag;

	TWeakObjectPtr<class UObject> RootMotionMontage;
	float RootMotionTrackPosition;
	FRootMotionMovementParams RootMotionMovement;

	FRootMotionSourceGroup SavedRootMotion;

	/** Threshold for deciding this is an "important" move based on DP with last acked acceleration. */
	float AccelDotThreshold;    
	/** Threshold for deciding is this is an important move because acceleration magnitude has changed too much */
	float AccelMagThreshold;	
	/** Threshold for deciding if we can combine two moves, true if cosine of angle between them is <= this. */
	float AccelDotThresholdCombine;
	/** Client saved moves will not combine if the result of GetMaxSpeed() differs by this much between moves. */
	float MaxSpeedThresholdCombine;
	
	/** Clear saved move properties, so it can be re-used. */

	enum EPostUpdateMode
	{
		PostUpdate_Record,		// Record a move after having run the simulation
		PostUpdate_Replay,		// Update after replaying a move for a client correction
	};

	// Bit masks used by GetCompressedFlags() to encode movement information.
	enum CompressedFlags
	{
		FLAG_JumpPressed	= 0x01,	// Jump pressed
		FLAG_WantsToCrouch	= 0x02,	// Wants to crouch
		FLAG_Reserved_1		= 0x04,	// Reserved for future use
		FLAG_Reserved_2		= 0x08,	// Reserved for future use
		// Remaining bit masks are available for custom flags.
		FLAG_Custom_0		= 0x10,
		FLAG_Custom_1		= 0x20,
		FLAG_Custom_2		= 0x40,
		FLAG_Custom_3		= 0x80,
	};
};

struct FClientAdjustment
{
	float TimeStamp;
	float DeltaTime;
	FVector NewLoc;
	FVector NewVel;
	FRotator NewRot;
	UObject* NewBase;
	FName NewBaseBoneName;
	bool bAckGoodMove;
	bool bBaseRelativePosition;
	uint8 MovementMode;
};

struct FCharacterMoveResponseDataContainer
{
public:
	void** VTable;
	bool bHasBase;
	bool bHasRotation; // By default ClientAdjustment.NewRot is not serialized. Set this to true after base ServerFillResponseData if you want Rotation to be serialized.
	bool bRootMotionMontageCorrection;
	bool bRootMotionSourceCorrection;

	// Client adjustment. All data other than bAckGoodMove and TimeStamp is only valid if this is a correction (not an ack).
	FClientAdjustment ClientAdjustment;

	float RootMotionTrackPosition;
	FVector RootMotionRotation;
    
    bool IsGoodMove() const        { return ClientAdjustment.bAckGoodMove;}
};

struct FBoxSphereBounds 
{
	// Fields
	struct FVector Origin; // Offset: 0x0 | Size: 0xc
	struct FVector BoxExtent; // Offset: 0xc | Size: 0xc
	float SphereRadius; // Offset: 0x18 | Size: 0x4
};

struct FCharacterNetworkMoveData
{
public:

	void** VTable;

	enum class ENetworkMoveType
	{
		NewMove,
		PendingMove,
		OldMove
	};
	// Indicates whether this was the latest new move, a pending/dual move, or old important move.
	ENetworkMoveType NetworkMoveType;

	//------------------------------------------------------------------------
	// Basic movement data.

	float TimeStamp;
	FVector Acceleration;
	FVector Location;		// Either world location or relative to MovementBase if that is set.
	FRotator ControlRotation;
	uint8 CompressedMoveFlags;

	UObject* MovementBase;
	FName MovementBaseBoneName;
	uint8 MovementMode;
};

struct FCharacterNetworkMoveDataContainer
{
public:
	void** VTable;
	//------------------------------------------------------------------------
	// Optional pending data used in "dual moves".
	bool bHasPendingMove;
	bool bIsDualHybridRootMotionMove;
	
	// Optional "old move" data, for redundant important old moves not yet ack'd.
	bool bHasOldMove;

	// True if we want to disable a scoped move around both dual moves (optional from bEnableServerDualMoveScopedMovementUpdates), typically set if bForceNoCombine was true which can indicate an important change in moves.
	bool bDisableCombinedScopedMove;
	
protected:

	FCharacterNetworkMoveData* NewMoveData;
	FCharacterNetworkMoveData* PendingMoveData;	// Only valid if bHasPendingMove is true
	FCharacterNetworkMoveData* OldMoveData;		// Only valid if bHasOldMove is true

private:

	FCharacterNetworkMoveData BaseDefaultMoveData[3];
};

struct FTribeAlliance 
{
	class FString AllianceName; // Offset: 0x0 | Size: 0x10
	uint32_t AllianceID; // Offset: 0x10 | Size: 0x4
	char pad_0x14[0x4]; // Offset: 0x14 | Size: 0x4
	class TArray<class FString> MembersTribeName; // Offset: 0x18 | Size: 0x10
	class TArray<uint32_t> MembersTribeID; // Offset: 0x28 | Size: 0x10
	class TArray<uint32_t> AdminsTribeID; // Offset: 0x38 | Size: 0x10
};

struct FTribeData 
{
	class FString TribeName;
	double LastNameChangeTime; 
	uint32_t OwnerPlayerDataId; 
	int32_t TribeID; 
	class TArray<class FString> MembersPlayerName;
	class TArray<uint32_t> MembersPlayerDataID;
	class TArray<char> MembersRankGroups; 
	class TArray<double> SlotFreedTime; 
	class TArray<uint32_t> TribeAdmins; 
	class TArray<FTribeAlliance> TribeAlliances;
};

enum class ECollisionChannel : uint8_t {
	ECC_WorldStatic = 0,
	ECC_WorldDynamic = 1,
	ECC_Visibility = 2,
	ECC_PhysicsBody = 3,
	ECC_Destructible = 4,
	ECC_GameTraceChannel1 = 5,
	ECC_GameTraceChannel2 = 6,
	ECC_GameTraceChannel3 = 7,
	ECC_GameTraceChannel4 = 8,
	ECC_GameTraceChannel5 = 9,
	ECC_GameTraceChannel6 = 10,
	ECC_GameTraceChannel7 = 11,
	ECC_GameTraceChannel8 = 12,
	ECC_GameTraceChannel9 = 13,
	ECC_GameTraceChannel10 = 14,
	ECC_GameTraceChannel11 = 15,
	ECC_GameTraceChannel12 = 16,
	ECC_GameTraceChannel13 = 17,
	ECC_GameTraceChannel14 = 18,
	ECC_GameTraceChannel15 = 19,
	ECC_GameTraceChannel16 = 20,
	ECC_GameTraceChannel17 = 21,
	ECC_GameTraceChannel18 = 22,
	ECC_GameTraceChannel19 = 23,
	ECC_GameTraceChannel20 = 24,
	ECC_GameTraceChannel21 = 25,
	ECC_GameTraceChannel22 = 26,
	ECC_GameTraceChannel23 = 27,
	ECC_GameTraceChannel24 = 28,
	ECC_GameTraceChannel25 = 29,
	ECC_GameTraceChannel26 = 30,
	ECC_GameTraceChannel27 = 31,
	ECC_OverlapAll_Deprecated = 32,
	ECC_MAX = 33
};

struct FWeaponData 
{
	bool bInfiniteAmmo; // Offset: 0x0 | Size: 0x1
	bool bNeverReload; // Offset: 0x1 | Size: 0x1
	bool bUsesAmmo; // Offset: 0x2 | Size: 0x1
	bool bDontTakeAmmoOnReload; // Offset: 0x3 | Size: 0x1
	bool bFullReloadFromSingleItem; // Offset: 0x4 | Size: 0x1
	bool bShowAmmoInClipAsPercent; // Offset: 0x5 | Size: 0x1
	bool bDontReloadEmptyClipOnEquip; // Offset: 0x6 | Size: 0x1
	char pad_0x7[0x1]; // Offset: 0x7 | Size: 0x1
	int32_t AmmoPerClip; // Offset: 0x8 | Size: 0x4
	int32_t AmmoPerShot; // Offset: 0xc | Size: 0x4
	bool bSilentOverflowReload; // Offset: 0x10 | Size: 0x1
	char pad_0x11[0x3]; // Offset: 0x11 | Size: 0x3
	float TimeBetweenShots; // Offset: 0x14 | Size: 0x4
	float NoAnimReloadDuration; // Offset: 0x18 | Size: 0x4
	bool bForceReloadDuration; // Offset: 0x1c | Size: 0x1
	bool bLoopedReloadAnim; // Offset: 0x1d | Size: 0x1
	char pad_0x1E[0x2]; // Offset: 0x1e | Size: 0x2
	float ReloadDurationPerAmmoCount; // Offset: 0x20 | Size: 0x4
	float ItemDurabilityToConsumePerShot; // Offset: 0x24 | Size: 0x4
	float HarvestResourceMultiplier; // Offset: 0x28 | Size: 0x4
};

enum EActorListsBP
{
	AL_PLAYERS = 0,
	AL_PLAYERSTATES = 1,
	AL_FLOATINGHUD = 2,
	AL_UNSTASISEDACTORS = 3,
	AL_NPC = 4,
	AL_NPC_ACTIVE = 5,
	AL_FORCEDHUD = 6,
	AL_NPCZONEMANAGERS = 7,
	AL_PLAYERCONTROLLERS = 8,
	AL_BEDS = 9,
	AL_BIOMEZONEVOLUMES = 10,
	AL_NPC_DEAD = 11,
	AL_DAYCYCLEAMBIENTSOUNDS = 12,
	AL_STRUCTURESCLIENT = 13,
	AL_STRUCTUREPREVENTIONVOLUMES = 14,
	AL_TRANSPONDERS = 15,
	AL_CUSTOMACTORLISTS = 16,
	AL_BLOCKINGVOLUMES = 17,
	AL_AMBIENTSOUNDS = 18,
	AL_CONNECTEDSHOOTERCHARACTERS = 19,
	AL_EXPLORERNOTECHESTS = 20,
	AL_SUPPLYCRATEVOLUMES = 21,
	AL_TAMED_DINOS = 22,
	MAX = 23
};


enum class EPrimalEquipmentType : uint8
{
	Hat = 0,
	Shirt = 1,
	Pants = 2,
	Boots = 3,
	Gloves = 4,
	DinoSaddle = 5,
	Trophy = 6,
	Costume = 7,
	Shield = 8,
	Weapon = 9,
	Snapshot = 10,
	MAX = 11
};

struct FURL
{
	// Fields
	class FString protocol; // Offset: 0x0 | Size: 0x10
	class FString Host; // Offset: 0x10 | Size: 0x10
	int32_t Port; // Offset: 0x20 | Size: 0x4
	int32_t Valid; // Offset: 0x24 | Size: 0x4
	class FString Map; // Offset: 0x28 | Size: 0x10
	class FString RedirectURL; // Offset: 0x38 | Size: 0x10
	class TArray<class FString> Op; // Offset: 0x48 | Size: 0x10
	class FString Portal; // Offset: 0x58 | Size: 0x10
};

class FSeamlessTravelHandler
{
private:
	FURL PendingTravelURL;
	int32 PendingTravelGuid[4];
	UObject* LoadedPackage;
	UObject* CurrentWorld;
	UObject* LoadedWorld;
	bool bTransitionInProgress;
	bool bSwitchedToDefaultMap;
	bool bPauseAtMidpoint;
	bool bNeedCancelCleanUp;
	FName WorldContextHandle;
	double SeamlessTravelStartTime = 0.f;

public:
	FSeamlessTravelHandler() = default;
 
	FORCEINLINE bool IsInTransition() const
	{
		return bTransitionInProgress;
	}

	FORCEINLINE bool HasSwitchedToDefaultMap() const
	{
		return IsInTransition() && bSwitchedToDefaultMap;
	}

	inline FString GetDestinationMapName() const
	{
		return (IsInTransition() ? PendingTravelURL.Map : u"");
	}

	inline const UObject* GetLoadedWorld() const
	{
		return LoadedWorld;
	}

};


struct FWorldContext
{
	FWorldContext() = default;

	uint8 WorldType;
	FSeamlessTravelHandler SeamlessTravelHandler;
	FName ContextHandle;
	FString TravelURL;
	uint8 TravelType;
	struct FURL LastURL;
	struct FURL LastRemoteURL;
	class UPendingNetGame * PendingNetGame;
	TArray<struct FFullyLoadedPackagesInfo> PackagesToFullyLoad;
	TArray<FName> LevelsToLoadForPendingMapChange;
	TArray<class UObject*> LoadedLevelsForPendingMapChange; // ULevel
	FString PendingMapChangeFailureDescription;								
	uint32 bShouldCommitPendingMapChange:1;
    TArray<class UObject*> ObjectReferencers;//UObjectReferencer
	TArray<struct FLevelStreamingStatus> PendingLevelStreamingStatusUpdates;
	class UObject* GameViewport; // UGameViewportClient
	class UObject* OwningGameInstance; // UGameInstance
	TArray<struct FNamedNetDriver> ActiveNetDrivers;
	int32	PIEInstance;
	FString	PIEPrefix;
	int32 PIEWorldFeatureLevel;
	bool RunAsDedicated;
	bool bWaitingOnOnlineSubsystem;
	uint32 AudioDeviceID;
	FString CustomDescription;
	float PIEFixedTickSeconds = 0.f;
	float PIEAccumulatedTickSeconds = 0.f;
	TArray<UObject**> ExternalReferences; // UWorld

	FORCEINLINE UObject* World() const
	{
		return ThisCurrentWorld;
	}

private:

	UObject* ThisCurrentWorld; // UWorld
};


struct FCraftingResourceRequirement
{
    // Fields
    float BaseResourceRequirement; // Offset: 0x0 | Size: 0x4
    char pad_0x4[0x4]; // Offset: 0x4 | Size: 0x4
    class UClass* ResourceItemType; // Offset: 0x8 | Size: 0x8
    bool bCraftingRequireExactResourceType; // Offset: 0x10 | Size: 0x1
    char pad_0x11[0x7]; // Offset: 0x11 | Size: 0x7
};


struct FGuid
{
    uint8 A, B, C, D;
};

struct FIntVector
{
    int32_t X, Y, Z;
};

struct FIntPoint
{
    int32 X, Y;

    FVector2D Vector() const
    {
        return {float(X), float(Y)};
    }
};


struct FRandomStream {
    // Fields
    int32_t InitialSeed; // Offset: 0x0 | Size: 0x4
    int32_t Seed; // Offset: 0x4 | Size: 0x4
};

// Object: ScriptStruct CoreUObject.DateTime
// Inherited Bytes: 0x0 | Struct Size: 0x8
struct FDateTime {
    // Fields
    uint8 pad_0x0[0x8]; // Offset: 0x0 | Size: 0x8
};

// Object: ScriptStruct CoreUObject.FrameNumber
// Inherited Bytes: 0x0 | Struct Size: 0x4
struct FFrameNumber {
    // Fields
    int32_t Value; // Offset: 0x0 | Size: 0x4
};

// Object: ScriptStruct CoreUObject.FrameRate
// Inherited Bytes: 0x0 | Struct Size: 0x8
struct FFrameRate {
    // Fields
    int32_t Numerator; // Offset: 0x0 | Size: 0x4
    int32_t Denominator; // Offset: 0x4 | Size: 0x4
};

// Object: ScriptStruct CoreUObject.FrameTime
// Inherited Bytes: 0x0 | Struct Size: 0x8
struct FFrameTime {
    // Fields
    struct FFrameNumber FrameNumber; // Offset: 0x0 | Size: 0x4
    float SubFrame; // Offset: 0x4 | Size: 0x4
};

// Object: ScriptStruct CoreUObject.QualifiedFrameTime
// Inherited Bytes: 0x0 | Struct Size: 0x10
struct FQualifiedFrameTime {
    // Fields
    struct FFrameTime Time; // Offset: 0x0 | Size: 0x8
    struct FFrameRate Rate; // Offset: 0x8 | Size: 0x8
};

// Object: ScriptStruct CoreUObject.Timecode
// Inherited Bytes: 0x0 | Struct Size: 0x14
struct FTimecode {
    // Fields
    int32_t Hours; // Offset: 0x0 | Size: 0x4
    int32_t Minutes; // Offset: 0x4 | Size: 0x4
    int32_t Seconds; // Offset: 0x8 | Size: 0x4
    int32_t Frames; // Offset: 0xc | Size: 0x4
    bool bDropFrameFormat; // Offset: 0x10 | Size: 0x1
    uint8 pad_0x11[0x3]; // Offset: 0x11 | Size: 0x3
};

// Object: ScriptStruct CoreUObject.Timespan
// Inherited Bytes: 0x0 | Struct Size: 0x8
struct FTimespan {
    // Fields
    uint8 pad_0x0[0x8]; // Offset: 0x0 | Size: 0x8
};

// Object: ScriptStruct CoreUObject.SoftObjectPath
// Inherited Bytes: 0x0 | Struct Size: 0x18
struct FSoftObjectPath {
    // Fields
    class FName AssetPathName; // Offset: 0x0 | Size: 0x8
    class FString SubPathString; // Offset: 0x8 | Size: 0x10
};

// Object: ScriptStruct CoreUObject.SoftClassPath
// Inherited Bytes: 0x18 | Struct Size: 0x18
struct FSoftClassPath : FSoftObjectPath {
};




// Object: ScriptStruct Engine.DepthFieldGlowInfo
// Inherited Bytes: 0x0 | Struct Size: 0x24
struct FDepthFieldGlowInfo {
    // Fields
    char bEnableGlow : 1; // Offset: 0x0 | Size: 0x1
    char pad_0x0_1 : 7; // Offset: 0x0 | Size: 0x1
    char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
    struct FLinearColor GlowColor; // Offset: 0x4 | Size: 0x10
    struct FVector2D GlowOuterRadius; // Offset: 0x14 | Size: 0x8
    struct FVector2D GlowInnerRadius; // Offset: 0x1c | Size: 0x8
    
    FDepthFieldGlowInfo()
            : bEnableGlow(false)
            , GlowColor()
            , GlowOuterRadius()
            , GlowInnerRadius()
        { }
};

struct FFontRenderInfo {
    // Fields
    char bClipText : 1; // Offset: 0x0 | Size: 0x1
    char bDownscaleWrappedTextToFit : 1; // Offset: 0x0 | Size: 0x1
    char bEnableShadow : 1; // Offset: 0x0 | Size: 0x1
    char pad_0x0_3 : 5; // Offset: 0x0 | Size: 0x1
    char pad_0x1[0x3]; // Offset: 0x1 | Size: 0x3
    struct FDepthFieldGlowInfo GlowInfo; // Offset: 0x4 | Size: 0x24
    
    FFontRenderInfo()
    : bClipText(false), bEnableShadow(false), bDownscaleWrappedTextToFit(false)
        {}
};


enum class ETimerStatus : uint8
{
    Pending,
    Active,
    Paused,
    Executing,
    ActivePendingRemoval
};

struct FTimerHandle
{
    uint64_t Handle;
    
    bool IsValid() const
    {
        return Handle != 0;
    }

    bool operator==(const FTimerHandle& Other) const
    {
        return Handle == Other.Handle;
    }

    bool operator!=(const FTimerHandle& Other) const
    {
        return Handle != Other.Handle;
    }
    static const uint32 IndexBits        = 24;
    static const uint32 SerialNumberBits = 40;
    
    static const int32  MaxIndex        = (int32)1 << IndexBits;
    static const uint64 MaxSerialNumber = (uint64)1 << SerialNumberBits;
    
    FORCEINLINE int32 GetIndex() const
    {
        return (int32)(Handle & (uint64)(MaxIndex - 1));
    }

};

enum class ELevelCollectionType : uint8
{
    DynamicSourceLevels,
    DynamicDuplicatedLevels,
    StaticLevels,
    MAX
};

struct FTimerData
{
    uint8 bLoop : 1;
    uint8 bRequiresDelegate : 1;
    ETimerStatus Status;
    float Rate;
    double ExpireTime;
    uint8_t Pad[0x60];
    FTimerHandle Handle; // 0x70
    const void* TimerIndicesByObjectKey;
    ELevelCollectionType LevelCollection;

    FTimerData()
        : bLoop(false)
        , bRequiresDelegate(false)
        , Status(ETimerStatus::Active)
        , Rate(0)
        , ExpireTime(0)
        , LevelCollection(ELevelCollectionType::DynamicSourceLevels)
    {}

    FTimerData(FTimerData&&) = default;
    FTimerData(const FTimerData&) = delete;
    FTimerData& operator=(FTimerData&&) = default;
    FTimerData& operator=(const FTimerData&) = delete;
};


class FTimerManager
{
public:
    void** VTable;
    TSparseArray<FTimerData> Timers;
    TArray<FTimerHandle> ActiveTimerHeap;
    TSet<FTimerHandle> PausedTimerSet;
    TSet<FTimerHandle> PendingTimerSet;
    TMap<const void*, TSet<FTimerHandle>> ObjectToTimers;
    double InternalTime;
    FTimerHandle CurrentlyExecutingTimer;
    uint64 LastTickedFrame;
    UObject* OwningGameInstance;
    
    FTimerData* FindTimer(FTimerHandle const& InHandle);
};


#endif /* D774ECC7_7E08_44B8_9F86_DA69E0557911 */


