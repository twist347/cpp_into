# into

Header-only C++23 library for generic conversions — "casts from anything to anything" — inspired by Rust's `TryFrom`. Built on `std::expected`.

```cpp
#include "into/into.h"

auto r = into::cast<int>(std::string_view{"42"});
if (r) {
    use(*r);           // 42
} else {
    log(into::to_string(r.error()));
}
```

## API

```cpp
template<typename To, typename From>
auto into::cast(From&&) -> into::Result<To>;
```

`Result<T>` is `std::expected<T, Error>`. `Error` is a plain `enum class`:

| Code | Meaning |
| --- | --- |
| `invalid_format` | input string cannot be parsed into the target type |
| `out_of_range` | value does not fit into the target type's range |
| `partial_parse` | value parsed, but trailing characters remained |
| `unrepresentable` | value cannot be represented (NaN → int, `nullopt` → T, etc.) |
| `unsupported` | conversion is not supported by any strategy |

Conversions are **lossless** (Rust-`TryFrom`-style): if the value cannot round-trip, you get an error rather than silent truncation.

## Built-in strategies

| Strategy | What it covers |
| --- | --- |
| **identity** | `T → T` |
| **numeric** | arithmetic ↔ arithmetic, range-checked. `int → int` clamps, `float → int` errors on NaN/inf/fractional, `int → float` allowed (matches Rust `From`) |
| **string** | `string_view`/`string` ↔ arithmetic via `from_chars`/`to_chars`, plus `string_view → string` |
| **enum** | `enum ↔ arithmetic`, `enum ↔ enum` (through the underlying type, range-checked) |
| **optional** | `T → optional<T>`, `optional<T> → T` (errors on `nullopt`), `optional<U> → optional<T>` (recursive) |
| **variant** | `T → variant<...>` (tries alternatives in pack order, first success wins), `variant<...> → T`, `variant<U...> → variant<T...>` |
| **tuple** | `tuple<U...> → tuple<T...>`, `pair ↔ pair`, `pair ↔ tuple` (equal arity, element-wise, first error wins) |
| **chrono** | `duration ↔ duration` and `time_point ↔ time_point` for the lossy direction (round-trip checked); the lossless direction is handled by the implicit strategy via chrono's converting constructor |
| **implicit** | last-resort fallback: any implicit conversion not handled by a more specific strategy |

## Extending

Specialize `into::Caster<To, From>` for your type — works for both your own types and third-party ones:

```cpp
template<>
struct into::Caster<MyType, std::string_view> {
    static auto cast(std::string_view s) -> into::Result<MyType> {
        // ...
    }
};
```

You may call `into::cast<Inner>(...)` recursively from inside your specialization to delegate sub-conversions.

## Build

Requires a C++23 compiler with `<expected>` (gcc 14+ or equivalent). Tested on gcc 15.

```sh
cmake -S . -B build
cmake --build build -j
ctest --test-dir build
```

## Status

Early. See `TODO.md` for the open work and for design decisions that were intentionally **not** taken (pipe adapter, ADL hook, range/container strategy).
