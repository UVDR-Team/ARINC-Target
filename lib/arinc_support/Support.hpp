// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_SUPPORT_HPP
#define ARINC_SUPPORT_SUPPORT_HPP

#include <map>
#include <string>

// Only the types required by the protocol libraries are retained. There is
// no helper library, platform-directory lookup, scheduler or logger here.
namespace ArincSupport {
struct VersionInformation;
using VersionsInformation = std::map< std::string, VersionInformation, std::less<> >;
}

#define ARINC_SUPPORT_EXPORT
#endif
