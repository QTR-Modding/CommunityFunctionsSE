#pragma once

#include "RE/Skyrim.h"

namespace CommunityFunctionsSE {
    inline constexpr std::uint32_t kFunctionBase = 1000;
    inline constexpr std::uint32_t kFunctionLimit = 10000;

    enum class ConditionParameter : std::uint8_t {
        kNone,
        kTarget,
        kForm,
        kReference,
        kInteger,
        kFloat
    };

    [[nodiscard]] inline void* EncodeParameter(const std::int32_t a_value) noexcept {
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::intptr_t>(a_value)));
    }

    [[nodiscard]] inline void* EncodeParameter(const float a_value) noexcept {
        return reinterpret_cast<void*>(
            static_cast<std::uintptr_t>(std::bit_cast<std::uint32_t>(a_value)));
    }

    [[nodiscard]] inline std::int32_t DecodeIntegerParameter(void* a_parameter) noexcept {
        return static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(a_parameter));
    }

    [[nodiscard]] inline float DecodeFloatParameter(void* a_parameter) noexcept {
        return std::bit_cast<float>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(a_parameter)));
    }

    struct RegistrationV1 {
        std::uint32_t id;
        const RE::SCRIPT_FUNCTION* function;
        std::array<ConditionParameter, 2> conditionParameters{};
    };

    namespace detail {
        template <std::size_t N>
        [[nodiscard]] consteval bool IsValid(const std::array<RegistrationV1, N>& a_entries) noexcept {
            std::array<bool, kFunctionLimit - kFunctionBase> usedIDs{};

            for (const auto& entry : a_entries) {
                if (entry.id < kFunctionBase || entry.id >= kFunctionLimit || !entry.function) {
                    return false;
                }
                if (entry.conditionParameters[0] == ConditionParameter::kNone &&
                    entry.conditionParameters[1] != ConditionParameter::kNone) {
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
