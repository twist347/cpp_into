#pragma once

#include <chrono>
#include <concepts>
#include <type_traits>

#include "into/core/error.h"
#include "into/core/cast.h"
#include "into/core/traits.h"

namespace into {
    namespace detail {
        template<typename>
        struct is_duration : std::false_type {
        };

        template<typename Rep, typename Period>
        struct is_duration<std::chrono::duration<Rep, Period> > : std::true_type {
        };

        template<typename T>
        constexpr bool is_duration_v = is_duration<T>::value;
    }

    // duration -> duration: handles the "lossy direction" (coarser to finer in
    // magnitude, or incompatible ratios). The lossless direction is already
    // covered by the implicit strategy via chrono's converting constructor.
    // Rust TryFrom-style: error if the round-trip is not exact.
    template<typename ToRep, typename ToPeriod, typename FromRep, typename FromPeriod>
        requires (!std::convertible_to<
            const std::chrono::duration<FromRep, FromPeriod> &,
            std::chrono::duration<ToRep, ToPeriod> >)
    struct Caster<std::chrono::duration<ToRep, ToPeriod>,
                std::chrono::duration<FromRep, FromPeriod> > {
        using To = std::chrono::duration<ToRep, ToPeriod>;
        using From = std::chrono::duration<FromRep, FromPeriod>;

        static constexpr auto cast(const From &v) noexcept -> Result<To> {
            const auto out = std::chrono::duration_cast<To>(v);
            if constexpr (std::chrono::treat_as_floating_point_v<ToRep> ||
                          std::chrono::treat_as_floating_point_v<FromRep>) {
                return out;
            } else {
                if (std::chrono::duration_cast<From>(out) != v) {
                    return std::unexpected{Error::unrepresentable};
                }
                return out;
            }
        }
    };

    // time_point -> time_point (same clock): delegate to the duration parser
    // on the underlying time_since_epoch() value.
    template<typename Clock, typename ToDur, typename FromDur>
        requires (!std::same_as<ToDur, FromDur>)
                 && (!std::convertible_to<
                     const std::chrono::time_point<Clock, FromDur> &,
                     std::chrono::time_point<Clock, ToDur> >)
    struct Caster<std::chrono::time_point<Clock, ToDur>,
                std::chrono::time_point<Clock, FromDur> > {
        using To = std::chrono::time_point<Clock, ToDur>;
        using From = std::chrono::time_point<Clock, FromDur>;

        static constexpr auto cast(const From &v) -> Result<To> {
            const auto d = into::cast<ToDur>(v.time_since_epoch());
            if (!d) {
                return std::unexpected{d.error()};
            }
            return To{*d};
        }
    };
}
