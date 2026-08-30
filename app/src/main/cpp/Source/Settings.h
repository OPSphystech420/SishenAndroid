#pragma once

#include <filesystem>
#include <fstream>
#include <dlfcn.h>



class Settings
{
public:
    Settings() = default;
    ~Settings() { Save(); }


    float FOV = 90.0f;
    bool GhostMode = false;

    bool EnableTime = false;
    float DayCycle = 6.0f;
    float DayCycleSky = 60.0f;
    float DayInterpolationContainer = 5.0f;
    float SM4_Sky = 6.0f;
    float Sunlight = 60.f;
    bool NoScopeOverlay = false;
    bool NoScopeSway = false;
    bool TameShooting = false;
    bool AutoArmor = false;
    bool AutoPickupEggs = false;
    bool AutoMeds = false;
    bool AutoRemount = false;
    bool AutoSteal = false;
    bool LootPlayers = false;
    bool LootTurrets = false;
    bool LootContainers = false;
    bool LootSupplyCrates = false;
    bool ForceOptimizeGame = false;

    float DrawScale = 1.0f;

    struct ExtrasensoryPerception
    {
        bool Enable;
        float Scale = 1.5f;

        bool PlayerChams = false;
        bool DinoChams = false;
        bool StructureChams = false;

        bool Players = false;
        bool Dead = false;
        bool Sleeping = false;
        bool Tracers = false;
        bool HPBar = false;
        bool Armor = false;
        bool Weapon = false;
        bool Box2D = false;
        bool Box3D = false;
        bool ShotTraces = false;
        bool Skeleton = false;
        bool TeamPlayers = false;
        bool AllyPlayers = false;

        bool Dinosaurs = false;
        int MinDinoLevel = 150;
        bool WildDino = false;
        bool TeamDino = false;
        bool AllyDino = false;

        bool Structures = false;
        bool TeamStructures = false;
        bool AllyStructures = false;
        bool Containers = false;
        bool Beds = false;
        bool Explosives = false;
        bool Turrets = false;
        bool SupplyCrate = false;
        bool ItemCache = false;
        bool PlantX = false;
        int MaxDistance = 3000;

        bool Resources = false;
        bool Metal = false;
        bool Oil = false;
        bool Obsidian = false;
        bool Perl = false;
        bool Crystal = false;
        bool ExplorerNotes = false;

        bool Eggs = false;

        bool HideStructureSwitch = false;
        bool HideResourceSwitch = false;
        bool HideDinosaurSwitch = false;

        bool HideStructure = false;
        bool HideDinosaur = false;
        bool HideResource = false;

        float PlayerAllyColor[3] = {0.0f, 1.0f, 1.0f};
        float PlayerTeamColor[3] = {0.0f, 1.0f, 0.0f};
        float PlayerEnemyColor[3] = {1.0f, 0.0f, 0.0f};

        float StructureAllyColor[3] = {0.0f, 1.0f, 1.0f};
        float StructureTeamColor[3] = {0.0f, 1.0f, 0.0f};

        float ContainersColor[3] = {1.0f, 0.0f, 0.0f};
        float BedsColor[3] = {1.0f, 0.0f, 0.0f};
        float ExplosivesColor[3] = {1.0f, 0.0f, 0.0f};
        float TurretsColor[3] = {1.0f, 0.0f, 0.0f};
        float SupplyCrateColor[3] = {1.0f, 0.0f, 0.0f};
        float PlantXColor[3] = {1.0f, 0.0f, 0.0f};
        float ItemCacheColor[3] = {1.0f, 0.0f, 0.0f};

        float DinoWildColor[3] = {1.0f, 1.0f, 0.0f};
        float DinoAllyColor[3] = {0.0f, 1.0f, 1.0f};
        float DinoTeamColor[3] = {0.0f, 1.0f, 0.0f};
        float DinoEnemyColor[3] = {1.0f, 0.0f, 0.0f};

        float ResourceColor[3] = {1.0f, 1.0f, 1.0f};

        bool UseDinoSearch = false;
        bool AllDinosaurs[747];

        bool ShowDinoGender = false;
        bool ShowDinoHealth = false;
        bool ShowDinoInfo = false;
    };

    ExtrasensoryPerception esp;


    bool UsePlayerSpeed = false;

    float PlayerSpeed = 1.0f;

    bool Freeze = false;

    bool EnableAimbot = false;
    int AimType = 0;

public:

    static inline std::filesystem::path FilePath = {};

    void Save()
    {
        std::ofstream OutFile(FilePath, std::ios::binary | std::ios::trunc);
        OutFile.write(reinterpret_cast<const char*>(this), sizeof(Settings));
        OutFile.close();
    }

    void Load()
    {
        if (std::filesystem::exists(FilePath))
        {
            std::ifstream InFile(FilePath, std::ios::binary);
            InFile.read(reinterpret_cast<char*>(this), sizeof(Settings));
            InFile.close();
        }
    }
};

inline Settings settings;