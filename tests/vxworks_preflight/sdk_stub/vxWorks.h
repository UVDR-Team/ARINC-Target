/* SPDX-License-Identifier: MPL-2.0
 *
 * STUB, NOT A WIND RIVER HEADER.
 *
 * Minimal approximation of the VxWorks 7 kernel header surface that
 * lib/arinc_support/BuildConfig.hpp and BoostVxWorks.hpp consume. It exists
 * only so a host compiler can syntax-check the __VXWORKS__ branch of the
 * production graph, which is otherwise never compiled off-target.
 *
 * It is NOT used by any production translation unit, is NOT shipped in the
 * office handoff, and is NOT a substitute for the licensed VxWorks 24.03 SDK.
 * A pass here does not predict a successful Workbench build; see README.md.
 */
#ifndef VXWORKS_STUB_H
#define VXWORKS_STUB_H
/* Compile-probe stub approximating the VxWorks 7 kernel header surface.
   Not a Wind River header. Exists only to exercise the __VXWORKS__ branch. */
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
typedef int STATUS;
typedef int BOOL;
#ifndef OK
#define OK 0
#endif
#ifndef ERROR
#define ERROR (-1)
#endif
#ifndef WAIT_FOREVER
#define WAIT_FOREVER (-1)
#endif
#ifndef NO_WAIT
#define NO_WAIT 0
#endif
#endif
