/* SPDX-License-Identifier: MPL-2.0 */
#include "arinc_615a_tha.h"
int main(void)
{
  arinc615a_tha_set_log_level(ARINC615A_LOG_OFF);
  if (arinc615a_tha_init_default() != ARINC615A_OK) return 1;
  if (arinc615a_tha_init_json(0) != ARINC615A_ERROR) return 2;
  if (arinc615a_tha_init_file(0) != ARINC615A_ERROR) return 3;
  arinc615a_tha_stop();
  return arinc615a_tha_is_running();
}
