// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @brief Compatibility header defining spdlog log levels.
 **/

#ifndef COMPAT_SPDLOG_COMMON_HPP
#define COMPAT_SPDLOG_COMMON_HPP

namespace spdlog {
namespace level {

enum level_enum : int {
  trace = 0,
  debug = 1,
  info = 2,
  warn = 3,
  err = 4,
  critical = 5,
  off = 6,
  n_levels
};

inline const char * to_string_view( level_enum level ) noexcept {
  switch ( level ) {
    case trace: return "trace";
    case debug: return "debug";
    case info: return "info";
    case warn: return "warn";
    case err: return "error";
    case critical: return "critical";
    case off: return "off";
    default: return "unknown";
  }
}

} // namespace level
} // namespace spdlog

#endif // COMPAT_SPDLOG_COMMON_HPP
