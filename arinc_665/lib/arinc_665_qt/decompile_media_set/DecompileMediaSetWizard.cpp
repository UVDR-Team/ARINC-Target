// SPDX-License-Identifier: MPL-2.0
/**
 * @file
 * @copyright
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @author Thomas Vogt, thomas@thomas-vogt.de
 *
 * @brief Definition of Class Arinc665Qt::DecompileMediaSetWizard.
 **/

#include "DecompileMediaSetWizard.hpp"

#include "ui_DecompileMediaSetWizard.h"

#include <arinc_665/utils/MediaSetDefaults.hpp>

namespace Arinc665Qt {

DecompileMediaSetWizard::DecompileMediaSetWizard( QWidget * const parent ) :
  QWizard{ parent },
  uiV{ std::make_unique< Ui::DecompileMediaSetWizard >() }
{
  uiV->setupUi( this );

  // set Logo of Wizard Pages
  QIcon icon{};
  icon.addFile(
    QString::fromUtf8( ":/fa/solid/arrow-right-to-bracket.svg" ),
    QSize{},
    QIcon::Normal,
    QIcon::Off );
  for ( const auto pageId : pageIds() )
  {
    page( pageId )->setPixmap(
      QWizard::WizardPixmap::LogoPixmap,
      icon.pixmap( 64 ) );
  }

  connect(
    uiV->settings,
    &DecompileMediaSetSettingsPage::mediaPathsChanged,
    this,
    &DecompileMediaSetWizard::mediaPathsChanged );
  connect(
    uiV->settings,
    &DecompileMediaSetSettingsPage::checkFileIntegrity,
    this,
    &DecompileMediaSetWizard::checkFileIntegrity );

    // finally set defaults (signals must be connected)
    uiV->settings->defaults( Arinc665::Utils::MediaSetDefaults{} );
}

DecompileMediaSetWizard::~DecompileMediaSetWizard() = default;

}
