#pragma once

#include "Menu.h"
#include "Login.h"
#include "Clipboard.h"
#include <jni.h>
#include "Includes.h"
#include "Main.h"

enum class MainTab { ESP, Misc, Aimbot };
static MainTab currentTab = MainTab::ESP;

void RenderMainMenu()
{
    ImGui::BeginChild("##Sidebar", ImVec2(400, 0), true);

    if (ImGui::Button("ESP", ImVec2(-1, 0))) currentTab = MainTab::ESP;
    if (ImGui::Button("Misc", ImVec2(-1, 0))) currentTab = MainTab::Misc;
    if (ImGui::Button("Aimbot", ImVec2(-1, 0))) currentTab = MainTab::Aimbot;

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("##MainContent", ImVec2(0, 0), true);
    if (currentTab == MainTab::ESP)
    {
        if (ImGui::BeginTabBar("##ESPTabs"))
        {
            if (ImGui::BeginTabItem("General"))
            {
                ImGui::Checkbox("Enable ESP", &settings.esp.Enable);
                ImGui::SliderFloat("ESP Scale", &settings.esp.Scale, 0.1f, 5.0f);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Players"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Checkbox("Players", &settings.esp.Players);
                ImGui::Checkbox("Dead", &settings.esp.Dead);
                ImGui::Checkbox("Sleeping", &settings.esp.Sleeping);
                ImGui::Checkbox("Tracers", &settings.esp.Tracers);
                ImGui::Checkbox("HP Bar", &settings.esp.HPBar);
                ImGui::NextColumn();
                ImGui::Checkbox("Armor", &settings.esp.Armor);
                ImGui::Checkbox("Weapon", &settings.esp.Weapon);
                ImGui::Checkbox("Box 2D", &settings.esp.Box2D);
                ImGui::Checkbox("Box 3D", &settings.esp.Box3D);
                ImGui::Checkbox("Skeleton", &settings.esp.Skeleton);
                ImGui::Checkbox("Shot Traces", &settings.esp.ShotTraces);
                ImGui::Checkbox("Team Players", &settings.esp.TeamPlayers);
                ImGui::Checkbox("Ally Players", &settings.esp.AllyPlayers);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Dinosaurs"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Checkbox("Dinosaurs", &settings.esp.Dinosaurs);
                ImGui::Checkbox("Wild Dino", &settings.esp.WildDino);
                ImGui::Checkbox("Team Dino", &settings.esp.TeamDino);
                ImGui::Checkbox("Ally Dino", &settings.esp.AllyDino);
                ImGui::Checkbox("Show Gender", &settings.esp.ShowDinoGender);
                ImGui::NextColumn();
                ImGui::SliderInt("Min Level", &settings.esp.MinDinoLevel, 1, 500);
                ImGui::Checkbox("Use Search", &settings.esp.UseDinoSearch);
                ImGui::Checkbox("Show Health", &settings.esp.ShowDinoHealth);
                ImGui::Checkbox("Show Info", &settings.esp.ShowDinoInfo);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Structures"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Checkbox("Structures", &settings.esp.Structures);
                ImGui::Checkbox("Team Structures", &settings.esp.TeamStructures);
                ImGui::Checkbox("Ally Structures", &settings.esp.AllyStructures);
                ImGui::Checkbox("Containers", &settings.esp.Containers);
                ImGui::Checkbox("Beds", &settings.esp.Beds);
                ImGui::NextColumn();
                ImGui::Checkbox("Explosives", &settings.esp.Explosives);
                ImGui::Checkbox("Turrets", &settings.esp.Turrets);
                ImGui::Checkbox("Supply Crate", &settings.esp.SupplyCrate);
                ImGui::Checkbox("Item Cache", &settings.esp.ItemCache);
                ImGui::Checkbox("Plant X", &settings.esp.PlantX);
                ImGui::SliderInt("Max Distance", &settings.esp.MaxDistance, 100, 10000);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Resources"))
            {
                ImGui::Columns(2, nullptr, false);
                ImGui::Checkbox("Resources", &settings.esp.Resources);
                ImGui::Checkbox("Metal", &settings.esp.Metal);
                ImGui::Checkbox("Oil", &settings.esp.Oil);
                ImGui::Checkbox("Obsidian", &settings.esp.Obsidian);
                ImGui::NextColumn();
                ImGui::Checkbox("Perl", &settings.esp.Perl);
                ImGui::Checkbox("Crystal", &settings.esp.Crystal);
                ImGui::Checkbox("Explorer Notes", &settings.esp.ExplorerNotes);
                ImGui::Checkbox("Eggs", &settings.esp.Eggs);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Visibility"))
            {
                ImGui::Checkbox("Hide Structure Switch", &settings.esp.HideStructureSwitch);
                ImGui::Checkbox("Hide Resource Switch", &settings.esp.HideResourceSwitch);
                ImGui::Checkbox("Hide Dinosaur Switch", &settings.esp.HideDinosaurSwitch);
                ImGui::Checkbox("Hide Structure", &settings.esp.HideStructure);
                ImGui::Checkbox("Hide Dinosaur", &settings.esp.HideDinosaur);
                ImGui::Checkbox("Hide Resource", &settings.esp.HideResource);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Colors"))
            {
                ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_NoInputs;

                ImGui::ColorEdit3("Ally Player", settings.esp.PlayerAllyColor, colorFlags);
                ImGui::ColorEdit3("Team Player", settings.esp.PlayerTeamColor, colorFlags);
                ImGui::ColorEdit3("Enemy Player", settings.esp.PlayerEnemyColor, colorFlags);

                ImGui::ColorEdit3("Wild Dino", settings.esp.DinoWildColor, colorFlags);
                ImGui::ColorEdit3("Ally Dino", settings.esp.DinoAllyColor, colorFlags);
                ImGui::ColorEdit3("Team Dino", settings.esp.DinoTeamColor, colorFlags);
                ImGui::ColorEdit3("Enemy Dino", settings.esp.DinoEnemyColor, colorFlags);

                ImGui::ColorEdit3("Structure Ally", settings.esp.StructureAllyColor, colorFlags);
                ImGui::ColorEdit3("Structure Team", settings.esp.StructureTeamColor, colorFlags);

                ImGui::ColorEdit3("Containers", settings.esp.ContainersColor, colorFlags);
                ImGui::ColorEdit3("Beds", settings.esp.BedsColor, colorFlags);
                ImGui::ColorEdit3("Explosives", settings.esp.ExplosivesColor, colorFlags);
                ImGui::ColorEdit3("Turrets", settings.esp.TurretsColor, colorFlags);
                ImGui::ColorEdit3("Supply Crate", settings.esp.SupplyCrateColor, colorFlags);
                ImGui::ColorEdit3("Item Cache", settings.esp.ItemCacheColor, colorFlags);
                ImGui::ColorEdit3("Plant X", settings.esp.PlantXColor, colorFlags);

                ImGui::ColorEdit3("Resources", settings.esp.ResourceColor, colorFlags);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    else if (currentTab == MainTab::Misc)
    {
        ImGui::SliderFloat("FOV", &settings.FOV, 10.0f, 175.0f);
        ImGui::SliderFloat("##Speed", &settings.PlayerSpeed, 1.0f, 20.0f);
        ImGui::SameLine();
        ImGui::Checkbox("Speed", &settings.UsePlayerSpeed);

        ImGui::Checkbox("World Settings", &settings.EnableTime);
        if (settings.EnableTime)
        {
            ImGui::SliderFloat("Sky Cycle", &settings.DayCycleSky, 30, 120);
            ImGui::SliderFloat("Day Cycle", &settings.DayCycle, 1, 10);
            ImGui::SliderFloat("Brightness", &settings.DayInterpolationContainer, 1, 10);
            ImGui::SliderFloat("Night Sky", &settings.SM4_Sky, 1, 10);
            ImGui::SliderFloat("Sun Position", &settings.Sunlight, 30, 120);
        }

        ImGui::Separator();

        ImGui::Columns(2, nullptr, false);
        ImGui::Checkbox("Ghost Mode", &settings.GhostMode);
        ImGui::NextColumn();
        ImGui::Checkbox("Force Optimize Game", &settings.ForceOptimizeGame);
        ImGui::NextColumn();
        ImGui::Checkbox("Tame Shooting", &settings.TameShooting);
        ImGui::NextColumn();
        ImGui::Checkbox("No Scope Sway", &settings.NoScopeSway);
        ImGui::NextColumn();
        ImGui::Checkbox("No Scope Overlay", &settings.NoScopeOverlay);
        ImGui::NextColumn();
        ImGui::Checkbox("Auto Armor", &settings.AutoArmor);
        ImGui::NextColumn();
        ImGui::Checkbox("Auto Meds", &settings.AutoMeds);
        ImGui::NextColumn();
        ImGui::Checkbox("Auto Remount", &settings.AutoRemount);
        ImGui::NextColumn();
        ImGui::Checkbox("Auto Pickup Eggs", &settings.AutoPickupEggs);
        ImGui::EndColumns();

        ImGui::Checkbox("Auto Loot", &settings.AutoSteal);
        if (settings.AutoSteal)
        {
            ImGui::Indent();
            ImGui::Checkbox("Loot Players", &settings.LootPlayers);
            ImGui::Checkbox("Loot Turrets", &settings.LootTurrets);
            ImGui::Checkbox("Loot Supply Crates", &settings.LootSupplyCrates);
            ImGui::Checkbox("Loot Containers", &settings.LootContainers);
            ImGui::Unindent();
        }
    }
    else if (currentTab == MainTab::Aimbot)
    {
        ImGui::Checkbox("Enable", &settings.EnableAimbot);
        if (settings.EnableAimbot)
        {
            ImGui::RadioButton("Hard Lock", &settings.AimType, 0);
            ImGui::RadioButton("Silent Aim", &settings.AimType, 1);
        }
    }
    ImGui::EndChild();
}



//======================================================================================================================
struct MenuVariables {
    float winWidth = 0.0f;
    float winHeight = 0.0f;
    float winPosWidth = 0.0f;
    float winPosHeight = 0.0f;
    ImVec4 textColor = ImVec4(0.990f, 0.0396f, 0.816f,
                              1.00f); // R G B + GAMMA | Using floats, check readme for picker
    ImVec4 color_red = ImVec4(1.0f, 0.0f, 0.0f, 1.00f); // RED - rgb01(1, 0, 0) | #ff0000
    ImVec4 color_purple = ImVec4(1.0f, 0.0f, 1.0f, 1.00f); // PURPLE - rgb01(1, 0, 1) | #ff00ff
    ImVec4 color_green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // GREEN - rgb01(0, 1, 0) | #00ff00
} MenuVars;

//========================================================================================================================
extern "C"
{
    JNIEXPORT void JNICALL Java_ark_use_sishen_ClipBoard_copyToClipboard(JNIEnv *env, jclass clazz, jstring text);
}

void copyToClipboard(JNIEnv *env, jobject context, const char* text) {
    // Find the ClipBoard class
    jclass clazz = env->FindClass("ark/use/sishen/ClipBoard");
    if (clazz == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "ClipBoardJNI", "Unable to find class ark/use/sishen/ClipBoard");
        return;
    }

    // Initialize the ClipBoard class with the context
    jmethodID initializeMethod = env->GetStaticMethodID(clazz, "initialize", "(Landroid/content/Context;)V");
    if (initializeMethod == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "ClipBoardJNI", "Unable to find method initialize");
        return;
    }
    env->CallStaticVoidMethod(clazz, initializeMethod, context);

    // Convert the C++ string to jstring
    jstring jText = env->NewStringUTF(text);

    // Call the JNI function
    Java_the_best_cheat_ClipBoard_copyToClipboard(env, clazz, jText);

    // Clean up
    env->DeleteLocalRef(jText);
}

void CalculateOptimalMenuSize(float screenWidth, float screenHeight, float& winWidth, float& winHeight)
{
    // Calculăm aspect ratio-ul
    float aspectRatio = screenWidth / screenHeight;

    // Stabilim dimensiunile în funcție de rezoluție și aspect ratio
    if (screenWidth >= 3840) { // 4K+
        winWidth = screenWidth * (aspectRatio > 2.0f ? 0.4f : 0.45f);
        winHeight = screenHeight * 0.5f;
    }
    else if (screenWidth >= 3000) { // Rezoluții înalte (2700x1440 etc)
        winWidth = screenWidth * 0.53f;
        winHeight = screenHeight * 0.63f;
    }
    else if (screenWidth >= 2700) { // Rezoluții înalte (2700x1440 etc)
        winWidth = screenWidth * 0.50f;
        winHeight = screenHeight * 0.60f;
    }
    else if (screenWidth >= 1980) { // Full HD (1920x1080)
        winWidth = screenWidth * (aspectRatio > 1.7f ? 0.53f : 0.58f);
        winHeight = screenHeight * 0.62f;
    }
    else if (screenWidth >= 1366) { // HD (1366x768)
        winWidth = screenWidth * (aspectRatio < 1.5f ? 0.7f : 0.65f);
        winHeight = screenHeight * 0.7f;
    }
    else if (screenWidth >= 1024) { // Tablete mici (1024x768)
        winWidth = screenWidth * 0.8f;
        winHeight = screenHeight * 0.75f;
    }
    else { // Telefoane mici (sub 1024px)
        winWidth = screenWidth * 0.9f;
        winHeight = screenHeight * 0.8f;
    }

    // Corecții finale
    winWidth = ImMin(winWidth, screenWidth * 0.95f);
    winHeight = ImMin(winHeight, screenHeight * 0.95f);
}

//==========================================================================================================
void DrawMenu()
{
    ImGui::SetNextWindowPos(ImVec2(ScreenWidth * 0.5f, ScreenHeight * 0.5f),ImGuiCond_Once);
    ImGui::SetNextWindowSize({ScreenWidth * 0.4f, ScreenHeight * 0.7f}, ImGuiCond_Once);

    ImGui::SetNextWindowCollapsed(true, ImGuiCond_::ImGuiCond_Once);
    ImGui::Begin("SishenTool", &ShowMenu, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse);
    g_window = ImGui::GetCurrentWindow();

    RenderMainMenu();

    ImGui::End();
}
