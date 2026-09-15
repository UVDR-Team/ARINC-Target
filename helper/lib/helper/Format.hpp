// SPDX-License-Identifier: MPL-2.0
#ifndef HELPER_FORMAT_HPP
#define HELPER_FORMAT_HPP
// Keep the formatting implementation outside namespace std. Some libc++
// headers expose native format declarations even without including <format>.
#ifdef ARINC615A_TARGET_BUILD
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#define ARINC_FORMAT_NAMESPACE fmt
#else
#include <format>
#define ARINC_FORMAT_NAMESPACE std
#endif
#endif
