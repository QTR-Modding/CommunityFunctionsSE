#pragma once

#include "CommunityFunctionsSE/Functions.hpp"

namespace CommunityFunctionsSE {
    [[nodiscard]] constexpr const RE::SCRIPT_FUNCTION* GetFunction(const std::uint32_t a_id) noexcept {
        if (a_id < kFunctionBase) {
            return nullptr;
        }

        const auto index = a_id - kFunctionBase;
        return index < detail::functions.size() ? detail::functions[index] : nullptr;
    }
}
