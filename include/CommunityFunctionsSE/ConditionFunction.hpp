#pragma once

#include "CommunityFunctionsSE/Registration.hpp"

namespace CommunityFunctionsSE {
    using Comparison = RE::CONDITION_ITEM_DATA::OpCode;
    using ConditionArgument = std::variant<std::int32_t, float, RE::TESForm*>;

    class ConditionFunction {
    public:
        [[nodiscard]] bool IsTrue(RE::TESObjectREFR* a_subject, RE::TESObjectREFR* a_target) const {
            std::array<RE::NiPointer<RE::TESObjectREFR>, 2> references{};
            std::array<void*, 2> parameters{};

            for (std::size_t i = 0; i < slots.size(); ++i) {
                const auto& slot = slots[i];
                if (std::holds_alternative<Target>(slot)) {
                    if (!a_target && required[i]) {
                        return false;
                    }
                    parameters[i] = a_target;
                } else if (const auto form = std::get_if<RE::TESForm*>(std::addressof(slot))) {
                    parameters[i] = *form;
                } else if (const auto handle = std::get_if<RE::ObjectRefHandle>(std::addressof(slot))) {
                    references[i] = handle->get();
                    if (!references[i] && required[i]) {
                        return false;
                    }
                    parameters[i] = references[i].get();
                } else if (const auto integer = std::get_if<std::int32_t>(std::addressof(slot))) {
                    parameters[i] = Parameters::EncodeParameter(*integer);
                } else if (const auto number = std::get_if<float>(std::addressof(slot))) {
                    parameters[i] = Parameters::EncodeParameter(*number);
                }
            }

            double result = 0.0;
            if (!callback(a_subject, parameters[0], parameters[1], result) || !std::isfinite(result)) {
                return false;
            }

            switch (comparison) {
                case Comparison::kEqualTo:
                    return result == comparisonValue;
                case Comparison::kNotEqualTo:
                    return result != comparisonValue;
                case Comparison::kGreaterThan:
                    return result > comparisonValue;
                case Comparison::kGreaterThanOrEqualTo:
                    return result >= comparisonValue;
                case Comparison::kLessThan:
                    return result < comparisonValue;
                case Comparison::kLessThanOrEqualTo:
                    return result <= comparisonValue;
                default:
                    return false;
            }
        }

        [[nodiscard]] static std::optional<ConditionFunction> Create(
            const RegistrationV1& a_entry, const Comparison a_comparison, const double a_comparisonValue,
            const std::span<const ConditionArgument> a_arguments) {
            if (!detail::IsCallable(a_entry) || !std::isfinite(a_comparisonValue) || !IsValid(a_comparison)) {
                return std::nullopt;
            }
            const auto function = a_entry.function;

            ConditionFunction result{ function->conditionFunction, a_comparison, a_comparisonValue };
            std::size_t argumentIndex = 0;

            for (std::size_t i = 0; i < result.slots.size(); ++i) {
                const auto binding = a_entry.conditionParameters[i];
                if (i >= function->numParams) {
                    continue;
                }
                result.required[i] = !function->params[i].optional;
                if (binding == Parameters::ConditionParameter::kTarget) {
                    result.slots[i] = Target{};
                    continue;
                }
                if (argumentIndex == a_arguments.size()) {
                    if (result.required[i]) {
                        return std::nullopt;
                    }
                    continue;
                }
                if (!Bind(binding, a_arguments[argumentIndex++], result.required[i], result.slots[i])) {
                    return std::nullopt;
                }
            }

            if (argumentIndex != a_arguments.size()) {
                return std::nullopt;
            }
            return result;
        }

    private:
        struct Target {};
        using Slot = std::variant<std::monostate, Target, RE::TESForm*, RE::ObjectRefHandle, std::int32_t, float>;

        ConditionFunction(RE::SCRIPT_FUNCTION::Condition_t* a_callback, const Comparison a_comparison,
                          const double a_comparisonValue) :
            callback(a_callback), comparison(a_comparison), comparisonValue(a_comparisonValue) {}

        [[nodiscard]] static bool IsValid(const Comparison a_comparison) noexcept {
            return a_comparison >= Comparison::kEqualTo && a_comparison <= Comparison::kLessThanOrEqualTo;
        }

        [[nodiscard]] static bool Bind(const Parameters::ConditionParameter a_binding,
                                       const ConditionArgument& a_argument,
                                       const bool a_required, Slot& a_slot) {
            if (a_binding == Parameters::ConditionParameter::kForm ||
                a_binding == Parameters::ConditionParameter::kReference) {
                const auto form = std::get_if<RE::TESForm*>(std::addressof(a_argument));
                if (!form || (!*form && a_required)) {
                    return false;
                }
                if (!*form) {
                    return true;
                }
                if (const auto reference = (*form)->As<RE::TESObjectREFR>()) {
                    a_slot = RE::ObjectRefHandle(reference);
                    return true;
                }
                if (a_binding == Parameters::ConditionParameter::kReference) {
                    return false;
                }
                a_slot = *form;
                return true;
            }

            if (a_binding == Parameters::ConditionParameter::kInteger) {
                const auto number = std::get_if<std::int32_t>(std::addressof(a_argument));
                if (!number) {
                    return false;
                }
                a_slot = *number;
                return true;
            }
            if (a_binding == Parameters::ConditionParameter::kFloat) {
                const auto number = std::get_if<float>(std::addressof(a_argument));
                if (!number || !std::isfinite(*number)) {
                    return false;
                }
                a_slot = *number;
                return true;
            }
            return false;
        }

        RE::SCRIPT_FUNCTION::Condition_t* callback;
        Comparison comparison;
        double comparisonValue;
        std::array<Slot, 2> slots{};
        std::array<bool, 2> required{};
    };
}
