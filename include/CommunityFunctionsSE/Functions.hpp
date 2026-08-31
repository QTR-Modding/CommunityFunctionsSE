#pragma once

#include "CommunityFunctionsSE/Registration.hpp"
#include "CommunityFunctionsSE/Functions/Quantumyilmaz/WouldBeStealing.hpp"

namespace CommunityFunctionsSE {
    namespace detail {
        inline constexpr std::array entries{
            Normalize(WouldBeStealing::registration)
        };

        static_assert(IsValid(entries), "Invalid community function registration");

        inline constexpr auto functions = BuildRegistry<GetLookupSize(entries)>(entries);
    }

    [[nodiscard]] constexpr const RE::SCRIPT_FUNCTION* GetFunction(const std::uint32_t a_id) noexcept {
        if (a_id < kFunctionBase) {
            return nullptr;
        }

        const auto index = a_id - kFunctionBase;
        return index < detail::functions.size() ? detail::functions[index] : nullptr;
    }
}
