/* SPDX-License-Identifier: MPL-2.0
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 *
 * @brief C-compatible interface for ARINC 615A Target Hardware Application (THA)
 *        designed for integration into VxWorks Downloadable Kernel Modules (DKM).
 */

#ifndef ARINC_615A_THA_H
#define ARINC_615A_THA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Log levels for arinc615a_tha_set_log_level
 */
#define ARINC615A_LOG_TRACE    0
#define ARINC615A_LOG_DEBUG    1
#define ARINC615A_LOG_INFO     2
#define ARINC615A_LOG_WARN     3
#define ARINC615A_LOG_ERROR    4
#define ARINC615A_LOG_CRITICAL 5
#define ARINC615A_LOG_OFF      6

/**
 * Return codes
 */
#define ARINC615A_OK           0
#define ARINC615A_ERROR       -1
#define ARINC615A_ALREADY_RUN -2

/**
 * @brief Initialize the THA Target with default configuration.
 *
 * @return ARINC615A_OK on success, or negative error code.
 */
int arinc615a_tha_init_default( void );

/**
 * @brief Initialize the THA Target using a JSON configuration string.
 *
 * @param[in] json_string
 *   Null-terminated string containing JSON configuration.
 *
 * @return ARINC615A_OK on success, or negative error code.
 */
int arinc615a_tha_init_json( const char * json_string );

/**
 * @brief Initialize the THA Target from a JSON file path.
 *
 * @param[in] json_file_path
 *   Null-terminated string containing path to JSON config file.
 *
 * @return ARINC615A_OK on success, or negative error code.
 */
int arinc615a_tha_init_file( const char * json_file_path );

/**
 * @brief Start the ARINC 615A THA Target service.
 *
 * Initializes the FIND server and target protocol engine.
 *
 * @param[in] run_in_background
 *   If non-zero, spawns a background thread/task and returns immediately.
 *   If zero, blocks the calling task/thread until arinc615a_tha_stop() is called.
 *
 * @return ARINC615A_OK on success, or negative error code.
 */
int arinc615a_tha_start( int run_in_background );

/**
 * @brief Stop the ARINC 615A THA Target service gracefully.
 */
void arinc615a_tha_stop( void );

/**
 * @brief Check if the THA Target service is currently running.
 *
 * @return 1 if running, 0 if stopped.
 */
int arinc615a_tha_is_running( void );

/**
 * @brief Set the logging severity level.
 *
 * @param[in] level
 *   One of ARINC615A_LOG_* constants.
 */
void arinc615a_tha_set_log_level( int level );

#ifdef __cplusplus
}
#endif

#endif /* ARINC_615A_THA_H */
