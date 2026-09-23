// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_SUPPORT_LOGGING_HPP
#define ARINC_SUPPORT_LOGGING_HPP

#include <atomic>
#include <arinc_support/Format.hpp>
#include <cstdio>
#include <string_view>
#include <utility>

namespace ArincSupport::Logging {
namespace level {
enum level_enum { trace, debug, info, warn, err, critical, off };
}

inline std::atomic< level::level_enum > minimumLevel{ level::warn };

inline void set_level( level::level_enum value ) noexcept
{
  minimumLevel.store( value, std::memory_order_relaxed );
}

// Synchronous diagnostics, using the existing C++ runtime. No spdlog/fmt
// package, background task, file sink or dynamically loaded library.
// Messages retain their original text; logger-specific timestamps/colour
// are intentionally not part of this adapter's output.
inline void write(level::level_enum value, std::string_view message) noexcept
{
  if (value < minimumLevel.load(std::memory_order_relaxed)) return;
  std::fwrite(message.data(), 1, message.size(), stdout);
  std::fputc('\n', stdout);
}

template< typename... Args >
void write( level::level_enum value, std::string_view pattern, Args&&... args ) noexcept
{
  if ( value < minimumLevel.load( std::memory_order_relaxed ) )
  {
    return;
  }
  try
  {
    auto message = ArincSupport::format( pattern, std::forward< Args >( args )... );
    message += '\n';
    std::fwrite( message.data(), 1, message.size(), stdout );
  }
  catch ( ... )
  {
    // Logging must never cause a protocol operation to fail.
    std::fputs( "ARINC diagnostic formatting failed\n", stderr );
  }
}
}

// Match spdlog's original default compile-time threshold (INFO).
#define ARINC_LOG_TRACE( ... ) do { } while ( false )
#define ARINC_LOG_DEBUG( ... ) do { } while ( false )
#define ARINC_LOG_INFO( ... ) ::ArincSupport::Logging::write( ::ArincSupport::Logging::level::info, __VA_ARGS__ )
#define ARINC_LOG_WARN( ... ) ::ArincSupport::Logging::write( ::ArincSupport::Logging::level::warn, __VA_ARGS__ )
#define ARINC_LOG_ERROR( ... ) ::ArincSupport::Logging::write( ::ArincSupport::Logging::level::err, __VA_ARGS__ )
#define ARINC_LOG_CRITICAL( ... ) ::ArincSupport::Logging::write( ::ArincSupport::Logging::level::critical, __VA_ARGS__ )
#endif
