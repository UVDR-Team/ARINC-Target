// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_SUPPORT_HPP
#define ARINC_SUPPORT_SUPPORT_HPP

#include <map>
#include <string>
#include <type_traits>

// Only the types required by the protocol libraries are retained. There is
// no helper library, platform-directory lookup, scheduler or logger here.
namespace ArincSupport {
template<class Enum>
constexpr std::underlying_type_t<Enum> toUnderlying(Enum value) noexcept
{
  return static_cast<std::underlying_type_t<Enum>>(value);
}
struct VersionInformation;
using VersionsInformation = std::map< std::string, VersionInformation, std::less<> >;
}

#define ARINC_SUPPORT_EXPORT
#endif
