#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "settings.h"
#include "memory.h"
#include "player.h"
#include "aimbot.h"
#include "esp.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace Hooks {
    // Original function types
    typedef HRESULT (WINAPI* EndScene_t)(IDirect3DDevice9* pDevice);
    typedef HRESULT (WINAPI* Reset_t)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
    
    // Original functions
    EndScene_t originalEndScene = nullptr;
    Reset_t originalReset = nullptr;
    
    // D3D9 Device
    IDirect3DDevice9* g_pDevice = nullptr;
    
    // ============ VTable Hook ============
    template<typename T>
    inline T HookVTable(void* instance, int index, T newFunction) {
        void** vtable = *(void***)instance;
        T oldFunction = (T)vtable[index];
        DWORD oldProtect;
        VirtualProtect(&vtable[index], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
        vtable[index] = (void*)newFunction;
        VirtualProtect(&vtable[index], sizeof(void*), oldProtect, &oldProtect);
        return oldFunction;
    }
    
    // ============ Get D3D9 Device ============
    IDirect3DDevice9* GetD3DDevice() {
        if (g_pDevice) return g_pDevice;
        
        // Method: Find window and get device via EndScene
        HWND hwnd = FindWindow(NULL, "AssaultCube");
        if (!hwnd) return nullptr;
        
        // Try to get device from window
        // This is a simplified approach - in practice you'd hook EndScene
        // and get device from there
        
        return nullptr;
    }
    
    // ============ Hooked EndScene ============
    HRESULT WINAPI HookedEndScene(IDirect3DDevice9* pDevice) {
        static bool initialized = false;
        
        if (!initialized) {
            g_pDevice = pDevice;
            
            // Initialize ESP
            ESP::Initialize(pDevice);
            
            initialized = true;
            printf("[Hooks] ESP initialized!\n");
        }
        
        // Draw ESP
        ESP::Update();
        
        // Return to original
        return originalEndScene(pDevice);
    }
    
    // ============ Hooked Reset ============
    HRESULT WINAPI HookedReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
        // Cleanup before reset
        ESP::Cleanup();
        
        HRESULT result = originalReset(pDevice, pPresentationParameters);
        
        // Reinitialize after reset
        if (result == D3D_OK) {
            ESP::Initialize(pDevice);
        }
        
        return result;
    }
    
    // ============ Initialize Hooks ============
    inline bool Initialize() {
        // Get D3D9 Device
        IDirect3DDevice9* device = GetD3DDevice();
        if (!device) {
            // Alternative: Hook EndScene via VTable
            // For this to work, we need to find the device properly
            
            // Try to get device from game
            HWND hwnd = FindWindow(NULL, "AssaultCube");
            if (!hwnd) return false;
            
            // We'll use a simpler approach - hook via pattern scanning
            // This is a placeholder for actual hook implementation
            
            printf("[Hooks] Waiting for D3D9 device...\n");
            return true;
        }
        
        // Hook EndScene (Index 42 in D3D9 device vtable)
        originalEndScene = HookVTable(device, 42, HookedEndScene);
        originalReset = HookVTable(device, 16, HookedReset);
        
        printf("[Hooks] D3D9 hooks installed! EndScene: 0x%p, Reset: 0x%p\n", 
               originalEndScene, originalReset);
        
        return true;
    }
    
    // ============ Cleanup Hooks ============
    inline void Cleanup() {
        // Restore original functions
        if (g_pDevice && originalEndScene) {
            // Unhook VTable
            void** vtable = *(void***)g_pDevice;
            DWORD oldProtect;
            VirtualProtect(&vtable[42], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
            vtable[42] = (void*)originalEndScene;
            VirtualProtect(&vtable[42], sizeof(void*), oldProtect, &oldProtect);
            
            VirtualProtect(&vtable[16], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
            vtable[16] = (void*)originalReset;
            VirtualProtect(&vtable[16], sizeof(void*), oldProtect, &oldProtect);
        }
        
        ESP::Cleanup();
        printf("[Hooks] Cleanup complete!\n");
    }
}
