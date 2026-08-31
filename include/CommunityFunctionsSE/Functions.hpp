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
}
