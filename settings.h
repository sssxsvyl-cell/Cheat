#pragma once
#include <windows.h>
#include <string>

namespace Settings {
    // ============ Aimbot Ayarları ============
    namespace Aimbot {
        inline float fov = 90.0f;              // Hedef bulma FOV'u (derece)
        inline float smooth = 2.0f;            // 1.0 = anlık, 2.0-5.0 = yumuşak
        inline float maxDistance = 150.0f;     // Maksimum nişan alma mesafesi
        inline int aimKey = VK_RBUTTON;        // VK_RBUTTON = sağ tık
        inline bool visibleCheck = false;      // Sadece görünen hedefler
        inline bool priorityHead = true;       // Öncelikli hedef kafa mı?
        inline bool rcsEnabled = true;         // Recoil kontrolü
        inline bool silentAim = false;         // Silent aim (görünmez)
        inline float rcsIntensity = 0.7f;      // RCS şiddeti (0-1)
    }
    
    // ============ ESP Ayarları ============
    namespace ESP {
        inline bool enabled = true;
        inline bool box = true;                // Kutu çiz
        inline bool healthBar = true;          // Can çubuğu
        inline bool name = true;               // İsim göster
        inline bool distance = true;           // Mesafe göster
        inline bool line = true;               // Çizgi (çaprazdan hedefe)
        inline bool skeleton = false;          // İskelet çiz
        inline bool headDot = true;            // Kafa noktası
        inline bool enemyOnly = true;          // Sadece düşmanlar
        inline bool teamColor = true;          // Takım renkleri
        inline float maxDistance = 200.0f;     // Maksimum gösterim mesafesi
        
        // Renkler (RGBA)
        inline float enemyColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };     // Kırmızı
        inline float teamColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };      // Mavi
        inline float healthColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };    // Yeşil
        inline float lineColor[4] = { 1.0f, 1.0f, 0.0f, 0.5f };      // Sarı (yarı saydam)
        
        // Yazı boyutları
        inline int fontSize = 14;
        inline int boxThickness = 2;
    }
    
    // ============ Triggerbot Ayarları ============
    namespace Triggerbot {
        inline bool enabled = true;
        inline int triggerKey = VK_LSHIFT;     // Sol Shift
        inline float delay = 50.0f;            // MS cinsinden gecikme
        inline float fov = 3.0f;               // Çapraz hassasiyeti (derece)
        inline bool onlyEnemy = true;
        inline bool autoFire = false;          // Otomatik ateş (tık gerekmez)
        inline bool weaponCheck = true;        // Sadece silah varken
    }
    
    // ============ Wallhack/Chams Ayarları ============
    namespace Wallhack {
        inline bool enabled = true;
        inline bool enemyGlow = true;          // Düşman parlaklığı
        inline bool teamGlow = false;          // Takım parlaklığı
        inline bool fullBright = false;        // Tam parlaklık
        inline float glowIntensity = 1.0f;     // Parlaklık şiddeti
        inline bool wireframe = false;         // Tel kafes modu
        inline bool xqz = true;                // Duvarları görünmez yap
    }
    
    // ============ Miscellaneous (Diğer) Ayarlar ============
    namespace Misc {
        inline bool speedHack = false;
        inline float speedValue = 5.0f;        // Hız çarpanı
        inline bool infiniteHealth = false;
        inline bool infiniteAmmo = false;
        inline bool noRecoil = true;
        inline bool noSpread = false;
        inline bool noFlash = true;
        inline bool noFog = true;
        inline bool noScope = false;
        inline bool autoStrafe = false;
        inline bool bunnyHop = false;          // Zıplama
        inline bool radarHack = true;          // Harita herkesi gösterir
        inline bool antiKick = false;          // Kick koruması
        
        // Hızlı tuşlar (Toggles)
        inline int keyMenu = VK_INSERT;        // Menü aç/kapat
        inline int keyAimbot = VK_F1;          // Aimbot aç/kapat
        inline int keyESP = VK_F2;             // ESP aç/kapat
        inline int keyWallhack = VK_F3;        // Wallhack aç/kapat
        inline int keyTriggerbot = VK_F4;      // Triggerbot aç/kapat
        inline int keyHealth = VK_F5;          // Sınırsız can
        inline int keyAmmo = VK_F6;            // Sınırsız mermi
        inline int keySpeed = VK_F7;           // Speedhack
        inline int keyExit = VK_END;           // Çıkış
    }
    
    // ============ Menü Ayarları ============
    namespace Menu {
        inline bool visible = false;           // Menü görünür mü?
        inline int posX = 100;                 // Menü X pozisyonu
        inline int posY = 100;                 // Menü Y pozisyonu
        inline int width = 300;                // Menü genişliği
        inline int height = 400;               // Menü yüksekliği
        inline bool rainbow = false;           // Rainbow renkler
        inline int selectedTab = 0;            // 0=Aimbot, 1=ESP, 2=Misc
        inline int selectedOption = 0;
    }
    
    // ============ Profil Yönetimi ============
    namespace Profile {
        inline std::string currentProfile = "default";
        inline bool autoSave = true;
        inline int saveInterval = 60;          // Saniye
    }
    
    // ============ Yardımcı Fonksiyonlar ============
    inline void LoadDefaults() {
        // Aimbot
        Aimbot::fov = 90.0f;
        Aimbot::smooth = 2.0f;
        Aimbot::maxDistance = 150.0f;
        Aimbot::aimKey = VK_RBUTTON;
        Aimbot::visibleCheck = false;
        Aimbot::priorityHead = true;
        Aimbot::rcsEnabled = true;
        Aimbot::silentAim = false;
        Aimbot::rcsIntensity = 0.7f;
        
        // ESP
        ESP::enabled = true;
        ESP::box = true;
        ESP::healthBar = true;
        ESP::name = true;
        ESP::distance = true;
        ESP::line = true;
        ESP::skeleton = false;
        ESP::headDot = true;
        ESP::enemyOnly = true;
        ESP::teamColor = true;
        ESP::maxDistance = 200.0f;
        ESP::enemyColor[0] = 1.0f; ESP::enemyColor[1] = 0.0f; ESP::enemyColor[2] = 0.0f; ESP::enemyColor[3] = 1.0f;
        ESP::teamColor[0] = 0.0f; ESP::teamColor[1] = 0.0f; ESP::teamColor[2] = 1.0f; ESP::teamColor[3] = 1.0f;
        ESP::fontSize = 14;
        ESP::boxThickness = 2;
        
        // Triggerbot
        Triggerbot::enabled = true;
        Triggerbot::triggerKey = VK_LSHIFT;
        Triggerbot::delay = 50.0f;
        Triggerbot::fov = 3.0f;
        Triggerbot::onlyEnemy = true;
        Triggerbot::autoFire = false;
        Triggerbot::weaponCheck = true;
        
        // Wallhack
        Wallhack::enabled = true;
        Wallhack::enemyGlow = true;
        Wallhack::teamGlow = false;
        Wallhack::fullBright = false;
        Wallhack::glowIntensity = 1.0f;
        Wallhack::wireframe = false;
        Wallhack::xqz = true;
        
        // Misc
        Misc::speedHack = false;
        Misc::speedValue = 5.0f;
        Misc::infiniteHealth = false;
        Misc::infiniteAmmo = false;
        Misc::noRecoil = true;
        Misc::noSpread = false;
        Misc::noFlash = true;
        Misc::noFog = true;
        Misc::noScope = false;
        Misc::autoStrafe = false;
        Misc::bunnyHop = false;
        Misc::radarHack = true;
        Misc::antiKick = false;
        Misc::keyMenu = VK_INSERT;
        Misc::keyAimbot = VK_F1;
        Misc::keyESP = VK_F2;
        Misc::keyWallhack = VK_F3;
        Misc::keyTriggerbot = VK_F4;
        Misc::keyHealth = VK_F5;
        Misc::keyAmmo = VK_F6;
        Misc::keySpeed = VK_F7;
        Misc::keyExit = VK_END;
    }
    
    // Tuş durumunu kontrol et (tek basım için)
    inline bool IsKeyPressed(int key) {
        static bool keys[256] = { false };
        bool currentlyDown = GetAsyncKeyState(key) & 0x8000;
        
        if (currentlyDown && !keys[key]) {
            keys[key] = true;
            return true;
        }
        else if (!currentlyDown) {
            keys[key] = false;
        }
        return false;
    }
    
    // Toggle fonksiyonları
    inline void ToggleAimbot() {
        Aimbot::fov = (Aimbot::fov > 0.0f) ? 0.0f : 90.0f;
    }
    
    inline void ToggleESP() {
        ESP::enabled = !ESP::enabled;
    }
    
    inline void ToggleWallhack() {
        Wallhack::enabled = !Wallhack::enabled;
    }
    
    inline void ToggleTriggerbot() {
        Triggerbot::enabled = !Triggerbot::enabled;
    }
    
    inline void ToggleInfiniteHealth() {
        Misc::infiniteHealth = !Misc::infiniteHealth;
    }
    
    inline void ToggleInfiniteAmmo() {
        Misc::infiniteAmmo = !Misc::infiniteAmmo;
    }
    
    inline void ToggleSpeedHack() {
        Misc::speedHack = !Misc::speedHack;
    }
}
