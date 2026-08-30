#pragma once

#include "Includes.h"
#include "UnrealEngine/ScriptCore.h"

template<StringLiteral Name>
UClass* GetStaticClassImpl()
{
    static UClass* StaticClass = nullptr;
    if (StaticClass == nullptr)
        StaticClass = UObject::FindStaticClass(Name);

    return StaticClass;
}

#define DEFINE_CLASS(ClassName)                                 \
class ClassName : public UObject                                \
{                                                               \
public:                                                         \
    static UClass* StaticClass()                                \
    {                                                           \
        return GetStaticClassImpl< #ClassName >();              \
    }                                                           \
};


DEFINE_CLASS(ADroppedItem)

DEFINE_CLASS(APlayerStart)

DEFINE_CLASS(APrimalStructureItemContainer)

DEFINE_CLASS(APrimalStructureTurret)

DEFINE_CLASS(APrimalStructureTurretPlant)

DEFINE_CLASS(APrimalStructureBed)

DEFINE_CLASS(AShooterWeapon)

DEFINE_CLASS(AShooterWeapon_Instant)

DEFINE_CLASS(AShooterWeapon_Melee)

DEFINE_CLASS(APrimalWeaponBow)

DEFINE_CLASS(AShooterWeapon_Projectile)

DEFINE_CLASS(AShooterHUD)

DEFINE_CLASS(AShooterPlayerState)

DEFINE_CLASS(APrimalStructureItemContainer_SupplyCrate)

DEFINE_CLASS(UPrimalHarvestingComponent)

DEFINE_CLASS(UFoliageInstancedStaticMeshComponent)

DEFINE_CLASS(APrimalWorldSettings)

DEFINE_CLASS(UCanvas)

DEFINE_CLASS(USceneComponent)

DEFINE_CLASS(USkinnedMeshComponent)

DEFINE_CLASS(AShooterCharacter)

DEFINE_CLASS(AShooterPlayerController)

DEFINE_CLASS(APrimalDinoCharacter)

DEFINE_CLASS(AExplorerChest)

DEFINE_CLASS(UInterpGroup)

DEFINE_CLASS(UUI_MainMenu)

DEFINE_CLASS(UShooterGameInstance)

DEFINE_CLASS(UWorld)

DEFINE_CLASS(ULevel)

DEFINE_CLASS(UShooterGameUserSettings)

DEFINE_CLASS(UShooterEngine)

DEFINE_CLASS(UShooterLocalPlayer)

DEFINE_CLASS(UShooterGameViewportClient)

DEFINE_CLASS(UPrimalItem)

DEFINE_CLASS(AShooterPlayerCameraManager)

DEFINE_CLASS(UPrimalInventoryComponent)

DEFINE_CLASS(USkeletalMeshComponent)

DEFINE_CLASS(UPrimalCharacterStatusComponent)

DEFINE_CLASS(AActor)

DEFINE_CLASS(UNetDriver)

DEFINE_CLASS(UNetConnection)

DEFINE_CLASS(UFont)

DEFINE_CLASS(UTexture2D)

DEFINE_CLASS(APrimalStructureExplosive)

DEFINE_CLASS(APrimalStructureExplosiveTransGPS)

DEFINE_CLASS(APrimalStructure)

DEFINE_CLASS(UCharacterMovementComponent)