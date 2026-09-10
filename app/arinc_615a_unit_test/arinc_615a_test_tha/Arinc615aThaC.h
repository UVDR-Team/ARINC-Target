// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC615A_THA_C_H
#define ARINC615A_THA_C_H
#ifdef __cplusplus
extern "C" {
#endif
/* Optional bridge for a C DKM. Existing integration wrappers may call the
 * equivalent C++ methods instead. No C++ types cross this boundary. */
/* Blocks in the caller's worker task. Returns 0 on stop, nonzero on error. */
int arinc615a_tha_run_file(const char *json_filename);
/* Returns 1 if a stop request was queued, 0 if idle or unable to queue it. */
int arinc615a_tha_request_stop(void);
#ifdef __cplusplus
}
#endif
#endif
