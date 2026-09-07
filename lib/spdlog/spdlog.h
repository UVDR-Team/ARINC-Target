// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @brief Dependency-free logging compatibility layer for the ARINC 615A DKM build.
 *
 * The production protocol must not depend on a user-space logger.  This small,
 * header-only replacement deliberately compiles logging calls away.  It keeps
 * the existing diagnostic call sites source-compatible while adding no code,
 * threads, files, heap use, or shared-library dependency to a VxWorks DKM.
 *
 * A DKM integrator may replace this header with a project-owned adapter to the
 * approved VxWorks logging facility when target diagnostics are required.
 */
#ifndef ARINC615A_DKM_SPDLOG_COMPATIBILITY_HPP
#define ARINC615A_DKM_SPDLOG_COMPATIBILITY_HPP

namespace spdlog {

namespace level {
enum level_enum
{
  trace,
  debug,
  info,
  warn,
  err,
  critical,
  off
};
}

inline void set_level( const level::level_enum ) noexcept
{
}

}

/*
 * Logging is intentionally compiled out in the DKM profile.  The macros
 * accept all existing arguments, so none of the ARINC state-machine paths is
 * changed when the external spdlog package is removed.
 */
#define SPDLOG_TRACE( ... )    do { } while ( false )
#define SPDLOG_DEBUG( ... )    do { } while ( false )
#define SPDLOG_INFO( ... )     do { } while ( false )
#define SPDLOG_WARN( ... )     do { } while ( false )
#define SPDLOG_ERROR( ... )    do { } while ( false )
#define SPDLOG_CRITICAL( ... ) do { } while ( false )

#endif
