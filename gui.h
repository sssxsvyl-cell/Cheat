#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <functional>
#include "settings.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace GUI {
    // ============ Renk Yardımcıları ============
    struct Color {
        float r, g, b, a;
        
        Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
        Color(float red, float green, float blue, float alpha = 1.0f) 
            : r(red), g(green), b(blue), a(alpha) {}
        
        DWORD ToDWORD() {
            return D3DCOLOR_RGBA((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
        }
    };
    
    // ============ Font Yönetimi ============
    class Font {
    private:
        ID3DXFont* font;
        int size;
        std::string name;
        
    public:
        Font() : font(nullptr), size(14), name("Arial") {}
        
        bool Create(IDirect3DDevice9* device, const std::string& fontName = "Arial", int fontSize = 14) {
            name = fontName;
            size = fontSize;
            
            return D3DXCreateFont(
                device,
                fontSize, 0, FW_NORMAL, 1, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                fontName.c_str(),
                &font
            ) == S_OK;
        }
        
        void DrawText(IDirect3DDevice9* device, const std::string& text, int x, int y, Color color) {
            if (!font) return;
            
            RECT rect = { x, y, x + 800, y + 50 };
            font->DrawText(NULL, text.c_str(), -1, &rect, DT_LEFT, color.ToDWORD());
        }
        
        void DrawTextCentered(IDirect3DDevice9* device, const std::string& text, int x, int y, Color color) {
            if (!font) return;
            
            RECT rect = { x, y, x + 800, y + 50 };
            font->DrawText(NULL, text.c_str(), -1, &rect, DT_CENTER, color.ToDWORD());
        }
        
        void DrawTextRight(IDirect3DDevice9* device, const std::string& text, int x, int y, Color color) {
            if (!font) return;
            
            RECT rect = { x, y, x + 800, y + 50 };
            font->DrawText(NULL, text.c_str(), -1, &rect, DT_RIGHT, color.ToDWORD());
        }
        
        ~Font() {
            if (font) font->Release();
        }
    };
    
    // ============ Çizim Yardımcıları ============
    class Draw {
    private:
        IDirect3DDevice9* device;
        ID3DXLine* line;
        ID3DXSprite* sprite;
        
    public:
        Draw() : device(nullptr), line(nullptr), sprite(nullptr) {}
        
        bool Initialize(IDirect3DDevice9* dev) {
            device = dev;
            
            if (D3DXCreateLine(device, &line) != S_OK) return false;
            if (D3DXCreateSprite(device, &sprite) != S_OK) return false;
            
            return true;
        }
        
        void DrawBox(int x, int y, int width, int height, Color color, int thickness = 1) {
            if (!device || !line) return;
            
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
        
        void DrawLine(int x1, int y1, int x2, int y2, Color color, int thickness = 1) {
            if (!device || !line) return;
            
            D3DXVECTOR2 points[2];
            points[0] = D3DXVECTOR2((float)x1, (float)y1);
            points[1] = D3DXVECTOR2((float)x2, (float)y2);
            
            line->SetWidth((float)thickness);
            line->Begin();
            line->Draw(points, 2, color.ToDWORD());
            line->End();
        }
        
        void DrawCircle(int cx, int cy, int radius, Color color, int segments = 32) {
            if (!device || !line) return;
            
            std::vector<D3DXVECTOR2> points;
            for (int i = 0; i <= segments; i++) {
                float angle = 2.0f * 3.14159f * i / segments;
                points.push_back(D3DXVECTOR2(
                    cx + radius * cos(angle),
                    cy + radius * sin(angle)
                ));
            }
            
            line->SetWidth(1.0f);
            line->Begin();
            line->Draw(points.data(), points.size(), color.ToDWORD());
            line->End();
        }
        
        void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color, int thickness = 1) {
            if (!device || !line) return;
            
            D3DXVECTOR2 points[4];
            points[0] = D3DXVECTOR2((float)x1, (float)y1);
            points[1] = D3DXVECTOR2((float)x2, (float)y2);
            points[2] = D3DXVECTOR2((float)x3, (float)y3);
            points[3] = D3DXVECTOR2((float)x1, (float)y1);
            
            line->SetWidth((float)thickness);
            line->Begin();
            line->Draw(points, 4, color.ToDWORD());
            line->End();
        }
        
        ~Draw() {
            if (line) line->Release();
            if (sprite) sprite->Release();
        }
    };
    
    // ============ GUI Kontrolleri ============
    struct Rect {
        int x, y, width, height;
        
        Rect() : x(0), y(0), width(0), height(0) {}
        Rect(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h) {}
        
        bool Contains(int px, int py) {
            return px >= x && px <= x + width && py >= y && py <= y + height;
        }
    };
    
    class Control {
    protected:
        Rect rect;
        bool visible;
        bool hovered;
        bool pressed;
        std::string text;
        
    public:
        Control() : visible(true), hovered(false), pressed(false) {}
        virtual ~Control() {}
        
        virtual void Update(int mouseX, int mouseY, bool mouseDown) = 0;
        virtual void Draw(IDirect3DDevice9* device, Draw* draw, Font* font) = 0;
        
        void SetRect(int x, int y, int w, int h) { rect = Rect(x, y, w, h); }
        void SetText(const std::string& t) { text = t; }
        void SetVisible(bool v) { visible = v; }
        bool IsVisible() { return visible; }
        bool IsHovered() { return hovered; }
    };
    
    // ============ Buton Kontrolü ============
    class Button : public Control {
    private:
        std::function<void()> onClick;
        Color normalColor;
        Color hoverColor;
        Color pressColor;
        Color textColor;
        
    public:
        Button() {
            normalColor = Color(0.2f, 0.2f, 0.3f, 0.8f);
            hoverColor = Color(0.3f, 0.3f, 0.5f, 0.8f);
            pressColor = Color(0.1f, 0.1f, 0.2f, 0.8f);
            textColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        
        void SetOnClick(std::function<void()> callback) {
            onClick = callback;
        }
        
        void SetColors(Color normal, Color hover, Color press, Color text) {
            normalColor = normal;
            hoverColor = hover;
            pressColor = press;
            textColor = text;
        }
        
        void Update(int mouseX, int mouseY, bool mouseDown) override {
            hovered = rect.Contains(mouseX, mouseY);
            
            if (hovered && mouseDown && !pressed) {
                pressed = true;
                if (onClick) onClick();
            }
            
            if (!mouseDown) {
                pressed = false;
            }
        }
        
        void Draw(IDirect3DDevice9* device, Draw* draw, Font* font) override {
            if (!visible) return;
            
            Color color = normalColor;
            if (pressed) color = pressColor;
            else if (hovered) color = hoverColor;
            
            draw->DrawFilledBox(rect.x, rect.y, rect.width, rect.height, color);
            draw->DrawBox(rect.x, rect.y, rect.width, rect.height, Color(0.5f, 0.5f, 0.5f, 0.5f));
            
            if (font) {
                int textX = rect.x + (rect.width / 2);
                int textY = rect.y + (rect.height / 2) - 7;
                font->DrawTextCentered(device, text, textX, textY, textColor);
            }
        }
    };
    
    // ============ Checkbox Kontrolü ============
    class Checkbox : public Control {
    private:
        bool* value;
        std::function<void(bool)> onChange;
        Color boxColor;
        Color checkColor;
        Color textColor;
        
    public:
        Checkbox(bool* val = nullptr) : value(val) {
            boxColor = Color(0.2f, 0.2f, 0.3f, 0.8f);
            checkColor = Color(0.0f, 1.0f, 0.0f, 1.0f);
            textColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        
        void SetValue(bool* val) { value = val; }
        void SetOnChange(std::function<void(bool)> callback) { onChange = callback; }
        
        void Update(int mouseX, int mouseY, bool mouseDown) override {
            hovered = rect.Contains(mouseX, mouseY);
            
            if (hovered && mouseDown && !pressed) {
                pressed = true;
                if (value) {
                    *value = !*value;
                    if (onChange) onChange(*value);
                }
            }
            
            if (!mouseDown) {
                pressed = false;
            }
        }
        
        void Draw(IDirect3DDevice9* device, Draw* draw, Font* font) override {
            if (!visible) return;
            
            int boxSize = rect.height - 4;
            int boxX = rect.x;
            int boxY = rect.y + 2;
            
            // Checkbox kutusu
            draw->DrawFilledBox(boxX, boxY, boxSize, boxSize, boxColor);
            draw->DrawBox(boxX, boxY, boxSize, boxSize, Color(0.5f, 0.5f, 0.5f, 0.5f));
            
            // Check işareti
            if (value && *value) {
                draw->DrawLine(boxX + 2, boxY + boxSize / 2, boxX + boxSize / 2, boxY + boxSize - 2, checkColor);
                draw->DrawLine(boxX + boxSize / 2, boxY + boxSize - 2, boxX + boxSize - 2, boxY + 2, checkColor);
            }
            
            // Text
            if (font && !text.empty()) {
                int textX = boxX + boxSize + 5;
                int textY = rect.y + 2;
                font->DrawText(device, text, textX, textY, textColor);
            }
        }
    };
    
    // ============ Slider Kontrolü ============
    class Slider : public Control {
    private:
        float* value;
        float minValue;
        float maxValue;
        float step;
        bool dragging;
        std::function<void(float)> onChange;
        Color bgColor;
        Color fillColor;
        Color handleColor;
        Color textColor;
        
    public:
        Slider(float* val = nullptr, float min = 0.0f, float max = 100.0f, float st = 1.0f) 
            : value(val), minValue(min), maxValue(max), step(st), dragging(false) {
            bgColor = Color(0.1f, 0.1f, 0.2f, 0.8f);
            fillColor = Color(0.0f, 0.5f, 1.0f, 0.8f);
            handleColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
            textColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
        }
        
        void SetValue(float* val) { value = val; }
        void SetRange(float min, float max) { minValue = min; maxValue = max; }
        void SetOnChange(std::function<void(float)> callback) { onChange = callback; }
        
        void Update(int mouseX, int mouseY, bool mouseDown) override {
            hovered = rect.Contains(mouseX, mouseY);
            
            if (hovered && mouseDown && !dragging) {
                dragging = true;
            }
            
            if (dragging) {
                if (!mouseDown) {
                    dragging = false;
                    return;
                }
                
                float percent = (float)(mouseX - rect.x) / rect.width;
                if (percent < 0) percent = 0;
                if (percent > 1) percent = 1;
                
                float newValue = minValue + (maxValue - minValue) * percent;
                newValue = floor(newValue / step + 0.5f) * step;
                
                if (value) {
                    *value = newValue;
                    if (onChange) onChange(newValue);
                }
            }
        }
        
        void Draw(IDirect3DDevice9* device, Draw* draw, Font* font) override {
            if (!visible) return;
            
            int sliderY = rect.y + rect.height / 2 - 4;
            int handleX = rect.x + (int)((*value - minValue) / (maxValue - minValue) * rect.width);
            
            // Arka plan
            draw->DrawFilledBox(rect.x, sliderY, rect.width, 8, bgColor);
            
            // Dolu kısım
            draw->DrawFilledBox(rect.x, sliderY, handleX - rect.x, 8, fillColor);
            
            // Tutamak
            draw->DrawFilledBox(handleX - 5, sliderY - 3, 10, 14, handleColor);
            draw->DrawBox(handleX - 5, sliderY - 3, 10, 14, Color(0.5f, 0.5f, 0.5f, 0.5f));
            
            // Text
            if (font && !text.empty()) {
                char buffer[32];
                sprintf_s(buffer, "%s: %.1f", text.c_str(), *value);
                int textY = rect.y - 18;
                font->DrawText(device, buffer, rect.x, textY, textColor);
            }
        }
    };
    
    // ============ Ana Menü ============
    class Menu {
    private:
        IDirect3DDevice9* device;
        Draw draw;
        Font font;
        bool initialized;
        
        Rect windowRect;
        bool dragging;
        int dragX, dragY;
        
        std::vector<Control*> controls;
        std::vector<std::string> tabs;
        int currentTab;
        
        // Tab kontrolü için butonlar
        std::vector<Button*> tabButtons;
        
    public:
        Menu() : device(nullptr), initialized(false), dragging(false), currentTab(0) {
            windowRect = Rect(100, 100, 350, 400);
        }
        
        bool Initialize(IDirect3DDevice9* dev) {
            device = dev;
            
            if (!draw.Initialize(device)) return false;
            if (!font.Create(device, "Arial", 14)) return false;
            
            initialized = true;
            CreateControls();
            return true;
        }
        
        void CreateControls() {
            // Tab'lar
            tabs = { "Aimbot", "ESP", "Wallhack", "Misc" };
            
            // Aimbot tab'ı
            Checkbox* aimbotEnabled = new Checkbox();
            aimbotEnabled->SetValue(&Settings::Aimbot::fov);
            aimbotEnabled->SetText("Enable Aimbot");
            aimbotEnabled->SetRect(20, 50, 150, 20);
            controls.push_back(aimbotEnabled);
            
            Slider* fovSlider = new Slider(&Settings::Aimbot::fov, 0.0f, 180.0f, 1.0f);
            fovSlider->SetText("FOV");
            fovSlider->SetRect(20, 80, 200, 30);
            controls.push_back(fovSlider);
            
            Slider* smoothSlider = new Slider(&Settings::Aimbot::smooth, 1.0f, 10.0f, 0.5f);
            smoothSlider->SetText("Smooth");
            smoothSlider->SetRect(20, 120, 200, 30);
            controls.push_back(smoothSlider);
            
            Slider* distSlider = new Slider(&Settings::Aimbot::maxDistance, 50.0f, 300.0f, 10.0f);
            distSlider->SetText("Max Distance");
            distSlider->SetRect(20, 160, 200, 30);
            controls.push_back(distSlider);
            
            Checkbox* rcsEnabled = new Checkbox(&Settings::Aimbot::rcsEnabled);
            rcsEnabled->SetText("RCS (Recoil Control)");
            rcsEnabled->SetRect(20, 200, 150, 20);
            controls.push_back(rcsEnabled);
            
            // ESP tab'ı
            Checkbox* espEnabled = new Checkbox(&Settings::ESP::enabled);
            espEnabled->SetText("Enable ESP");
            espEnabled->SetRect(20, 50, 150, 20);
            controls.push_back(espEnabled);
            
            Checkbox* espBox = new Checkbox(&Settings::ESP::box);
            espBox->SetText("Box ESP");
            espBox->SetRect(20, 80, 150, 20);
            controls.push_back(espBox);
            
            Checkbox* espHealth = new Checkbox(&Settings::ESP::healthBar);
            espHealth->SetText("Health Bar");
            espHealth->SetRect(20, 110, 150, 20);
            controls.push_back(espHealth);
            
            Checkbox* espName = new Checkbox(&Settings::ESP::name);
            espName->SetText("Show Name");
            espName->SetRect(20, 140, 150, 20);
            controls.push_back(espName);
            
            Checkbox* espDist = new Checkbox(&Settings::ESP::distance);
            espDist->SetText("Show Distance");
            espDist->SetRect(20, 170, 150, 20);
            controls.push_back(espDist);
            
            // Wallhack tab'ı
            Checkbox* whEnabled = new Checkbox(&Settings::Wallhack::enabled);
            whEnabled->SetText("Enable Wallhack");
            whEnabled->SetRect(20, 50, 150, 20);
            controls.push_back(whEnabled);
            
            Checkbox* whEnemy = new Checkbox(&Settings::Wallhack::enemyGlow);
            whEnemy->SetText("Enemy Glow");
            whEnemy->SetRect(20, 80, 150, 20);
            controls.push_back(whEnemy);
            
            Checkbox* whTeam = new Checkbox(&Settings::Wallhack::teamGlow);
            whTeam->SetText("Team Glow");
            whTeam->SetRect(20, 110, 150, 20);
            controls.push_back(whTeam);
            
            Checkbox* whXQZ = new Checkbox(&Settings::Wallhack::xqz);
            whXQZ->SetText("XQZ (Wallhack)");
            whXQZ->SetRect(20, 140, 150, 20);
            controls.push_back(whXQZ);
            
            // Misc tab'ı
            Checkbox* infiniteHealth = new Checkbox(&Settings::Misc::infiniteHealth);
            infiniteHealth->SetText("Infinite Health");
            infiniteHealth->SetRect(20, 50, 150, 20);
            controls.push_back(infiniteHealth);
            
            Checkbox* infiniteAmmo = new Checkbox(&Settings::Misc::infiniteAmmo);
            infiniteAmmo->SetText("Infinite Ammo");
            infiniteAmmo->SetRect(20, 80, 150, 20);
            controls.push_back(infiniteAmmo);
            
            Checkbox* speedHack = new Checkbox(&Settings::Misc::speedHack);
            speedHack->SetText("Speed Hack");
            speedHack->SetRect(20, 110, 150, 20);
            controls.push_back(speedHack);
            
            Slider* speedSlider = new Slider(&Settings::Misc::speedValue, 1.0f, 20.0f, 0.5f);
            speedSlider->SetText("Speed Value");
            speedSlider->SetRect(20, 140, 200, 30);
            controls.push_back(speedSlider);
            
            Checkbox* noRecoil = new Checkbox(&Settings::Misc::noRecoil);
            noRecoil->SetText("No Recoil");
            noRecoil->SetRect(20, 180, 150, 20);
            controls.push_back(noRecoil);
            
            Checkbox* bunnyHop = new Checkbox(&Settings::Misc::bunnyHop);
            bunnyHop->SetText("Bunny Hop");
            bunnyHop->SetRect(20, 210, 150, 20);
            controls.push_back(bunnyHop);
        }
        
        void Update() {
            if (!initialized || !Settings::Menu::visible) return;
            
            POINT mouse;
            GetCursorPos(&mouse);
            ScreenToClient(FindWindow(NULL, "AssaultCube"), &mouse);
            
            bool mouseDown = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
            
            // Pencere sürükleme
            if (mouseDown && windowRect.Contains(mouse.x, mouse.y) && 
                mouse.y < windowRect.y + 30) {
                if (!dragging) {
                    dragging = true;
                    dragX = mouse.x - windowRect.x;
                    dragY = mouse.y - windowRect.y;
                }
            }
            
            if (dragging) {
                if (!mouseDown) {
                    dragging = false;
                } else {
                    windowRect.x = mouse.x - dragX;
                    windowRect.y = mouse.y - dragY;
                }
            }
            
            // Kontrolleri güncelle
            for (auto control : controls) {
                control->Update(mouse.x, mouse.y, mouseDown);
            }
        }
        
        void DrawMenu() {
            if (!initialized || !Settings::Menu::visible) return;
            
            // Arka plan
            draw.DrawFilledBox(windowRect.x, windowRect.y, windowRect.width, windowRect.height, Color(0.1f, 0.1f, 0.15f, 0.9f));
            draw.DrawBox(windowRect.x, windowRect.y, windowRect.width, windowRect.height, Color(0.0f, 0.5f, 1.0f, 0.8f), 2);
            
            // Başlık
            font.DrawText(device, "LUBV V1 - AssaultCube", windowRect.x + 10, windowRect.y + 8, Color(1.0f, 0.5f, 0.0f, 1.0f));
            
            // Close butonu (X)
            draw.DrawFilledBox(windowRect.x + windowRect.width - 25, windowRect.y + 5, 20, 20, Color(0.5f, 0.0f, 0.0f, 0.8f));
            font.DrawText(device, "X", windowRect.x + windowRect.width - 18, windowRect.y + 5, Color(1.0f, 1.0f, 1.0f, 1.0f));
            
            // Tab'lar
            int tabWidth = (windowRect.width - 20) / tabs.size();
            for (int i = 0; i < tabs.size(); i++) {
                int x = windowRect.x + 10 + i * tabWidth;
                int y = windowRect.y + 35;
                
                Color tabColor = (i == currentTab) ? 
                    Color(0.0f, 0.5f, 1.0f, 0.8f) : 
                    Color(0.2f, 0.2f, 0.3f, 0.6f);
                
                draw.DrawFilledBox(x, y, tabWidth - 2, 25, tabColor);
                font.DrawTextCentered(device, tabs[i], x + tabWidth / 2, y + 5, Color(1.0f, 1.0f, 1.0f, 1.0f));
            }
            
            // Kontrolleri çiz
            for (auto control : controls) {
                control->Draw(device, &draw, &font);
            }
        }
        
        void HandleInput(int key) {
            // Tab değiştirme
            if (key >= VK_F1 && key <= VK_F4) {
                currentTab = key - VK_F1;
            }
        }
        
        ~Menu() {
            for (auto control : controls) {
                delete control;
            }
            controls.clear();
        }
    };
    
    // ============ Global Menu Instance ============
    inline Menu* g_Menu = nullptr;
    
    // ============ Initialize GUI ============
    inline bool Initialize(IDirect3DDevice9* device) {
        if (!g_Menu) {
            g_Menu = new Menu();
        }
        return g_Menu->Initialize(device);
    }
    
    // ============ Update GUI ============
    inline void Update() {
        if (g_Menu) {
            g_Menu->Update();
        }
    }
    
    // ============ Draw GUI ============
    inline void Draw() {
        if (g_Menu) {
            g_Menu->DrawMenu();
        }
    }
    
    // ============ Cleanup GUI ============
    inline void Cleanup() {
        if (g_Menu) {
            delete g_Menu;
            g_Menu = nullptr;
        }
    }
}
```

hooks.h güncellemesi (GUI eklemek için):

```cpp
// hooks.h içinde EndScene Hook'u güncelle
int WINAPI HookedEndScene(IDirect3DDevice9* device) {
    // GUI'yi güncelle ve çiz
    if (GUI::g_Menu) {
        GUI::Update();
        GUI::Draw();
    }
    
    // ESP çiz
    DrawESP(device);
    
    // Orijinal fonksiyona devam et
    return originalEndScene(device);
}
