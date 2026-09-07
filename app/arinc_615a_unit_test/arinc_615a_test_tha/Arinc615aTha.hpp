// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @ingroup arinc_615a_test_tha
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Declaration of Namespace Arinc615aTha.
 **/

#ifndef ARINC615ATHA_HPP
#define ARINC615ATHA_HPP

/**
 * @brief ARINC 615A Test THA Data Loader
 **/
namespace Arinc615aTha {

/**
 * @brief Starts the target data loader runtime.
 *
 * This is deliberately a C++ library entry point rather than a process entry
 * point.  The existing C wrapper can call it from the DKM after supplying its
 * configuration-file argument.  It retains the original test application's
 * command-line and JSON configuration behaviour.
 */
int run( int argc, char *argv[] );

}

#endif
