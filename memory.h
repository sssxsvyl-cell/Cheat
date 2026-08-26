#pragma once
#include <windows.h>
#include <cstdint>   // std::uintptr_t için
#include <synchapi.h> // WakeByAddressSingle için (isteğe bağlı)

namespace mem {
    inline std::uintptr_t Base() {
        return reinterpret_cast<std::uintptr_t>(GetModuleHandleA("ac_client.exe"));
    }
}

// Geçerli adres kontrolü - düzeltilmiş versiyon
inline bool Valid(std::uintptr_t address) {
    // Basitçe adresin 0 olup olmadığını ve modül aralığında olup olmadığını kontrol et
    return (address != 0 && address >= mem::Base() && address < mem::Base() + 0x1000000);
}
