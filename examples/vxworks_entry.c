/* SPDX-License-Identifier: MPL-2.0 */
#include "arinc_615a_tha.h"
#include <stdio.h>

/* Invoke from an application-owned worker task, with a persistent writable
 * filesystem configured for uploads/downloads. Link the DKM as C++ so its
 * constructors and the SDK C++ runtime are included. */
int arinc615aDkmRun(const char *configuration_file)
{
  int status = arinc615a_tha_init_file(configuration_file);
  if (status != ARINC615A_OK) {
    printf("ARINC615A configuration failed: %d\n", status);
    return status;
  }
  return arinc615a_tha_start(0);
}

/* Call from another task. After this returns, also wait for the worker task
 * above to return before unloading the module. Do not forcibly delete it. */
void arinc615aDkmStop(void)
{
  arinc615a_tha_stop();
}
