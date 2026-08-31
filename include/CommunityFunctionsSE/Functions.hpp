#pragma once

#include "RE/Skyrim.h"

namespace CommunityFunctionsSE {
    inline constexpr std::uint32_t kFunctionBase = 1000;

    [[nodiscard]] inline const RE::SCRIPT_FUNCTION* GetFunction(const std::uint32_t) noexcept {
        return nullptr;
    }
}
