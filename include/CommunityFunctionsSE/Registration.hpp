#pragma once

#include "RE/Skyrim.h"

namespace CommunityFunctionsSE {
    inline constexpr std::uint32_t kFunctionBase = 1000;
    inline constexpr std::uint32_t kFunctionLimit = 10000;

    struct RegistrationV1 {
        std::uint32_t id;
        const RE::SCRIPT_FUNCTION* function;
    };

    namespace detail {
        struct Entry {
            std::uint32_t id;
            const RE::SCRIPT_FUNCTION* function;
        };

        [[nodiscard]] consteval Entry Normalize(const RegistrationV1& a_registration) noexcept {
            return { a_registration.id, a_registration.function };
        }

        template <std::size_t N>
        [[nodiscard]] consteval bool IsValid(const std::array<Entry, N>& a_entries) noexcept {
            std::array<bool, kFunctionLimit - kFunctionBase> usedIDs{};

            for (const auto& entry : a_entries) {
                if (entry.id < kFunctionBase || entry.id >= kFunctionLimit || !entry.function) {
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
        [[nodiscard]] consteval std::size_t GetLookupSize(const std::array<Entry, N>& a_entries) noexcept {
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
        [[nodiscard]] consteval auto BuildRegistry(const std::array<Entry, N>& a_entries) noexcept {
            std::array<const RE::SCRIPT_FUNCTION*, Size> functions{};

            for (const auto& entry : a_entries) {
                if (entry.id >= kFunctionBase) {
                    const auto index = entry.id - kFunctionBase;
                    if (index < Size) {
                        functions[index] = entry.function;
                    }
                }
            }

            return functions;
        }
    }
}
