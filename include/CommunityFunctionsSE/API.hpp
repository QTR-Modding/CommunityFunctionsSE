#pragma once

#include "CommunityFunctionsSE/ConditionFunction.hpp"
#include "CommunityFunctionsSE/Functions.hpp"

namespace CommunityFunctionsSE {
    [[nodiscard]] constexpr const RE::SCRIPT_FUNCTION* GetFunction(const std::uint32_t a_id) noexcept {
        const auto entry = detail::GetEntry(a_id);
        return entry ? entry->function : nullptr;
    }

    [[nodiscard]] inline std::optional<FunctionCall> GetFunctionCall(
        const std::uint32_t a_id, const std::span<const FunctionArgument> a_arguments) {
        const auto entry = detail::GetEntry(a_id);
        return entry ? FunctionCall::Create(*entry, a_arguments) : std::nullopt;
    }

    template <class... Args>
    [[nodiscard]] std::optional<FunctionCall> GetFunctionCall(const std::uint32_t a_id, Args&&... a_arguments) {
        static_assert(sizeof...(Args) <= 2, "A condition function accepts at most two arguments");
        const std::array<FunctionArgument, sizeof...(Args)> arguments{
            FunctionArgument(std::forward<Args>(a_arguments))...
        };
        return GetFunctionCall(a_id, std::span<const FunctionArgument>{ arguments });
    }

    [[nodiscard]] inline std::optional<ConditionFunction> GetConditionFunction(
        const std::uint32_t a_id, const Comparison a_comparison, const double a_comparisonValue,
        const std::span<const ConditionArgument> a_arguments) {
        const auto entry = detail::GetEntry(a_id);
        return entry ? ConditionFunction::Create(*entry, a_comparison, a_comparisonValue, a_arguments) : std::nullopt;
    }

    template <class... Args>
    [[nodiscard]] std::optional<ConditionFunction> GetConditionFunction(
        const std::uint32_t a_id, const Comparison a_comparison, const double a_comparisonValue,
        Args&&... a_arguments) {
        static_assert(sizeof...(Args) <= 2, "A condition function accepts at most two arguments");
        const std::array<ConditionArgument, sizeof...(Args)> arguments{
            ConditionArgument(std::forward<Args>(a_arguments))...
        };
        return GetConditionFunction(a_id, a_comparison, a_comparisonValue,
                                    std::span<const ConditionArgument>{ arguments });
    }
}
