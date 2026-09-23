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
#ifndef SELECTLIB_STUB_H
#define SELECTLIB_STUB_H
#include <sys/select.h>
#include <sys/time.h>
#endif
