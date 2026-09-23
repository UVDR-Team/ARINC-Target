// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC_BOOST_VXWORKS_HPP
#define ARINC_BOOST_VXWORKS_HPP
// Modern VxWorks 7 platform profile: use the SDK APIs, without defining
// replacement operating-system functions inside Boost headers.
#include <vxWorks.h>
#include <unistd.h>
#include <pthread.h>
#define BOOST_PLATFORM "VxWorks 7"
#define BOOST_HAS_UNISTD_H
#define BOOST_HAS_STDINT_H
#define BOOST_HAS_DIRENT_H
#define BOOST_HAS_THREADS
#define BOOST_HAS_PTHREADS
#define BOOST_HAS_SCHED_YIELD
#define BOOST_HAS_NANOSLEEP
#define BOOST_HAS_GETTIMEOFDAY
#define BOOST_HAS_CLOCK_GETTIME
#endif
