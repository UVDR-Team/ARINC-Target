// SPDX-License-Identifier: MPL-2.0
#include "Arinc615aThaC.h"
int arinc_c_interface_smoke(void)
{
  return arinc615a_tha_run_file((const char *)0) != 0
    && arinc615a_tha_request_stop() == 0;
}
