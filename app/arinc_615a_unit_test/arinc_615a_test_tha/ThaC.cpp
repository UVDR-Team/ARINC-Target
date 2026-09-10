// SPDX-License-Identifier: MPL-2.0
#include "Arinc615aThaC.h"
#include "Arinc615aTha.hpp"
#include <cstdlib>

extern "C" int arinc615a_tha_run_file(const char *json_filename)
{
  try { return Arinc615aTha::runFromFile(json_filename); }
  catch (...) { return EXIT_FAILURE; }
}

extern "C" int arinc615a_tha_request_stop(void)
{
  return Arinc615aTha::requestStop() ? 1 : 0;
}
