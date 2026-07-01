#pragma once

#include <cstdint>
#include <unordered_map>

namespace Patch {

    class Addresses {

    public:
        Addresses();
        ~Addresses() = default;

        bool Load();

        uintptr_t GetAddress(const std::string& name) const;
        void SetAddress(const std::string& name, uintptr_t address);

    private:
        std::unordered_map<std::string, uintptr_t> m_Addresses;
        bool m_Loaded = false;
    };
}
