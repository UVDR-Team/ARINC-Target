// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665::Utils::MediaSetValidatorImpl.
 **/

#include "MediaSetValidatorImpl.hpp"

namespace Arinc665::Utils {

MediaSetValidatorImpl::MediaSetValidatorImpl() = default;

MediaSetValidator &MediaSetValidatorImpl::readFileHandler( ReadFileHandler readFileHandler )
{
  readFileHandlerV = std::move( readFileHandler );
  return *this;
}

MediaSetValidator &MediaSetValidatorImpl::informationHandler( ValidatorInformationHandler informationHandler )
{
  informationHandlerV = std::move( informationHandler );
  return *this;
}


bool MediaSetValidatorImpl::operator()()
{
  informationHandlerV( "Not implemented" );

  return false;
}

}
