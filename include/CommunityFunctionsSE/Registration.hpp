#pragma once

#include "CommunityFunctionsSE/Parameters.hpp"
#include "RE/Skyrim.h"

namespace CommunityFunctionsSE {
    inline constexpr std::uint32_t kFunctionBase = 1000;
    inline constexpr std::uint32_t kFunctionLimit = 10000;

    struct RegistrationV1 {
        std::uint32_t id;
        const RE::SCRIPT_FUNCTION* function;
        std::array<Parameters::ConditionParameter, 2> conditionParameters{};
    };

    namespace detail {
        [[nodiscard]] constexpr bool Matches(const Parameters::ConditionParameter a_binding,
                                             const RE::SCRIPT_PARAM_TYPE a_type) noexcept {
            switch (a_binding) {
                case Parameters::ConditionParameter::kTarget:
                case Parameters::ConditionParameter::kReference:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kObjectRef;
                case Parameters::ConditionParameter::kForm:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kForm;
                case Parameters::ConditionParameter::kInteger:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kInt;
                case Parameters::ConditionParameter::kFloat:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kFloat;
                default:
                    return false;
            }
        }

        [[nodiscard]] inline bool IsCallable(const RegistrationV1& a_entry) noexcept {
            const auto function = a_entry.function;
            if (!function || !function->conditionFunction || function->numParams > a_entry.conditionParameters.size() ||
                (function->numParams != 0 && !function->params)) {
                return false;
            }

            for (std::size_t i = 0; i < a_entry.conditionParameters.size(); ++i) {
                const auto binding = a_entry.conditionParameters[i];
                if (i >= function->numParams) {
                    if (binding != Parameters::ConditionParameter::kNone) {
                        return false;
                    }
                } else if (!Matches(binding, function->params[i].paramType.get())) {
                    return false;
                }
            }
            return true;
        }

        template <std::size_t N>
        [[nodiscard]] consteval bool IsValid(const std::array<RegistrationV1, N>& a_entries) noexcept {
            std::array<bool, kFunctionLimit - kFunctionBase> usedIDs{};

            for (const auto& entry : a_entries) {
                if (entry.id < kFunctionBase || entry.id >= kFunctionLimit || !entry.function) {
                    return false;
                }
                if (entry.conditionParameters[0] == Parameters::ConditionParameter::kNone &&
                    entry.conditionParameters[1] != Parameters::ConditionParameter::kNone) {
                    return false;
                }

                const auto index = entry.id - kFunctionBase;
                if (usedIDs[index]) {
                    return false;
                }
                usedIDs[index] = true;
            }

            return true;
        }

        template <std::size_t N>
        [[nodiscard]] consteval std::size_t GetLookupSize(const std::array<RegistrationV1, N>& a_entries) noexcept {
            std::size_t size = 0;

            for (const auto& entry : a_entries) {
                if (entry.id >= kFunctionBase && entry.id < kFunctionLimit) {
                    const auto requiredSize = static_cast<std::size_t>(entry.id - kFunctionBase) + 1;
                    if (requiredSize > size) {
                        size = requiredSize;
                    }
                }
            }

            return size;
        }

        template <std::size_t Size, std::size_t N>
        [[nodiscard]] consteval auto BuildRegistry(const std::array<RegistrationV1, N>& a_entries) noexcept {
            std::array<RegistrationV1, Size> result{};

            for (const auto& entry : a_entries) {
                if (entry.id >= kFunctionBase) {
                    const auto index = entry.id - kFunctionBase;
                    if (index < Size) {
                        result[index] = entry;
                    }
                }
            }

            return result;
        }
    }
}
