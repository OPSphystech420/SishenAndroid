#include "Main.h"
#include <thread>
#include <sys/mman.h>
#include <unistd.h>

#include "Tools/Memory.h"
#include "Tools/Timer.h"
#include "UnrealEngine/ScriptCore.h"
#include "GameStructs.h"
#include "StaticClasses.h"
#include "Offsets.h"


#include "../Includes/fmt/format.h"
#include "../Includes/fmt/xchar.h"

#include "../And64InlineHook/And64InlineHook.hpp"

#pragma mark - Utilities -


std::pair<int, int> GetMinAndSec(double Time)
{
    int Minutes = static_cast<int>(Time / 60);
    int Seconds = static_cast<int>(Time) % 60;
    return std::make_pair(Minutes, Seconds);
}

inline bool IsShooterCharacter(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 4;
}

inline bool IsShooterPlayerController(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 1;
}

inline bool IsPrimalDino(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 2;
}

inline bool IsPrimalCharacter(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 8;
}

inline bool IsPrimalStructure(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 16;
}

inline bool IsPrimalStructureExplosive(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 32;
}

inline bool IsPrimalCharacterOrStructure(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 24;
}

inline bool IsPrimalDinoOrStructure(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 18;
}

inline bool IsInstancedFoliage(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 64;
}

inline bool IsPrimalStructureOrShooterCharacter(UObject* Actor)
{
    return *(uint8_t*)((uint8*)Actor + 0x30) & 20;
}

FORCEINLINE FVector GetActorLocation(UObject* Actor)
{
    if (USceneComponent* RootComponent = *Actor->GetMember<USceneComponent*>(SPECK_OBFUSCATE("RootComponent")))
    {
        const FTransform& ComponentToWorld = *(FTransform*)((uint8*)RootComponent + Off::ComponentToWorld);
        return ComponentToWorld.GetLocation();
    }
    return FVector::Zero;
}

FORCEINLINE FRotator GetActorRotation(UObject* Actor)
{
    if (USceneComponent* RootComponent = *Actor->GetMember<USceneComponent*>("RootComponent"))
    {
        const FTransform& ComponentToWorld = *(FTransform*)((uint8*)RootComponent + Off::ComponentToWorld);
        return ComponentToWorld.Rotation.Rotator();
    }
    return {};
}

UShooterEngine* GetEngine()
{
    static UShooterEngine* ShooterEngine = nullptr;
    if (ShooterEngine)
        return ShooterEngine;

    ShooterEngine = UObject::FindObject<UShooterEngine>("ShooterEngine Transient.ShooterEngine");
    return ShooterEngine;
}

UShooterGameViewportClient* GetGameViewportClient()
{
    static UShooterGameViewportClient* ShooterGameViewportClient = nullptr;
    if (ShooterGameViewportClient)
        return ShooterGameViewportClient;

    ShooterGameViewportClient = UObject::FindObject<UShooterGameViewportClient>("ShooterGameViewportClient Transient.ShooterEngine.ShooterGameViewportClient");
    return ShooterGameViewportClient;
}

UWorld* GetWorld()
{
    UShooterGameViewportClient* GameViewport = GetGameViewportClient();
    if (!GameViewport)
        return nullptr;

    return *GameViewport->GetMember<UWorld*>("World");
}

UShooterLocalPlayer* GetLocalPlayer()
{
    static UShooterLocalPlayer* LocalPlayer = nullptr;
    if (LocalPlayer)
        return LocalPlayer;

    LocalPlayer = UObject::FindObject<UShooterLocalPlayer>("ShooterLocalPlayer Transient.ShooterEngine.ShooterLocalPlayer");
    return LocalPlayer;
}

UShooterGameUserSettings* GetGameUserSettings()
{
    static UShooterGameUserSettings* GameUserSettings = nullptr;
    if (GameUserSettings)
        return GameUserSettings;

    GameUserSettings = UObject::FindObject<UShooterGameUserSettings>("ShooterGameUserSettings Transient.ShooterGameUserSettings");
    return GameUserSettings;
}

AShooterPlayerController* GetPlayerController()
{
    UShooterLocalPlayer* LP = GetLocalPlayer();
    if (!LP)
        return nullptr;

    auto PC = *LP->GetMember<AShooterPlayerController*>("PlayerController");
    if (!PC || !IsShooterPlayerController(PC))
        return nullptr;

    return PC;
}

AShooterCharacter* GetPlayerCharacter(AShooterPlayerController* PlayerController = nullptr)
{
    AShooterPlayerController *PC = PlayerController ? PlayerController : GetPlayerController();
    if (!PC)
        return nullptr;

    AShooterCharacter *Character = *PC->GetMember<AShooterCharacter*>("Character");
    if (!Character)
        return nullptr;

    if (IsShooterCharacter(Character))
        return Character;

    TWeakObjectPtr<AShooterCharacter> Rider = *Character->GetMember<TWeakObjectPtr<AShooterCharacter>>("Rider");
    return Rider.GetSafe();
}


FORCEINLINE TArray<UObject*> GetActors(ULevel* Level = nullptr, int ActorType = -1)
{
    if ( Level != nullptr && Level->GetBitMember("bIsVisible"))
        return *(TArray<UObject*>*)((uint8*)Level + 0x98);

    UWorld* World = GetWorld();
    if (!World)
        return {};

    ULevel* PersistentLevel = *World->GetMember<ULevel*>("PersistentLevel");
    if (!PersistentLevel && PersistentLevel->GetBitMember("bIsVisible"))
        return {};

    if (ActorType != -1)
    {
        APrimalWorldSettings* WorldSettings = *PersistentLevel->GetMember<APrimalWorldSettings*>("WorldSettings");
        if (WorldSettings)
        {
            return *(TArray<UObject*>*)(((uint8*)WorldSettings + 0x430) + ActorType * 0x10);
        }
    }

    return *(TArray<UObject*>*)((uint8*)PersistentLevel + 0x98);
}

bool IsAllied(AShooterCharacter* Character, int32 TeamID)
{
    AShooterPlayerState* PlayerState = *Character->GetMember<AShooterPlayerState*>("PlayerState");
    if (PlayerState && PlayerState->IsA(AShooterPlayerState::StaticClass()))
    {
        FTribeData* TribeData = PlayerState->GetMember<FTribeData>("MyTribeData");
        if (!TribeData->TribeAlliances)
            return false;

        for (const FTribeAlliance& Alliance : TribeData->TribeAlliances)
        {
            if (!Alliance.MembersTribeID)
                continue;

            for (int32 TribeID : Alliance.MembersTribeID)
            {
                if (TribeID == TeamID)
                    return true;
            }
        }
    }
    return false;
}

bool IsInServer()
{
    if (UWorld* World = GetWorld())
    {
        UNetDriver* NetDriver = *World->GetMember<UNetDriver*>("NetDriver");
        if (!NetDriver)
            return false;

        return *NetDriver->GetMember<UNetConnection*>("ServerConnection") != nullptr;
    }
    return false;
}


#pragma mark - Cheat -

UObject* (*GetInventoryUISelectedItemLocal)(UObject* _this) = nullptr;
UObject* (*GetInventoryUISelectedItemRemote)(UObject* _this) = nullptr;

void (*ClientCheatWalk)(UObject* _this) = nullptr;
void (*ClientCheatGhost)(UObject* _this) = nullptr;
void (*SetReplicateMovement)(UObject* _this, bool bInReplicateMovement) = nullptr;
void (*SetReplicates)(UObject* _this, bool bInReplicates) = nullptr;
void (*ServerEquipPawnItem)(AShooterPlayerController* _this, FItemNetID ItemID) = nullptr;
void (*ServerRequestInventoryUseItem)(AShooterPlayerController* _this, UPrimalInventoryComponent* InventoryComp, FItemNetID ItemID);
void (*ServerTransferAllFromRemoteInventory)(AShooterPlayerController* _this, UPrimalInventoryComponent* InventoryComp, FString const& CurrentCustomFolderFilter, FString const& CurrentNameFilter, FString const& CurrentDestinationFolder, bool bNoFolderView);
void (*ServerRequestActorItems)(AShooterPlayerController* _this, UPrimalInventoryComponent* forInventory, bool bInventoryItems, bool bWithFirstSpawn);
void (*ServerMultiUse)(AShooterPlayerController* _this, UObject* ForObject, int32_t UseIndex);
void (*SendToConsole)(AShooterPlayerController* _this, FString const& Command);
bool (*GetInstanceTransform)(UObject* _this, int32_t InstanceIndex, FTransform* OutInstanceTransform, bool bWorldSpace);
bool (*LocalIsPerMapExplorerNoteUnlocked)(int32_t ExplorerNoteIndex);
void (*PawnClientRestart)(UObject* _this);
void (*ServerAcknowledgePossession)(AShooterPlayerController* _this, UObject* Pawn);
bool (*LineOfSightTo)(AShooterPlayerController* _this, UObject* Other, FVector ViewPoint, bool bAlternateChecks);
void (*ClientSetControlRotation)(AShooterPlayerController* _this, FRotator NewRotation);

TFreedArray<UObject*> (*K2_GetComponentsByClass)(UObject* _this, UClass* ComponentClass);
APrimalWorldSettings* (*GetWorldSettings)(UWorld* _this, bool bCheckStreamingPersistent, bool bChecked);
UTexture2D* (*ImportBufferAsTexture2D)(TFreedArray<uint8> const& Buffer);
void (*Decode)(FFreedString const& Base64String, TFreedArray<uint8>& OutData);

/* Internal Rendering */
void (*K2_DrawText)(UObject*,UObject *,FString const&,FVector2D,FVector2D,FLinearColor,float,FLinearColor,FVector2D,bool,bool,bool,FLinearColor);
void (*K2_DrawTexture)(UObject *,UObject *,FVector2D,FVector2D,FVector2D,FVector2D,FLinearColor,EBlendMode,float,FVector2D);
void (*K2_DrawLine)(UObject *,FVector2D,FVector2D,float,FLinearColor);
FVector2D (*K2_TextSize)(UObject*, UObject*, const FString&, FVector2D) = nullptr;


UTexture2D* GetTexture2DFromBase64(const char16_t* Base64Str)
{
    TFreedArray<uint8> Buffer;
    FFreedString Base64(Base64Str);
    Decode(Base64, Buffer);

    UTexture2D* Texture = ImportBufferAsTexture2D(Buffer);
    if (Texture)
    {
        const_cast<FUObjectItem*>(UObject::GUObjectArray->ObjObjects.IndexToObject(Texture->InternalIndex))->SetRootSet();
    }
    return Texture;
}

const char16_t* SleepIconBase64 = u"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";


namespace Automatics
{
    void ArmorEquipment(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void MedConsumption(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void RemountingDino(AShooterCharacter* MySelf, AShooterPlayerController* PC);

    void DinosaurHealing(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void Looting(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void UnlockNearbyNotes(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void TurretSettings(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void PickupEggs(AShooterCharacter* MySelf, AShooterPlayerController* PC);
    void RemountDinosaur(AShooterCharacter* MySelf, AShooterPlayerController* PC);
}


void Automatics::ArmorEquipment(AShooterCharacter* MySelf, AShooterPlayerController* PC)
{
    UPrimalInventoryComponent* MyInventoryComponent = *MySelf->GetMember<UPrimalInventoryComponent*>("MyInventoryComponent");
    if (!MyInventoryComponent)
        return;

    TArray<UPrimalItem*> InventoryItems = *MyInventoryComponent->GetMember<TArray<UPrimalItem*>>("InventoryItems");
    TArray<UPrimalItem*> EquippedItems  = *MyInventoryComponent->GetMember<TArray<UPrimalItem*>>("EquippedItems");

    UPrimalItem* NullEquip[5] = {};
    UPrimalItem* QueueEquip[5] = {};

    for (int i = 0; i < EquippedItems.Num(); ++i)
    {
        UPrimalItem* EquippedItem = EquippedItems[i];
        if (!EquippedItem)
            continue;

        NullEquip[*EquippedItem->GetMember<uint8>("MyEquipmentType")] = EquippedItem;
    }

    for (int i = 0; i < InventoryItems.Num(); ++i)
    {
        UPrimalItem* InventoryItem = InventoryItems[i];
        if (!InventoryItem)
            continue;

        float ItemDurability = *InventoryItem->GetMember<float>("ItemDurability");
        if (ItemDurability == 0.0f)
            continue;

        uint8 ItemIndex = *InventoryItem->GetMember<uint8>("MyEquipmentType");

        bool bIsBlueprint       = InventoryItem->GetBitMember("bIsBlueprint");
        bool bAllowEquppingItem = InventoryItem->GetBitMember("bAllowEquppingItem");
        bool bIsEngram          = InventoryItem->GetBitMember("bIsEngram");

        if (ItemIndex > 4 || bIsBlueprint || !bAllowEquppingItem || bIsEngram)
            continue;

        if (!NullEquip[ItemIndex] || *NullEquip[ItemIndex]->GetMember<float>("ItemDurability") < ItemDurability)
        {
            QueueEquip[ItemIndex] = InventoryItem;
        }
    }

    for (uint8 i = 0; i < 5; ++i)
    {
        if (QueueEquip[i])
        {
            ServerEquipPawnItem(PC, *QueueEquip[i]->GetMember<FItemNetID>("ItemID"));
        }
    }
}

void Automatics::MedConsumption(AShooterCharacter* MySelf, AShooterPlayerController* PC)
{
    UPrimalInventoryComponent* InventoryComponent = *MySelf->GetMember<UPrimalInventoryComponent*>("MyInventoryComponent");
    if (!InventoryComponent)
        return;

    TArray<UPrimalItem*> ItemSlots      = *InventoryComponent->GetMember<TArray<UPrimalItem*>>("ItemSlots");
    TArray<UPrimalItem*> InventoryItems = *InventoryComponent->GetMember<TArray<UPrimalItem*>>("InventoryItems");
    if (!ItemSlots || !InventoryItems)
        return;

    UPrimalCharacterStatusComponent* StatusComponent = *MySelf->GetMember<UPrimalCharacterStatusComponent*>("MyCharacterStatusComponent");
    if (!StatusComponent)
        return;

    static constexpr float BrewHealthRegen = 40.0f;
    static float MaxHealth = 0.0f;
    static float CurrentFrameHealth = 0.0f;
    static float LastFrameHealth = 0.0f;
    static float HealthAfterBrew = 0.0f;
    static int DrinkAmount = 0;

    auto CalculateAutoBrew = []() -> int
    {
        int BrewsToDrink = 0;
        if ((LastFrameHealth - CurrentFrameHealth) > 0)
        {
            HealthAfterBrew -= (LastFrameHealth - CurrentFrameHealth);

            BrewsToDrink = (MaxHealth - HealthAfterBrew) / BrewHealthRegen;
            HealthAfterBrew += (BrewHealthRegen * BrewsToDrink);
        }
        LastFrameHealth = CurrentFrameHealth;
        return BrewsToDrink;
    };

    CurrentFrameHealth  = *StatusComponent->GetMember<float>("CurrentStatusValues");
    MaxHealth           = *StatusComponent->GetMember<float>("MaxStatusValues");
    DrinkAmount         = CalculateAutoBrew();

    if (DrinkAmount <= 0)
        return;

    static FName PrimalItemConsumable_HealSoup_C = FName(u"PrimalItemConsumable_HealSoup_C");

    for (UObject* Item : ItemSlots)
    {
        if (!Item || Item->NamePrivate != PrimalItemConsumable_HealSoup_C)
            continue;

        return ServerRequestInventoryUseItem(PC, InventoryComponent, *Item->GetMember<FItemNetID>("ItemID"));
    }

    for (UObject* Item : InventoryItems)
    {
        if (!Item || Item->NamePrivate != PrimalItemConsumable_HealSoup_C)
            continue;

        return ServerRequestInventoryUseItem(PC, InventoryComponent, *Item->GetMember<FItemNetID>("ItemID"));
    }
}

void Automatics::Looting(AShooterCharacter *MySelf, AShooterPlayerController *PC)
{
    FVector MyLocation = GetActorLocation(MySelf);
    int32 MyTargetingTeam = *MySelf->GetMember<int32>("TargetingTeam");

    TArray<UObject*> Actors = GetActors();
    for (UObject* Actor : Actors)
    {
        if (!Actor || !IsPrimalStructureOrShooterCharacter(Actor))
            continue;

        if (!MySelf || !PC)
            break;

        if (Actor == MySelf)
            continue;

        if (IsShooterCharacter(Actor) && settings.LootPlayers)
        {
            AShooterCharacter* Player = (AShooterCharacter*)Actor;

            if (Player->GetBitMember("bIsSleeping") || Player->GetBitMember("bIsDead"))
                continue;

            int32 TargetingTeam = *Player->GetMember<int32>("TargetingTeam");
            if (MyTargetingTeam == TargetingTeam || IsAllied(MySelf, TargetingTeam))
                continue;

            FVector Location = GetActorLocation(Player);
            if (Location.GetDistanceTo(MyLocation) > 900.0f)
                continue;

            UPrimalInventoryComponent* MyInventoryComponent = *Player->GetMember<UPrimalInventoryComponent*>("MyInventoryComponent");
            if (MyInventoryComponent)
            {
                ServerTransferAllFromRemoteInventory(PC, MyInventoryComponent, u"", u"", u"", true);
            }
        }
        else if (Actor->IsA(APrimalStructureItemContainer::StaticClass()))
        {
            APrimalStructureItemContainer* Container = (APrimalStructureItemContainer*)Actor;

            const bool IsTurret = Container->IsA(APrimalStructureTurret::StaticClass());
            if (!settings.LootTurrets && IsTurret)
                continue;

            const bool IsSupplyCrate = Container->IsA(APrimalStructureItemContainer_SupplyCrate::StaticClass());
            if (IsSupplyCrate && !settings.LootSupplyCrates)
                continue;

            if (!IsTurret && !IsSupplyCrate && !settings.LootContainers)
                continue;

            int32 TargetingTeam = *Container->GetMember<int32>("TargetingTeam");
            if (MyTargetingTeam == TargetingTeam || IsAllied(MySelf, TargetingTeam))
                continue;

            FVector Location = GetActorLocation(Container);
            if (Location.GetDistanceTo(MyLocation) > 900.0f)
                continue;

            UPrimalInventoryComponent* MyInventoryComponent = *Container->GetMember<UPrimalInventoryComponent*>("MyInventoryComponent");
            if (MyInventoryComponent)
            {
                if ( IsSupplyCrate )
                {
                    ServerRequestActorItems(PC, MyInventoryComponent, true, false);
                    ServerTransferAllFromRemoteInventory(PC, MyInventoryComponent, u"", u"", u"", true);
                    continue;
                }

                if (Container->GetBitMember("bIsLocked") || Container->GetBitMember("bIsPinLocked"))
                    continue;

                ServerTransferAllFromRemoteInventory(PC, MyInventoryComponent, u"", u"", u"", true);
            }
        }

    }
}

void Automatics::RemountDinosaur(AShooterCharacter* MySelf, AShooterPlayerController* PC)
{
    if (MySelf->GetBitMember("bIsRiding"))
        return;

    TArray<UObject*> TamedDinosaurs = GetActors(nullptr, AL_TAMED_DINOS);
    if (!TamedDinosaurs)
        return;

    FVector MyLocation = GetActorLocation(MySelf);
    int32 MyTargetingTeam = *MySelf->GetMember<int32>("TargetingTeam");

    float CurrentDistance = FLT_MAX;
    UObject* Dino = nullptr;

    for (UObject* TamedDino : TamedDinosaurs)
    {
        if (!TamedDino)
            continue;

        if (TamedDino->GetBitMember("bIsDead"))
            continue;

        if (MyTargetingTeam != *TamedDino->GetMember<int32>("TargetingTeam"))
            continue;

        FVector Location = GetActorLocation(TamedDino);

        float Distance = MyLocation.GetDistanceTo(Location);
        if (Distance > 800.0f)
            continue;

        if (Distance < CurrentDistance)
        {
            Dino = TamedDino;
            CurrentDistance = Distance;
        }
    }

    if (!Dino)
        return;

    return ServerMultiUse(PC, Dino, 100);
}

void Automatics::PickupEggs(AShooterCharacter *MySelf, AShooterPlayerController *PC)
{
    FVector MyLocation = GetActorLocation(MySelf);

    TArray<UObject*> Actors = GetActors();
    for (UObject* Actor : Actors)
    {
        if (!Actor || IsPrimalCharacterOrStructure(Actor))
            continue;

        if (!Actor->IsA(ADroppedItem::StaticClass()))
            continue;

        UObject* MyItem = *Actor->GetMember<UObject*>("MyItem");
        if (!MyItem || !MyItem->GetBitMember("bIsEgg"))
            continue;

        FVector Location = GetActorLocation(Actor);
        if (MyLocation.GetDistanceTo(Location) > 800.0f)
            continue;

        return ServerMultiUse(PC, Actor, 100);
    }
}

FORCEINLINE void ForceOptimize(AShooterPlayerController *PC)
{
    static bool bAlreadyExecuted = false;
    if (bAlreadyExecuted)
        return;

    const TCHAR* Commands[]
    {
            u"foliage.disableculling 0",
            u"r.Foliage.DensityScale 0",
            u"grass.density 0",
            u"r.ForceLOD 2",
            u"r.StaticMeshLODDistanceScale 10",
            u"r.ShadowQuality 0",
            u"r.LightFunctionQuality 0",
            u"r.SSAOQuality 0",
            u"r.PostProcessAAQuality 2",
            u"r.TonemapperQuality 0",
            u"r.ReflectionEnvironment 0",
            u"r.MotionBlurQuality 0",
            u"r.DepthOfFieldQuality 0",
            u"t.MaxFPS 60",
            u"r.VSync 1",
            u"r.DisableAllScreenMessages 1",
            u"r.SkyLightingQuality 0",
            u"r.TranslucencyLightingVolume 0",
            u"r.DistanceFieldAO 0",
            u"r.ContactShadows 0",
            u"r.LensFlareQuality 0",
            u"grass.off",
            u"r.WaterRefractionQuality 0",
            u"grass.enable 0",
            u"sg.GroundClutterQuality 0",
            u"r.fog 0",
            u"sg.GlobalIlluminationQuality 1",
            u"r.BloomQuality 0",
            u"r.LightShafts 0",
            u"r.VolumetricCloud 0",
            u"r.AmbientOcclusionLevels 0",
            u"r.VolumetricFog 0",
            u"r.Atmosphere 0",
            u"r.SSR.Quality 0",
            u"r.MipMapLODBias 1",
            u"r.DynamicGlobalIlluminationMethod 1",
            u"r.Nanite.MaxPixelsPerEdge 1",
            u"r.Shadow.CSM.MaxCascades 0",
            u"r.Water.SingleLayer.Reflection 0",
            u"FX.MaxCPUParticlesPerEmitter 0",
            u"fx.MaxNiagaraGPUParticlesSpawnPerFrame 0",
            u"sg.FoliageQuality 0",
            u"wp.Runtime.UpdateStreamingSources 0",
            u"ShowFlag.Materials 0",
            u"FogDensity 0.0",
            u"r.SkyAtmosphere 0",
            u"r.TrueSkyQuality 0",
            u"r.HZBOcclusion 0",
            u"r.DetailMode 0",
            u"r.PostProcessing.DisableMaterials 1",
            u"r.SceneColorFringeQuality 0",
            u"r.DefaultFeature.PostProcessing False",
            u"r.DefaultFeature.AutoExposure False",
            u"r.ExposureOffset 1.5",
            u"r.SkylightIntensityMultiplier 10",
            u"slate.contrast 1",
            u"foliage.LODDistanceScale .7",
            u"r.Nanite.MaxPixelsPerEdge 4",
            u"sg.AntiAliasingQuality 0",
            u"sg.EffectsQuality 0",
            u"sg.IBLQuality 0",
            u"r.SSAOSmartBlur 0",
            u"sg.ResolutionQuality 10",
            u"sg.PostProcessQuality 0",
            u"sg.TextureQuality 0",
            u"sg.TrueSkyQuality 0",
            u"sg.HeightFieldShadowQuality 0",
            u"r.EarlyZPass 0",
            u"r.SSS.Scale 0",
            u"r.SSS.SampleSet 0",
            u"r.MaxAnisotropy 0",
            u"r.oneframethreadlag 1",
            u"r.simpledynamiclighting 1",
            u"r.LightShaftQuality 0",
            u"r.RefractionQuality 0",
            u"r.UpsampleQuality 0",
            u"grass.sizedensity 0",
            u"grass.sizescale 0",
            u"r.postprocessing.disable_motionblur 0",
            u"r.postprocessing.disablematerials 0",
            u"r.lumen.reflections.allow 0",
            u"r.dynamicglobalilluminationmethod 0",
            u"r.MaterialQualityLevel 1",
            u"R.streaming.poolsize 0"
    };

    for (const TCHAR* Command : Commands)
    {
        SendToConsole(PC, Command);
    }

    bAlreadyExecuted = true;
}

UCanvas* GetDebugCanvasObject()
{
    static UCanvas* DebugCanvasObject = nullptr;
    if (!DebugCanvasObject)
        DebugCanvasObject = UObject::FindObjectFast<UCanvas>("DebugCanvasObject");

    return DebugCanvasObject;
}

FLinearColor GetColorByQuality(uint8 Quality)
{
    switch (Quality)
    {
        case 1: return FLinearColor::White; break;
        case 2: return FLinearColor::Green; break;
        case 3: return FLinearColor::Blue; break;
        case 5: return FLinearColor::Purple; break;
        case 4: return FLinearColor::Yellow; break;
        case 6: return FLinearColor::Cyan; break;
        default:
            return FLinearColor::White;
            break;
    }
}

UFont* GetRenderFont()
{
    static UFont* Font = nullptr;
    if (Font == nullptr)
        Font = UObject::FindObject<UFont>("Font ArkDefaultFont.ArkDefaultFont");

    return Font;
}

FORCEINLINE const FMatrix& GetViewProjectionMatrix(UCanvas* Canvas = nullptr)
{
    UCanvas* DebugCanvasObject = Canvas ? Canvas : GetDebugCanvasObject();
    if (DebugCanvasObject)
    {
        return *(FMatrix*)((uint8*)DebugCanvasObject + Off::ViewProjectionMatrix);
    }
    return FMatrix::Identity;
}

constexpr int32 MaleBoneIndices[15] =
{
    static_cast<int32>(MaleBones::Cnt_Neck_Joint001_JNT_SKL),
    static_cast<int32>(MaleBones::Cnt_Neck_Joint000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Arm_001Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Arm_001Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Arm_002Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Arm_002Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Arm_002Tear006_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Arm_002Tear006_JNT_SKL),
    static_cast<int32>(MaleBones::Cnt_Pelvis_000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Leg_001Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Leg_001Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Leg_002Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Leg_002Tear000_JNT_SKL),
    static_cast<int32>(MaleBones::Lft_Leg_002_JNT_SKL),
    static_cast<int32>(MaleBones::Rht_Leg_002_JNT_SKL)
};

constexpr int32 FemaleBoneIndices[15] =
{
    static_cast<int32>(FemaleBones::Cnt_Neck_Joint001_JNT_SKL),
    static_cast<int32>(FemaleBones::Cnt_Neck_Joint000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Arm_001Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Arm_001Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Arm_002Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Arm_002Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Arm_002Tear006_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Arm_002Tear006_JNT_SKL),
    static_cast<int32>(FemaleBones::Cnt_Pelvis_000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Leg_001Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Leg_001Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Leg_002Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Leg_002Tear000_JNT_SKL),
    static_cast<int32>(FemaleBones::Lft_Leg_002_JNT_SKL),
    static_cast<int32>(FemaleBones::Rht_Leg_002_JNT_SKL)
};



FVector GetBoneLocation(USkeletalMeshComponent* Mesh, int32 BoneIndex, const FTransform& LocalToWorld)
{
    if (!Mesh)
        return FVector::Zero;

    TArray<FTransform> Transforms = *(TArray<FTransform>*)((uint8*)Mesh + Off::ComponentSpaceTransformsArray);
    if ( !Transforms )
        Transforms = *(TArray<FTransform>*)((uint8*)Mesh + (Off::ComponentSpaceTransformsArray + 0x10));

    if (Transforms && Transforms.IsValidIndex(BoneIndex))
    {
        return Transforms[BoneIndex] * LocalToWorld;
    }
    return FVector::Zero;
}

std::vector<FOtherActorData> OtherDrawData;
std::vector<FOtherActorData> BackupOtherDrawData;

std::vector<FPlayerActorData> PlayerDrawData;
std::vector<FPlayerActorData> BackupPlayerDrawData;

std::mutex TraceMutex;
std::mutex DataMutex;

FORCEINLINE void ClearVectors(bool bBackupsAswell = false)
{
    std::lock_guard<std::mutex> lock(DataMutex);

    OtherDrawData.clear();
    PlayerDrawData.clear();

    if (bBackupsAswell)
    {
        BackupPlayerDrawData.clear();
        BackupOtherDrawData.clear();
    }
}

/*
// Works but useless
UObject* GetWorld(UObject* Actor)
{
    if (!Actor->HasAnyFlags(EObjectFlags::ClassDefaultObject) &&
        !Actor->GetOuter()->HasAnyFlags(EObjectFlags::BeginDestroyed) &&
        !Actor->GetOuter()->IsUnreachable())
    {
        if (UObject* Level = Actor->GetTypedOuter(EClassCastFlags::Level))
        {
            return *Level->GetMember<UObject*>("OwningWorld");
        }
    }
    return nullptr;
}
 */

UTexture2D* SleepIcon = nullptr;

UTexture2D* GetIcon(EIconType IconType)
{
    static UTexture2D* GenderMale_Icon;
    static UTexture2D* GenderFemale_Icon;
    static UTexture2D* Knocked_Icon;
    static UTexture2D* Skull_Icon;

    switch (IconType)
    {
        case EIconType::GenderMale:
            if (!GenderMale_Icon) {
                GenderMale_Icon = UObject::FindObject<UTexture2D>("Texture2D Gender_Small_Male.Gender_Small_Male");
            }
            return GenderMale_Icon;

        case EIconType::GenderFemale:
            if (!GenderFemale_Icon) {
                GenderFemale_Icon = UObject::FindObject<UTexture2D>("Texture2D Gender_Small_Female.Gender_Small_Female");
            }
            return GenderFemale_Icon;

        case EIconType::Knocked:
            /*if (!Knocked_Icon) {
                Knocked_Icon = UObject::FindObject<UTexture2D>("Texture2D Torpid_Icon.Torpid_Icon");
            }*/
            return SleepIcon;

        case EIconType::Skull:
            if (!Skull_Icon) {
                Skull_Icon = UObject::FindObject<UTexture2D>("Texture2D SkullIcon.SkullIcon");
            }
            return Skull_Icon;
        default:
            return nullptr;
    }
}

bool ProjectWorldToScreen(UCanvas* Canvas, const FVector& WorldPosition, FVector2D& OutScreenPos, bool bShouldCalcOutsideViewPosition = false)
{
    const FMatrix& VPM = GetViewProjectionMatrix(Canvas);

    float W = VPM.M[0][3] * WorldPosition.X + VPM.M[1][3] * WorldPosition.Y + VPM.M[2][3] * WorldPosition.Z + VPM.M[3][3];

    bool bIsInsideView = W > 0.0f;

    if ( !bIsInsideView )
    {
        if ( !bShouldCalcOutsideViewPosition )
            return false;

        W = 0.01f;
    }

    float X = VPM.M[0][0] * WorldPosition.X + VPM.M[1][0] * WorldPosition.Y + VPM.M[2][0] * WorldPosition.Z + VPM.M[3][0];
    float Y = VPM.M[0][1] * WorldPosition.X + VPM.M[1][1] * WorldPosition.Y + VPM.M[2][1] * WorldPosition.Z + VPM.M[3][1];

    const float RHW = 1.0f / W;
    float PosInScreenSpaceX = X * RHW;
    float PosInScreenSpaceY = Y * RHW;

    const FVector2D HalfClip = GCanvasSize.Vector() * 0.5f;

    OutScreenPos.X = HalfClip.X + (HalfClip.X * PosInScreenSpaceX);
    OutScreenPos.Y = HalfClip.Y - (HalfClip.Y * PosInScreenSpaceY);

    return bIsInsideView;
}

FORCEINLINE void DrawLine(UCanvas* Canvas, const FVector2D& PosA, const FVector2D& PosB, float Thickness, const FLinearColor& Color)
{
    return K2_DrawLine(Canvas, PosA, PosB, Thickness, Color);
}

FORCEINLINE void DrawText(UCanvas* Canvas, UFont* Font, const FString& Text, const FVector2D& ScreenPosition, const FLinearColor& Color, float Scale, bool Outline = true, bool Shadow = false, bool Center = true)
{
    if ( !Font )
        return;

    return K2_DrawText(Canvas, Font, Text, ScreenPosition, FVector2D(Scale * 0.9f, Scale), Color, *Font->GetMember<float>("Kerning"), Shadow ? FLinearColor::Black : FLinearColor::Transparent, Shadow ? FVector2D(1.0f, 1.0f) : FVector2D::Zero, Center ? true : false, Center ? true : false, Outline, FLinearColor::Black);
}

FORCEINLINE void DrawTexture(UCanvas* Canvas, UTexture2D* RenderTexture, const FVector2D& ScreenPosition, const FVector2D& SizeXY, const FLinearColor& Color, bool bCenterX = true, bool bCenterY = true)
{
    FVector2D CorrectedPositon(bCenterX ? ScreenPosition.X - (SizeXY.X * 0.5f) : ScreenPosition.X, bCenterY ? ScreenPosition.Y - SizeXY.Y : ScreenPosition.Y);
    return K2_DrawTexture(Canvas, RenderTexture, CorrectedPositon, SizeXY, FVector2D(0, 0), FVector2D(1, 1), Color, EBlendMode::BLEND_Translucent, 0.0f, FVector2D(0.5f, 0.5f));
}

FORCEINLINE void DrawRectFilled(UCanvas* Canvas, const FVector2D& ScreenPosition, const FVector2D& SizeXY, const FLinearColor& Color)
{
    return K2_DrawLine(Canvas, ScreenPosition, FVector2D(ScreenPosition.X + SizeXY.X, ScreenPosition.Y), SizeXY.Y, Color);
}

void Draw3DBox(UCanvas* Canvas, UObject* Player, const FLinearColor& BoxColor)
{
    if (!Canvas || !Player)
        return;

    UObject* Mesh = *Player->GetMember<UObject*>("Mesh");
    if (!Mesh)
        return;

    const FBoxSphereBounds& Bounds = *(FBoxSphereBounds*)((uint8*)Mesh + Off::Bounds);
    const FQuat& Rotation = ((FTransform*)((uint8*)Mesh + Off::ComponentToWorld))->Rotation;

    const FVector Origin = Bounds.Origin;
    const FVector Extent = Bounds.BoxExtent;

    const FVector X = Rotation.RotateVector(FVector(Extent.X, 0, 0));
    const FVector Y = Rotation.RotateVector(FVector(0, Extent.Y, 0));
    const FVector Z = Rotation.RotateVector(FVector(0, 0, Extent.Z));

    FVector CornerXZ_Neg  = Origin - X - Z;
    FVector CornerXZ_Pos  = Origin + X - Z;
    FVector CornerXZ_NegZ = Origin - X + Z;
    FVector CornerXZ_PosZ = Origin + X + Z;

    FVector Corners[8];
    Corners[0] = CornerXZ_Neg  - Y;
    Corners[1] = CornerXZ_Pos  - Y;
    Corners[2] = CornerXZ_Pos  + Y;
    Corners[3] = CornerXZ_Neg  + Y;
    Corners[4] = CornerXZ_NegZ - Y;
    Corners[5] = CornerXZ_PosZ - Y;
    Corners[6] = CornerXZ_PosZ + Y;
    Corners[7] = CornerXZ_NegZ + Y;


    FVector2D Screen[8];
    for (int i = 0; i < 8; i++)
    {
        if (!ProjectWorldToScreen(Canvas, Corners[i], Screen[i]))
            return;
    }

    static constexpr int Edges[12][2] =
    {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    constexpr float Thickness = 2.0f;
    for (const auto& Edge : Edges)
    {
        K2_DrawLine(Canvas, Screen[Edge[0]], Screen[Edge[1]], Thickness, BoxColor);
    }
}


void Draw2DBox(UCanvas* Canvas, UObject* Player, FLinearColor const& BoxColor)
{
    if (!Canvas || !Player)
        return;

    UObject* SkelMesh = *Player->GetMember<UObject*>("Mesh");
    if (!SkelMesh)
        return;

    const FBoxSphereBounds& Bounds = *(FBoxSphereBounds*)((uint8*)SkelMesh + Off::Bounds);
    const FQuat& Rotation = ((FTransform*)((uint8*)SkelMesh + Off::ComponentToWorld))->Rotation;

    FVector RotatedBoxExtent = Rotation.RotateVector(Bounds.BoxExtent);

    FVector WorldMin = Bounds.Origin - RotatedBoxExtent;
    FVector WorldMax = Bounds.Origin + RotatedBoxExtent;

    FVector2D ScreenMin, ScreenMax;
    if (!ProjectWorldToScreen(Canvas, WorldMin, ScreenMin) ||
        !ProjectWorldToScreen(Canvas, WorldMax, ScreenMax))
    {
        return;
    }

    FVector2D TopLeft     = FVector2D(ScreenMin.X, ScreenMin.Y);
    FVector2D TopRight    = FVector2D(ScreenMax.X, ScreenMin.Y);
    FVector2D BottomLeft  = FVector2D(ScreenMin.X, ScreenMax.Y);
    FVector2D BottomRight = FVector2D(ScreenMax.X, ScreenMax.Y);

    constexpr float Thickness = 2.0f;

    K2_DrawLine(Canvas, TopLeft, TopRight, Thickness, BoxColor);
    K2_DrawLine(Canvas, TopRight, BottomRight, Thickness, BoxColor);
    K2_DrawLine(Canvas, BottomRight, BottomLeft, Thickness, BoxColor);
    K2_DrawLine(Canvas, BottomLeft, TopLeft, Thickness, BoxColor);
}

void DrawSkeleton(UCanvas* Canvas, AShooterCharacter* Player, const FLinearColor& Color)
{
    if (!Player)
        return;

    USkeletalMeshComponent* Mesh = *Player->GetMember<USkeletalMeshComponent*>("Mesh");
    if (!Mesh)
        return;

    const int32* BoneIndices = Player->GetBitMember("bIsFemale") ? FemaleBoneIndices : MaleBoneIndices;

    const FTransform& ComponentToWorld = *(FTransform*)((uint8*)Mesh + Off::ComponentToWorld);

    FVector2D BonePositions[15];
    for (int i = 0; i < 15; ++i)
    {
        FVector WorldBonePosition = GetBoneLocation(Mesh, BoneIndices[i], ComponentToWorld);
        if (!ProjectWorldToScreen(Canvas, WorldBonePosition, BonePositions[i]))
            return;
    }

    static constexpr int LinePairs[14][2] =
    {
            {0, 1}, {1, 2}, {1, 3}, {2, 4}, {3, 5},
            {4, 6}, {5, 7}, {1, 8}, {8, 9}, {8, 10},
            {9, 11}, {10, 12}, {11, 13}, {12, 14}
    };

    for (const auto& Pair : LinePairs)
    {
        FVector2D Start = FVector2D(BonePositions[Pair[0]].X, BonePositions[Pair[0]].Y);
        FVector2D End   = FVector2D(BonePositions[Pair[1]].X, BonePositions[Pair[1]].Y);

        DrawLine(Canvas, Start, End, 3.0f, Color);
    }
}

void DrawHealthBar(UObject* Canvas, const FVector2D& ScreenPosition, const FVector2D& SizeXY, const FLinearColor& Color, float HealthPercentage, float Thickness)
{
    FVector2D CenteredPos(ScreenPosition.X - SizeXY.X * 0.5f, ScreenPosition.Y);

    K2_DrawLine(Canvas, CenteredPos, FVector2D(CenteredPos.X + SizeXY.X, CenteredPos.Y), SizeXY.Y + Thickness * 2, FLinearColor::Black);

    FVector2D HealthBarSize(SizeXY.X * HealthPercentage, SizeXY.Y);
    K2_DrawLine(Canvas, CenteredPos, FVector2D(CenteredPos.X + HealthBarSize.X, CenteredPos.Y), SizeXY.Y, Color);
}

void DrawWeapon(UCanvas* Canvas, UObject* Player, const FVector2D& ScreenPosition, const FVector2D& SizeXY)
{
    if (!Player)
        return;

    AShooterWeapon* CurrentWeapon = *Player->GetMember<AShooterWeapon*>("CurrentWeapon");
    if (!CurrentWeapon)
        return;

    UPrimalItem* WeaponItem = *CurrentWeapon->GetMember<UPrimalItem*>("AssociatedPrimalItem");
    if ( !WeaponItem )
        return;

    UTexture2D* Texture = *WeaponItem->GetDefault()->GetMember<UTexture2D*>("ItemIcon");
    if (!Texture)
        return;

    static UTexture2D* ItemQualityBg = nullptr;
    if (ItemQualityBg == nullptr)
        ItemQualityBg = UObject::FindObject<UTexture2D>("Texture2D Slot_Item_Quality_Overlay_BG.Slot_Item_Quality_Overlay_BG");

    FLinearColor Color = GetColorByQuality(*WeaponItem->GetMember<uint8>("ItemQualityIndex"));

    DrawTexture(Canvas, ItemQualityBg, ScreenPosition, SizeXY + (SizeXY * 0.20f), Color);
    DrawTexture(Canvas, Texture, ScreenPosition, SizeXY, FLinearColor::White);
}

void DrawArmor(UCanvas* Canvas, AShooterCharacter* Player, const FVector2D& ScreenPosition, const FVector2D& SizeXY)
{
    if (!Player)
        return;

    UPrimalInventoryComponent* MyInventoryComponent = *Player->GetMember<UPrimalInventoryComponent*>("MyInventoryComponent");
    if (!MyInventoryComponent)
        return;

    TArray<UPrimalItem*> EquippedItems = *MyInventoryComponent->GetMember<TArray<UPrimalItem*>>("EquippedItems");
    if (!EquippedItems)
        return;

    auto GetSlotIndex = [](UPrimalItem* Item) -> int32
    {
        switch (*Item->GetMember<EPrimalEquipmentType>("MyEquipmentType"))
        {
            case EPrimalEquipmentType::Hat: return 0;
            case EPrimalEquipmentType::Shirt: return 1;
            case EPrimalEquipmentType::Gloves: return 2;
            case EPrimalEquipmentType::Pants: return 3;
            case EPrimalEquipmentType::Boots: return 4;
            default: return -1;
        }
    };

    std::array<UPrimalItem*, 5> ValidArmorItems = {nullptr, nullptr, nullptr, nullptr, nullptr};
    int Index = 0;

    for (UPrimalItem* Armor : EquippedItems)
    {
        if (Armor)
        {
            int32 SlotIndex = GetSlotIndex(Armor);
            if (SlotIndex >= 0 && Index < ValidArmorItems.size())
                ValidArmorItems[Index++] = Armor;
        }
    }

    if (Index == 0)
        return;

    std::sort(ValidArmorItems.begin(), ValidArmorItems.begin() + Index, [&](UPrimalItem* A, UPrimalItem* B) {
        return GetSlotIndex(A) < GetSlotIndex(B);
    });

    const float TotalWidth = Index * SizeXY.X;
    const float StartX = ScreenPosition.X - (TotalWidth / 2.0f);

    UFont* Font = GetRenderFont();

    for (int32 i = 0; i < Index; ++i)
    {
        UPrimalItem* Armor = ValidArmorItems[i];
        if (!Armor)
            continue;

        UTexture2D* Texture = *Armor->GetDefault()->GetMember<UTexture2D*>("ItemIcon");
        if (!Texture)
            continue;

        FLinearColor Color = GetColorByQuality(*Armor->GetMember<uint8>("ItemQualityIndex"));
        Color.A = 0.5f;

        const FVector2D DrawPosition(
                StartX + (i * SizeXY.X),
                ScreenPosition.Y + (SizeXY.Y * 1.75f)
        );

        DrawTexture(Canvas, nullptr, DrawPosition, SizeXY, Color, false);
        DrawTexture(Canvas, Texture, DrawPosition, SizeXY, FLinearColor::White, false);

        int32 ItemDurability = int32(*Armor->GetMember<float>("ItemDurability"));
        const FVector2D TextPosition(
                StartX + (i * SizeXY.X) + (SizeXY.X * 0.5f),
                ScreenPosition.Y + (SizeXY.Y * 2.f)
        );

        DrawText(Canvas, Font, fmt::format(u"{}", ItemDurability).c_str(), TextPosition, FLinearColor::White, 0.75f);
    }
}

const std::unordered_map<std::string, EActorSpecificType> ResourceTypes =
{
        {"MetalHarvestComponent_C", EActorSpecificType::Metal},
        {"MetalHarvestComponent_Rich_C", EActorSpecificType::Metal},
        {"MountainObsidianHarvestComponent_C", EActorSpecificType::Obsidian},
        {"SiliconHarvestComponent_C", EActorSpecificType::Perl},
        {"OilHarvestComponent_C", EActorSpecificType::Oil},
        {"OilHarvestComponentRich_C", EActorSpecificType::Oil},
        {"OilHarvestComponentUnderwater_C", EActorSpecificType::Oil},
        {"ObsidianHarvestComponent_C", EActorSpecificType::Obsidian},
        {"CrystalHarvestComponent_C", EActorSpecificType::Crystal},
        {"CrystalHarvestComponent_Summit_C", EActorSpecificType::Crystal},
        {"CrystalHarvestComponent_UnderwaterCave_C", EActorSpecificType::Crystal}
};

std::map<int32, std::vector<FVector>> ResourceWorldPositions;
const std::vector<FVector>& GetFoliageLocations(UObject* Resource)
{
    const int32 InternalIndex = Resource->InternalIndex;

    std::vector<FVector>& Data = ResourceWorldPositions[InternalIndex];
    if (!Data.empty())
        return Data;

    TArray<FMatrix> PerInstanceSMData = *Resource->GetMember<TArray<FMatrix>>("PerInstanceSMData");
    if (!PerInstanceSMData)
        return Data;

    Data.reserve(PerInstanceSMData.Num() + 1);
    for (int32 i = 0; i < PerInstanceSMData.Num(); ++i)
    {
        FTransform OutTransform;
        if (GetInstanceTransform(Resource, i, &OutTransform, true))
        {
            Data.push_back(OutTransform.GetLocation());
        }
    }
    return Data;
}


FORCEINLINE EActorAssociation GetActorAssociation(AShooterCharacter* MyCharacter, UObject* Target)
{
    int32 TargetingTeam = *Target->GetMember<int32>("TargetingTeam");

    if (*MyCharacter->GetMember<int32>("TargetingTeam") == TargetingTeam)
        return EActorAssociation::Team;
    else if (IsAllied(MyCharacter, TargetingTeam))
        return EActorAssociation::Ally;

    return EActorAssociation::Enemy;
}

EActorSpecificType GetSpecificStructureType(UObject* Actor)
{
    if (Actor->IsA(APrimalStructureBed::StaticClass()))
    {
        return EActorSpecificType::Bed;
    }

    if (Actor->IsA(APrimalStructureItemContainer::StaticClass()))
    {
        if (Actor->IsA(APrimalStructureTurret::StaticClass()))
        {
            if (Actor->IsA(APrimalStructureTurretPlant::StaticClass()))
                return EActorSpecificType::PlantX;
            else
                return EActorSpecificType::Turret;
        }
        else if (Actor->IsA(APrimalStructureExplosive::StaticClass()))
        {
            return Actor->IsA(APrimalStructureExplosiveTransGPS::StaticClass())
                   ? EActorSpecificType::None : EActorSpecificType::Explosive;
        }
        else if (Actor->IsA(APrimalStructureItemContainer_SupplyCrate::StaticClass()))
        {
            return EActorSpecificType::SupplyCrate;
        }
        else if (Actor->NamePrivate.IsAny(FNames::DeathItemCache_C, FNames::DeathItemCache_PlayerDeath_C))
        {
            return EActorSpecificType::ItemCache;
        }
        else
        {
            return EActorSpecificType::Container;
        }
    }
    return EActorSpecificType::None;
}

FORCEINLINE bool ShouldStructureBeDrawn(EActorSpecificType Type)
{
    switch (Type)
    {
        case EActorSpecificType::Explosive:
            return settings.esp.Explosives;
        case EActorSpecificType::Bed:
            return settings.esp.Beds;
        case EActorSpecificType::Turret:
            return settings.esp.Turrets;
        case EActorSpecificType::PlantX:
            return settings.esp.PlantX;
        case EActorSpecificType::Container:
            return settings.esp.Containers;
        case EActorSpecificType::ItemCache:
            return settings.esp.ItemCache;
        case EActorSpecificType::SupplyCrate:
            return settings.esp.SupplyCrate;
        default:
            return false;
    }
}

FORCEINLINE bool ShouldResourceBeDrawn(EActorSpecificType Type)
{
    switch (Type)
    {
        case EActorSpecificType::Obsidian:
            return settings.esp.Obsidian;
        case EActorSpecificType::Oil:
            return settings.esp.Oil;
        case EActorSpecificType::Metal:
            return settings.esp.Metal;
        case EActorSpecificType::Crystal:
            return settings.esp.Crystal;
        case EActorSpecificType::Perl:
            return settings.esp.Perl;
        default:
            return false;
    }
}

FORCEINLINE FLinearColor GetColorByAssociationPlayer(EActorAssociation Association)
{
    if (Association == EActorAssociation::Team)
        return ConvertToFLinearColor(settings.esp.PlayerTeamColor);
    else if (Association == EActorAssociation::Ally)
        return ConvertToFLinearColor(settings.esp.PlayerAllyColor);
    else if (Association == EActorAssociation::Enemy)
        return ConvertToFLinearColor(settings.esp.PlayerEnemyColor);

    return {0.0f, 0.0f, 0.0f, 0.0f};
}

FORCEINLINE FLinearColor GetColorByAssociationDino(EActorAssociation Association)
{
    if (Association == EActorAssociation::Team)
        return ConvertToFLinearColor(settings.esp.DinoTeamColor);
    else if (Association == EActorAssociation::Ally)
        return ConvertToFLinearColor(settings.esp.DinoAllyColor);
    else if (Association == EActorAssociation::Enemy)
        return ConvertToFLinearColor(settings.esp.DinoEnemyColor);

    return {0.0f, 0.0f, 0.0f, 0.0f};
}

FORCEINLINE FLinearColor GetSpecificStructureColor(EActorSpecificType Type)
{
    switch (Type)
    {
        case EActorSpecificType::Explosive:
            return ConvertToFLinearColor(settings.esp.ExplosivesColor);
        case EActorSpecificType::Bed:
            return ConvertToFLinearColor(settings.esp.BedsColor);
        case EActorSpecificType::Turret:
            return ConvertToFLinearColor(settings.esp.TurretsColor);
        case EActorSpecificType::PlantX:
            return ConvertToFLinearColor(settings.esp.PlantXColor);
        case EActorSpecificType::Container:
            return ConvertToFLinearColor(settings.esp.ContainersColor);
        case EActorSpecificType::ItemCache:
            return ConvertToFLinearColor(settings.esp.ItemCacheColor);
        case EActorSpecificType::SupplyCrate:
            return ConvertToFLinearColor(settings.esp.SupplyCrateColor);
        default:
            return FLinearColor();
    }
}

FORCEINLINE FLinearColor GetColorByAssociationStructure(EActorAssociation Association, EActorSpecificType Type)
{
    if (Association == EActorAssociation::Team)
        return ConvertToFLinearColor(settings.esp.StructureTeamColor);
    else if (Association == EActorAssociation::Ally)
        return ConvertToFLinearColor(settings.esp.StructureAllyColor);
    else if (Association == EActorAssociation::Enemy)
        return GetSpecificStructureColor(Type);

    return {0.0f, 0.0f, 0.0f, 0.0f};
}

/*
std::unordered_map<int32, int32> FNameDinoMap(DinoBPNames.size());
FORCEINLINE bool ShouldSpecificDinoBeDrawn(FName NamePrivate)
{
    int32 ComparisonIndex = NamePrivate.GetDisplayIndex();
    if (auto it = FNameDinoMap.find(ComparisonIndex); it != FNameDinoMap.end())
    {
        return settings.esp.AllDinosaurs[it->second];
    }
    return false;
}*/


struct FAimbotInfo
{
    AShooterCharacter* Target;
    FVector Location3D;
    FVector2D Location2D;

    FAimbotInfo() : Target(nullptr), Location3D(FVector::Zero), Location2D(FVector2D::Zero) {}

    operator bool() const
    {
        return Target && !Location2D.IsZero() && !Location3D.IsZero();
    }
};


FAimbotInfo GetBestTarget()
{
    FAimbotInfo Info = {};

    AShooterPlayerController* PlayerController = GetPlayerController();
    if (!PlayerController)
        return Info;

    AShooterCharacter* ShooterSelf = GetPlayerCharacter(PlayerController);
    if (!ShooterSelf)
        return Info;

    AShooterWeapon* CurrentWeapon = *ShooterSelf->GetMember<AShooterWeapon*>("CurrentWeapon");
    if (!CurrentWeapon || !CurrentWeapon->IsA(AShooterWeapon_Instant::StaticClass()))
         return Info;

    FVector2D MidScreen = GCanvasSize.Vector() * 0.5f;

    const FMatrix& ViewProjMatrix = GetViewProjectionMatrix();

    float CurrentBestDistance = FLT_MAX;

    TArray<UObject*> Players = GetActors(nullptr, AL_PLAYERS);
    for (UObject* Player : Players)
    {
        if (!ShooterSelf || !PlayerController)
            break;

        if (!Player || Player == ShooterSelf)
            continue;

        USkeletalMeshComponent* Mesh = *Player->GetMember<USkeletalMeshComponent*>("Mesh");
        if (!Mesh)
            continue;

        if (Player->GetBitMember("bIsDead") || Player->GetBitMember("bIsSleeping"))
            continue;

        if (EActorAssociation::Enemy != GetActorAssociation(ShooterSelf, Player))
            continue;

        const FTransform& ComponentToWorld = *(FTransform*)((uint8*)Mesh + Off::ComponentToWorld);

        FVector AimLocation = GetBoneLocation(Mesh, 8, ComponentToWorld);
        if (!LineOfSightTo(PlayerController, ShooterSelf, AimLocation, false))
            continue;

        FVector2D ScreenLocation;
        ProjectWorldToScreen(nullptr, AimLocation, ScreenLocation, true);

        float Distance = MidScreen.GetDistanceTo(ScreenLocation);
        if (Distance > CurrentBestDistance)
            continue;

        CurrentBestDistance = Distance;

        Info.Target = (AShooterCharacter*)Player;
        Info.Location2D = ScreenLocation;
        Info.Location3D = AimLocation;
    }

    return Info;
}


class Semaphore final
{
public:
    Semaphore(int count = 1) : Count(count) {}

    void Aquire()
    {
        std::unique_lock<std::mutex> Lock(Mutex);
        CV.wait(Lock, [this] { return Count > 0; });
        Count = 0;
    }

    bool IsAquired() const
    {
        return Count == 0;
    }

    void Release()
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        Count = 1;
        CV.notify_one();
    }

private:
    std::mutex Mutex;
    std::condition_variable CV;
    int Count;
};
Semaphore GSemaphore;

static int NumPlayers = 0;

std::atomic<bool> bDidThreadUpdate(false);

bool DidRelease = false;

void DrawDataThread()
{
    while (true)
    {
        if (!settings.esp.Enable)
            continue;

        AShooterPlayerController* PC = GetPlayerController();
        if (!PC)
            continue;

        AShooterCharacter* MyCharacter = GetPlayerCharacter(PC);
        if (!MyCharacter)
            continue;

        FVector MyLocation = GetActorLocation(MyCharacter);

        GSemaphore.Aquire();
        {
            ClearVectors();

            if (settings.esp.Players)
            {
                TArray<UObject*> Players = GetActors(nullptr, AL_PLAYERS);
                for (UObject* Actor : Players)
                {
                    if (!MyCharacter || !PC)
                        break;

                    AShooterCharacter* Player = (AShooterCharacter*)Actor;
                    if (!Player)
                        continue;

                    if (Player == MyCharacter)
                        continue;

                    USkeletalMeshComponent* Mesh = *Player->GetMember<USkeletalMeshComponent*>("Mesh");
                    if (!Mesh)
                        continue;

                    EActorAssociation Association = GetActorAssociation(MyCharacter, Player);

                    if (Player == MyCharacter)
                        Association = EActorAssociation::Enemy;

                    if (!settings.esp.AllyPlayers && Association == EActorAssociation::Ally)
                        continue;

                    if (!settings.esp.TeamPlayers && Association == EActorAssociation::Team)
                        continue;

                    bool IsSleeping = Player->GetBitMember("bIsSleeping");
                    bool IsDead     = Player->GetBitMember("bIsDead");

                    if (!settings.esp.Dead && IsDead)
                        continue;

                    if (!settings.esp.Sleeping && IsSleeping)
                        continue;

                    FVector PlayerLocation = GetActorLocation(Player);

                    int Distance = MyLocation.GetDistanceToInMeters(PlayerLocation);

                    if (Distance > 2000.0f)
                        continue;

                    FVector TopLocation = PlayerLocation;

                    TopLocation.Z += ((FBoxSphereBounds*)((uint8*)Mesh + Off::Bounds))->BoxExtent.Z;

                    FVector2D TopScreenLocation;
                    if (ProjectWorldToScreen(nullptr, TopLocation, TopScreenLocation, true))
                    {
                        FLinearColor DrawColor = GetColorByAssociationPlayer(Association);

                        UObject* MyCharacterStatusComponent = *Player->GetMember<UObject*>("MyCharacterStatusComponent");
                        if (!MyCharacterStatusComponent)
                            continue;

                        int Level = *MyCharacterStatusComponent->GetMember<int32>("BaseCharacterLevel") + *MyCharacterStatusComponent->GetMember<uint16>("ExtraCharacterLevel");

                        FString PlayerName = *Player->GetMember<FString>("PlayerName");
                        if (!PlayerName)
                            PlayerName = u"No Name";

                        std::u16string WStr = fmt::format(u"Lvl-{} {} [{}m]", Level, PlayerName.CStr(), Distance);

                        PlayerDrawData.emplace_back(std::move(WStr), Player, TopScreenLocation, EActorType::Player, Association, true, DrawColor);
                    }
                    else
                    {
                        if (IsDead || IsSleeping)
                            continue;

                        if (Association == EActorAssociation::Enemy)
                            PlayerDrawData.emplace_back(u"", Player, TopScreenLocation, EActorType::Player, Association, false, FLinearColor::Yellow);
                    }
                }
            }

            if (settings.esp.ExplorerNotes)
            {
                TArray<UObject*> ExplorerNotes = GetActors(nullptr, AL_EXPLORERNOTECHESTS);
                for (UObject* ExplorerNote : ExplorerNotes)
                {
                    if (!ExplorerNote)
                        continue;

                    if (!MyCharacter || !PC)
                        break;

                    if (*ExplorerNote->GetMember<bool>("bIsUnlocked"))
                        continue;

                    int32 ExplorerNoteIndex = *ExplorerNote->GetMember<int32>("ExplorerNoteIndex");
                    if (LocalIsPerMapExplorerNoteUnlocked(ExplorerNoteIndex))
                        continue;

                    FVector ExplorerNoteLocation = GetActorLocation(ExplorerNote);

                    FVector2D ScreenLocation;
                    if (!ProjectWorldToScreen(nullptr, ExplorerNoteLocation, ScreenLocation))
                        continue;

                    int32 Distance = MyLocation.GetDistanceToInMeters(ExplorerNoteLocation);

                    std::u16string Name = fmt::format(u"Explorer Note [{}m]", Distance);

                    OtherDrawData.emplace_back(std::move(Name), ExplorerNote, ScreenLocation, EActorType::FoliageActor, ConvertToFLinearColor(settings.esp.ResourceColor));
                }
            }

            if (settings.esp.Dinosaurs || settings.esp.Structures || settings.esp.Eggs)
            {
                TArray<UObject*> Actors = GetActors();
                for (UObject* Actor : Actors)
                {
                    if (!Actor)
                        continue;

                    if (!MyCharacter || !PC)
                        break;

                    if (IsPrimalDinoOrStructure(Actor))
                    {
                        if (settings.esp.Dinosaurs && IsPrimalDino(Actor))
                        {
                            if (settings.esp.HideDinosaur)
                                continue;

                            APrimalDinoCharacter* Dino = (APrimalDinoCharacter*)Actor;
                            if (!Dino)
                                continue;

                            USkeletalMeshComponent* Mesh = *Dino->GetMember<USkeletalMeshComponent*>("Mesh");
                            if (!Mesh)
                                continue;

                            bool IsWild = 50000 > *Dino->GetMember<int32>("TargetingTeam");

                            if (!settings.esp.WildDino && IsWild)
                                continue;

                            EActorAssociation Association = GetActorAssociation(MyCharacter, Actor);

                            if (!settings.esp.AllyDino && Association == EActorAssociation::Ally)
                                continue;

                            if (!settings.esp.TeamDino && Association == EActorAssociation::Team)
                                continue;

                            UObject* MyCharacterStatusComponent = *Dino->GetMember<UObject*>("MyCharacterStatusComponent");
                            if (!MyCharacterStatusComponent)
                                continue;

                            int Level = *MyCharacterStatusComponent->GetMember<int32>("BaseCharacterLevel") + *MyCharacterStatusComponent->GetMember<uint16>("ExtraCharacterLevel");

                            if (IsWild && settings.esp.MinDinoLevel > Level)
                                continue;

                            FVector Location = GetActorLocation(Dino);

                            FVector2D ScreenLocation;
                            if (!ProjectWorldToScreen(nullptr, Location, ScreenLocation))
                                continue;

                            int Distance = MyLocation.GetDistanceToInMeters(Location);

                            FString DescriptiveName = *Dino->GetMember<FString>("DescriptiveName");
                            if (!DescriptiveName)
                                continue;

                            std::u16string Name = fmt::format(u"Level-{} {} [{}m]", Level, DescriptiveName.CStr(), Distance);
                            if (settings.esp.ShowDinoInfo)
                            {
                                int ReplicatedCurrentTorpor = (int)*Dino->GetMember<float>("ReplicatedCurrentTorpor");
                                int ReplicatedMaxTorpor     = (int)*Dino->GetMember<float>("ReplicatedMaxTorpor");
                                int ReplicatedCurrentHealth = (int)*Dino->GetMember<float>("ReplicatedCurrentHealth");
                                int ReplicatedMaxHealth     = (int)*Dino->GetMember<float>("ReplicatedMaxHealth");
                                Name = fmt::format(u"{} [TP:{}/{}] [HP:{}/{}]", Name, ReplicatedCurrentTorpor, ReplicatedMaxTorpor, ReplicatedCurrentHealth, ReplicatedMaxHealth);;
                            }

                            OtherDrawData.emplace_back(std::move(Name), Dino, ScreenLocation, EActorType::Dinosaur, IsWild ? ConvertToFLinearColor(settings.esp.DinoWildColor) : GetColorByAssociationDino(Association));
                        }
                        else
                        if (settings.esp.Structures && IsPrimalStructure(Actor))
                        {
                            if (settings.esp.HideStructure)
                                continue;

                            APrimalStructure* Structure = (APrimalStructure*)Actor;
                            if (!Structure)
                                continue;

                            EActorAssociation Association = GetActorAssociation(MyCharacter, Actor);

                            if (!settings.esp.AllyStructures && Association == EActorAssociation::Ally)
                                continue;

                            if (!settings.esp.TeamStructures && Association == EActorAssociation::Team)
                                continue;

                            EActorSpecificType SpecType = GetSpecificStructureType(Structure);
                            if (!ShouldStructureBeDrawn(SpecType))
                                continue;

                            FVector StructureLocation = GetActorLocation(Structure);

                            int Distance = MyLocation.GetDistanceToInMeters(StructureLocation);

                            if (settings.esp.MaxDistance < Distance)
                                continue;

                            FVector2D ScreenLocation;
                            if (!ProjectWorldToScreen(nullptr, StructureLocation, ScreenLocation))
                                continue;

                            FString DescriptiveName = *Structure->GetMember<FString>("DescriptiveName");
                            if (!DescriptiveName)
                                continue;

                            std::u16string Name = fmt::format(u"{} [{}m]", DescriptiveName.CStr(), Distance);

                            if (SpecType == EActorSpecificType::Turret)
                            {
                                bool bIsPinLocked = Structure->GetBitMember("bIsPinLocked");

                                int32 NumBullets = *Structure->GetMember<int32>("NumBullets");
                                if (NumBullets)
                                    Name = fmt::format(u"{}\n[Bullets: {}] {}", Name, NumBullets,  bIsPinLocked ? u"Locked" : u"Unlocked");
                                else
                                    Name = fmt::format(u"{}\n[Empty] {}", Name, bIsPinLocked ? u"Locked" : u"Unlocked");
                            }

                            OtherDrawData.emplace_back(std::move(Name), Structure, ScreenLocation, EActorType::Structure, GetColorByAssociationStructure(Association, SpecType));
                        }
                    }
                    else if (settings.esp.Eggs && Actor->IsA(ADroppedItem::StaticClass()))
                    {
                        UPrimalItem* MyItem = *Actor->GetMember<UPrimalItem*>("MyItem");
                        if (!MyItem || !MyItem->GetBitMember("bIsEgg"))
                            continue;

                        FVector Location = GetActorLocation(Actor);

                        FVector2D ScreenLocation;
                        if (!ProjectWorldToScreen(nullptr, Location, ScreenLocation))
                            continue;

                        int Distance = MyLocation.GetDistanceToInMeters(Location);

                        FString DescriptiveNameBase = *MyItem->GetMember<FString>("DescriptiveNameBase");
                        if (!DescriptiveNameBase)
                            continue;

                        FString CustomItemDescription = *MyItem->GetMember<FString>("CustomItemDescription");
                        if (!CustomItemDescription)
                        {
                            CustomItemDescription = u"No Parents";
                        }

                        std::u16string Name = fmt::format(u"{} [{}m]\n{}", DescriptiveNameBase.CStr(), Distance, CustomItemDescription.CStr());
                        OtherDrawData.emplace_back(std::move(Name), Actor, ScreenLocation, EActorType::Egg, FLinearColor::Gray);
                    }
                }
            }

            if (settings.esp.Resources && !settings.esp.HideResource)
            {
                static std::unordered_map<int32, EActorSpecificType> ResourceTypes =
                {
                        {FName(u"MetalHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Metal},
                        {FName(u"MetalHarvestComponent_Rich_C").GetDisplayIndex(), EActorSpecificType::Metal},
                        {FName(u"MountainObsidianHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Obsidian},
                        {FName(u"SiliconHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Perl},
                        {FName(u"OilHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Oil},
                        {FName(u"OilHarvestComponentRich_C").GetDisplayIndex(), EActorSpecificType::Oil},
                        {FName(u"OilHarvestComponentUnderwater_C").GetDisplayIndex(), EActorSpecificType::Oil},
                        {FName(u"ObsidianHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Obsidian},
                        {FName(u"CrystalHarvestComponent_C").GetDisplayIndex(), EActorSpecificType::Crystal},
                        {FName(u"CrystalHarvestComponent_Summit_C").GetDisplayIndex(), EActorSpecificType::Crystal},
                        {FName(u"CrystalHarvestComponent_UnderwaterCave_C").GetDisplayIndex(), EActorSpecificType::Crystal}
                };

                if (UWorld* World = GetWorld())
                {
                    TArray<ULevel*> Levels = *World->GetMember<TArray<ULevel*>>("Levels");
                    for (int Idx = 1; Idx < Levels.Num(); ++Idx)
                    {
                        ULevel* Level = Levels[Idx];
                        if (!Level)
                            continue;

                        TArray<UObject*> Actors = GetActors(Level);
                        for (UObject* Actor : Actors)
                        {
                            if (!MyCharacter || !PC)
                                break;

                            if (!Actor || !IsInstancedFoliage(Actor))
                                continue;

                            TFreedArray<UObject*> Components = K2_GetComponentsByClass(Actor, UFoliageInstancedStaticMeshComponent::StaticClass());
                            if (!Components)
                                continue;

                            for (UObject* Component : Components)
                            {
                                if (!Component)
                                    continue;

                                TArray<uint32> InstancesVisibility = *Component->GetMember<TArray<uint32>>("InstancesVisibility");

                                TArray<UObject*> ReferencedAttachedComponentObjects = *Component->GetMember<TArray<UObject*>>("ReferencedAttachedComponentObjects");
                                if (!ReferencedAttachedComponentObjects)
                                    continue;

                                for (UObject* ReferencedAttachedComponent : ReferencedAttachedComponentObjects)
                                {
                                    if (!ReferencedAttachedComponent || !ReferencedAttachedComponent->IsA(UPrimalHarvestingComponent::StaticClass()))
                                        continue;

                                    auto ResourceInfo = ResourceTypes.find(ReferencedAttachedComponent->NamePrivate.GetDisplayIndex());
                                    if (ResourceInfo == ResourceTypes.end())
                                        continue;

                                    if (!ShouldResourceBeDrawn(ResourceInfo->second))
                                        continue;

                                    FString DescriptiveName = *ReferencedAttachedComponent->GetMember<FString>("DescriptiveName");
                                    if (!DescriptiveName)
                                        continue;

                                    const std::vector<FVector>& FoliageLocations = GetFoliageLocations(Component);
                                    for (int i = 0; i < FoliageLocations.size(); ++i)
                                    {
                                        bool IsVisible = InstancesVisibility[i / 32] >> i & 1;
                                        if (!IsVisible)
                                            continue;

                                        const FVector& Location = FoliageLocations[i];

                                        FVector2D ScreenLocation;
                                        if (!ProjectWorldToScreen(nullptr, Location, ScreenLocation))
                                            continue;

                                        int Distance = int(MyLocation.GetDistanceToInMeters(Location));
                                        std::u16string Name = fmt::format(u"{} [{}m]", DescriptiveName.CStr(), Distance);

                                        OtherDrawData.emplace_back(std::move(Name), Actor, ScreenLocation, EActorType::FoliageActor, ConvertToFLinearColor(settings.esp.ResourceColor));
                                    }
                                }
                            }
                        }
                    }
                }
            }



            DidRelease = false;
        }

        bDidThreadUpdate.store(true, std::memory_order_release);
    }
}


double GetUploadTime(UObject* GameState)
{
    double CurrentTime = *GameState->GetMember<double>("NetworkTime");
    double LastServerSaveTime = *GameState->GetMember<double>("LastServerSaveTime");
    double ServerSaveInterval = *GameState->GetMember<float>("ServerSaveInterval");
    return (LastServerSaveTime + ServerSaveInterval) - CurrentTime;
}


void GetMapCoords(UWorld* World, FVector const& CurrentPos, float& OutLat, float& OutLong)
{
    APrimalWorldSettings* WorldSettings = GetWorldSettings(World, false, true);
    if (WorldSettings && WorldSettings->IsA(APrimalWorldSettings::StaticClass()))
    {
        if (*WorldSettings->GetMember<bool>("bOverrideLongitudeAndLatitude"))
        {
            float LongitudeScale = *WorldSettings->GetMember<float>("LongitudeScale");
            float LatitudeScale  = *WorldSettings->GetMember<float>("LatitudeScale");

            float LongitudeOrigin = *WorldSettings->GetMember<float>("LongitudeOrigin");
            float LatitudeOrigin  = *WorldSettings->GetMember<float>("LatitudeOrigin");

            OutLong = (CurrentPos.X - LongitudeOrigin) / (LongitudeScale * 10);
            OutLat  = (CurrentPos.Y - LatitudeOrigin) / (LatitudeScale * 10);
        }
    }
    /*FName WorldName = World->NamePrivate;

    if (WorldName.IsAny(FNames::TheIsland,FNames::ScorchedEarth_P,FNames::Aberration_P))
    {
        OutLat  = (CurrentPos.Y / 8000.0f) + 50.0f;
        OutLong = (CurrentPos.X / 8000.0f) + 50.0f;
        OutAlt  = (CurrentPos.Z / 8000.0f) + 50.0f;
    }
    else if (WorldName == FNames::Ragnarok)
    {
        OutLat  = (CurrentPos.Y / 13100.0f) + 50.0f;
        OutLong = (CurrentPos.X / 13100.0f) + 50.0f;
        OutAlt  = (CurrentPos.Z / 13100.0f) + 50.0f;
    }*/
}



void CleanUpDrawing()
{
    ClearVectors(true);

    if (!DidRelease && GSemaphore.IsAquired())
    {
        GSemaphore.Release();
        DidRelease = true;
    }
}

void DrawInViewport(UCanvas* Canvas)
{
    if (!settings.esp.Enable)
        return CleanUpDrawing();

    AShooterPlayerController* PC = GetPlayerController();
    if (!PC)
        return CleanUpDrawing();

    AShooterCharacter* MyCharacter = GetPlayerCharacter(PC);
    if (!MyCharacter)
        return CleanUpDrawing();

    if (bDidThreadUpdate.load(std::memory_order_acquire))
    {
        std::lock_guard<std::mutex> lock(DataMutex);

        std::swap(OtherDrawData, BackupOtherDrawData);
        std::swap(PlayerDrawData, BackupPlayerDrawData);

        GSemaphore.Release();
        DidRelease = true;

        bDidThreadUpdate.store(false, std::memory_order_release);
    }

    int32 SizeX = *Canvas->GetMember<int32>("SizeX");
    int32 SizeY = *Canvas->GetMember<int32>("SizeY");

    UFont* ArkFont = GetRenderFont();

    for (const FOtherActorData& Data : BackupOtherDrawData)
    {
        if (!Data.TheActor || Data.Location.IsZero())
            continue;

        const float Scale = settings.esp.Scale;

        if (Data.Type == EActorType::Dinosaur && settings.esp.ShowDinoGender)
        {
            if (50000 > *Data.TheActor->GetMember<int32>("TargetingTeam"))
            {
                FVector2D TextSize = K2_TextSize(Canvas, ArkFont, Data.Name.c_str(), FVector2D(Scale * 0.9, Scale));

                bool bIsFemale = Data.TheActor->GetBitMember("bIsFemale");

                UTexture2D* Texture = GetIcon(bIsFemale ? EIconType::GenderFemale : EIconType::GenderMale);

                FVector2D IconLocation = Data.Location;
                IconLocation.X -= (TextSize.X / 2) + 15.0f;
                IconLocation.Y += 15.0f;

                DrawTexture(Canvas, Texture, IconLocation, FVector2D(30.0f, 30.0f), bIsFemale ? FLinearColor::Purple : FLinearColor::Blue);
            }
        }

        DrawText(Canvas, ArkFont, Data.Name.c_str(), Data.Location, Data.DrawColor, Scale);
    }

    int NumPlayersDrawn = 0;

    for (const FPlayerActorData& Data : BackupPlayerDrawData)
    {
        //if (!IsActorValid(PC) || !IsActorValid(MyCharacter))
        //break;

        if (!Data.ThePlayer || Data.TopLocation.IsZero())
            continue;

        if (!Data.IsVisible && settings.esp.Tracers)
        {
            DrawLine(Canvas, {SizeX / 2, 50}, Data.TopLocation, 3.0f, Data.DrawColor);
            NumPlayersDrawn++;
            continue;
        }

        const float Scale = settings.esp.Scale;

        FVector2D TextSize = K2_TextSize(Canvas, ArkFont, Data.Name.c_str(), FVector2D(Scale * 0.9, Scale));

        bool IsSleeping = Data.ThePlayer->GetBitMember("bIsSleeping");
        bool IsDead     = Data.ThePlayer->GetBitMember("bIsDead");

        if (IsSleeping || IsDead)
        {
            UTexture2D* Texture = GetIcon(IsSleeping ? EIconType::Knocked : EIconType::Skull);

            FVector2D IconLocation = Data.TopLocation;
            IconLocation.X -= (TextSize.X / 2) + 15.0f;
            IconLocation.Y += 15.0f;

            DrawTexture(Canvas, Texture, IconLocation, FVector2D(30.0f, 30.0f), FLinearColor::White);
            DrawText(Canvas, ArkFont, Data.Name.c_str(), Data.TopLocation, Data.DrawColor, Scale);
            continue;
        }

        if (!IsDead && !IsSleeping && Data.Association == EActorAssociation::Enemy)
        {
            NumPlayersDrawn++;

            const float HalfTextSize = TextSize.Y * 0.5f;

            if (settings.esp.Box3D)
                Draw3DBox(Canvas, (AShooterCharacter*)Data.ThePlayer, Data.DrawColor);

            if (settings.esp.Box2D)
                Draw2DBox(Canvas, (AShooterCharacter*)Data.ThePlayer, Data.DrawColor);

            if (settings.esp.Skeleton)
                DrawSkeleton(Canvas, (AShooterCharacter*)Data.ThePlayer, Data.DrawColor);

            if (settings.esp.Tracers)
                DrawLine(Canvas, {Data.TopLocation.X, Data.TopLocation.Y - HalfTextSize}, FVector2D(SizeX / 2, 50), 3.0f, Data.DrawColor);

            if (settings.esp.HPBar)
            {
                float ReplicatedCurrentHealth = *Data.ThePlayer->GetMember<float>("ReplicatedCurrentHealth");
                float ReplicatedMaxHealth     = *Data.ThePlayer->GetMember<float>("ReplicatedMaxHealth");
                DrawHealthBar(Canvas, FVector2D(Data.TopLocation.X, Data.TopLocation.Y + HalfTextSize + 5.0f), FVector2D(100, 10), FLinearColor::Red, ReplicatedCurrentHealth / ReplicatedMaxHealth, 1.5f);
            }

            if (settings.esp.Armor)
                DrawArmor(Canvas,(AShooterCharacter*)Data.ThePlayer, Data.TopLocation, FVector2D(35.0f, 35.0f));

            if (settings.esp.Weapon)
                DrawWeapon(Canvas, (AShooterCharacter*)Data.ThePlayer, FVector2D(Data.TopLocation.X, Data.TopLocation.Y - (HalfTextSize * 1.25f)), FVector2D(50.0f, 50.0f));
        }

        DrawText(Canvas, ArkFont, Data.Name.c_str(), Data.TopLocation, Data.DrawColor, Scale);
    }

    DrawText(Canvas, ArkFont, fmt::format(u"{}", NumPlayersDrawn).c_str(), FVector2D(SizeX / 2, 90), FLinearColor::Red, 2.0);

    if (UWorld* World = GetWorld())
    {
        if (UObject* GameState = *World->GetMember<UObject*>("GameState"))
        {
            double CurrentTime = *GameState->GetMember<double>("NetworkTime");
            double LastServerSaveTime = *GameState->GetMember<double>("LastServerSaveTime");
            double ServerSaveInterval = *GameState->GetMember<float>("ServerSaveInterval");

            double UploadTime = (LastServerSaveTime + ServerSaveInterval) - CurrentTime;

            auto [Minutes, Seconds] = GetMinAndSec(UploadTime);

            FVector MapPosition = GetActorLocation(MyCharacter);

            float Latitude, Longitude;
            GetMapCoords(World, MapPosition, Latitude, Longitude);

            std::u16string CommonInfo = fmt::format(u"Latitude: {:.2f}\nLongitude: {:.2f}\nUpload Time: {}{}:{}{}\nPlayers Online: {}",
                                                    Latitude,
                                                    Longitude,
                                                    int(Minutes / 10),
                                                    int(Minutes % 10),
                                                    int(Seconds / 10),
                                                    int(Seconds % 10),
                                                    *GameState->GetMember<int32>("NumPlayerConnected")
            );

            DrawText(Canvas, ArkFont, CommonInfo.c_str(), FVector2D(170, SizeY * 0.25), FLinearColor::White, 1.0);
        }
    }


    /*UObject* CurrentWeapon = *MyCharacter->GetMember<UObject*>("CurrentWeapon");
    if (CurrentWeapon && CurrentWeapon->IsA(StaticClass::ShooterWeapon_Instant()))
    {
        AimTargetInfo AimbotInfo = GetBestAimTarget();
        if (AimbotInfo.IsValid())
        {
            FVector2D MuzzleLocation;
            if (ProjectWorldToScreen(GetMuzzleLocation(CurrentWeapon), MuzzleLocation))
                DrawLine(Canvas, MuzzleLocation.X, MuzzleLocation.Y, AimbotInfo.ScreenLocation, 3.f, Green);
        }
    }

    if (settings.UseAimFOV)
    {
        DrawCircle(Canvas, FVector2D(SizeX / 2, SizeY / 2), settings.AimFOVRadius, Magenta, 20, 3.0f);
    }

    if (!ShotTraces.empty())
    {
        std::lock_guard<std::mutex> lock(TraceMutex);
        for (int32 i = ShotTraces.size() - 1; i >= 0; --i)
        {
            ShotTraces[i].Draw(Canvas);

            if (!ShotTraces[i].IsActive())
            {
                ShotTraces.erase(ShotTraces.begin() + i);
            }
        }
        TraceMutex.unlock();
    }*/
}


void (*orig_TickComponent)(UCharacterMovementComponent* CharacterMovement, float DeltaTime, int32 TickType, void* ThisTickFunction);
void new_TickComponent(UCharacterMovementComponent* CharacterMovement, float DeltaTime, int32 TickType, void* ThisTickFunction)
{

    if (settings.UsePlayerSpeed)
    {
        if (UObject* CharacterOwner = *CharacterMovement->GetMember<UObject*>("CharacterOwner"))
        {
            static ExecutionTimer ClientRestartTimer;
            ClientRestartTimer.UpdateWithInterval(3.0f, PawnClientRestart, CharacterOwner);
        }
    }

    if (settings.Freeze)
        DeltaTime = 0.0f;

    return orig_TickComponent(CharacterMovement, DeltaTime, TickType, ThisTickFunction);
}

void (*orig_ClientHandleMoveResponse)(UCharacterMovementComponent* _this, FCharacterMoveResponseDataContainer& MoveResponse);
void new_ClientHandleMoveResponse(UCharacterMovementComponent* _this, FCharacterMoveResponseDataContainer& MoveResponse)
{
    if (settings.UsePlayerSpeed && !MoveResponse.IsGoodMove())
    {
        if (AShooterPlayerController* PC = GetPlayerController())
        {
            if (UObject* Pawn = *PC->GetMember<UObject*>("Pawn"))
            {
                *PC->GetMember<UObject*>("Pawn") = nullptr;
                ServerAcknowledgePossession(PC, nullptr);
                *PC->GetMember<UObject*>("Pawn") = Pawn;
                ServerAcknowledgePossession(PC, Pawn);
            }
        }
    }
    return orig_ClientHandleMoveResponse(_this, MoveResponse);
}

float (*orig_GetMaxSpeed_Internal)(UCharacterMovementComponent* _this);
float new_GetMaxSpeed_Internal(UCharacterMovementComponent* _this)
{
    if (settings.UsePlayerSpeed)
        return orig_GetMaxSpeed_Internal(_this) * settings.PlayerSpeed;

    return orig_GetMaxSpeed_Internal(_this);
}

void VTableSwapFunc(void** VTable, int Index, void* NewFunc, void*& OrigFunc)
{
    if (VTable[Index] != NewFunc)
    {
        const size_t PageSize = sysconf(_SC_PAGESIZE);
        uintptr_t VTableEntry = (uintptr_t)&VTable[Index];
        uintptr_t PageStart = VTableEntry & ~(PageSize - 1);

        if (mprotect((void*)PageStart, PageSize, PROT_READ | PROT_WRITE) != 0)
            return;

        OrigFunc = VTable[Index];
        VTable[Index] = NewFunc;

        mprotect((void*)PageStart, PageSize, PROT_READ | PROT_EXEC);
    }
}

void OnTick_Internal(AShooterPlayerController* PlayerController, AShooterCharacter* Character)
{
    if (settings.ForceOptimizeGame)
        ForceOptimize(PlayerController);

    if (UCharacterMovementComponent* CharacterMovement = *Character->GetMember<UCharacterMovementComponent*>("CharacterMovement"))
    {
        if (settings.UsePlayerSpeed)
        {
            void** VTable = CharacterMovement->VTable;

            VTableSwapFunc(VTable, 115, (void*)new_TickComponent, (void*&)orig_TickComponent);
            VTableSwapFunc(VTable, 368, (void*)new_ClientHandleMoveResponse, (void*&)orig_ClientHandleMoveResponse);
            VTableSwapFunc(VTable, 439, (void*)new_GetMaxSpeed_Internal, (void*&)orig_GetMaxSpeed_Internal);
        }
    }

    if (UShooterGameUserSettings* GameUserSettings = GetGameUserSettings())
    {
        *GameUserSettings->GetMember<float>("FOVMultiplier") = settings.FOV * 0.01;
    }

    static bool IsGhost = false;
    if (settings.GhostMode)
    {
        SetReplicates(Character, false);
        SetReplicateMovement(Character, false);
        ClientCheatGhost(Character);
        IsGhost = true;
    }
    else if (IsGhost)
    {
        ClientCheatWalk(Character);
        SetReplicateMovement(Character, true);
        SetReplicates(Character, true);
        IsGhost = false;
    }

    AShooterWeapon* CurrentWeapon = *Character->GetMember<AShooterWeapon*>("CurrentWeapon");
    if (CurrentWeapon && !CurrentWeapon->IsA(AShooterWeapon_Melee::StaticClass()))
    {
        if (settings.TameShooting)
            *CurrentWeapon->GetMember<bool>("bForceTPV_EquippedWhileRiding") = true;

        if (settings.NoScopeOverlay)
            CurrentWeapon->SetBitMember("bUseScopeOverlay", false);

        if (settings.NoScopeSway)
        {
            *CurrentWeapon->GetMember<float>("AimDriftYawAngle") = 0.00f;
            *CurrentWeapon->GetMember<float>("AimDriftPitchAngle") = 0.00f;
        }
    }
}





#pragma mark - Hooks -

void (*orig_AShooterPlayerController$TickActor)(AShooterPlayerController* _this, float DeltaTime, int32 TickType, void* ThisTickFunction);
void new_AShooterPlayerController$TickActor(AShooterPlayerController* _this, float DeltaTime, int32 TickType, void* ThisTickFunction)
{
    if (AShooterCharacter* Character = GetPlayerCharacter(_this))
    {
        OnTick_Internal(_this, Character);

        if (settings.AutoArmor)
            Automatics::ArmorEquipment(Character, _this);

        if (settings.AutoMeds)
        {
            static ExecutionTimer AutoMeds_Timer;
            AutoMeds_Timer.UpdateWithInterval(1.0f / 10.0f, Automatics::MedConsumption, Character, _this);
        }

        if (settings.AutoPickupEggs)
        {
            static ExecutionTimer AutoPickupEggs_Timer;
            AutoPickupEggs_Timer.UpdateWithInterval(1.0f / 10.0f, Automatics::PickupEggs, Character, _this);
        }

        if (settings.AutoRemount)
        {
            static ExecutionTimer AutoRemount_Timer;
            AutoRemount_Timer.UpdateWithInterval(1.0f / 20.0f, Automatics::RemountDinosaur, Character, _this);
        }

        if (settings.AutoSteal)
        {
            static ExecutionTimer AutoLoot_Timer;
            AutoLoot_Timer.UpdateWithInterval(1.0f / 5.0f, Automatics::Looting, Character, _this);
        }
    }


    ShooterPlayerControllerQueue.Release(_this);

    return orig_AShooterPlayerController$TickActor(_this, DeltaTime, TickType, ThisTickFunction);
}

void (*orig_GetPlayerViewPoint)(AShooterPlayerController *_this, FVector* OutLocation, FRotator* OutRotation, bool ForAiming);
void new_GetPlayerViewPoint(AShooterPlayerController *_this, FVector* OutLocation, FRotator* OutRotation, bool ForAiming)
{
    orig_GetPlayerViewPoint(_this, OutLocation, OutRotation, ForAiming);

    if (settings.EnableAimbot)
    {
        if (/*settings.LockAim && */settings.AimType == 1)
        {
            FAimbotInfo AimbotInfo = GetBestTarget();
            if (AimbotInfo)
            {
                FRotator ShootRot = FMatrix::MakeFromX(AimbotInfo.Location3D - *OutLocation).Rotator();

                *OutRotation = ShootRot;
                ClientSetControlRotation(_this, ShootRot);
            }
        }

        if (settings.AimType == 0 && ForAiming)
        {
            FAimbotInfo AimbotInfo = GetBestTarget();
            if (AimbotInfo)
            {
                *OutRotation = FMatrix::MakeFromX(AimbotInfo.Location3D - *OutLocation).Rotator();
            }
        }
    }
}


void (*orig_UGameViewportClient$DrawTransition)(UShooterGameViewportClient* _this, UCanvas* Canvas);
void new_UGameViewportClient$DrawTransition(UShooterGameViewportClient* _this, UCanvas* Canvas)
{
    if (Canvas != nullptr)
    {
        GCanvasSize = *Canvas->GetMember<FIntPoint>("SizeX");

        DrawInViewport(Canvas);
    }

    return orig_UGameViewportClient$DrawTransition(_this, Canvas);
}


void (*orig_UInterpGroup$UpdateGroup)(UInterpGroup* _this, float NewPosition, UObject* GrInst, bool bPreview, bool bJump);
void new_UInterpGroup$UpdateGroup(UInterpGroup* _this, float NewPosition, UObject* GrInst, bool bPreview, bool bJump)
{
    if (settings.EnableTime)
    {
        FName Input = *_this->GetMember<FName>("GroupName");

        static FName NAME_DayCycle = FName(u"DayCycle");
        static FName NAME_SM4_Sky = FName(u"SM4_Sky");
        static FName NAME_Sunlight = FName(u"Sunlight");
        static FName NAME_DayInterpolationContainer = FName(u"DayInterpolationContainer");

        if (Input == NAME_DayCycle)
            NewPosition = NewPosition > 20.f ? settings.DayCycleSky : settings.DayCycle;
        else if (Input == NAME_SM4_Sky)
            NewPosition = settings.SM4_Sky;
        else if (Input == NAME_Sunlight)
            NewPosition = settings.Sunlight;
        else if (Input == NAME_DayInterpolationContainer)
            NewPosition = settings.DayInterpolationContainer;

    }

    return orig_UInterpGroup$UpdateGroup(_this, NewPosition, GrInst, bPreview, bJump);
}


void (*orig_UShooterGameInstance$HandleLoginCompleteNative)(UShooterGameInstance* _this, int a1, bool a2, void* a3, FString const& a4);
void new_UShooterGameInstance$HandleLoginCompleteNative(UShooterGameInstance* _this, int a1, bool a2, void* a3, FString const& a4)
{
    for (int i = 0; i < 5; ++i)
    {
        orig_UShooterGameInstance$HandleLoginCompleteNative(_this, a1, a2, a3, a4);
    }
}

void (*orig_UUI_MainMenu$HandleLoginComplete)(UUI_MainMenu* _this, int a1, bool a2, void* a3, FString const& a4);
void new_UUI_MainMenu$HandleLoginComplete(UUI_MainMenu* _this, int a1, bool a2, void* a3, FString const& a4)
{
    for (int i = 0; i < 5; ++i)
    {
        orig_UUI_MainMenu$HandleLoginComplete(_this, a1, a2, a3, a4);
    }
}

#pragma mark - Entry -

void __Init()
{
    IMemoryUtils* MemUtils = IMemoryUtils::Get("libUE4.so");
    do
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (!MemUtils->IsLibraryLoaded());

    void* GNameBlocksDebug = *(void**)MemUtils->FindSymbol("GNameBlocksDebug");
    InitializePointer(FName::NamePoolData, (void*)((uint8*)GNameBlocksDebug - offsetof(FNamePool, Blocks)));
    InitializePointer(UObject::GUObjectArray, MemUtils->FindSymbol("GUObjectArray"));


#define InitFunc(Func, Symbol) InitializeFunction(Func, MemUtils->FindSymbol(Symbol))

    InitFunc(FMemory::EngineRealloc, "_ZN7FMemory7ReallocEPvyj");

    InitFunc(GetInventoryUISelectedItemLocal, "_ZN24AShooterPlayerController31GetInventoryUISelectedItemLocalEv");
    InitFunc(GetInventoryUISelectedItemRemote, "_ZN24AShooterPlayerController32GetInventoryUISelectedItemRemoteEv");

    InitFunc(ClientCheatWalk, "_ZN10ACharacter15ClientCheatWalkEv");
    InitFunc(ClientCheatGhost, "_ZN10ACharacter16ClientCheatGhostEv");
    InitFunc(SetReplicateMovement, "_ZN6AActor20SetReplicateMovementEb");
    InitFunc(SetReplicates, "_ZN6AActor13SetReplicatesEb");
    InitFunc(ServerEquipPawnItem, "_ZN24AShooterPlayerController19ServerEquipPawnItemE10FItemNetID");
    InitFunc(ServerRequestInventoryUseItem, "_ZN24AShooterPlayerController29ServerRequestInventoryUseItemEP25UPrimalInventoryComponent10FItemNetID");
    InitFunc(ServerTransferAllFromRemoteInventory, "_ZN24AShooterPlayerController36ServerTransferAllFromRemoteInventoryEP25UPrimalInventoryComponentRK7FStringS4_S4_b");
    InitFunc(ServerRequestActorItems, "_ZN24AShooterPlayerController23ServerRequestActorItemsEP25UPrimalInventoryComponentbb");
    InitFunc(ServerMultiUse, "_ZN24AShooterPlayerController14ServerMultiUseEP7UObjecti");
    InitFunc(SendToConsole, "_ZN17APlayerController13SendToConsoleERK7FString");
    InitFunc(K2_DrawLine, "_ZN7UCanvas11K2_DrawLineE9FVector2DS0_f12FLinearColor");
    InitFunc(K2_DrawText, "_ZN7UCanvas11K2_DrawTextEP5UFontRK7FString9FVector2DS5_12FLinearColorfS6_S5_bbbS6_");
    InitFunc(K2_DrawTexture, "_ZN7UCanvas14K2_DrawTextureEP8UTexture9FVector2DS2_S2_S2_12FLinearColor10EBlendModefS2_");
    InitFunc(K2_TextSize, "_ZN7UCanvas11K2_TextSizeEP5UFontRK7FString9FVector2D");
    InitFunc(GetWorldSettings, "_ZNK6UWorld16GetWorldSettingsEbb");
    InitFunc(GetInstanceTransform, "_ZNK29UInstancedStaticMeshComponent20GetInstanceTransformEiR10FTransformb");
    InitFunc(LocalIsPerMapExplorerNoteUnlocked, "_ZN15UPrimalGameData33LocalIsPerMapExplorerNoteUnlockedEi");
    InitFunc(K2_GetComponentsByClass, "_ZNK6AActor23K2_GetComponentsByClassE11TSubclassOfI15UActorComponentE");
    InitFunc(ImportBufferAsTexture2D, "_ZN11FImageUtils23ImportBufferAsTexture2DERK6TArrayIh22TSizedDefaultAllocatorILi32EEE");
    InitFunc(Decode, "_ZN7FBase646DecodeERK7FStringR6TArrayIh22TSizedDefaultAllocatorILi32EEE");
    InitFunc(PawnClientRestart, "_ZN10ACharacter17PawnClientRestartEv");
    InitFunc(ServerAcknowledgePossession, "_ZN17APlayerController27ServerAcknowledgePossessionEP5APawn");
    InitFunc(LineOfSightTo, "_ZNK11AController13LineOfSightToEPK6AActor7FVectorb");
    InitFunc(ClientSetControlRotation, "_ZN24AShooterPlayerController24ClientSetControlRotationE8FRotator");


#undef InitFunc

    std::this_thread::sleep_for(std::chrono::seconds(1));

#define InlineHook(symbol, replace, result) A64HookFunction(MemUtils->FindSymbol(symbol), (void*)replace, (void**)&result)

    InlineHook("_ZN24AShooterPlayerController9TickActorEf10ELevelTickR18FActorTickFunction", new_AShooterPlayerController$TickActor, orig_AShooterPlayerController$TickActor);
    InlineHook("_ZN20UShooterGameInstance25HandleLoginCompleteNativeEibRK12FUniqueNetIdRK7FString", new_UShooterGameInstance$HandleLoginCompleteNative, orig_UShooterGameInstance$HandleLoginCompleteNative);
    InlineHook("_ZN12UUI_MainMenu19HandleLoginCompleteEibRK12FUniqueNetIdRK7FString", new_UUI_MainMenu$HandleLoginComplete, orig_UUI_MainMenu$HandleLoginComplete);
    InlineHook("_ZN12UInterpGroup11UpdateGroupEfP16UInterpGroupInstbb", new_UInterpGroup$UpdateGroup, orig_UInterpGroup$UpdateGroup);
    InlineHook("_ZN19UGameViewportClient14DrawTransitionEP7UCanvas", new_UGameViewportClient$DrawTransition, orig_UGameViewportClient$DrawTransition);
    InlineHook("_ZNK24AShooterPlayerController18GetPlayerViewPointER7FVectorR8FRotatorb", new_GetPlayerViewPoint, orig_GetPlayerViewPoint);

#undef InlineHook

    std::this_thread::sleep_for(std::chrono::seconds(3));

    FNames::FindAll();

    SleepIcon = GetTexture2DFromBase64(SleepIconBase64);

    Off::ViewProjectionMatrix = UCanvas::StaticClass()->Size - sizeof(FQuat) - sizeof(FMatrix);

    if (FProperty* VertexOffsetUsage = USkinnedMeshComponent::StaticClass()->GetPropertyPtr("VertexOffsetUsage"))
        Off::ComponentSpaceTransformsArray = VertexOffsetUsage->Offset_Internal + VertexOffsetUsage->ElementSize;

    if (FProperty* bIgnoreParentTransformUpdate = USceneComponent::StaticClass()->GetPropertyPtr("bIgnoreParentTransformUpdate"))
        Off::ComponentToWorld = bIgnoreParentTransformUpdate->Offset_Internal - bIgnoreParentTransformUpdate->ElementSize - sizeof(FTransform);

    if (FProperty* RelativeLocation = USceneComponent::StaticClass()->GetPropertyPtr("RelativeLocation"))
        Off::Bounds = RelativeLocation->Offset_Internal - sizeof(FBoxSphereBounds);

    std::thread _DrawDataThread(DrawDataThread);
    _DrawDataThread.detach();
}

__attribute__((constructor(101))) void _Entry()
{
    std::thread _Init_Thread(__Init);
    _Init_Thread.detach();
}
