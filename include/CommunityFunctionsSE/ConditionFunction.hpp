#pragma once

#include "CommunityFunctionsSE/Registration.hpp"

namespace CommunityFunctionsSE {
    using Comparison = RE::CONDITION_ITEM_DATA::OpCode;
    using FunctionArgument = std::variant<std::int32_t, float, RE::TESForm*, double>;
    using ConditionArgument = std::variant<std::int32_t, float, RE::TESForm*>;

    class FunctionCall {
    public:
        [[nodiscard]] bool Evaluate(RE::TESObjectREFR* a_subject, RE::TESObjectREFR* a_target,
                                    double& a_result) const {
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
                } else if (const auto encoded = std::get_if<std::uintptr_t>(std::addressof(slot))) {
                    parameters[i] = reinterpret_cast<void*>(*encoded);
                }
            }

            a_result = 0.0;
            return callback(a_subject, parameters[0], parameters[1], a_result) && std::isfinite(a_result);
        }

        [[nodiscard]] static std::optional<FunctionCall> Create(
            const RegistrationV1& a_entry, const std::span<const FunctionArgument> a_arguments) {
            const auto function = a_entry.function;
            if (!function || !function->conditionFunction || function->numParams > 2 ||
                (function->numParams != 0 && !function->params)) {
                return std::nullopt;
            }

            FunctionCall result{ function->conditionFunction };
            std::size_t argumentIndex = 0;

            for (std::size_t i = 0; i < result.slots.size(); ++i) {
                const auto binding = a_entry.conditionParameters[i];
                if (i >= function->numParams) {
                    if (binding != ConditionParameter::kNone) {
                        return std::nullopt;
                    }
                    continue;
                }
                if (!Matches(binding, function->params[i].paramType.get())) {
                    return std::nullopt;
                }

                result.required[i] = !function->params[i].optional;
                if (binding == ConditionParameter::kTarget) {
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
        using Slot = std::variant<std::monostate, Target, RE::TESForm*, RE::ObjectRefHandle, std::uintptr_t>;

        explicit FunctionCall(RE::SCRIPT_FUNCTION::Condition_t* a_callback) : callback(a_callback) {}

        [[nodiscard]] static bool Matches(const ConditionParameter a_binding,
                                          const RE::SCRIPT_PARAM_TYPE a_type) noexcept {
            switch (a_binding) {
                case ConditionParameter::kTarget:
                case ConditionParameter::kReference:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kObjectRef;
                case ConditionParameter::kForm:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kForm;
                case ConditionParameter::kInteger:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kInt;
                case ConditionParameter::kFloat:
                    return a_type == RE::SCRIPT_PARAM_TYPE::kFloat;
                default:
                    return false;
            }
        }

        [[nodiscard]] static bool Bind(const ConditionParameter a_binding, const FunctionArgument& a_argument,
                                       const bool a_required, Slot& a_slot) {
            if (a_binding == ConditionParameter::kForm || a_binding == ConditionParameter::kReference) {
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
                if (a_binding == ConditionParameter::kReference) {
                    return false;
                }
                a_slot = *form;
                return true;
            }

            if (a_binding == ConditionParameter::kInteger) {
                if (const auto number = std::get_if<std::int32_t>(std::addressof(a_argument))) {
                    a_slot = static_cast<std::uintptr_t>(static_cast<std::intptr_t>(*number));
                    return true;
                }
                const auto number = std::get_if<double>(std::addressof(a_argument));
                if (!number || !std::isfinite(*number) || std::trunc(*number) != *number ||
                    *number < static_cast<double>(std::numeric_limits<std::int32_t>::min()) ||
                    *number > static_cast<double>(std::numeric_limits<std::int32_t>::max())) {
                    return false;
                }
                a_slot = static_cast<std::uintptr_t>(
                    static_cast<std::intptr_t>(static_cast<std::int32_t>(*number)));
                return true;
            }
            if (a_binding == ConditionParameter::kFloat) {
                if (const auto number = std::get_if<float>(std::addressof(a_argument))) {
                    if (!std::isfinite(*number)) {
                        return false;
                    }
                    a_slot = static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(*number));
                    return true;
                }
                const auto number = std::get_if<double>(std::addressof(a_argument));
                if (!number || !std::isfinite(*number) ||
                    *number < static_cast<double>(std::numeric_limits<float>::lowest()) ||
                    *number > static_cast<double>(std::numeric_limits<float>::max())) {
                    return false;
                }
                a_slot = static_cast<std::uintptr_t>(
                    std::bit_cast<std::uint32_t>(static_cast<float>(*number)));
                return true;
            }
            return false;
        }

        RE::SCRIPT_FUNCTION::Condition_t* callback;
        std::array<Slot, 2> slots{};
        std::array<bool, 2> required{};
    };

    class ConditionFunction {
    public:
        [[nodiscard]] bool IsTrue(RE::TESObjectREFR* a_subject, RE::TESObjectREFR* a_target) const {
            double result;
            if (!call.Evaluate(a_subject, a_target, result)) {
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
            if (!std::isfinite(a_comparisonValue) || !IsValid(a_comparison)) {
                return std::nullopt;
            }

            std::array<FunctionArgument, 2> arguments;
            if (a_arguments.size() > arguments.size()) {
                return std::nullopt;
            }
            for (std::size_t i = 0; i < a_arguments.size(); ++i) {
                arguments[i] = std::visit(
                    []<class T>(const T& a_argument) -> FunctionArgument { return a_argument; }, a_arguments[i]);
            }

            auto call = FunctionCall::Create(
                a_entry, std::span<const FunctionArgument>{ arguments.data(), a_arguments.size() });
            if (!call) {
                return std::nullopt;
            }
            return ConditionFunction{ std::move(*call), a_comparison, a_comparisonValue };
        }

    private:
        ConditionFunction(FunctionCall a_call, const Comparison a_comparison, const double a_comparisonValue) :
            call(std::move(a_call)), comparison(a_comparison), comparisonValue(a_comparisonValue) {}

        [[nodiscard]] static bool IsValid(const Comparison a_comparison) noexcept {
            return a_comparison >= Comparison::kEqualTo && a_comparison <= Comparison::kLessThanOrEqualTo;
        }

        FunctionCall call;
        Comparison comparison;
        double comparisonValue;
    };
}
