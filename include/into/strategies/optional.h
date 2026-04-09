#pragma once

#include <concepts>
#include <optional>
#include <type_traits>

#include "into/core/concepts.h"
#include "into/core/error.h"
#include "into/core/cast.h"
#include "into/core/traits.h"

namespace into {
    namespace detail {
        template<typename>
        struct is_optional : std::false_type {
        };

        template<typename T>
        struct is_optional<std::optional<T> > : std::true_type {
        };

        template<typename T>
        constexpr bool is_optional_v = is_optional<T>::value;
    }

    // T -> optional<T>: always wraps. Excluded for nested optionals so the
    // recursive optional<U> -> optional<T> specialization handles them.
    template<typename T>
        requires (!detail::is_optional_v<T>)
    struct Caster<std::optional<T>, T> {
        static constexpr auto cast(const T &v) -> Result<std::optional<T> > {
            return std::optional<T>{v};
        }
    };

    // optional<T> -> T: error on nullopt.
    template<typename T>
        requires (!detail::is_optional_v<T>)
    struct Caster<T, std::optional<T> > {
        static constexpr auto cast(const std::optional<T> &v) -> Result<T> {
            if (!v) {
                return std::unexpected{Error::unrepresentable};
            }
            return *v;
        }
    };

    // optional<U> -> optional<T>: nullopt passes through, otherwise recursive parse.
    template<typename To, typename From>
        requires (!std::same_as<To, From>) && Castable<To, From>
    struct Caster<std::optional<To>, std::optional<From> > {
        static constexpr auto cast(const std::optional<From> &v) -> Result<std::optional<To> > {
            if (!v) {
                return std::optional<To>{};
            }
            const auto inner = into::cast<To>(*v);
            if (!inner) {
                return std::unexpected{inner.error()};
            }
            return std::optional<To>{*inner};
        }
    };
}
