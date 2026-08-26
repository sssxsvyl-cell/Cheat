namespace offsets {
    // Base adresler
    inline constexpr std::uintptr_t localPlayer = 0x57E0A8;      // doğru offset
    inline constexpr std::uintptr_t entityList = 0x18AC04;      // doğru offset
    inline constexpr std::uintptr_t playerCount = 0x18AC0C;     // doğru offset
    
    // Player struct offsetleri
    inline constexpr std::uintptr_t health = 0xEC;
    inline constexpr std::uintptr_t posX = 0x4;
    inline constexpr std::uintptr_t posY = 0x8;
    inline constexpr std::uintptr_t posZ = 0xC;
    inline constexpr std::uintptr_t yaw = 0x34;
    inline constexpr std::uintptr_t pitch = 0x38;
    inline constexpr std::uintptr_t team = 0x30;
    inline constexpr std::uintptr_t name = 0x205;
}
