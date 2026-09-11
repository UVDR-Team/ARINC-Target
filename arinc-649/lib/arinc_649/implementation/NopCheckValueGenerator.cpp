// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc649::NopCheckValueGenerator.
 **/

#include "NopCheckValueGenerator.hpp"

#include <arinc_649/CheckValue.hpp>

namespace Arinc649 {

void NopCheckValueGenerator::reset()
{
  // No Operation
}

void NopCheckValueGenerator::process( [[maybe_unused]] Helper::ConstRawDataSpan data )
{
  // No Operation
}

CheckValue NopCheckValueGenerator::checkValue()
{
  return CheckValue::NoCheckValue;
}

}
