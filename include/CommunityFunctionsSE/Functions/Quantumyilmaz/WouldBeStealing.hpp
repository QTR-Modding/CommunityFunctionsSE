#pragma once

#include "CommunityFunctionsSE/Registration.hpp"

namespace CommunityFunctionsSE::WouldBeStealing {
    inline constexpr std::uint32_t kID = kFunctionBase;

    inline bool Evaluate(RE::TESObjectREFR* a_subject, void* a_target, void*, double& a_result) {
        const auto actor = a_subject ? a_subject->As<RE::Actor>() : nullptr;
        const auto target = static_cast<RE::TESObjectREFR*>(a_target);
        if (!actor || !target) {
            return false;
        }

        a_result = actor->WouldBeStealing(target);
        return true;
    }

    inline RE::SCRIPT_PARAMETER parameters[]{
        { .paramName = "Object Reference", .paramType = RE::SCRIPT_PARAM_TYPE::kObjectRef }
    };

    inline const RE::SCRIPT_FUNCTION function{
        .functionName = "WouldBeStealing",
        .numParams = static_cast<std::uint16_t>(std::size(parameters)),
        .params = parameters,
        .conditionFunction = Evaluate
    };

    inline constexpr RegistrationV1 registration{
        .id = kID,
        .function = &function
    };
}
