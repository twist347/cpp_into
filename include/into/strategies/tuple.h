#pragma once

#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

#include "into/core/concepts.h"
#include "into/core/error.h"
#include "into/core/cast.h"
#include "into/core/traits.h"

namespace into {
    namespace detail {
        template<typename>
        struct is_tuple_like : std::false_type {
        };

        template<typename... Ts>
        struct is_tuple_like<std::tuple<Ts...> > : std::true_type {
        };

        template<typename A, typename B>
        struct is_tuple_like<std::pair<A, B> > : std::true_type {
        };

        template<typename T>
        constexpr bool is_tuple_like_v = is_tuple_like<T>::value;

        // Element-wise parse from any tuple-like (std::tuple or std::pair) into another.
        // Equal arity is required by the caller's concept.
        template<typename To, typename From>
        constexpr auto cast_tuple_like(const From &src) -> Result<To> {
            constexpr std::size_t N = std::tuple_size_v<From>;
            static_assert(std::tuple_size_v<To> == N);

            return [&]<std::size_t... I>(std::index_sequence<I...>) -> Result<To> {
                // Parse every element first; tuples are tiny so the lack of
                // short-circuit is fine and keeps the code straightforward.
                std::tuple<Result<std::tuple_element_t<I, To> >...> parsed{
                    into::cast<std::tuple_element_t<I, To> >(std::get<I>(src))...
                };

                Error err{};
                bool ok = true;
                (((void) (std::get<I>(parsed)
                              ? true
                              : (ok
                                     ? (err = std::get<I>(parsed).error(), ok = false)
                                     : false))),
                    ...);
                if (!ok) {
                    return std::unexpected{err};
                }
                return To{*std::move(std::get<I>(parsed))...};
            }(std::make_index_sequence<N>{});
        }
    }

    // tuple<Ts...> <- tuple<Us...>
    template<typename... Ts, typename... Us>
        requires (!std::same_as<std::tuple<Ts...>, std::tuple<Us...> >)
                 && (sizeof...(Ts) == sizeof...(Us))
                 && (Castable<Ts, Us> && ...)
    struct Caster<std::tuple<Ts...>, std::tuple<Us...> > {
        static constexpr auto cast(const std::tuple<Us...> &v)
            -> Result<std::tuple<Ts...> > {
            return detail::cast_tuple_like<std::tuple<Ts...> >(v);
        }
    };

    // pair<T1,T2> <- pair<U1,U2>
    template<typename T1, typename T2, typename U1, typename U2>
        requires (!std::same_as<std::pair<T1, T2>, std::pair<U1, U2> >)
                 && Castable<T1, U1> && Castable<T2, U2>
    struct Caster<std::pair<T1, T2>, std::pair<U1, U2> > {
        static constexpr auto cast(const std::pair<U1, U2> &v)
            -> Result<std::pair<T1, T2> > {
            return detail::cast_tuple_like<std::pair<T1, T2> >(v);
        }
    };

    // tuple<T1,T2> <- pair<U1,U2>
    template<typename T1, typename T2, typename U1, typename U2>
        requires Castable<T1, U1> && Castable<T2, U2>
    struct Caster<std::tuple<T1, T2>, std::pair<U1, U2> > {
        static constexpr auto cast(const std::pair<U1, U2> &v)
            -> Result<std::tuple<T1, T2> > {
            return detail::cast_tuple_like<std::tuple<T1, T2> >(v);
        }
    };

    // pair<T1,T2> <- tuple<U1,U2>
    template<typename T1, typename T2, typename U1, typename U2>
        requires Castable<T1, U1> && Castable<T2, U2>
    struct Caster<std::pair<T1, T2>, std::tuple<U1, U2> > {
        static constexpr auto cast(const std::tuple<U1, U2> &v)
            -> Result<std::pair<T1, T2> > {
            return detail::cast_tuple_like<std::pair<T1, T2> >(v);
        }
    };
}
