// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @brief Lightweight header-only spdlog compatibility shim for VxWorks / embedded targets.
 **/

#ifndef COMPAT_SPDLOG_HPP
#define COMPAT_SPDLOG_HPP

#include <spdlog/common.h>

#ifndef FMT_HEADER_ONLY
#  define FMT_HEADER_ONLY 1
#endif
#include <fmt/core.h>
#include <fmt/format.h>

#include <cstdio>
#include <string>
#include <string_view>
#include <atomic>

namespace spdlog {

inline std::atomic<level::level_enum>& default_log_level() noexcept {
  static std::atomic<level::level_enum> current_level{level::info};
  return current_level;
}

inline void set_level( level::level_enum log_level ) noexcept {
  default_log_level() = log_level;
}

inline level::level_enum get_level() noexcept {
  return default_log_level().load(std::memory_order_relaxed);
}

inline bool should_log( level::level_enum lvl ) noexcept {
  return lvl != level::off && lvl >= get_level();
}

template< typename... Args >
inline void log( level::level_enum lvl, fmt::format_string< Args... > fmt, Args &&... args ) {
  if ( should_log( lvl ) ) {
    try {
      std::string formatted = fmt::format( fmt, std::forward< Args >( args )... );
      std::printf( "[%s] %s\n", level::to_string_view( lvl ), formatted.c_str() );
    } catch ( ... ) {
      // Avoid throwing from logging
    }
  }
}

template< typename... Args >
inline void trace( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::trace, fmt, std::forward< Args >( args )... );
}

template< typename... Args >
inline void debug( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::debug, fmt, std::forward< Args >( args )... );
}

template< typename... Args >
inline void info( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::info, fmt, std::forward< Args >( args )... );
}

template< typename... Args >
inline void warn( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::warn, fmt, std::forward< Args >( args )... );
}

template< typename... Args >
inline void error( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::err, fmt, std::forward< Args >( args )... );
}

template< typename... Args >
inline void critical( fmt::format_string< Args... > fmt, Args &&... args ) {
  log( level::critical, fmt, std::forward< Args >( args )... );
}

} // namespace spdlog

// Macros matching standard spdlog macro names
#define SPDLOG_TRACE( ... )    ::spdlog::trace( __VA_ARGS__ )
#define SPDLOG_DEBUG( ... )    ::spdlog::debug( __VA_ARGS__ )
#define SPDLOG_INFO( ... )     ::spdlog::info( __VA_ARGS__ )
#define SPDLOG_WARN( ... )     ::spdlog::warn( __VA_ARGS__ )
#define SPDLOG_ERROR( ... )    ::spdlog::error( __VA_ARGS__ )
#define SPDLOG_CRITICAL( ... ) ::spdlog::critical( __VA_ARGS__ )

#endif // COMPAT_SPDLOG_HPP
