#pragma once

#include "RE/Skyrim.h"

namespace CommunityFunctionsSE {
    inline constexpr std::uint32_t kFunctionBase = 1000;
    inline constexpr std::uint32_t kWouldBeStealing = kFunctionBase;

    namespace detail {
        inline bool WouldBeStealing(RE::TESObjectREFR* a_subject, void* a_target, void*, double& a_result) {
            const auto actor = a_subject ? a_subject->As<RE::Actor>() : nullptr;
            const auto target = static_cast<RE::TESObjectREFR*>(a_target);
            if (!actor || !target) {
                return false;
            }

            a_result = actor->WouldBeStealing(target);
            return true;
        }

        inline RE::SCRIPT_PARAMETER wouldBeStealingParameters[]{
            { .paramName = "Object Reference", .paramType = RE::SCRIPT_PARAM_TYPE::kObjectRef }
        };

        inline const RE::SCRIPT_FUNCTION wouldBeStealing{
            .functionName = "WouldBeStealing",
            .numParams = static_cast<std::uint16_t>(std::size(wouldBeStealingParameters)),
            .params = wouldBeStealingParameters,
            .conditionFunction = WouldBeStealing
        };
    }

    [[nodiscard]] inline const RE::SCRIPT_FUNCTION* GetFunction(const std::uint32_t a_id) noexcept {
        return a_id == kWouldBeStealing ? &detail::wouldBeStealing : nullptr;
    }
}
