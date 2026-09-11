// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Namespace Arinc665Qt::Resources.
 **/

#include "Resources.hpp"

#include <QDir>

/**
 * @brief Initialises the resources.
 **/
static void Resources_initalise();

namespace Arinc665Qt::Resources {

void initialise()
{
  Resources_initalise();
}

}

static void Resources_initalise()
{
  Q_INIT_RESOURCE( arinc_665_qt_resources );
}
