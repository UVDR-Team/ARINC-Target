// SPDX-License-Identifier: MPL-2.0
/**
 * @brief Optional host-process entry point for the ARINC 615A THA test tool.
 *
 * The DKM links arinc_615a_tha_target and calls Arinc615aTha::run() through
 * its existing C wrapper; it must not link this translation unit.
 */

#include "Arinc615aTha.hpp"

int main( const int argc, char *argv[] )
{
  return Arinc615aTha::run( argc, argv );
}
