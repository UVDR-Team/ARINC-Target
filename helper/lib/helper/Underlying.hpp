// SPDX-License-Identifier: MPL-2.0
#ifndef HELPER_UNDERLYING_HPP
#define HELPER_UNDERLYING_HPP
#include <type_traits>
namespace Helper {
template<class Enum>
constexpr std::underlying_type_t<Enum> toUnderlying(Enum value) noexcept
{
  return static_cast<std::underlying_type_t<Enum>>(value);
}
}
#endif
