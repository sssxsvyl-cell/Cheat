#pragma once
#include "player.h"
#include "offsets.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace Aimbot {
    // Ayarlar
    inline float aimFov = 90.0f;           // FOV derece
    inline float aimSmooth = 1.0f;          // 1.0 = anlık, 2.0 = yavaş
    inline bool visibleCheck = false;       // Sadece görünen hedeflere nişan al
    inline int aimKey = VK_RBUTTON;         // Sağ tuş
    inline float maxDistance = 150.0f;      // Maksimum menzil
    
    struct Target {
        Player player;
        float distance;
        float fov;
        float screenX;
        float screenY;
        
        Target() : distance(0), fov(360.0f), screenX(0), screenY(0) {}
        Target(Player p) : player(p), distance(0), fov(360.0f), screenX(0), screenY(0) {}
    };
    
    // Hedefi bul
    Target FindBestTarget(Player& local) {
        Target bestTarget;
        float bestFov = aimFov;
        float bestDist = maxDistance;
        
        std::uintptr_t listAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::entityList);
        int count = mem::Read<int>(mem::Base() + offsets::playerCount);
        
        if (count > 32) count = 32;
        
        for (int i = 0; i < count; i++) {
            std::uintptr_t entityAddr = mem::Read<std::uintptr_t>(listAddr + i * 4);
            Player entity(entityAddr);
            
            // Geçerlilik kontrolleri
            if (!entity.IsValid()) continue;
            if (entityAddr == local.GetAddress()) continue;
            if (entity.GetHealth() <= 0 || entity.GetHealth() > 100) continue;
            if (entity.GetTeam() == local.GetTeam()) continue;
            
            // Mesafe
            float dist = local.DistanceTo(entity);
            if (dist > maxDistance) continue;
            
            // Açı hesapla
            float dx = entity.GetPosX() - local.GetPosX();
            float dy = entity.GetPosY() - local.GetPosY();
            float dz = (entity.GetPosZ() - local.GetPosZ()) - 8.0f; // Kafa hizası
            
            float targetYaw = atan2(dy, dx) * 180.0f / 3.14159f;
            float targetPitch = -atan2(dz, sqrt(dx * dx + dy * dy)) * 180.0f / 3.14159f;
            
            // Mevcut açılarla fark
            float currentYaw = local.GetYaw();
            float currentPitch = local.GetPitch();
            
            float diffYaw = fabs(currentYaw - targetYaw);
            float diffPitch = fabs(currentPitch - targetPitch);
            
            // FOV hesapla (Pisagor)
            float fov = sqrt(diffYaw * diffYaw + diffPitch * diffPitch);
            
            if (fov < bestFov && fov < aimFov) {
                bestFov = fov;
                bestDist = dist;
                bestTarget = Target(entity);
                bestTarget.distance = dist;
                bestTarget.fov = fov;
            }
        }
        
        return bestTarget;
    }
    
    // Açıyı hedefe çevir
    void AimAtTarget(Player& local, Target& target) {
        if (!target.player.IsValid()) return;
        
        float dx = target.player.GetPosX() - local.GetPosX();
        float dy = target.player.GetPosY() - local.GetPosY();
        float dz = (target.player.GetPosZ() - local.GetPosZ()) - 8.0f;
        
        float targetYaw = atan2(dy, dx) * 180.0f / 3.14159f;
        float targetPitch = -atan2(dz, sqrt(dx * dx + dy * dy)) * 180.0f / 3.14159f;
        
        float currentYaw = local.GetYaw();
        float currentPitch = local.GetPitch();
        
        // Smooth aim
        if (aimSmooth > 1.0f) {
            float yawDiff = targetYaw - currentYaw;
            float pitchDiff = targetPitch - currentPitch;
            
            // Normalize
            if (yawDiff > 180) yawDiff -= 360;
            if (yawDiff < -180) yawDiff += 360;
            
            targetYaw = currentYaw + yawDiff / aimSmooth;
            targetPitch = currentPitch + pitchDiff / aimSmooth;
        }
        
        local.SetYaw(targetYaw);
        local.SetPitch(targetPitch);
    }
    
    // Ana aimbot fonksiyonu
    void Run() {
        std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
        Player local(localAddr);
        
        if (!local.IsValid()) return;
        if (local.GetHealth() <= 0) return;
        
        // Aimbot tuşu kontrolü
        if (GetAsyncKeyState(aimKey) & 0x8000) {
            Target best = FindBestTarget(local);
            
            if (best.player.IsValid()) {
                AimAtTarget(local, best);
            }
        }
    }
    
    // Triggerbot (otomatik ateş)
    void Triggerbot() {
        std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
        Player local(localAddr);
        
        if (!local.IsValid()) return;
        if (local.GetHealth() <= 0) return;
        
        // Çaprazda kimse var mı?
        std::uintptr_t listAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::entityList);
        int count = mem::Read<int>(mem::Base() + offsets::playerCount);
        
        if (count > 32) count = 32;
        
        for (int i = 0; i < count; i++) {
            std::uintptr_t entityAddr = mem::Read<std::uintptr_t>(listAddr + i * 4);
            Player entity(entityAddr);
            
            if (!entity.IsValid()) continue;
            if (entityAddr == localAddr) continue;
            if (entity.GetHealth() <= 0) continue;
            if (entity.GetTeam() == local.GetTeam()) continue;
            
            // Çaprazda mı?
            float dx = entity.GetPosX() - local.GetPosX();
            float dy = entity.GetPosY() - local.GetPosY();
            float dz = (entity.GetPosZ() - local.GetPosZ()) - 8.0f;
            
            float targetYaw = atan2(dy, dx) * 180.0f / 3.14159f;
            float targetPitch = -atan2(dz, sqrt(dx * dx + dy * dy)) * 180.0f / 3.14159f;
            
            float diffYaw = fabs(local.GetYaw() - targetYaw);
            float diffPitch = fabs(local.GetPitch() - targetPitch);
            
            // Crosshair 3 derece içindeyse ateş et
            if (diffYaw < 3.0f && diffPitch < 3.0f) {
                // Ateş et (in_attack)
                mem::Write<int>(mem::Base() + 0x1A0E98, 1);
                return;
            }
        }
    }
    
    // RCS (Recoil Control System)
    void NoRecoil() {
        std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
        Player local(localAddr);
        
        if (!local.IsValid()) return;
        
        // Recoil offsetleri
        mem::Write<float>(localAddr + 0x138, 0.0f);  // recoil yaw
        mem::Write<float>(localAddr + 0x13C, 0.0f);  // recoil pitch
    }
}
