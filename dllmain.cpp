#include <windows.h>
#include <cstdio>
#include "memory.h"
#include "offsets.h"
#include "player.h"
#include "settings.h"
#include "aimbot.h"
#include "esp.h"
#include "hooks.h"

DWORD WINAPI MainThread(LPVOID lpParam) {
    // Console
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    SetConsoleTitle("LUBV V1 - AssaultCube Cheat");
    
    printf("========================================\n");
    printf("     LUBV V1 - AssaultCube Cheat        \n");
    printf("========================================\n\n");
    
    // Load settings
    Settings::LoadDefaults();
    printf("[+] Settings loaded\n");
    
    // Wait for game
    printf("[*] Waiting for AssaultCube...\n");
    while (!FindWindow(NULL, "AssaultCube")) {
        Sleep(100);
    }
    printf("[+] Game found!\n");
    
    // Initialize hooks
    if (!Hooks::Initialize()) {
        printf("[-] Failed to initialize hooks!\n");
    } else {
        printf("[+] Hooks initialized!\n");
    }
    
    // Controls
    printf("\n========================================\n");
    printf("              CONTROLS                  \n");
    printf("========================================\n");
    printf("  F1  - Toggle Aimbot    [%s]\n", Settings::Aimbot::fov > 0 ? "ON" : "OFF");
    printf("  F2  - Toggle ESP       [%s]\n", Settings::ESP::enabled ? "ON" : "OFF");
    printf("  F3  - Toggle Wallhack  [%s]\n", Settings::Wallhack::enabled ? "ON" : "OFF");
    printf("  F4  - Toggle Triggerbot[%s]\n", Settings::Triggerbot::enabled ? "ON" : "OFF");
    printf("  F5  - Infinite Health  [%s]\n", Settings::Misc::infiniteHealth ? "ON" : "OFF");
    printf("  F6  - Infinite Ammo    [%s]\n", Settings::Misc::infiniteAmmo ? "ON" : "OFF");
    printf("  F7  - Speed Hack       [%s]\n", Settings::Misc::speedHack ? "ON" : "OFF");
    printf("  INS - Toggle Menu\n");
    printf("  END - Exit\n");
    printf("========================================\n\n");
    printf("[*] Cheat loaded successfully!\n\n");
    
    // Main loop
    while (true) {
        // Exit
        if (GetAsyncKeyState(Settings::Misc::keyExit) & 1) break;
        
        // Toggles
        if (Settings::IsKeyPressed(Settings::Misc::keyAimbot)) {
            Settings::ToggleAimbot();
            printf("[Aimbot] %s\n", Settings::Aimbot::fov > 0 ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keyESP)) {
            Settings::ToggleESP();
            printf("[ESP] %s\n", Settings::ESP::enabled ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keyWallhack)) {
            Settings::ToggleWallhack();
            printf("[Wallhack] %s\n", Settings::Wallhack::enabled ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keyTriggerbot)) {
            Settings::ToggleTriggerbot();
            printf("[Triggerbot] %s\n", Settings::Triggerbot::enabled ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keyHealth)) {
            Settings::ToggleHealth();
            printf("[Infinite Health] %s\n", Settings::Misc::infiniteHealth ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keyAmmo)) {
            Settings::ToggleAmmo();
            printf("[Infinite Ammo] %s\n", Settings::Misc::infiniteAmmo ? "ON" : "OFF");
        }
        if (Settings::IsKeyPressed(Settings::Misc::keySpeed)) {
            Settings::ToggleSpeed();
            printf("[Speed Hack] %s\n", Settings::Misc::speedHack ? "ON" : "OFF");
        }
        
        // Get local player
        Player local = Player::GetLocalPlayer();
        
        // Aimbot
        if (Settings::Aimbot::fov > 0) {
            Aimbot::Run();
        }
        
        // Triggerbot
        if (Settings::Triggerbot::enabled) {
            if (GetAsyncKeyState(Settings::Triggerbot::triggerKey) & 0x8000) {
                Aimbot::Triggerbot();
            }
        }
        
        // No Recoil
        if (Settings::Aimbot::rcsEnabled) {
            Aimbot::NoRecoil();
        }
        
        // Infinite Health
        if (Settings::Misc::infiniteHealth && local.IsValid()) {
            local.SetHealth(100);
            local.SetArmor(100);
        }
        
        // Infinite Ammo
        if (Settings::Misc::infiniteAmmo && local.IsValid()) {
            local.SetInfiniteAmmo();
        }
        
        // Speed Hack
        if (Settings::Misc::speedHack && local.IsValid()) {
            local.SpeedHack(Settings::Misc::speedValue);
        }
        
        // Bunny Hop
        if (Settings::Misc::bunnyHop && local.IsValid()) {
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                local.Jump();
            }
        }
        
        Sleep(10);
    }
    
    // Cleanup
    printf("\n[*] Cleaning up...\n");
    Hooks::Cleanup();
    
    FreeConsole();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, MainThread, NULL, 0, NULL);
    }
    return TRUE;
}
