// SPDX-License-Identifier: MPL-2.0
#ifndef ARINC615A_VXWORKS_PLATFORM_HPP
#define ARINC615A_VXWORKS_PLATFORM_HPP
#include <vxWorks.h>
#ifndef _WRS_KERNEL
#error "ARINC615A requires the VxWorks DKM toolchain, not an RTP toolchain"
#endif
// Load network headers before PropertyTree and fmt. The SDK's legacy mbuf
// member aliases otherwise rewrite unrelated C++ identifiers in those headers.
#include <boost/asio.hpp>
#ifdef m_data
#undef m_data
#endif
#ifdef m_type
#undef m_type
#endif
#ifdef m_flags
#undef m_flags
#endif
#endif
