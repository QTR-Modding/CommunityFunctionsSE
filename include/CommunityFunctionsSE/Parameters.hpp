#pragma once

#include <bit>
#include <cstdint>

namespace CommunityFunctionsSE::Parameters {
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
}
