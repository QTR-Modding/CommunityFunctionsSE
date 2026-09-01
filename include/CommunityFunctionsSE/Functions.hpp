#pragma once

#include "CommunityFunctionsSE/Registration.hpp"
#include "CommunityFunctionsSE/Functions/Quantumyilmaz/WouldBeStealing.hpp"

namespace CommunityFunctionsSE {
    namespace detail {
        inline constexpr std::array entries{
            WouldBeStealing::registration
        };

        static_assert(IsValid(entries), "Invalid community function registration");

        inline constexpr auto registry = BuildRegistry<GetLookupSize(entries)>(entries);

        [[nodiscard]] inline const RegistrationV1* GetEntry(const std::uint32_t a_id) noexcept {
            if (a_id < kFunctionBase) {
                return nullptr;
            }

            const auto index = a_id - kFunctionBase;
            if (index >= registry.size()) {
                return nullptr;
            }

            const auto entry = std::addressof(registry[index]);
            return IsCallable(*entry) ? entry : nullptr;
        }
    }
}
