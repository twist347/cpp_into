#pragma once

#include <concepts>
#include <type_traits>
#include <variant>

#include "into/core/concepts.h"
#include "into/core/error.h"
#include "into/core/cast.h"
#include "into/core/traits.h"

namespace into {
    namespace detail {
        template<typename>
        struct is_variant : std::false_type {
        };

        template<typename... Ts>
        struct is_variant<std::variant<Ts...> > : std::true_type {
        };

        template<typename T>
        constexpr bool is_variant_v = is_variant<T>::value;
    }

    // T -> variant<Ts...>: try alternatives in pack order, return the first that
    // parses successfully. Excluded when T itself is a variant (handled below).
    template<typename From, typename... Ts>
        requires (!detail::is_variant_v<From>)
                 && (Castable<Ts, From> || ...)
    struct Caster<std::variant<Ts...>, From> {
        static constexpr auto cast(const From &v) -> Result<std::variant<Ts...> > {
            using To = std::variant<Ts...>;
            Result<To> out = std::unexpected{Error::unsupported};
            // Fold over alternatives; stop at the first success.
            (void) (([&] {
                if (out) {
                    return true;
                }
                if constexpr (Castable<Ts, From>) {
                    if (auto r = into::cast<Ts>(v)) {
                        out = To{std::in_place_type<Ts>, *std::move(r)};
                        return true;
                    }
                }
                return false;
            }()) || ...);
            return out;
        }
    };

    // variant<Us...> -> T: visit and recursively parse the active alternative.
    template<typename To, typename... Us>
        requires (!detail::is_variant_v<To>)
                 && (Castable<To, Us> && ...)
    struct Caster<To, std::variant<Us...> > {
        static constexpr auto cast(const std::variant<Us...> &v) -> Result<To> {
            return std::visit(
                [](const auto &alt) -> Result<To> {
                    return into::cast<To>(alt);
                },
                v);
        }
    };

    // variant<Us...> -> variant<Ts...> (different): visit + recursive parse into the target variant.
    template<typename... Ts, typename... Us>
        requires (!std::same_as<std::variant<Ts...>, std::variant<Us...> >)
                 && (Castable<std::variant<Ts...>, Us> && ...)
    struct Caster<std::variant<Ts...>, std::variant<Us...> > {
        static constexpr auto cast(const std::variant<Us...> &v) -> Result<std::variant<Ts...> > {
            using To = std::variant<Ts...>;
            return std::visit(
                [](const auto &alt) -> Result<To> {
                    return into::cast<To>(alt);
                },
                v);
        }
    };
}
