#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>
#include <cmath>
#include "player.h"
#include "settings.h"
#include "memory.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace ESP {
    // ============ World to Screen ============
    struct Vector3 {
        float x, y, z;
        
        Vector3() : x(0), y(0), z(0) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    };
    
    struct Vector2 {
        float x, y;
        
        Vector2() : x(0), y(0) {}
        Vector2(float _x, float _y) : x(_x), y(_y) {}
    };
    
    // ViewMatrix için offset (AssaultCube)
    #define OFFSET_VIEWMATRIX 0x1A0D8C
    
    class WorldToScreen {
    private:
        float viewMatrix[4][4];
        int screenWidth;
        int screenHeight;
        
    public:
        WorldToScreen() {
            screenWidth = GetSystemMetrics(SM_CXSCREEN);
            screenHeight = GetSystemMetrics(SM_CYSCREEN);
        }
        
        bool Update() {
            // ViewMatrix'i oku
            std::uintptr_t matrixAddr = mem::Base() + OFFSET_VIEWMATRIX;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    viewMatrix[i][j] = mem::Read<float>(matrixAddr + (i * 16 + j * 4));
                }
            }
            return true;
        }
        
        bool WorldToScreen3D(Vector3 worldPos, Vector2& screenPos) {
            // 3D'den 2D'ye dönüştürme
            float w = viewMatrix[3][0] * worldPos.x + 
                      viewMatrix[3][1] * worldPos.y + 
                      viewMatrix[3][2] * worldPos.z + 
                      viewMatrix[3][3];
            
            if (w < 0.01f) return false; // Arkada
            
            float x = viewMatrix[0][0] * worldPos.x + 
                      viewMatrix[0][1] * worldPos.y + 
                      viewMatrix[0][2] * worldPos.z + 
                      viewMatrix[0][3];
            
            float y = viewMatrix[1][0] * worldPos.x + 
                      viewMatrix[1][1] * worldPos.y + 
                      viewMatrix[1][2] * worldPos.z + 
                      viewMatrix[1][3];
            
            // NDC'ye dönüştür
            x /= w;
            y /= w;
            
            // Ekran koordinatlarına dönüştür
            screenPos.x = (x + 1.0f) * 0.5f * screenWidth;
            screenPos.y = (1.0f - y) * 0.5f * screenHeight;
            
            return true;
        }
    };
    
    // ============ ESP Çizim Sınıfı ============
    class ESPRenderer {
    private:
        IDirect3DDevice9* device;
        ID3DXLine* line;
        ID3DXFont* font;
        WorldToScreen w2s;
        bool initialized;
        
        // Renkler
        struct Color {
            float r, g, b, a;
            
            Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
            Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
            
            DWORD ToDWORD() {
                return D3DCOLOR_RGBA((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
            }
        };
        
    public:
        ESPRenderer() : device(nullptr), line(nullptr), font(nullptr), initialized(false) {}
        
        bool Initialize(IDirect3DDevice9* dev) {
            device = dev;
            
            // Line oluştur
            if (D3DXCreateLine(device, &line) != S_OK) {
                return false;
            }
            
            // Font oluştur
            if (D3DXCreateFont(
                device,
                14, 0, FW_NORMAL, 1, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                "Arial",
                &font
            ) != S_OK) {
                return false;
            }
            
            initialized = true;
            return true;
        }
        
        void DrawLine(int x1, int y1, int x2, int y2, Color color, int thickness = 1) {
            if (!line || !device) return;
            
            D3DXVECTOR2 points[2];
            points[0] = D3DXVECTOR2((float)x1, (float)y1);
            points[1] = D3DXVECTOR2((float)x2, (float)y2);
            
            line->SetWidth((float)thickness);
            line->Begin();
            line->Draw(points, 2, color.ToDWORD());
            line->End();
        }
        
        void DrawBox(int x, int y, int width, int height, Color color, int thickness = 1) {
            if (!line || !device) return;
            
            D3DXVECTOR2 points[5];
            points[0] = D3DXVECTOR2((float)x, (float)y);
            points[1] = D3DXVECTOR2((float)(x + width), (float)y);
            points[2] = D3DXVECTOR2((float)(x + width), (float)(y + height));
            points[3] = D3DXVECTOR2((float)x, (float)(y + height));
            points[4] = D3DXVECTOR2((float)x, (float)y);
            
            line->SetWidth((float)thickness);
            line->Begin();
            line->Draw(points, 5, color.ToDWORD());
            line->End();
        }
        
        void DrawFilledBox(int x, int y, int width, int height, Color color) {
            if (!device) return;
            
            D3DRECT rect = { x, y, x + width, y + height };
            device->Clear(1, &rect, D3DCLEAR_TARGET, color.ToDWORD(), 0, 0);
        }
        
        void DrawText(const std::string& text, int x, int y, Color color) {
            if (!font || !device) return;
            
            RECT rect = { x, y, x + 800, y + 50 };
            font->DrawText(NULL, text.c_str(), -1, &rect, DT_LEFT, color.ToDWORD());
        }
        
        void DrawTextCentered(const std::string& text, int x, int y, Color color) {
            if (!font || !device) return;
            
            RECT rect = { x, y, x + 800, y + 50 };
            font->DrawText(NULL, text.c_str(), -1, &rect, DT_CENTER, color.ToDWORD());
        }
        
        void DrawHealthBar(int x, int y, int width, int height, int health, Color color) {
            if (health < 0) health = 0;
            if (health > 100) health = 100;
            
            // Arka plan (siyah)
            DrawFilledBox(x, y, width, height, Color(0.0f, 0.0f, 0.0f, 0.5f));
            
            // Can çubuğu
            int filledWidth = (int)((width - 2) * (health / 100.0f));
            if (filledWidth > 0) {
                DrawFilledBox(x + 1, y + 1, filledWidth, height - 2, color);
            }
            
            // Kenarlık
            DrawBox(x, y, width, height, Color(1.0f, 1.0f, 1.0f, 0.5f));
        }
        
        void DrawCornerBox(int x, int y, int width, int height, Color color, int size = 10, int thickness = 2) {
            // Sol üst
            DrawLine(x, y + size, x, y, color, thickness);
            DrawLine(x, y, x + size, y, color, thickness);
            
            // Sağ üst
            DrawLine(x + width, y + size, x + width, y, color, thickness);
            DrawLine(x + width - size, y, x + width, y, color, thickness);
            
            // Sol alt
            DrawLine(x, y + height - size, x, y + height, color, thickness);
            DrawLine(x, y + height, x + size, y + height, color, thickness);
            
            // Sağ alt
            DrawLine(x + width, y + height - size, x + width, y + height, color, thickness);
            DrawLine(x + width - size, y + height, x + width, y + height, color, thickness);
        }
        
        void DrawSkeleton(Player& player, Vector2 head, Vector2 feet) {
            // Basit iskelet - sadece baş ve vücut çizgisi
            DrawLine((int)head.x, (int)head.y, (int)feet.x, (int)feet.y, Color(1.0f, 1.0f, 1.0f, 0.7f), 1);
            
            // Kollar (basit)
            int midX = (int)((head.x + feet.x) / 2);
            int midY = (int)((head.y + feet.y) / 2);
            int armLen = (int)(abs(feet.y - head.y) * 0.3f);
            
            DrawLine((int)head.x, (int)head.y, midX - armLen, midY, Color(1.0f, 1.0f, 1.0f, 0.5f), 1);
            DrawLine((int)head.x, (int)head.y, midX + armLen, midY, Color(1.0f, 1.0f, 1.0f, 0.5f), 1);
            
            // Bacaklar (basit)
            DrawLine((int)feet.x, (int)feet.y, midX - armLen/2, midY + armLen, Color(1.0f, 1.0f, 1.0f, 0.5f), 1);
            DrawLine((int)feet.x, (int)feet.y, midX + armLen/2, midY + armLen, Color(1.0f, 1.0f, 1.0f, 0.5f), 1);
        }
        
        void DrawHeadDot(Vector2 head, Color color, int size = 4) {
            DrawFilledBox((int)(head.x - size/2), (int)(head.y - size/2), size, size, color);
        }
        
        ~ESPRenderer() {
            if (line) line->Release();
            if (font) font->Release();
        }
    };
    
    // ============ ESP Ana Sınıfı ============
    class ESPManager {
    private:
        ESPRenderer renderer;
        WorldToScreen w2s;
        bool initialized;
        
        // Oyuncu bilgilerini çiz
        void DrawPlayer(Player& local, Player& entity, Vector2 screenPos, float distance) {
            if (!Settings::ESP::enabled) return;
            
            // Renkleri belirle
            bool isEnemy = (entity.GetTeam() != local.GetTeam());
            bool isVisible = true; // Görünürlük kontrolü eklenecek
            
            Color color = isEnemy ? 
                Color(Settings::ESP::enemyColor[0], Settings::ESP::enemyColor[1], Settings::ESP::enemyColor[2], Settings::ESP::enemyColor[3]) :
                Color(Settings::ESP::teamColor[0], Settings::ESP::teamColor[1], Settings::ESP::teamColor[2], Settings::ESP::teamColor[3]);
            
            // Sadece düşmanlar
            if (Settings::ESP::enemyOnly && !isEnemy) return;
            
            // Box boyutları
            float height = 60.0f / (distance * 0.1f);
            if (height < 10.0f) height = 10.0f;
            if (height > 80.0f) height = 80.0f;
            
            float width = height * 0.5f;
            
            int boxX = (int)(screenPos.x - width / 2);
            int boxY = (int)(screenPos.y - height);
            
            // Box ESP
            if (Settings::ESP::box) {
                renderer.DrawBox(boxX, boxY, (int)width, (int)height, color, Settings::ESP::boxThickness);
            }
            
            // Corner Box
            if (Settings::ESP::box) {
                renderer.DrawCornerBox(boxX, boxY, (int)width, (int)height, color);
            }
            
            // Health Bar
            if (Settings::ESP::healthBar) {
                int health = entity.GetHealth();
                Color healthColor = Color(
                    1.0f - (health / 100.0f),
                    health / 100.0f,
                    0.0f,
                    1.0f
                );
                renderer.DrawHealthBar(boxX - 6, boxY, 4, (int)height, health, healthColor);
            }
            
            // İsim
            if (Settings::ESP::name) {
                std::string name = entity.GetName();
                renderer.DrawTextCentered(name, (int)screenPos.x, boxY - 20, color);
            }
            
            // Mesafe
            if (Settings::ESP::distance) {
                char distText[32];
                sprintf_s(distText, "%.0fm", distance);
                renderer.DrawTextCentered(distText, (int)screenPos.x, boxY + (int)height + 2, Color(1.0f, 1.0f, 1.0f, 0.7f));
            }
            
            // Head Dot
            if (Settings::ESP::headDot) {
                renderer.DrawHeadDot(screenPos, Color(1.0f, 0.0f, 0.0f, 1.0f));
            }
            
            // Skeleton
            if (Settings::ESP::skeleton) {
                Vector2 feetPos(screenPos.x, screenPos.y + height);
                renderer.DrawSkeleton(entity, screenPos, feetPos);
            }
            
            // Çizgi (çaprazdan hedefe)
            if (Settings::ESP::line) {
                int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2;
                int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2;
                renderer.DrawLine(screenCenterX, screenCenterY, (int)screenPos.x, (int)screenPos.y, 
                    Color(1.0f, 1.0f, 0.0f, 0.3f), 1);
            }
        }
        
    public:
        ESPManager() : initialized(false) {}
        
        bool Initialize(IDirect3DDevice9* device) {
            if (!renderer.Initialize(device)) return false;
            initialized = true;
            return true;
        }
        
        void UpdateAndDraw() {
            if (!initialized || !Settings::ESP::enabled) return;
            
            // ViewMatrix güncelle
            if (!w2s.Update()) return;
            
            // Local player
            std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
            Player local(localAddr);
            if (!local.IsValid()) return;
            if (local.GetHealth() <= 0) return;
            
            // Entity list
            std::uintptr_t listAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::entityList);
            int count = mem::Read<int>(mem::Base() + offsets::playerCount);
            if (count > 32) count = 32;
            
            // Her entity için
            for (int i = 0; i < count; i++) {
                std::uintptr_t entityAddr = mem::Read<std::uintptr_t>(listAddr + i * 4);
                Player entity(entityAddr);
                
                if (!entity.IsValid()) continue;
                if (entityAddr == localAddr) continue;
                
                int health = entity.GetHealth();
                if (health <= 0 || health > 150) continue;
                
                // Mesafe
                float distance = local.DistanceTo(entity);
                if (distance > Settings::ESP::maxDistance) continue;
                
                // World to Screen
                Vector3 worldPos(
                    entity.GetPosX(),
                    entity.GetPosY(),
                    entity.GetPosZ() + 5.0f // Kafa hizası
                );
                
                Vector2 screenPos;
                if (!w2s.WorldToScreen3D(worldPos, screenPos)) continue;
                
                // Ayak pozisyonu (daha düşük)
                Vector3 feetPos(
                    entity.GetPosX(),
                    entity.GetPosY(),
                    entity.GetPosZ() - 3.0f
                );
                
                Vector2 feetScreen;
                if (w2s.WorldToScreen3D(feetPos, feetScreen)) {
                    // Çiz
                    DrawPlayer(local, entity, screenPos, distance);
                }
            }
        }
        
        // Radar Hack
        void DrawRadar() {
            if (!Settings::Misc::radarHack) return;
            
            // Radar çizimi
            int radarX = GetSystemMetrics(SM_CXSCREEN) - 200;
            int radarY = 10;
            int radarSize = 150;
            
            // Radar arka planı
            renderer.DrawFilledBox(radarX, radarY, radarSize, radarSize, Color(0.0f, 0.0f, 0.0f, 0.5f));
            renderer.DrawBox(radarX, radarY, radarSize, radarSize, Color(0.5f, 0.5f, 0.5f, 0.5f));
            
            // Radar merkezi
            int centerX = radarX + radarSize / 2;
            int centerY = radarY + radarSize / 2;
            
            // Local player
            std::uintptr_t localAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::localPlayer);
            Player local(localAddr);
            if (!local.IsValid()) return;
            
            // Entity list
            std::uintptr_t listAddr = mem::Read<std::uintptr_t>(mem::Base() + offsets::entityList);
            int count = mem::Read<int>(mem::Base() + offsets::playerCount);
            if (count > 32) count = 32;
            
            for (int i = 0; i < count; i++) {
                std::uintptr_t entityAddr = mem::Read<std::uintptr_t>(listAddr + i * 4);
                Player entity(entityAddr);
                
                if (!entity.IsValid()) continue;
                if (entityAddr == localAddr) continue;
                if (entity.GetHealth() <= 0) continue;
                
                // Göreceli pozisyon
                float dx = entity.GetPosX() - local.GetPosX();
                float dy = entity.GetPosY() - local.GetPosY();
                
                // Radar boyutuna ölçekle
                float scale = 2.0f;
                int rx = (int)(dx * scale);
                int ry = (int)(dy * scale);
                
                // Radar sınırları
                if (abs(rx) > radarSize / 2 - 5) continue;
                if (abs(ry) > radarSize / 2 - 5) continue;
                
                // Renk
                bool isEnemy = (entity.GetTeam() != local.GetTeam());
                Color color = isEnemy ? Color(1.0f, 0.0f, 0.0f, 1.0f) : Color(0.0f, 0.0f, 1.0f, 1.0f);
                
                // Nokta çiz
                renderer.DrawFilledBox(
                    centerX + rx - 2,
                    centerY + ry - 2,
                    4, 4,
                    color
                );
            }
            
            // Merkez noktası
            renderer.DrawFilledBox(centerX - 3, centerY - 3, 6, 6, Color(0.0f, 1.0f, 0.0f, 1.0f));
        }
        
        void Cleanup() {
            // Renderer destructor handle eder
        }
    };
    
    // ============ Global ESP Instance ============
    inline ESPManager* g_ESP = nullptr;
    
    // ============ Initialize ESP ============
    inline bool Initialize(IDirect3DDevice9* device) {
        if (!g_ESP) {
            g_ESP = new ESPManager();
        }
        return g_ESP->Initialize(device);
    }
    
    // ============ Update ESP ============
    inline void Update() {
        if (g_ESP) {
            g_ESP->UpdateAndDraw();
            g_ESP->DrawRadar();
        }
    }
    
    // ============ Cleanup ESP ============
    inline void Cleanup() {
        if (g_ESP) {
            delete g_ESP;
            g_ESP = nullptr;
        }
    }
}
