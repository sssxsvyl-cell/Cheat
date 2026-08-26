#pragma once
#include <windows.h>
#include <string>
#include <cmath>
#include "memory.h"
#include "offsets.h"

namespace offsets {
    // Player struct offsetleri (PLAYER + X)
    inline constexpr std::uintptr_t health = 0xEC;
    inline constexpr std::uintptr_t armor = 0xF0;
    inline constexpr std::uintptr_t posX = 0x4;
    inline constexpr std::uintptr_t posY = 0x8;
    inline constexpr std::uintptr_t posZ = 0xC;
    inline constexpr std::uintptr_t yaw = 0x34;
    inline constexpr std::uintptr_t pitch = 0x38;
    inline constexpr std::uintptr_t team = 0x30;
    inline constexpr std::uintptr_t name = 0x205;
    inline constexpr std::uintptr_t weapon = 0x32C;
    inline constexpr std::uintptr_t ammo = 0x164;
    inline constexpr std::uintptr_t ammoMax = 0x168;
    inline constexpr std::uintptr_t isAlive = 0xEC; // health > 0
    inline constexpr std::uintptr_t isShooting = 0x1A0E98;
    inline constexpr std::uintptr_t velocity = 0x28;
    inline constexpr std::uintptr_t gravity = 0x2C;
    inline constexpr std::uintptr_t jump = 0x18;
    inline constexpr std::uintptr_t flags = 0x1C;
    inline constexpr std::uintptr_t viewMatrix = 0x1A0D8C;
}

class Player {
private:
    std::uintptr_t address;
    bool isValid;
    
public:
    // ============ Constructor / Destructor ============
    Player() : address(0), isValid(false) {}
    
    Player(std::uintptr_t addr) : address(addr) {
        isValid = mem::Valid(address);
    }
    
    ~Player() {}
    
    // ============ Temel Getter'lar ============
    std::uintptr_t GetAddress() const {
        return address;
    }
    
    bool IsValid() const {
        return isValid && mem::Valid(address);
    }
    
    void SetAddress(std::uintptr_t addr) {
        address = addr;
        isValid = mem::Valid(address);
    }
    
    // ============ Health ============
    int GetHealth() const {
        if (!IsValid()) return 0;
        return mem::Read<int>(address + offsets::health);
    }
    
    void SetHealth(int value) {
        if (!IsValid()) return;
        mem::Write<int>(address + offsets::health, value);
    }
    
    bool IsAlive() const {
        return GetHealth() > 0 && GetHealth() <= 100;
    }
    
    // ============ Armor ============
    int GetArmor() const {
        if (!IsValid()) return 0;
        return mem::Read<int>(address + offsets::armor);
    }
    
    void SetArmor(int value) {
        if (!IsValid()) return;
        mem::Write<int>(address + offsets::armor, value);
    }
    
    // ============ Position ============
    float GetPosX() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::posX);
    }
    
    float GetPosY() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::posY);
    }
    
    float GetPosZ() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::posZ);
    }
    
    void SetPosX(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::posX, value);
    }
    
    void SetPosY(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::posY, value);
    }
    
    void SetPosZ(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::posZ, value);
    }
    
    void GetPosition(float& x, float& y, float& z) const {
        x = GetPosX();
        y = GetPosY();
        z = GetPosZ();
    }
    
    void SetPosition(float x, float y, float z) {
        SetPosX(x);
        SetPosY(y);
        SetPosZ(z);
    }
    
    // ============ Angles ============
    float GetYaw() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::yaw);
    }
    
    float GetPitch() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::pitch);
    }
    
    void SetYaw(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::yaw, value);
    }
    
    void SetPitch(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::pitch, value);
    }
    
    void GetAngles(float& yaw, float& pitch) const {
        yaw = GetYaw();
        pitch = GetPitch();
    }
    
    void SetAngles(float yaw, float pitch) {
        SetYaw(yaw);
        SetPitch(pitch);
    }
    
    // ============ Team ============
    int GetTeam() const {
        if (!IsValid()) return -1;
        return mem::Read<int>(address + offsets::team);
    }
    
    void SetTeam(int value) {
        if (!IsValid()) return;
        mem::Write<int>(address + offsets::team, value);
    }
    
    bool IsEnemy(const Player& other) const {
        if (!IsValid() || !other.IsValid()) return false;
        return GetTeam() != other.GetTeam();
    }
    
    bool IsTeammate(const Player& other) const {
        if (!IsValid() || !other.IsValid()) return false;
        return GetTeam() == other.GetTeam();
    }
    
    // ============ Name ============
    std::string GetName() const {
        if (!IsValid()) return "";
        char name[64] = {0};
        mem::ReadMemory(address + offsets::name, name, sizeof(name) - 1);
        return std::string(name);
    }
    
    void SetName(const std::string& name) {
        if (!IsValid()) return;
        mem::WriteString(address + offsets::name, name);
    }
    
    // ============ Weapon ============
    std::uintptr_t GetWeaponAddress() const {
        if (!IsValid()) return 0;
        return mem::Read<std::uintptr_t>(address + offsets::weapon);
    }
    
    int GetAmmo() const {
        std::uintptr_t weapon = GetWeaponAddress();
        if (!weapon) return 0;
        return mem::Read<int>(weapon + offsets::ammo);
    }
    
    int GetAmmoMax() const {
        std::uintptr_t weapon = GetWeaponAddress();
        if (!weapon) return 0;
        return mem::Read<int>(weapon + offsets::ammoMax);
    }
    
    void SetAmmo(int value) {
        std::uintptr_t weapon = GetWeaponAddress();
        if (!weapon) return;
        mem::Write<int>(weapon + offsets::ammo, value);
        mem::Write<int>(weapon + offsets::ammoMax, value);
    }
    
    void SetInfiniteAmmo() {
        SetAmmo(999);
    }
    
    bool HasWeapon() const {
        return GetWeaponAddress() != 0;
    }
    
    // ============ Movement ============
    float GetVelocity() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::velocity);
    }
    
    void SetVelocity(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::velocity, value);
    }
    
    float GetGravity() const {
        if (!IsValid()) return 0.0f;
        return mem::Read<float>(address + offsets::gravity);
    }
    
    void SetGravity(float value) {
        if (!IsValid()) return;
        mem::Write<float>(address + offsets::gravity, value);
    }
    
    bool IsJumping() const {
        if (!IsValid()) return false;
        return mem::Read<int>(address + offsets::jump) == 1;
    }
    
    void Jump() {
        if (!IsValid()) return;
        mem::Write<int>(address + offsets::jump, 1);
    }
    
    // ============ Distance ============
    float DistanceTo(const Player& other) const {
        if (!IsValid() || !other.IsValid()) return 999999.0f;
        
        float dx = GetPosX() - other.GetPosX();
        float dy = GetPosY() - other.GetPosY();
        float dz = GetPosZ() - other.GetPosZ();
        
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    float DistanceTo(float x, float y, float z) const {
        if (!IsValid()) return 999999.0f;
        
        float dx = GetPosX() - x;
        float dy = GetPosY() - y;
        float dz = GetPosZ() - z;
        
        return sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    // ============ Angle Calculation ============
    void GetAngleTo(const Player& target, float& yaw, float& pitch) const {
        if (!IsValid() || !target.IsValid()) {
            yaw = 0;
            pitch = 0;
            return;
        }
        
        float dx = target.GetPosX() - GetPosX();
        float dy = target.GetPosY() - GetPosY();
        float dz = (target.GetPosZ() - GetPosZ()) - 8.0f; // Head height
        
        yaw = atan2(dy, dx) * 180.0f / 3.14159f;
        pitch = -atan2(dz, sqrt(dx * dx + dy * dy)) * 180.0f / 3.14159f;
    }
    
    float GetFOVTo(const Player& target) const {
        if (!IsValid() || !target.IsValid()) return 360.0f;
        
        float targetYaw, targetPitch;
        GetAngleTo(target, targetYaw, targetPitch);
        
        float currentYaw = GetYaw();
        float currentPitch = GetPitch();
        
        float diffYaw = fabs(currentYaw - targetYaw);
        float diffPitch = fabs(currentPitch - targetPitch);
        
        return sqrt(diffYaw * diffYaw + diffPitch * diffPitch);
    }
    
    bool IsInFOV(const Player& target, float fov) const {
        return GetFOVTo(target) <= fov;
    }
    
    // ============ Aim at Target ============
    void AimAt(const Player& target, float smooth = 1.0f) {
        if (!IsValid() || !target.IsValid()) return;
        
        float targetYaw, targetPitch;
        GetAngleTo(target, targetYaw, targetPitch);
        
        if (smooth > 1.0f) {
            float currentYaw = GetYaw();
            float currentPitch = GetPitch();
            
            float yawDiff = targetYaw - currentYaw;
            float pitchDiff = targetPitch - currentPitch;
            
            // Normalize
            if (yawDiff > 180.0f) yawDiff -= 360.0f;
            if (yawDiff < -180.0f) yawDiff += 360.0f;
            
            targetYaw = currentYaw + yawDiff / smooth;
            targetPitch = currentPitch + pitchDiff / smooth;
        }
        
        SetYaw(targetYaw);
        SetPitch(targetPitch);
    }
    
    // ============ Shooting ============
    void Shoot(bool shoot) {
        if (!IsValid()) return;
        mem::Write<int>(mem::Base() + offsets::isShooting, shoot ? 1 : 0);
    }
    
    void TriggerBot(const Player& target, float fov = 3.0f) {
        if (!IsValid() || !target.IsValid()) return;
        
        if (IsInFOV(target, fov)) {
            Shoot(true);
        } else {
            Shoot(false);
        }
    }
    
    // ============ Recoil ============
    void SetRecoil(float yaw, float pitch) {
        if (!IsValid()) return;
        mem::Write<float>(address + 0x138, yaw);
        mem::Write<float>(address + 0x13C, pitch);
    }
    
    void NoRecoil() {
        SetRecoil(0.0f, 0.0f);
    }
    
    // ============ Head Position ============
    void GetHeadPosition(float& x, float& y, float& z) const {
        x = GetPosX();
        y = GetPosY();
        z = GetPosZ() + 8.0f; // Head height
    }
    
    // ============ Feet Position ============
    void GetFeetPosition(float& x, float& y, float& z) const {
        x = GetPosX();
        y = GetPosY();
        z = GetPosZ() - 3.0f;
    }
    
    // ============ Visible Check ============
    bool IsVisible(const Player& target) const {
        // Basit görünürlük kontrolü - Trace/Raycast implementasyonu
        // Burada basitçe true döndürüyoruz, gerçek implementasyon için
        // oyunun içindeki trace fonksiyonları kullanılmalı
        return true;
    }
    
    // ============ Entity List ============
    static std::vector<Player> GetEntityList() {
        std::vector<Player> entities;
        
        std::uintptr_t listAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::entityList);
        int count = mem::Read<int>(mem::Base() + offsets::playerCount);
        
        if (count > 32) count = 32;
        
        for (int i = 0; i < count; i++) {
            std::uintptr_t entityAddr = mem::Read<std::uintptr_t>(listAddr + i * 4);
            Player entity(entityAddr);
            if (entity.IsValid()) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }
    
    static Player GetLocalPlayer() {
        std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
        return Player(localAddr);
    }
    
    // ============ Operator Overloads ============
    bool operator==(const Player& other) const {
        return address == other.address;
    }
    
    bool operator!=(const Player& other) const {
        return address != other.address;
    }
    
    bool operator<(const Player& other) const {
        return address < other.address;
    }
    
    // ============ Debug ============
    void PrintInfo() const {
        if (!IsValid()) {
            printf("Player: Invalid\n");
            return;
        }
        
        printf("=== Player Info ===\n");
        printf("Address: 0x%X\n", address);
        printf("Name: %s\n", GetName().c_str());
        printf("Team: %d\n", GetTeam());
        printf("Health: %d\n", GetHealth());
        printf("Armor: %d\n", GetArmor());
        printf("Position: %.2f, %.2f, %.2f\n", GetPosX(), GetPosY(), GetPosZ());
        printf("Angles: %.2f, %.2f\n", GetYaw(), GetPitch());
        printf("Ammo: %d/%d\n", GetAmmo(), GetAmmoMax());
        printf("Is Alive: %s\n", IsAlive() ? "Yes" : "No");
        printf("Has Weapon: %s\n", HasWeapon() ? "Yes" : "No");
        printf("===================\n");
    }
    
    // ============ Extra Features ============
    void SetGodMode() {
        SetHealth(100);
        SetArmor(100);
    }
    
    void TeleportTo(const Player& target) {
        if (!IsValid() || !target.IsValid()) return;
        SetPosition(target.GetPosX(), target.GetPosY(), target.GetPosZ());
    }
    
    void TeleportTo(float x, float y, float z) {
        SetPosition(x, y, z);
    }
    
    void SpeedHack(float speed) {
        SetVelocity(speed);
    }
    
    void FlyHack(bool enabled) {
        if (enabled) {
            SetGravity(0.0f);
        } else {
            SetGravity(800.0f); // Normal gravity
        }
    }
    
    void NoClip(bool enabled) {
        if (enabled) {
            // NoClip implementasyonu
            // Oyunun collision flag'ini kapat
            mem::Write<int>(address + offsets::flags, 0);
        } else {
            mem::Write<int>(address + offsets::flags, 1);
        }
    }
};

// ============ Memory Helper Extensions ============
namespace mem {
    inline void ReadMemory(std::uintptr_t address, void* buffer, size_t size) {
        ReadProcessMemory(GetCurrentProcess(), (LPCVOID)address, buffer, size, NULL);
    }
    
    inline void WriteMemory(std::uintptr_t address, const void* buffer, size_t size) {
        WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, buffer, size, NULL);
    }
    
    inline void WriteString(std::uintptr_t address, const std::string& str) {
        WriteMemory(address, str.c_str(), str.length() + 1);
    }
}
